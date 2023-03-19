import zlib
from unsigned_to_signed import unsigned_to_signed


def calc_crc32(data):
    return unsigned_to_signed(zlib.crc32(data))
