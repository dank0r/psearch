#include "KMP.h"
#include <vector>
#include <string>
#include <iostream>

using std::cout;
using std::endl;
using std::vector;
using std::string;


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
	for(int c = 32; c < 128; c++) {
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
int KMP::find(char *begin, char *end) {
	char *symbol = begin;
	int i = 0;
	while(symbol != end && state != end_state) {
		state = tr_f[state][symbol[0]];
		symbol++;
		i++;
	}

	bool isAccepted = (state == end_state);
	state = 0;
	return isAccepted ? i - substr.length() : -1;
}
