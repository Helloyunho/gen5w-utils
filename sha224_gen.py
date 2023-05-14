import hashlib
from pathlib import Path

SHA_BLOCK_SIZE = 64
SHA_DIGEST_SIZE = 28
SHA224_DIGEST_SIZE = 28
STR_MAX_LENGTH = 128

SHA_MAX_BUF_SIZE = 16 * 1024 * 1024
SHA_BUF_SIZE = SHA_MAX_BUF_SIZE - (SHA_BLOCK_SIZE * 2)


def generate_hash(path: Path):
    first_hash = bytearray()
    with open(path, "rb") as f:
        data = bytearray(f.read(SHA_BUF_SIZE))
        while data:
            hasher = hashlib.sha224()
            hasher.update(data)
            first_hash.extend(hasher.digest())
            data = bytearray(f.read(SHA_BUF_SIZE))

    second_hasher = hashlib.sha224()
    second_hasher.update(first_hash)
    return second_hasher.hexdigest()


if __name__ == "__main__":
    import argparse
    from pathlib import Path

    ap = argparse.ArgumentParser()
    ap.add_argument("file", help="file path to generate sha224", type=Path)

    args = ap.parse_args()
    file: Path = args.file.resolve()

    if file.exists():
        print(generate_hash(file))
    else:
        raise FileNotFoundError("File not found.")
