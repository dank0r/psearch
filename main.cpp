#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <thread>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;
using std::cout;
using std::endl;
using std::cerr;
using std::string;
using std::vector;
using std::tuple;
using std::make_tuple;
using std::get;
using std::thread;
using std::getline;
using std::ifstream;

void add_strings(string &file_path, vector<tuple<string, int, string> > &strings) {
	ifstream in(file_path.c_str());
	if(!in) {
		cerr << "Cannot open file " << file_path << endl;
	}
	string str;
	int i = 0;
	while(getline(in, str)) {
		i++;
		if(str.size() > 0) {
			strings.push_back(make_tuple(str, i, file_path));
		}
	}
}

bool search(string &str, string &substr) {
	return str.find(substr) != string::npos;
}

int main() {
	int n_threads = 4;
	string dir_path = "/home/dank0r/Desktop/text";
	string substr = "the";
	vector<tuple<string, int, string> > strings;
	vector<thread> threads(n_threads);
	
	for(auto &p : fs::recursive_directory_iterator(dir_path)) {
		if(fs::is_regular_file(p)) {
			string file_path = p.path().string();
			add_strings(file_path, strings);
		}
	}
	for(auto &occ : strings) {
		if(search(get<0>(occ), substr)) {
			cout << "Found occurrence in file " << get<2>(occ) << ":" << endl;
			cout << get<1>(occ) << ": " << get<0>(occ) << endl;
		}
	}
	
}
