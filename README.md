# Cube256 C++ implementation  
Cube is an advanced substitution stream cipher.  It is classified as an Expensive Cipher.  If you have the resources to encrypt large files with Cube, you'll be sure your secret will be safe for ages to come.

Cube passes NIST and DieHarder statistical tests.  Because of the way Cube utilizes the key values, the minimum key length is 16 bytes or 128 bits.  Key lengths of a single byte have been identified as weak.

(Warning: Side channel attacks are possible against Cube.)

# Usage:  
Cube cube;

cube.encrypt(data, key, nonce);  

cube.decrypt(data, key, nonce);  

# CubeSum (Slow hash function)  
CubeSum hash;  

hash.digest(data, optionalkey, length_in_bits);  

# CubeKDF  
CubeKDF kdf;  

kdf.genkey(key, keylen_in_bytes, iterations);  

# CubeRandom  
CubeRandom rand;  

rand.random(num_of_bytes);  
