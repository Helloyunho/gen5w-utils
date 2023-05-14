"""
So .ver file in firmware looks like this:
+|year|version string|type(KM for Kia cars, HM for Hyundai cars, and GN for Genesis cars)|car codename|device m code(another model specific code I guess)|1(it's always 1)
car codename|file name|file version|crc32(in signed int32)|file size(in signed int32)|1(again, it's always 1)
... and so on

This tool reads .ver file and prints it in a more readable, computer-friendly format.
"""

from typing import TypedDict


class VerFileEntry(TypedDict):
    file_name: str
    file_version: int
    crc32: int
    file_size: int


class VerFile(TypedDict):
    year: str
    version: str
    type: str
    car_codename: str
    device_m_code: int
    files: list[VerFileEntry]


def parse_ver(ver: str) -> VerFile:
    lines = ver.splitlines()
    line_idx = 0
    result: VerFile = {
        "year": "",
        "version": "",
        "type": "",
        "car_codename": "",
        "device_m_code": 0,
        "files": [],
    }
    while line_idx < len(lines):
        line = lines[line_idx]
        if line_idx == 0:
            _, year, version, type_, car_codename, device_m_code, _ = line.split("|")
            device_m_code = int(device_m_code)
            result["year"] = year
            result["version"] = version
            result["type"] = type_
            result["car_codename"] = car_codename
            result["device_m_code"] = device_m_code
        else:
            _, file_name, file_version, crc32, file_size, _ = line.split("|")
            file_version, crc32, file_size = (
                int(file_version),
                int(crc32),
                int(file_size),
            )
            result["files"].append(
                {
                    "file_name": file_name,
                    "file_version": file_version,
                    "crc32": crc32,
                    "file_size": file_size,
                }
            )
        line_idx += 1
    return result


def generate_ver(ver: VerFile) -> str:
    lines = []
    lines.append(
        f"+|{ver['year']}|{ver['version']}|{ver['type']}|{ver['car_codename']}|{ver['device_m_code']}|1"
    )
    for file in ver["files"]:
        lines.append(
            f"{ver['car_codename']}|{file['file_name']}|{file['file_version']}|{file['crc32']}|{file['file_size']}|1"
        )
    return "\n".join(lines)


if __name__ == "__main__":
    import argparse
    from pathlib import Path

    ap = argparse.ArgumentParser()
    ap.add_argument("ver_file", help="input .ver file", type=Path)

    args = ap.parse_args()
    ver_file: Path = args.ver_file.resolve()

    if ver_file.exists():
        with open(ver_file, "r") as f:
            ver = f.read()
        parsed = parse_ver(ver)
        print(f"Year Code: {parsed['year']}")
        print(f"Version: {parsed['version']}")
        car_type = ""
        if parsed["type"] == "KM":
            car_type = "Kia"
        elif parsed["type"] == "HM":
            car_type = "Hyundai"
        elif parsed["type"] == "GN":
            car_type = "Genesis"
        print(f"Type: {car_type}")
        print(f"Car Codename: {parsed['car_codename']}")
        print(f"Device M Code: {parsed['device_m_code']}")
        print("----------------------------------------")
        for file in parsed["files"]:
            print(f"File Name: {file['file_name']}")
            print(f"File Version: {file['file_version']}")
            print(f"CRC32: {file['crc32']}")
            print(f"File Size: {file['file_size']}")
            print("----------------------------------------")
    else:
        raise FileNotFoundError("File not found.")
