class FNV1a:
    def __init__(self):
        self._val32 = 0x811c9dc5
        self._prime32 = 0x1000193
        self._val64 = 0xcbf29ce484222325
        self._prime64 = 0x100000001b3

    def hash_32_fnv1a(self, s: str) -> int:
        hash_value = self._val32
        for byte in s.encode('utf-8'):
            hash_value ^= byte
            hash_value *= self._prime32
            hash_value &= 0xffffffff
        return hash_value

    def hash_64_fnv1a(self, s: str) -> int:
        hash_value = self._val64
        for byte in s.encode('utf-8'):
            hash_value ^= byte
            hash_value *= self._prime64
            hash_value &= 0xffffffffffffffff
        return hash_value
