#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <utility>
#include <algorithm>
#include "cube.h"

using namespace std;

int main(int argc, char** argv) {
    ifstream infile;
    ofstream outfile;
    string mode;
    string data;
    string in;
    string out;
    string key;
    unsigned char b;
    mode = argv[1];
    in = argv[2];
    out = argv[3];
    key = argv[4];
    infile.open(in.c_str(), std::ios::binary);
    while (!infile.eof()) {
	    data.append(1, infile.get());
    }
    infile.close();
    string c;
    if (mode == "encrypt") {
    	c = cube_encrypt(data, key);
    }
    else if (mode == "decrypt") {
	c = cube_decrypt(data, key);
    }
    outfile.open(out.c_str());
    outfile << c;
    outfile.close();
}
