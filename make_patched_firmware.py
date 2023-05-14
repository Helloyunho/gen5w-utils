if __name__ == "__main__":
    import argparse
    from pathlib import Path
    import tarfile
    import shutil
    from tarlist_decrypt import decrypt_tarlist
    from parse_tarlist import parse_tarlist, generate_tarlist
    from parse_ver import parse_ver, generate_ver
    from decrypt_navi import decrypt_navi
    from patch_navi import patch_ExSLNavi
    from encrypt_navi import encrypt_navi
    from utils.calc_crc32 import calc_crc32
    from sha224_gen import generate_hash

    ap = argparse.ArgumentParser()
    ap.add_argument(
        "firmware_dir", help="firmware directory path", type=Path, default=Path(".")
    )
    ap.add_argument("-s", "--skip", help="skip patching", action="store_true")

    args = ap.parse_args()
    firmware_dir: Path = args.firmware_dir.resolve()
    skip_patching: bool = args.skip

    if firmware_dir.exists():
        # Check if every necessary file exists
        print("Checking if every necessary file exists...")
        tarlist_enc_path = firmware_dir / "TarList.txt_encrypted"
        tarlist_path = firmware_dir / "TarList.txt"
        ver_path = list(firmware_dir.glob("*.ver"))[0]
        tar_path = firmware_dir / "sw_backup.tar"

        if not tarlist_enc_path.exists() and not tarlist_path.exists():
            raise FileNotFoundError(
                "TarList.txt_encrypted or TarList.txt does not exist in firmware directory."
            )
        if not ver_path.exists():
            raise FileNotFoundError("*.ver does not exist in firmware directory.")
        if not tar_path.exists():
            raise FileNotFoundError(
                "sw_backup.tar does not exist in firmware directory."
            )

        # Decrypt TarList.txt_encrypted to TarList.txt
        print("Decrypting TarList.txt_encrypted to TarList.txt...")
        if tarlist_enc_path.exists():
            decrypt_tarlist(tarlist_enc_path, tarlist_path)
        tarlist = parse_tarlist(tarlist_path.read_text())
        print(
            f"SW Version: {tarlist['sw_version']['region']}.{tarlist['sw_version']['year']:02d}{tarlist['sw_version']['month']:02d}{tarlist['sw_version']['day']:02d}.{tarlist['sw_version']['ver1']:x}"
        )

        # Parse .ver
        print("Parsing .ver...")
        ver = parse_ver(ver_path.read_text())

        # Untar sw_backup.tar in sw_backup/
        print("Untaring sw_backup.tar in sw_backup/...")
        with tarfile.open(tar_path, "r") as tar:
            tar.extractall(firmware_dir / "sw_backup")

        # Search Navi_Version.txt in sw_backup/
        print("Searching Navi_Version.txt in sw_backup/...")
        navi_version_paths = list(
            (firmware_dir / "sw_backup").glob("**/Navi_Version.txt")
        )
        if len(navi_version_paths) == 0:
            raise FileNotFoundError(
                "Necessary files do not exist in sw_backup directory. The firmware might be corrupted."
            )

        # Patch Navi_Version.txt
        print("Patching Navi_Version.txt...")
        for navi_version_path in navi_version_paths:
            navi_version_tags = navi_version_path.read_text().split(".")
            tarlist["sw_version"]["ver1"] += 1
            tarlist["sw_version"]["day"] += 1
            navi_version_tags[
                3
            ] = f"{tarlist['sw_version']['year']:02d}{tarlist['sw_version']['month']:02d}{tarlist['sw_version']['day']:02d}"
            navi_version_path.write_text(".".join(navi_version_tags))

        if not skip_patching:
            # Search ExSLNavi in sw_backup/
            print("Searching ExSLNavi in sw_backup/...")
            library_paths = list(
                (firmware_dir / "sw_backup").glob("**/enc_libExSLAndroidJNI.so")
            )
            if len(library_paths) == 0:
                raise FileNotFoundError(
                    "Necessary libraries do not exist in sw_backup directory. The firmware might be corrupted."
                )
            for library_path in library_paths:
                # Decrypt ExSLNavi
                print(f"Decrypting {library_path}...")
                decrypted_library_path = library_path.with_name(library_path.name[4:])
                decrypted_library_path.write_bytes(
                    decrypt_navi(library_path.read_bytes())
                )

                # Patch ExSLNavi
                print(f"Patching {decrypted_library_path}...")
                patched_library_path = decrypted_library_path.with_name(
                    decrypted_library_path.name[:-3] + "_p.so"
                )
                patch_ExSLNavi(
                    str(decrypted_library_path.resolve()),
                    str(patched_library_path.resolve()),
                )

                # Encrypt ExSLNavi
                print(f"Encrypting {patched_library_path}...")
                encrypted_library_path = patched_library_path.with_name(
                    patched_library_path.name[:-5] + "_e.so"
                )
                encrypted_library_path.write_bytes(
                    encrypt_navi(patched_library_path.read_bytes())
                )

                # Replace ExSLNavi
                print(f"Replacing {library_path}...")
                library_path.unlink()
                encrypted_library_path.rename(library_path)

                # Remove decrypted and patched ExSLNavi
                decrypted_library_path.unlink()
                patched_library_path.unlink()

        # Tar sw_backup/ to sw_backup.tar
        print("Taring sw_backup/ to sw_backup.tar...")
        with tarfile.open(tar_path, "w") as tar:
            tar.add(firmware_dir / "sw_backup", arcname="")

        # Update hash in TarList.txt
        print("Updating hash in TarList.txt...")
        for f in tarlist["file_list"]:
            if f["file_name"] == "sw_backup.tar":
                f["sha224"] = generate_hash(tar_path)

        # Update CRC32 in .ver
        print("Updating CRC32 in .ver...")
        for f in ver["files"]:
            if f["file_name"] == "sw_backup.tar":
                crc32 = calc_crc32(tar_path.read_bytes())
                f["crc32"] = crc32
                size = tar_path.stat().st_size
                f["file_size"] = size

        # Write TarList.txt
        print("Writing TarList.txt...")
        tarlist_path.write_text(generate_tarlist(tarlist))

        # Write .ver
        print("Writing .ver...")
        ver_path.write_text(generate_ver(ver))

        # Encrypt TarList.txt to TarList.txt_encrypted
        print("Encrypting TarList.txt to TarList.txt_encrypted...")
        if tarlist_enc_path.exists():
            tarlist_enc_path.unlink()
        decrypt_tarlist(
            tarlist_path, tarlist_enc_path
        )  # it uses XOR, so it's reversible
        tarlist_path.unlink()

        # Remove sw_backup/
        print("Removing sw_backup/...")
        shutil.rmtree(firmware_dir / "sw_backup")

        print("Done.")
    else:
        raise FileNotFoundError(
            "firmware directory does not exist. Please check again."
        )
