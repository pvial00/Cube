#include <string>
#include <iostream>
#include "cubehash.h"

using namespace std;

string genkey (string key, int length, int iterations) {
    string d;
    int x;
    string iv;
    string h;
    h = key;
    for (x = 0; x < iterations; x++) {
	    h = digest(h, length);
    }
    return h;
}
