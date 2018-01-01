#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <utility>
#include <algorithm>

using namespace std;
vector< vector< vector<int> > > state;
int size_factor = 3;
int alphabet_size = 256;

void gen_cube (int depth, int width, int length) {
    for (int z=0; z < depth; z++) {
        vector< vector<int> > section;
        for (int y=0; y < width; y++) {
            vector<int> alphabet;
            for (int x=0; x < length; x++) {
                alphabet.push_back(x);
            }
            for (int mod=0; mod < y; mod++) {
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

void key_cube (string key) {
    int z;
    int x;
    int y;
    int k;
    int shuffle;
    int key_sub;
    int sized_pos;
    for (z=0; z < state.size(); z++) {
        for (unsigned char k: key) {
            for (x=0; x < state[z].size(); x++) {
                key_sub = state[z][x].at(int(k));
		state[z][x].erase(state[z][x].begin()+int(k));
                state[z][x].push_back(key_sub);
                for (y=0; y < int(k); y++) {
                    if (y % 2 == 0) {
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

string key_scheduler (string key) {
    int x;
    int sized_pos;
    int sub;
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

void morph_cube (int counter, string k) {
    int mod_value;
    int shift;
    int z;
    int y;
    int ke;
    vector< vector<int> >  section_shift;
    mod_value = counter % alphabet_size;
    for (z=0; z < state.size(); z++) {
	for (unsigned char key_element : k) {
            for (y=0; y < state[z].size(); y++) {
		    swap(state[z][y][mod_value], state[z][y][int(key_element)]);
		    ke = key_element;
            }
	}
        shift = int(ke) % size_factor;
        section_shift = state.at(shift);
	state.erase(state.begin()+shift);
	state.push_back(section_shift);
    }
}

string encrypt (string data, string key) {
    int ctr = 0;
    int sub;
    int sub_pos;
    int shift;
    int z;
    int y;
    string cipher_text;
    string sub_key;
    sub_key = key;
    for (unsigned char byte: data) {
        sub = byte;
	for (z=0; z < state.size(); z++) {
	    for (y=0; y < state[z].size(); y++) {
                sub_pos = sub;
		sub = state[z][y].at(sub_pos);
		shift = state[z][y].at(0);
		state[z][y].erase(state[z][y].begin()+0);
		state[z][y].push_back(shift);
	    }
	}
	sub_key = key_scheduler(sub_key);
	morph_cube(ctr, sub_key);
	cipher_text.push_back(char(sub));
        ctr++;
    }
    return cipher_text;
}

string decrypt (string data, string key) {
    int ctr = 0;
    int sub;
    int sub_pos;
    int shift;
    int z;
    int y;
    string cipher_text;
    string sub_key;
    sub_key = key;
    for (unsigned char byte: data) {
        sub = byte;
	for (z=state.size(); z--> 0;) {
	    for (y=state[z].size(); y --> 0;) {
		sub = find(state[z][y].begin(), state[z][y].end(), sub) - state[z][y].begin();
		shift = state[z][y].at(0);
		state[z][y].erase(state[z][y].begin()+0);
		state[z][y].push_back(shift);
	    }
	}
	sub_key = key_scheduler(sub_key);
	morph_cube(ctr, sub_key);
	cipher_text.push_back(char(sub));
        ctr++;
    }
    return cipher_text;
}

string cube_encrypt (string data, string key, string nonce = string()) {
    string c;
    gen_cube(size_factor, size_factor, alphabet_size);
    key_cube(key);
    key_cube(nonce);
    c = encrypt(data, key);
    return c;
}

string cube_decrypt (string data, string key, string nonce = string()) {
    string c;
    gen_cube(size_factor, size_factor, alphabet_size);
    key_cube(key);
    key_cube(nonce);
    c = decrypt(data, key);
    return c;
}
