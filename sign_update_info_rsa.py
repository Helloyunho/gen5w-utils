import subprocess
from sha224_gen import generate_hash
from pathlib import Path


def sign_update_info_rsa(update_info: Path, private_key: Path) -> bytes:
    _hash = generate_hash(update_info)
    signed_hash = subprocess.run(
        ["openssl", "rsautl", "-sign", "-inkey", str(private_key), "-sign"],
        input=(_hash + "\n").encode("utf-8"),
        capture_output=True,
    )
    return signed_hash.stdout


if __name__ == "__main__":
    import argparse
    from pathlib import Path

    ap = argparse.ArgumentParser()
    ap.add_argument("private_key", help="private key path", type=Path)
    ap.add_argument("update_info", help="update.info path", type=Path)
    ap.add_argument("-o", "--output", help="output path", type=Path)

    args = ap.parse_args()
    update_info: Path = args.update_info.resolve()
    private_key: Path = args.private_key.resolve()
    output: Path = (
        args.output.resolve() if args.output else update_info.with_name("update_info")
    )

    if update_info.exists() and private_key.exists():
        with open(output, "wb") as f:
            f.write(sign_update_info_rsa(update_info, private_key))
    else:
        raise FileNotFoundError("File not found.")
