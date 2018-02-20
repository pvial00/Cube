#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <utility>
#include <algorithm>
#include "cube.cpp"

int hashlen = 128;

using namespace std;

void usage() {
	    cout << "CubeSum - A slow hash algorithm based on the Cube256 Cipher" << "\n";
	    cout << "---------------------------------------------------------\n";
	    cout << "***  Hash length defaults to 128 bits\n\n";
	    cout << "Usage: cubeh <input file> <optional hash length in bits>" << "\n";
}

int main(int argc, char** argv) {
    ifstream infile;
    string in, data, digest;
    unsigned char b;
    if (argc < 2) {
        usage();
	exit(EXIT_FAILURE);
    }
    in = argv[1];
    if (argc >= 3) {
        hashlen = atoi(argv[2]);
    }
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
    CubeSum hash;
    digest = hash.digest(data, string(), hashlen);
    cout << in << ": ";
    for (unsigned char c: digest) {
        cout << std::hex << static_cast<unsigned int>(c);
    }
    cout << "\n";
    return 0;
}
