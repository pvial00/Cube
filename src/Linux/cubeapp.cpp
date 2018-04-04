#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <utility>
#include <algorithm>
#include <iomanip>
#include "cubecrypt.cpp"
#include <openssl/hmac.h>

char version[] = "1.1.0";
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
    cout << "Cube v" << version << " *** Cube ciphering machine ***\n\n";
    cout << "Warning: This machine is designed to work as is, without modification.\n";
    cout << "Use at your at own risk!\n";
}
void usage() {
    desc();
    cout << "Author: pvial@kryptomagik.com\n\n";
    cout << "-e <input file> <output file> <password> (Encrypt)\n";
    cout << "-d <input file> <output file> <password> (Decrypt)\n";
    cout << "-emac <input file> <output file> <password> (Encrypt with HMAC)\n";
    cout << "-dmac <input file> <output file> <password> (Decrypt with HMAC)\n";
    cout << "-sum <input file>\n";
    cout << "-kdf <password> <optional keylength in bytes> <optional # of iterations>\n";
    cout << "-random <optional number of bytes default is 1 byte>\n";
}
void kdfusage() {
    desc();
    cout << "Author: pvial@kryptomagik.com\n";
    cout << "Usage: cubecrypt <encrypt/decrypt> <input file> <output file> <password>" << "\n";
}
void sumusage() {
    desc();
    cout << "Author: pvial@kryptomagik.com\n";
    cout << "Usage: cubecrypt <encrypt/decrypt> <input file> <output file> <password>" << "\n";
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
	nonce = rand.random(nonce_length);
    	cube.encrypt(in, out, key, nonce, buffersize);
        return 0;
    }
    else if (mode == "-emac") {
        if (argc < 5) {
            usage();
	    exit(EXIT_FAILURE);
        }
        in = argv[2];
        out = argv[3];
        key = argv[4];
        key = kdf.genkey(key, keylen, iterations);
	nonce = rand.random(nonce_length);
    	cube.encrypt(in, out, key, nonce, buffersize);
        infile.open(out.c_str(), std::ios::binary);
	if(infile.is_open()) {
            infile.seekg(0, ios::end);
            int fsize = infile.tellg();
            infile.seekg(0, ios::beg);
	    for (int i = 0; i < fsize; i++) {
	        b = infile.get();
		data.push_back(b);
	    }
            infile.close();
	    mac = HMAC(EVP_sha256(), key.c_str(), key.length(), reinterpret_cast<const unsigned char *>(data.c_str()), data.length(), NULL, NULL);
	    string mm(reinterpret_cast<char*>(mac));
            outfile.open(out.c_str(), std::ios::app|std::ios::binary);
            outfile << mm;
            cout << mm;
	    outfile.close();
	}
	else {
            file_missing();
	    exit(EXIT_FAILURE);
	}
    }
    else if (mode == "-d") {
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
    else if (mode == "-dmac") {
        if (argc < 5) {
            usage();
	    exit(EXIT_FAILURE);
        }
        in = argv[2];
        out = argv[3];
        key = argv[4];
        key = kdf.genkey(key, keylen, iterations);
        infile.open(in.c_str(), std::ios::binary);
	if(infile.is_open()) {
            infile.seekg(0, ios::end);
            int fsize = infile.tellg();
            infile.seekg(0, ios::beg);
	    if (fsize < (nonce_length + mac_length + 1)) {
	        cout << "Error:  File size failure\n";
	        exit(2);
	    }
	    for (int i = 0; i < fsize; i++) {
	        b = infile.get();
		data.push_back(b);
	    }
	    infile.close();
	    string m, ctxt;
	    m = data.substr((data.length() - mac_length), mac_length);
	    ctxt = data.substr(0, (data.length() - mac_length));
	    data.clear();
	    mac = HMAC(EVP_sha256(), key.c_str(), key.length(), reinterpret_cast<const unsigned char *>(ctxt.c_str()), ctxt.length(), NULL, NULL);
	    string m2(reinterpret_cast<char*>(mac));
	    if (m2.compare(m) == 0) {
		outfile.open(in.c_str(), std::ios::binary);
		outfile << ctxt;
		outfile.close();
	        cube.decrypt(in, out, key, nonce_length, buffersize);
	    }
	    else {
                cout << "MAC failed: message has been tampered with." << "\n";
	        exit(1);
	    }
	}
	else {
	    file_missing();
	    exit(EXIT_FAILURE);
	}
    }
    else if(mode == "-kdf") {
        if (argc < 3) {
            kdfusage();
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
            int i = int(k);
            if (i <= 15) {
                    cout << setfill('0');
                    cout << std::hex << setw(2) << i;
                }
                else {
                    cout << std::hex << i;
                }
	}
	cout << "\n";
	return 0;
    }
    else if(mode == "-sum") {
	string digest;
	int i;
	unsigned char b;
	if (argc < 3) {
	    sumusage();
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
                int i = int(d);
                if (i <= 15) {
                    cout << setfill('0');
                    cout << std::hex << setw(2) << i;
                }
                else {
                    cout << std::hex << i;
                }
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
	data = rand.random(numbytes);
	for (unsigned char b: data) {
	    cout << b;
	}
	return 0;
    }
    else if(mode == "-v") {
       cout << version << "\n";
    }
}
