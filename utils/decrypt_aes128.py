from Crypto.Cipher import AES
from Crypto.Util.Padding import pad


def decrypt_aes128(key, iv, data):
    if len(key) > 16:
        key = key[:16]
    elif len(key) < 16:
        key = pad(key, 16)
    cipher = AES.new(key, AES.MODE_CBC, iv)
    return cipher.decrypt(data)
