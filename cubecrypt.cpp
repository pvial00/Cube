#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <utility>
#include <algorithm>
#include "cube.cpp"
#include <openssl/hmac.h>

int iterations = 10;
int keylen = 128;
int nonce_length = 16;
int mac_length = 32;

using namespace std;

void usage() {
    cout << "CubeCrypt v0.6 - File encryption utilizing the Cube256 cipher, nonce generation with CubeRandom and HMAC with OpenSSL SHA256 HMAC\n";
    cout << "Author: pvial@kryptomagik.com\n";
    cout << "Usage: cubecrypt <encrypt/decrypt> <input file> <output file> <password>" << "\n";
}

int main(int argc, char** argv) {
    ifstream infile;
    ofstream outfile;
    string mode, in, out, key, msg, nonce, data;
    unsigned char b;
    if (argc < 5) {
        usage();
	exit(EXIT_FAILURE);
    }
    mode = argv[1];
    in = argv[2];
    out = argv[3];
    key = argv[4];
    string random_dev = "/dev/urandom";
    int i;
    infile.open(in.c_str(), std::ios::binary);
    infile.seekg(0, ios::end);
    int fsize = infile.tellg();
    infile.close();
    infile.open(in.c_str(), std::ios::binary);
    for (i = 0; i < fsize; i++) {
        b = infile.get();
	data.push_back(b);
    }
    infile.close();
    string c;
    unsigned char *mac;
    CubeKDF kdf;
    key = kdf.genkey(key, keylen, iterations);
    Cube cube;
    int s;
    if (mode == "encrypt") {
	CubeRandom rand;
	nonce = rand.random(nonce_length);
    	data = cube.encrypt(data, key, nonce);
        string ctxt = nonce + data;
	mac = HMAC(EVP_sha256(), key.c_str(), key.length(), reinterpret_cast<const unsigned char *>(ctxt.c_str()), ctxt.length(), NULL, NULL);
        outfile.open(out.c_str());
        outfile << mac;
        outfile << nonce;
        outfile << data;
	outfile.close();
    }
    else if (mode == "decrypt") {
	string m;
	m = data.substr(0, mac_length);
	nonce = data.substr(mac_length, nonce_length);
	msg = data.substr(mac_length+nonce_length, (data.length() - (nonce_length + mac_length)));
        data.clear();
        string ctxt = nonce + msg;
	mac = HMAC(EVP_sha256(), key.c_str(), key.length(), reinterpret_cast<const unsigned char *>(ctxt.c_str()), ctxt.length(), NULL, NULL);
	string m2(reinterpret_cast<char*>(mac));
	if (m2.compare(m) == 0) {
	    c = cube.decrypt(msg, key, nonce);
            outfile.open(out.c_str());
            outfile << c;
            outfile.close();
	}
	else {
            cout << "MAC failed: message has been tampered with." << "\n";
	    exit(1);
	}
    return 0;
    }
}
