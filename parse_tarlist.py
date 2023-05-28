"""
So TarList.txt file in firmware looks like this:
@FILE:file name,file version,customized sha224
... and so on
then
@VERSION:REGION.VER1(2 digits).VER2(2 digits).VER3(2 digits).VER4(3 digits).VER5(3 digits).VER6(1 digit)
@NEW_VERSION:(same as above)
@SWVERSION_K:STD5W.REGION.KIA.DATE(in YY/MM/DD).VER1(in hex, 7 digits)
@SWVERSION_H:(same as above except KIA -> HMC)
@SWVERSION_G:(same as above except KIA -> GNS)
@NEW_SWVERSION_K:STD5W.REGION.KIA.DATE(in YY/MM/DD).VER1(in hex, 7 digits)
@NEW_SWVERSION_H:(same as above except KIA -> HMC)
@NEW_SWVERSION_G:(same as above except KIA -> GNS)

This tool reads TarList.txt file and prints it in a more readable, computer-friendly format.
"""

from typing import TypedDict


class TarListFileEntry(TypedDict):
    file_name: str
    file_version: int
    sha224: str


class TarListMapVersion(TypedDict):
    region: str
    ver1: int
    ver2: int
    ver3: int
    ver4: int
    ver5: int
    ver6: int


class TarListSWVersion(TypedDict):
    region: str
    year: int
    month: int
    day: int
    ver1: int


class TarListFile(TypedDict):
    file_list: list[TarListFileEntry]
    map_version: TarListMapVersion
    sw_version: TarListSWVersion


def parse_tarlist(tarlist: str) -> TarListFile:
    lines = tarlist.splitlines()
    line_idx = 0
    result: TarListFile = {
        "file_list": [],
        "map_version": {
            "region": "",
            "ver1": 0,
            "ver2": 0,
            "ver3": 0,
            "ver4": 0,
            "ver5": 0,
            "ver6": 0,
        },
        "sw_version": {
            "region": "",
            "year": 0,
            "month": 0,
            "day": 0,
            "ver1": 0,
        },
    }
    while line_idx < len(lines):
        line = lines[line_idx]
        _type = line.split(":")[0]
        value = line.split(":")[1]
        if _type == "@FILE":
            file_name, file_version, sha224 = value.split(",")
            file_version = int(file_version)
            result["file_list"].append(
                {
                    "file_name": file_name,
                    "file_version": file_version,
                    "sha224": sha224,
                }
            )
        elif _type == "@VERSION" or _type == "@NEW_VERSION":
            region, ver1, ver2, ver3, ver4, ver5, ver6 = value.split(".")
            ver1, ver2, ver3, ver4, ver5, ver6 = (
                int(ver1),
                int(ver2),
                int(ver3),
                int(ver4),
                int(ver5),
                int(ver6),
            )
            result["map_version"] = {
                "region": region,
                "ver1": ver1,
                "ver2": ver2,
                "ver3": ver3,
                "ver4": ver4,
                "ver5": ver5,
                "ver6": ver6,
            }
        elif (
            _type == "@SWVERSION_K"
            or _type == "@SWVERSION_H"
            or _type == "@SWVERSION_G"
            or _type == "@NEW_SWVERSION_K"
            or _type == "@NEW_SWVERSION_H"
            or _type == "@NEW_SWVERSION_G"
        ):
            _, region, _, date, ver1 = value.split(".")
            year, month, day, ver1 = (
                int(date[:2]),
                int(date[2:4]),
                int(date[4:6]),
                int(ver1, 16),
            )
            result["sw_version"] = {
                "region": region,
                "year": year,
                "month": month,
                "day": day,
                "ver1": ver1,
            }
        line_idx += 1
    return result


def generate_tarlist(tarlist: TarListFile) -> str:
    lines = []
    for file in tarlist["file_list"]:
        lines.append(
            f"@FILE:{file['file_name']},{file['file_version']},{file['sha224']}"
        )
    for _t in ["VERSION", "NEW_VERSION"]:
        lines.append(
            f"@{_t}:{tarlist['map_version']['region']}.{tarlist['map_version']['ver1']:02d}.{tarlist['map_version']['ver2']:02d}.{tarlist['map_version']['ver3']:02d}.{tarlist['map_version']['ver4']:03d}.{tarlist['map_version']['ver5']:03d}.{tarlist['map_version']['ver6']}"
        )
    for _t in ["SWVERSION", "NEW_SWVERSION"]:
        for car_manufacture in ["K", "H", "G"]:
            lines.append(
                f"@{_t}_{car_manufacture}:STD5W.{tarlist['sw_version']['region']}.{('KIA' if _t == 'NEW_SWVERSION' else 'KMC') if car_manufacture == 'K' else 'HMC' if car_manufacture == 'H' else 'GNS' if car_manufacture == 'G' else 'UNKNOWN'}.{tarlist['sw_version']['year']:02d}{tarlist['sw_version']['month']:02d}{tarlist['sw_version']['day']:02d}.{tarlist['sw_version']['ver1']:x}"
            )
    return "\n".join(lines)


if __name__ == "__main__":
    import argparse
    from pathlib import Path

    ap = argparse.ArgumentParser()
    ap.add_argument("tarlist_txt", help="input TarList.txt file", type=Path)

    args = ap.parse_args()
    tarlist_file: Path = args.tarlist_txt.resolve()

    if tarlist_file.exists():
        with open(tarlist_file, "r") as f:
            ver = f.read()
        parsed = parse_tarlist(ver)
        print(f"Region: {parsed['map_version']['region']}")
        print(
            f"Map Version: {parsed['map_version']['region']}.{parsed['map_version']['ver1']:02d}.{parsed['map_version']['ver2']:02d}.{parsed['map_version']['ver3']:02d}.{parsed['map_version']['ver4']:03d}.{parsed['map_version']['ver5']:03d}.{parsed['map_version']['ver6']}"
        )
        print(
            f"SW Version: {parsed['sw_version']['region']}.{parsed['sw_version']['year']:02d}{parsed['sw_version']['month']:02d}{parsed['sw_version']['day']:02d}.{parsed['sw_version']['ver1']:x}"
        )
        print(
            f"Published Date: 20{parsed['sw_version']['year']}/{parsed['sw_version']['month']}/{parsed['sw_version']['day']}"
        )
        for file in parsed["file_list"]:
            print(
                f"File: {file['file_name']}, Version: {file['file_version']}, SHA224: {file['sha224']}"
            )
    else:
        raise FileNotFoundError("File not found.")
