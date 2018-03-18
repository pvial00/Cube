# Cube256 C++ implementation  
Cube is an advanced substitution stream cipher/cipher machine.  It is what I call an Expensive Cipher because of how slow Cube is compared to most modern ciphers.

Cube passes NIST and DieHarder statistical tests.  Because of the way Cube utilizes the key values, the minimum key length is 16 bytes or 128 bits.  Key lengths of a single byte have been identified as weak.

(Caution: Use at your own risk!)  
(Caution: Side channel attacks are possible against Cube.)

# Cube cipher machine file encryption usage (Mac/Linux/Windows):  
Cube v1.0.3 *** Cube ciphering machine ***

Warning: This machine is designed to work as is, without modification.
Use at your at own risk!

Author: pvial@kryptomagik.com

-e <input file> <output file> <password> (Encrypt)  
-d <input file> <output file> <password> (Decrypt)  
-emac <input file> <output file> <password> (Encrypt with HMAC)  
-dmac <input file> <output file> <password> (Decrypt with HMAC)  
-sum <input file>  
-kdf <password> <optional keylength in bytes> <optional # of iterations>  
-random <optional number of bytes default is 1 byte>  

* Dependendcies to build Cube in Linux  
libbsd-dev  
libssl-dev  

* Dependencies for MacOS  
openssl  


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
