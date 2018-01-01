#include "cube.h"
#include <string>
#include <iostream>
int hash_length = 256;

string digest (string data) {
    string d;
    int x;
    string iv;
    for (x = 0; x < (hash_length / 8); x++) {
	    iv.push_back(char(0));
    }
    d = cube_encrypt(iv, iv, data);
    return d;
}
