#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <utility>
#include <algorithm>
#include <unistd.h>
#include "cube.cpp"

using namespace std;

class CubeCrypt {
    public:
    int size_factor = 3;
    int alphabet_size = 256;
    unsigned long long fsize = 0;
    vector< vector< vector<int> > > state;
    ifstream infile;
    ofstream outfile;
    unsigned long max_stdin_buf = 4294967295;

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
        int shift;
        int z;
        int y;
        int ke;
        vector< vector<int> >  section_shift;
        for (z=0; z < state.size(); z++) {
	    for (unsigned char key_element : k) {
                for (y=0; y < state[z].size(); y++) {
		    swap(state[z][y][counter], state[z][y][int(key_element)]);
		    ke = key_element;
                }
	    }
            shift = int(ke) % size_factor;
            section_shift = state.at(shift);
	    state.erase(state.begin()+shift);
	    state.push_back(section_shift);
        }
    }

    void encrypt (string input, string output, string key, string nonce = string(), int blocksize = 100) {
	int stdin_waiting = 0;
        int ctr = 0;
        int sub, sub_pos, shift, z, y;
        string sub_key, ctxt;
        sub_key = key;
        int extrablock = 0;
        int extra = 0;
        int blocks = 0;
	if (!isatty(STDIN_FILENO)) {
	    cout << nonce;
	    fsize = max_stdin_buf;
	    stdin_waiting = 1;
            blocksize = 1;
	}
	else {
            infile.open(input.c_str(), std::ios::binary);
	    if(infile.is_open()) {
                infile.seekg(0, ios::end);
		fsize = infile.tellg();
                infile.close();
                infile.open(input.c_str(), std::ios::binary);
                outfile.open(output.c_str(), std::ios::binary);
                outfile << nonce;
                if (fsize <= blocksize) {
                    blocks = 1;
                    blocksize = fsize;
                }
                else {
                    blocks = (fsize / blocksize);
                    if ((fsize % blocksize) != 0) {
                        extra = fsize % blocksize;
                        blocks++;
                    }
                }
	    }
	    else {
                cout << "Error: Input file is missing.\n";
		exit(EXIT_FAILURE);
	    }
	}
        char buf[blocksize];
        gen_cube(size_factor, size_factor, alphabet_size);
        key_cube(key);
        key_cube(nonce);
	if (stdin_waiting == 0) {
            for (int b = 0; b < blocks; b++) {
                if ((extra > 0) && (b == (blocks - 1))) {
                    blocksize = extra;
                }
                infile.read(buf, blocksize);
                for (int i = 0; i < blocksize; i++) {
                    sub = static_cast<unsigned char>(buf[i]);
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
                    ctxt.push_back(char(sub));
                    ctr = (ctr + 1) % alphabet_size;
                }
                outfile << ctxt;
                ctxt.clear();
            } 
        infile.close();
        outfile.close();
	}
	else if (stdin_waiting == 1) {
	    blocksize = 1;
            while (cin) {
	        if (cin) {
	            cin.read(buf, blocksize);
                    for (unsigned char byte: buf) {
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
                        ctxt.push_back(char(sub));
                        ctr = (ctr + 1) % alphabet_size;
                    }
		    if (!cin.eof()) {
	                cout << ctxt;
	            }
                    ctxt.clear();
		}
            }
        }
    }

   void decrypt (string input, string output, string key, int nonce_length = 16, int blocksize = 100) {
	int stdin_waiting = 0;
        int ctr = 0;
        int sub, shift, sub_pos, z, y;
        string sub_key, ptxt;
        sub_key = key;
        int extrablock = 0;
        int extra = 0;
        int blocks = 0;
        char nonce_buf[nonce_length];
	if (!isatty(STDIN_FILENO)) {
	    fsize = max_stdin_buf;
	    cin.read(nonce_buf, nonce_length);
	    stdin_waiting = 1;
            blocksize = 1;
	}
	else {
            infile.open(input.c_str(), std::ios::binary);
	    if (infile.is_open()) {
                infile.seekg(0, ios::end);
                fsize = infile.tellg();
                fsize = fsize - nonce_length;
                infile.close();
                infile.open(input.c_str(), std::ios::binary);
                infile.read(nonce_buf, nonce_length);
                outfile.open(output.c_str(), std::ios::binary);
                if (fsize <= blocksize) {
                    blocks = 1;
                    blocksize = fsize;
                }
                else {
                    blocks = (fsize / blocksize);
                    if ((fsize % blocksize) != 0) {
                        blocks++;
                        extra = fsize % blocksize;
                    }
                }
	    }
	    else {
                cout << "Error: Input file missing.";
		exit(EXIT_FAILURE);
	    }
	}
	string n(nonce_buf);
        string nonce = n.substr(0, nonce_length);
        gen_cube(size_factor, size_factor, alphabet_size);
        key_cube(key);
	key_cube(nonce);
        char buf[blocksize];
	if (stdin_waiting == 0) {
            for (int b = 0; b < blocks; b++) {
                if ((extra > 0) && (b == (blocks - 1))) {
                    blocksize = extra;
                }
                infile.read(buf, blocksize);
                for (int i = 0; i < blocksize; i++) {
                    sub = static_cast<unsigned char>(buf[i]);
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
                    ptxt.push_back(char(sub));
                    ctr = (ctr + 1) % alphabet_size;
                }
                outfile << ptxt;
                ptxt.clear();
            }
            infile.close();
            outfile.close();
        }
	else if (stdin_waiting == 1) {
	    blocksize = 1;
            while (cin) {
		if (cin) {
                    cin.read(buf, blocksize);
                    for (unsigned char byte: buf) {
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
                        ptxt.push_back(char(sub));
                        ctr = (ctr + 1) % alphabet_size;
                    }
		    if(!cin.eof()) {
	                cout << ptxt;
                        ptxt.clear();
	            }
                    else { break; }
		}
            }
       }
    }
};
