def signed_to_unsigned(value, bits=32):
    if value < 0:
        return value + 2**bits
    return value
