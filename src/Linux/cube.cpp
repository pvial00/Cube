#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <utility>
#include <algorithm>
#include <bsd/stdlib.h>

using namespace std;

// Cube 128 bit ciphering machine
class Cube {
    // Globals that determine the dimensions of the Cube state (3x3x256)
    // Key length is always 16 bytes / 128 bits
    // Don't touch!
    public:
    int size_factor = 3;
    int alphabet_size = 256;
    int key_length = 16;
    vector< vector< vector<int> > > state;

    // Generate the 3 dimensional Cube state
    // Each state is initially shuffled using an algorithm derived from Chaociopher
    // Each alphabet shuffle is done Y rounds making the 3 alphabets in each section unique but not entirely unique in the entire state
    void gen_cube (int depth, int width, int length) {
        for (int z=0; z < depth; z++) {
            vector< vector<int> > section;
            for (int y=0; y < width; y++) {
                vector<int> alphabet;
                for (int x=0; x < length; x++) {
                    alphabet.push_back(x);
                }
                for (int m=0; m < y; m++) {
    		    int shift;
                    shift = alphabet.at(0);
		    alphabet.erase(alphabet.begin()+0);
                    alphabet.push_back(shift);
                    shift = alphabet.at(2);
                    alphabet.erase(alphabet.begin()+2);
                    alphabet.insert(alphabet.begin()+127,shift);
                }
                section.push_back(alphabet);
            }
            state.push_back(section);
        }
    }
    
    // Use the supplied key or nonce to shuffle the initial Cube state
    // The alphabets in the resulting state end up all unique and the state is ready for use.
    void key_cube (string key) {
        int z, x, y, k, shuffle, key_sub, sized_pos;
        for (z=0; z < state.size(); z++) {
            for (unsigned char k: key) {
                for (x=0; x < state[z].size(); x++) {
	            // We append the key character position to the end
                    key_sub = state[z][x].at(int(k));
		    state[z][x].erase(state[z][x].begin()+int(k));
                    state[z][x].push_back(key_sub);
		    // We shuffle however many times the current key char value
                    for (y=0; y < int(k); y++) {
			// We shuffle only on even rounds to keep states unique
                        if (y % 2 == 0) {
			    // Again modified Chaocipher shuffle
                            shuffle = state[z][x].at(0);
                            state[z][x].erase(state[z][x].begin()+0);
                            state[z][x].push_back(shuffle);
                            shuffle = state[z][x].at(2);
                            state[z][x].erase(state[z][x].begin()+2);
                            state[z][x].insert(state[z][x].begin()+127,shuffle);
                        }
                    }
                }
            }
        }
	// Last step in keying the Cube state is to pop and push back the state that equals the current key value modulo the size factor which is 3
	// This procedure defines a unique order of states
        vector< vector<int> > section;
        for (unsigned char k: key) {
	    sized_pos = int(k) % size_factor;
	    for (y=0; y < int(k); y++) {
		    section = state.at(sized_pos);
		    state.erase(state.begin()+sized_pos);
		    state.push_back(section);
            }
        }
    }
    
    // The key scheduler generates a new, unique round key between each byte substitution
    // If we simply substituted the key through the Cube state we may continually have key values that always remain identical to each other
    // This algorithm ensures that the key values are substituted and then the value that had just immediately been substituted is appended to the end of the current alphabet.  This also helps uniformity and acts as another kind of counter.
    string key_scheduler (string key) {
        int x, sized_pos, sub;
        vector< vector<int> > section;
        vector<int> sub_alpha;
        string sub_key;
        for (unsigned char k: key) {
	    sized_pos = int(k) % size_factor;
            sub = state[sized_pos][sized_pos][int(k)];
	    state[sized_pos][sized_pos].erase(state[sized_pos][sized_pos].begin()+int(k));
            state[sized_pos][sized_pos].push_back(sub); 
	    sub_key.push_back(char(sub));
        }
        return sub_key;
    }

    // morph_cube is the heart of the PRF swapping all 16 bytes through all 9 alphabets
    // The counter supplied gives a fixed starting point per round where key value positions are swapped with the counter
    // After 16 rounds/ bytes of cipher text process all 256 values in all 9 alphabets will have been swapped
    void morph_cube (int counter, string k) {
        int shift, z, y, ke;
        vector< vector<int> >  section_shift;
        for (z=0; z < state.size(); z++) {
	    for (unsigned char key_element : k) {
                for (y=0; y < state[z].size(); y++) {
		    swap(state[z][y][counter], state[z][y][int(key_element)]);
		    ke = key_element;
                }
	    }
	    // The last byte in the key always determines which section of 3 alphabets is to be appended to the end of the state
            shift = int(ke) % size_factor;
            section_shift = state.at(shift);
	    state.erase(state.begin()+shift);
	    state.push_back(section_shift);
        }
    }

    bool keylen_check (string key) {
	int key_length = 16;
        if (key.length() < key_length) {
	    return false;
	}
	else {
	    return true;
	}
    }
    
    // If for some reason we have a small key (ie CubeKDF usage) we pad it with zeros to equal the standard key length
    string padkey (string key) {
        if (key.length() < key_length) {
	    int dif = key_length - key.length();
            for (int x = 0; x < (key_length - key.length()); x++) {
                key.push_back(char(0));
            }   
        }
	return key;
    }

    string encrypt (string data, string key, string nonce = string()) {
	bool kc;
        kc = keylen_check(key);
	if (!kc) {
	    key = padkey(key);
	    cout << "Cube: Warning: Invalid key size. Key was subjected to automatic padding.\n";
	}
	// Generate the Cube State
        gen_cube(size_factor, size_factor, alphabet_size);
	// Key the Cube State with the Key
        key_cube(key);
	// Key the Cube State with the Nonce
        key_cube(nonce);
        int ctr = 0;
        int sub, shift, y, z;
        string sub_key;
        sub_key = key;
        for (unsigned char byte: data) {
            sub = byte;
	    for (z=0; z < state.size(); z++) {
	        for (y=0; y < state[z].size(); y++) {
		    // Lookup current alphabet value for input plain text value recursively through the Cube state
		    sub = state[z][y].at(sub);
		    shift = state[z][y].at(0);
		    // Part of the key to uniformity is by rotating each alphabet one position after each substitution
		    state[z][y].erase(state[z][y].begin()+0);
		    state[z][y].push_back(shift);
	        }
	    }
	    // Generate a new sub_key
	    sub_key = key_scheduler(sub_key);
	    // Execute the PRF 
	    morph_cube(ctr, sub_key);
	    // Write the subsituted byte back onto the input/output buffer
	    data[ctr] = char(sub);
	    // Increment the counter mod 256
            ctr = (ctr + 1) % alphabet_size;
        }
        return data;
    }

   string decrypt (string data, string key, string nonce = string()) {
	bool kc;
        kc = keylen_check(key);
	if (!kc) {
	    key = padkey(key);
	    cout << "Cube: Warning: Invalid key size. Key was subjected to automatic padding.\n";
	}
	// Generate the Cube State
        gen_cube(size_factor, size_factor, alphabet_size);
	// Key the Cube with the Key
        key_cube(key);
	// Key the Cube with the Nonce
        key_cube(nonce);
        int ctr = 0;
        int sub;
        int sub_pos;
        int shift;
        int z;
        int y;
        string sub_key;
        sub_key = key;
        for (unsigned char byte: data) {
            sub = byte;
	    for (z=state.size(); z--> 0;) {
	        for (y=state[z].size(); y --> 0;) {
		    // Lookup cipher text byte by index in the state
		    sub = find(state[z][y].begin(), state[z][y].end(), sub) - state[z][y].begin();
		    shift = state[z][y].at(0);
		    // Same one byte rotation like encryption
		    state[z][y].erase(state[z][y].begin()+0);
		    state[z][y].push_back(shift);
	        }
	    }
	    // Generate a new sub_key
	    sub_key = key_scheduler(sub_key);
	    // Execute the PRF 
	    morph_cube(ctr, sub_key);
	    // Write the subsituted byte back onto the input/output buffer
	    data[ctr] = char(sub);
	    // Increment the counter mod 256
            ctr = (ctr + 1) % alphabet_size;
        }
        return data;
    }
};

// CubeSum - keyed hashing algorithm and key building block to CubeKDF
class CubeSum {
    public:
    string digest (string data, string key = string(), int length = 16) {
	int min_key_length = 16;
        string d;
        int x;
        string iv;
	// if no key is supplied the data is hashed against an all zero data input to Cube.  The input data to CubeSum always becomes the key for Cube and thus it becomes very slow.  This is good for key deriveration.
        if (key.length() < min_key_length) {
	    int dif = min_key_length - key.length();
            for (x = 0; x < (min_key_length - key.length()); x++) {
                key.push_back(char(0));
            }   
        }
        Cube cube;
	// Process the data as the key and the key as the input data and nonce for Cube
        d = cube.encrypt(key, data, key);
        return d;
    }
};

// Key derivation function - 16 byte minimum
// Default is 10 iterations (1 iteration consists of 2 CubeSum calls
class CubeKDF {
    public:
    string genkey (string key, int length, int iterations = 10) {
	int min_key_length = 16;
        string d;
        int x;
        string iv;
        string h;
        CubeSum sum;
	if (key.length() < min_key_length) {
	    int dif = min_key_length - key.length();
	    for (x = 0; x < dif; x++) {
	        key.push_back(char(0));
	    }
	}
	key = sum.digest(key, key, length);
        h = key;
        for (x = 0; x < iterations; x++) {
           h = sum.digest(h, key, length);
        }
        return h;
    }
};

// Cryptographically Secure Random Number Generator
// Initial entropy is supplied by the CSPRNG arc4random (only implementations of arc4random that use ChaCha20 or AES should be used)
// arc4random seeds 16 bytes for the Cube's nonce, 16 bytes to feed CubeKDF which becomes the key used to generate the random bytes and lastly 16 bytes as input data to Cube
class CubeRandom {
    public:
    string random (int length) {
	long x;
	int keylen = 16;
	int noncelen = 16;
	int iterations = 10;
        char seed[length];
        char seedk[keylen];
        char seedn[noncelen];
	Cube cube;
	CubeKDF kdf;
	string data, k, rand, key, nonce;
        arc4random_buf(seed, length);
        arc4random_buf(seedk, keylen);
        arc4random_buf(seedn, noncelen);
        for (x = 0; x < length; x++) {
            data.push_back(static_cast<unsigned char>(seed[x]));
        }
        for (x = 0; x < keylen; x++) {
            k.push_back(static_cast<unsigned char>(seedk[x]));
        }
        for (x = 0; x < noncelen; x++) {
            nonce.push_back(static_cast<unsigned char>(seedn[x]));
        }
	key = kdf.genkey(k, keylen, iterations);
	data = cube.encrypt(data, key, nonce);
        return data;
    }
};
