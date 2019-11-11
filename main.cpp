#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <thread>
#include <filesystem>
#include <fstream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

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

struct line {
	char *begin;
	char *end;
	int file_id;
};

struct file {
	int id;
	string path;
	char *buf;
	int fd;
	size_t fs;
};

void add_lines(file &f, vector<line> &lines) {
	f.fd = open(f.path.c_str(), O_RDONLY);
	if(f.fd == -1) {
		cerr << "Cannot open file " << f.path << endl;
	}
	struct stat fs;
	if(fstat(f.fd, &fs) == -1) {
		cerr << "Cannot open file " << f.path << endl;
		close(f.fd);
	}
	f.fs = static_cast<size_t>(fs.st_size);

	//place for madvise and posix_fadvise
	
	f.buf = static_cast<char *>(mmap(NULL, f.fs, PROT_READ, MAP_SHARED, f.fd, 0));
	if(f.buf == MAP_FAILED) {
		cerr << "Cannot memory map file " << f.path << endl;
		close(f.fd);
	}
	char *buf_end = f.buf + fs.st_size;
	char *begin = f.buf;
	char *end = NULL;
	while((end = static_cast<char *>(memchr(begin, '\n', static_cast<size_t>(buf_end - begin)))) != NULL) {
		line l {begin, end, f.id};
		lines.push_back(l);
		if(end != buf_end) {
			begin = end + 1;
		} else {
			break;
		}
	}
	close(f.fd);
}

int main() {
	int n_threads = 4;
	string dir_path = "/home/dank0r/Desktop/text";
	string substr = "the";
	vector<line> lines;
	vector<thread> threads(n_threads);
	vector<file> files;
	for(auto &p : fs::recursive_directory_iterator(dir_path)) {
		if(fs::is_regular_file(p)) {
			string file_path = p.path().string();
			int id = files.size();
			file f {id, file_path};
			files.push_back(f);
			add_lines(f, lines);
		}
	}
	for(auto &occ : lines) {
		for(auto it = occ.begin; it != occ.end; it++) {
			cout << *it;
		}
		cout << endl;
	}
	for(auto &f : files) {
		munmap(f.buf, f.fs);
	}
	
}
