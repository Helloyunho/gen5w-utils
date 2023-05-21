import lief
from keystone import Ks, KS_ARCH_ARM, KS_MODE_THUMB


def patch_ExSLNavi(filepath, dest):
    orig = lief.parse(filepath)
    hook = lief.parse("hook.o")

    hooked_func = hook.get_symbol("hooked_func")
    target_func = orig.get_symbol("printJNILog_JNI")

    added_section = orig.add(hooked_func.section)
    rodata_section = orig.get_section(".rodata")

    func_addr = added_section.virtual_address

    # checking
    expected_hex = [0x01, 0xD0]
    if (
        orig.get_content_from_virtual_address(target_func.value + 0xBB, 2)
        != bytes(expected_hex)
    ):
        raise ValueError(
            "The target opcode is not found. It may be already patched or the binary is not supported."
        )
    # check for "JNI_Layer" hex
    expected_hex = [0x4A, 0x4E, 0x49, 0x5F, 0x4C, 0x61, 0x79, 0x65, 0x72]
    if (
        orig.get_content_from_virtual_address(rodata_section.virtual_address, 9)
        != bytes(expected_hex)
    ):
        raise ValueError(
            "The target text is not found. It may be already patched or the binary is not supported."
        )

    # patching
    ks = Ks(KS_ARCH_ARM, KS_MODE_THUMB)
    patch_asm, _ = ks.asm(f"bl #{func_addr - (target_func.value + 0xBB)}")
    patch_hex = [0x0, 0xBF, 0x00, 0xBF, 0x00, 0xBF]
    for i, b in enumerate(patch_asm):
        patch_hex[i] = b
    orig.patch_address(target_func.value + 0xBB, patch_hex)

    # patching JNI_Layer
    # JNI_HACKD
    patch_hex = [0x4A, 0x4E, 0x49, 0x5F, 0x48, 0x41, 0x43, 0x4B, 0x44]
    orig.patch_address(rodata_section.virtual_address, patch_hex)
    orig.write(dest)


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
        else filepath.with_suffix(filepath.suffix + ".patched")
    )

    if filepath.exists():
        patch_ExSLNavi(str(filepath), str(dest))
    else:
        raise FileNotFoundError("File not found.")
