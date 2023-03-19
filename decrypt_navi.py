from utils.decrypt_aes128 import decrypt_aes128


def decrypt_navi(data):
    key = (0xFCF53C45E1E3F1C1C5480C3B20A6A45A).to_bytes(
        16, "big"
    )  # found by reverse engineering the uagentd binary
    iv = (0x000102030405060708090A0B0C0D0E0F).to_bytes(
        16, "big"
    )  # no kidding, they use this as iv
    return decrypt_aes128(key, iv, data)


if __name__ == "__main__":
    import argparse
    from pathlib import Path

    ap = argparse.ArgumentParser()
    ap.add_argument("filepath", help="input file", type=Path)
    ap.add_argument("-o", "--out", help="output file", type=Path, default=None)

    args = ap.parse_args()
    filepath: Path = args.filepath.resolve()
    dest: Path = (
        args.out.resolve()
        if args.out is not None
        else filepath.with_suffix(filepath.suffix + ".decrypted")
    )

    if args.filepath.exists():
        filepath: Path = args.filepath
        with open(filepath, "rb") as f:
            data = f.read()
        data = decrypt_navi(data)
        with open(dest, "wb") as f:
            f.write(data)
    else:
        raise FileNotFoundError("File not found.")
