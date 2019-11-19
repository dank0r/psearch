#include <vector>
#include <string>
#include <iostream>
#include <mutex>

using std::cout;
using std::endl;
using std::vector;
using std::string;
using std::mutex;
using callback = void (*)(unsigned long long, char*, size_t, size_t, string, mutex&, string);


class KMP {
	private:
		//state-trinsition function
		vector<vector<int> > tr_f;
                //number of nodes (states)
		int n_nodes;
		//current state
                int state;
		//end (final) state
                int end_state;
		//substring to find
		string substr;
		
		//prefix function returns number of node (which btw is length of the word contained in node)
		int pr_func(string str);
	public:
		//builds KMP finite-state machine
		KMP(string str);

		//prints transition function
		void print();

		//if substring has been found returns position of first occurence in input string, if not found returns -1
		int find(char *begin, char *end, callback found, string path, mutex &print_m);

		void reset();
};
