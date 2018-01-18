# Cube256 C++ implementation  

# Usage:  
Cube cube;

cube.encrypt(data, key, nonce);  

cube.decrypt(data, key, nonce);  

# CubeHash  
CubeHash hash;  

hash.digest(data, optionalkey, length_in_bits);  

# CubeKDF  
CubeKDF kdf;  

kdf.genkey(key, keylen_in_bytes, iterations);  

# CubeRandom  
CubeRandom rand;  

rand.random(num_of_bytes);  
