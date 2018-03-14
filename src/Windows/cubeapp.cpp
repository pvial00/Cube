#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <utility>
#include <algorithm>
#include "cubecrypt.cpp"

int iterations = 10;
int keylen = 16;
int nonce_length = 16;
int mac_length = 32;
int buffersize = 100;
int hashlen = 16;
int numbytes = 1;
int seedlength = keylen;
int keylength = 16;

void desc() {
    cout << "Cube v1.0 *** Cube ciphering machine ***\n\n";
    cout << "Warning: This machine is designed to work as is, without modification.\n";
    cout << "Use at your at own risk!\n";
}
void usage() {
    desc();
    cout << "Author: pvial@kryptomagik.com\n\n";
    cout << "-e <input file> <output file> <password> (Encrypt)\n";
    cout << "-d <input file> <output file> <password> (Decrypt)\n";
    cout << "-sum <input file>\n";
    cout << "-kdf <password> <optional keylength in bytes> <optional # of iterations>\n";
    cout << "-random <optional number of bytes default is 1 byte>\n";
}

void file_missing() {
    cout << "Error: input file missing.\n";
}

int main(int argc, char** argv) {
    ifstream infile;
    ofstream outfile;
    string in, out, key, msg, nonce, data, mode;
    unsigned char b;
    int i;
    string c;
    unsigned char *mac;
    CubeKDF kdf;
    CubeSum cubesum;
    CubeCrypt cube;
    CubeRandom rand;
    int s;
    if (argc >= 2) {
        mode = argv[1];
	if (!isatty(STDIN_FILENO)) {
	    key = argv[2];
	    in = "0";
	    out = "0";
	}
    }
    else {
        usage();
	exit(EXIT_FAILURE);
    }

    if (mode == "-e") {
	if (isatty(STDIN_FILENO)) {
            if (argc < 5) {
                usage();
	        exit(EXIT_FAILURE);
            }
            in = argv[2];
            out = argv[3];
            key = argv[4];
        }
        key = kdf.genkey(key, keylen, iterations);
	nonce = rand.random(nonce_length, nonce_length, nonce_length, iterations);
    	cube.encrypt(in, out, key, nonce, buffersize);
        return 0;
    }
    else if (mode == "-d") {
        /*if (argc < 5) {
            usage();
	    exit(EXIT_FAILURE);
        }*/
	if (isatty(STDIN_FILENO)) {
            in = argv[2];
            out = argv[3];
            key = argv[4];
	}
        key = kdf.genkey(key, keylen, iterations);
	if (isatty(STDIN_FILENO)) {
            infile.open(in.c_str(), std::ios::binary);
	    if(infile.is_open()) {
                infile.seekg(0, ios::end);
                int fsize = infile.tellg();
	        infile.close();
	        if (fsize < (nonce_length + 1)) {
	            cout << "Error:  File size failure\n";
	            exit(2);
                }
	    }
	    else {
		file_missing();
		exit(EXIT_FAILURE);
	    }
	}
	cube.decrypt(in, out, key, nonce_length, buffersize);
	return 0;
    }
    else if(mode == "-kdf") {
        if (argc < 3) {
            usage();
	    exit(EXIT_FAILURE);
        }

	if(argc >= 3) {
            key = argv[2];
	}
	if(argc >= 4) {
	    keylen = atoi(argv[3]);
	}
	if(argc >= 5) {
	    iterations = atoi(argv[4]);
	}
	key = kdf.genkey(key, keylen, iterations);
	for (unsigned char k: key) {
	    cout << std::hex << static_cast<unsigned int>(k);
	}
	cout << "\n";
	return 0;
    }
    else if(mode == "-sum") {
	string digest;
	int i;
	unsigned char b;
	if (argc < 3) {
	    usage();
	    exit(EXIT_FAILURE);
	}
	else {
	    in = argv[2];
	}
	if (argc >= 4) {
	    hashlen = atoi(argv[3]);
	}
	infile.open(in.c_str(), std::ios::binary);
	if(infile.is_open()) {
	    infile.seekg(0, ios::end);
	    int fsize = infile.tellg();
	    infile.close();
	    infile.open(in.c_str(), std::ios::binary);
	    for (int i = 0; i < fsize; i++) {
	        b = infile.get();
	        data.push_back(b);
	    }
	    infile.close();
	    digest = cubesum.digest(data, string(), hashlen);
	    cout << in << ": ";
	    for (unsigned char d: digest) {
	        cout << std::hex << static_cast<unsigned int>(d);
	    }
	    cout << "\n";
	    return 0;
	}
	else {
	    file_missing();
	    exit(EXIT_FAILURE);
	}
    }
    else if(mode == "-random") {
	if (argc == 3) {
	    numbytes = atoi(argv[2]);
	}
	else {
            numbytes = 1;
	}
	data = rand.random(numbytes, seedlength, keylen, iterations);
	for (unsigned char b: data) {
	    cout << b;
	}
	return 0;
    }
}
