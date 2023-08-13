if __name__ == "__main__":
    import binascii
    import struct
    import argparse
    import numpy as np
    from pathlib import Path
    from io import SEEK_CUR, SEEK_END

    ap = argparse.ArgumentParser()
    ap.add_argument("recovery_path", help="recovery image path", type=Path)

    args = ap.parse_args()
    recovery_path: Path = args.recovery_path.resolve()

    if not recovery_path.exists():
        raise FileNotFoundError("Recovery image does not exist. Please check again.")

    with open(
        recovery_path,
        "r+b",
    ) as f:
        magic = f.read(8)
        print(magic)
        kernel_size = struct.unpack("<I", f.read(4))[0]
        print(f"kernel_size: {kernel_size}")
        f.seek(4, SEEK_CUR)
        ramdisk_size = struct.unpack("<I", f.read(4))[0]
        print(f"ramdisk_size: {ramdisk_size}")
        f.seek(4, SEEK_CUR)
        second_size = struct.unpack("<I", f.read(4))[0]
        print(f"second_size: {second_size}")
        f.seek(8, SEEK_CUR)
        page_size = struct.unpack("<I", f.read(4))[0]
        print(f"page_size: {page_size}")

        recovery_page_cnt = (
            ((kernel_size + page_size - 1) // page_size)
            + ((ramdisk_size + page_size - 1) // page_size)
            + ((second_size + page_size - 1) // page_size)
            + 1
        )

        print(f"recovery_page_cnt: {recovery_page_cnt}")
        print(f"recovery_page_cnt * page_size: {recovery_page_cnt * page_size}")

        check_zero = 0
        sum_ = np.int32(0)
        f.seek(0)
        d = f.read(4)
        while d:
            d = struct.unpack("<i", d)[0]
            sum_ += np.int32(d)
            if d != 0:
                check_zero = 1
            d = f.read(4)

        if check_zero == 1:
            print("Check zero enabled. Patching recovery image...")
            f.seek(-4, SEEK_END)
            d = f.read(4)
            d = struct.unpack("<i", d)[0]
            sum_ -= np.int32(d)
            f.seek(-4, SEEK_END)
            f.write(struct.pack("<i", -sum_))
            print("Patching done.")
        else:
            print("Check zero disabled. Skipping patching...")
