#include "KMP.h"
#include <vector>
#include <string>
#include <iostream>
#include <utility>
#include <string.h>
#include <mutex>

using std::cout;
using std::endl;
using std::vector;
using std::string;
using std::pair;
using std::make_pair;
using std::mutex;
using callback = void (*)(unsigned long long, char*, size_t, size_t, string, mutex&);

//prefix function returns number of node (which btw is length of the word contained in node)
int KMP::pr_func(string str) {
	int result = 0;
	for(int i = 1; i < str.length(); i++) {
		if(str.find(&str[str.length() - i]) == 0) {
			result = i;
		}
	}
	//if there are no non-zero prefixes return empty word (node #0)
	return result;
}

//builds KMP finite-state machine
KMP::KMP(string str) {
	state = 0;
	substr = str;
	n_nodes = str.length() + 1;
	end_state = n_nodes - 1;

	//initializing transition function
	for(int i = 0; i < n_nodes; i++) {
		tr_f.push_back(vector<int>(128, -1));
	}

	//filling transition function
	for(int i = 0; i < n_nodes - 1; i++) {
		tr_f[i][str[i]] = i + 1;
	}
	//if there is a '\n' symbol on the strip, return to start state
	for(int i = 0; i < n_nodes; i++) {
		tr_f[i][10] = 0;
	}
	for(int c = 0; c < 128; c++) {
		for(int i = 0; i < n_nodes; i++) {
			if(i != n_nodes - 1 && c == str[i]) {
				continue;
			}
			string word = str.substr(0, i) + string(1, static_cast<char>(c));
			tr_f[i][c] = pr_func(word);
				
		}
	}
}

//prints transition function
void KMP::print() {
	for(int i = 0; i < n_nodes; i++) {
		for(int j = 0; j < 128; j++) {
			cout << tr_f[i][j] << " ";
		}
		cout << endl;
	}
}

//if substring has been found returns position of first occurence in input string, if not found returns -1
int KMP::find(char *begin, char *end, callback found, string path, mutex &print_m) {
	char *symbol = begin;
	unsigned long long pos = 0;
	//number of line
	unsigned long long nol = 1;
	char *line_begin = begin;
	while(symbol != end) {
		if(symbol[0] == '\n') {
			nol++;
			line_begin = symbol + 1;
			pos = 0;
		} else if(state == end_state) {
			char *line_end = static_cast<char *>(memchr(symbol, '\n', static_cast<size_t>(end - symbol)));
			size_t line_length = static_cast<size_t>(line_end - line_begin);
			found(nol, line_begin, line_length, pos - substr.length(), path, print_m);
		}
		//cout << "processing " << symbol[0] << endl;
		state = tr_f[state][symbol[0]];
		symbol++;
		pos++;
	}

	bool isAccepted = (state == end_state) && symbol[0] != '\n';
	state = 0;
	return isAccepted ? pos : -1;
}

void KMP::reset() {
	state = 0;
}
