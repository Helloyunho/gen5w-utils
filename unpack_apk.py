from pathlib import Path
import subprocess
from typing import Optional
from odex_to_dex import odex_to_dex


def unpack_apk(
    filepath: Path, system_path: Optional[Path] = None, outpath: Optional[Path] = None
):
    outpath = outpath if outpath is not None else filepath.with_suffix("")
    if filepath.exists() and filepath.suffix == ".apk":
        subprocess.run(f"apktool d {filepath} -o {outpath}", shell=True, check=True)

        if filepath.with_suffix(".odex").exists():
            if system_path is None or not system_path.exists():
                raise FileNotFoundError(
                    "Odex file is detected but no proper system path is provided."
                )
            odexpath = filepath.with_suffix(".odex")
            odex_to_dex(odexpath, system_path, outpath / "classes.dex")
    else:
        raise FileNotFoundError("File not found or not an APK file.")


if __name__ == "__main__":
    import argparse

    ap = argparse.ArgumentParser()
    ap.add_argument("filepath", help="input file", type=Path)
    ap.add_argument(
        "-s",
        "--system-path",
        help="system path (for odex)",
        type=Path,
        default=Path("system/"),
        dest="system_path",
    )
    ap.add_argument("-o", "--out", help="output file", type=Path, default=None)

    args = ap.parse_args()
    filepath: Path = args.filepath.resolve()
    system_path: Path = args.system_path.resolve()
    outpath = args.out.resolve() if args.out is not None else filepath.with_suffix("")
    unpack_apk(filepath, system_path, outpath)
