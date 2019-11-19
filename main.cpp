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
#include <mutex>
#include "KMP.h"

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
using std::mutex;

struct file {
	int id;
	string path;
	char *buf;
	int fd;
	size_t fs;
};

void add_file(file &f) {
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
	//posix_fadvise(f.fd, 0, 0, POSIX_FADV_NORMAL);

	f.buf = static_cast<char *>(mmap(NULL, f.fs, PROT_READ, MAP_SHARED, f.fd, 0));
	if(f.buf == MAP_FAILED) {
		cerr << "Cannot memory map file " << f.path << endl;
		close(f.fd);
	}

	close(f.fd);
}

void found(unsigned long long nol, char *line, size_t length, size_t pos, string path, mutex &print_m) {
	print_m.lock();
	cout << "Found occurence in file " << path << endl << "at line " << nol << " (position #" << pos << ")" << ":" << endl;
	for(size_t i = 0; i < length; i++) {
		cout << line[i];
	}
	cout << endl;
	print_m.unlock();
}

void thr(vector<file> &files, string substr, mutex &print_m) {
	KMP fsm(substr);
	for(auto &f : files) {
		fsm.find(f.buf, static_cast<char *>(f.buf + f.fs), found, f.path, print_m);
	}
}

int main(int argc, char *argv[]) {
	int n_threads = 1;
	bool is_current_path = true;
	string dir_path = fs::current_path();
	bool is_recursive = true;
	string substr = "";
	for(int i = 1; i < argc; i++) {
		if(strncmp(argv[i], "-t", 2) == 0) {
			n_threads = atoi(argv[i] + 2);
		} else if(argv[i][0] == '/') {
			dir_path = argv[i];
			is_current_path = false;
		} else if(strcmp(argv[i], "-n") == 0) {
			is_recursive = false;
		} else {
			substr = argv[i];
		}
	}
	cout << "threads: " << n_threads << endl;
	cout << "dir_path: " << dir_path << endl;
	cout << "is_recursive: " << is_recursive << endl;
	cout << "substr: " << substr << endl;
	KMP fsm(substr);
	vector<thread> threads;
	vector<file> files;
	mutex print_m;

	if(is_recursive) {
		for(auto &p : fs::recursive_directory_iterator(dir_path)) {
			if(fs::is_regular_file(p)) {
				if(is_current_path) {
	                                char exe_path[1024];
	                                int len = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
	                                if(len == -1) {
	                                        cerr << "Can't read name of executable" << endl;
	                                        return 1;
	                                }
	                                exe_path[len] = 0;
                        	        if(strcmp(p.path().string().c_str(), exe_path) != 0) {
                     	                   string file_path = p.path().string();
                                	        int id = files.size();
                        	                file f {id, file_path};
                	                        add_file(f);
        	                                files.push_back(f);
	                                }
				} else {
                                        string file_path = p.path().string();
                                        int id = files.size();
                                        file f {id, file_path};
                                        add_file(f);
                                        files.push_back(f);
				}
			}
		}
	} else {
                for(auto &p : fs::directory_iterator(dir_path)) {
                        if(fs::is_regular_file(p)) {
                                if(is_current_path) {
                                        char exe_path[1024];
                                        int len = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
                                        if(len == -1) {
                                                cerr << "Can't read name of executable" << endl;
                                                return 1;
                                        }
                                        exe_path[len] = 0;
                                        if(strcmp(p.path().string().c_str(), exe_path) != 0) {
                                           string file_path = p.path().string();
                                                int id = files.size();
                                                file f {id, file_path};
                                                add_file(f);
                                                files.push_back(f);
                                        }
                                } else {
                                        string file_path = p.path().string();
                                        int id = files.size();
                                        file f {id, file_path};
                                        add_file(f);
                                        files.push_back(f);
                                }

                        }
                }

	}

	vector<vector<file> > data(n_threads, vector<file>());
	for(int i = 0; i < files.size(); i++) {
		data[i % n_threads].push_back(files[i]);
	}

	for(int i = 0; i < n_threads; i++) {
		threads.push_back(thread(thr, std::ref(data[i]), substr, std::ref(print_m)));
	}
	for(auto &t : threads) {
		t.join();
	}
	for(auto &f : files) {
		munmap(f.buf, f.fs);
	}
	
}
