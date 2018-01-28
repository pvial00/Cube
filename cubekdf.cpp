#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <utility>
#include <algorithm>
#include "cube.cpp"

int iterations = 10;
int keylen = 128;

using namespace std;

void usage() {
    cout << "CubeKDF - A Key Derivation Function based on the Cube256 Cipher\n";
    cout << "---------------------------------------------------------\n";
    cout << "Usage: cubekdf <password> <optional key length> <optional number of iterations" << "\n";
}

int main(int argc, char** argv) {
    string key, digest;
    unsigned char b;
    if (argc < 2) {
        usage();
	exit(EXIT_FAILURE);
    }
    key = argv[1];
    if (argc >= 3) {
        keylen = atoi(argv[2]);
    }
    if (argc  == 4) {
        iterations = atoi(argv[3]);
    }
    int i;
    CubeKDF kdf;
    key = kdf.genkey(key, keylen, iterations);
    for (unsigned char c: key) {
        cout << std::hex << static_cast<unsigned int>(c);
    }
    cout << "\n";
    return 0;
}
