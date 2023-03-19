def unsigned_to_signed(value, bits=32):
    v = value & (2**bits - 1)
    if v & 2 ** (bits - 1):
        return v - 2**bits
    return v
