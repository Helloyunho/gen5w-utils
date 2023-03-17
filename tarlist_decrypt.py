import argparse
from pathlib import Path


def decrypt_tarlist(enc_file, dest):
    if not dest.parent.exists():
        dest.parent.mkdir(parents=True)
    with open(enc_file, "rb") as f:
        data = f.read()
    with open(dest, "wb") as f:
        for i in range(len(data)):
            f.write(bytes([data[i] ^ 0x11]))


if __name__ == "__main__":
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
        decrypt_tarlist(filepath, dest)
    else:
        raise FileNotFoundError("File not found.")
