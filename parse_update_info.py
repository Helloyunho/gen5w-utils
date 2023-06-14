"""
So update.info file in firmware looks like this:
SWVERSION - TYPE(KMC for Kia cars, HMC for Hyundai cars, and GNS for Genesis cars).REGION.MAJOR.(V)MINOR

./directory/path
: file_name:sha224
... and so on

This tool reads update.info file and prints it in a more readable, computer-friendly format.
"""

from typing import TypedDict


class UpdateInfoFileVersion(TypedDict):
    type: str
    region: str
    major: int
    minor: int


class UpdateInfoFileEntry(TypedDict):
    path: str
    file_name: str
    sha224: str


class UpdateInfoFile(TypedDict):
    version: UpdateInfoFileVersion
    files: list[UpdateInfoFileEntry]


def parse_update_info(update_info: str) -> UpdateInfoFile:
    lines = update_info.splitlines()
    line_idx = 0
    result: UpdateInfoFile = {
        "version": {
            "type": "",
            "region": "",
            "major": 0,
            "minor": 0,
        },
        "files": [],
    }
    curr_dir = ""
    while line_idx < len(lines):
        line = lines[line_idx]
        if line_idx == 0:
            type_, region, major, minor = line[12:].split(".")
            major, minor = int(major), int(minor[1:])
            result["version"] = {
                "type": type_,
                "region": region,
                "major": major,
                "minor": minor,
            }
        else:
            if line.startswith("./"):
                curr_dir = line[2:]
            elif line.startswith(":"):
                file_name, sha224 = line[2:].split(":")
                result["files"].append(
                    {
                        "path": curr_dir + "/" + file_name,
                        "file_name": file_name,
                        "sha224": sha224,
                    }
                )
        line_idx += 1

    return result


def generate_update_info(update_info: UpdateInfoFile) -> str:
    lines = []
    lines.append(
        f"SWVERSION - {update_info['version']['type']}.{update_info['version']['region']}.{update_info['version']['major']:04d}.V{update_info['version']['minor']:03d}"
    )
    lines.append("")
    curr_dir = ""
    for file in update_info["files"]:
        if curr_dir != file["path"].split("/")[:-1]:
            curr_dir = file["path"].split("/")[:-1]
            lines.append(f"./{'/'.join(curr_dir)}")
        lines.append(f": {file['file_name']}:{file['sha224']}")
    return "\n".join(lines)


if __name__ == "__main__":
    import argparse
    from pathlib import Path

    ap = argparse.ArgumentParser()
    ap.add_argument("update_info", help="input update.info file", type=Path)

    args = ap.parse_args()
    update_info_file: Path = args.update_info.resolve()

    if update_info_file.exists():
        update_info = update_info_file.read_text()
        update_info_parsed = parse_update_info(update_info)
        print(f"Car type: {update_info_parsed['version']['type']}")
        print(f"Region: {update_info_parsed['version']['region']}")
        print(
            f"Version: {update_info_parsed['version']['major']}.V{update_info_parsed['version']['minor']}"
        )

        for file in update_info_parsed["files"]:
            print(f"Path: {file['path']}, SHA224: {file['sha224']}")
    else:
        raise FileNotFoundError("File not found.")
