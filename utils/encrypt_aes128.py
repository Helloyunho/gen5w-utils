from Crypto.Cipher import AES
from Crypto.Util.Padding import pad


def encrypt_aes128(key, iv, data):
    if len(key) > 16:
        key = key[:16]
    elif len(key) < 16:
        key = pad(key, AES.block_size)
    if len(iv) > 16:
        iv = iv[:16]
    elif len(iv) < 16:
        iv = pad(iv, AES.block_size)
    data = pad(data, AES.block_size)
    cipher = AES.new(key, AES.MODE_CBC, iv)
    return cipher.encrypt(data)
