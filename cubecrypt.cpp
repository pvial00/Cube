#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <utility>
#include <algorithm>
#include "cubeclass.h"

int iterations = 10;
int keylen = 128;
int nonce_length = 16;

using namespace std;

int main(int argc, char** argv) {
    ifstream infile;
    ofstream outfile;
    string mode, in, out, key, msg, nonce, data;
    unsigned char b;
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
    CubeKDF kdf;
    key = kdf.genkey(key, keylen, iterations);
    Cube cube;
    if (mode == "encrypt") {
	CubeRandom rand;
	nonce = rand.random(nonce_length);
    	c = cube.encrypt(data, key, nonce);
        outfile.open(out.c_str());
        outfile << nonce;
        outfile << c;
	outfile.close();
    }
    else if (mode == "decrypt") {
	nonce = data.substr(0, nonce_length);
	msg = data.substr(nonce_length, (data.length() - nonce_length));
	c = cube.decrypt(msg, key, nonce);
        outfile.open(out.c_str());
        outfile << c;
        outfile.close();
    }
    return 0;
}
