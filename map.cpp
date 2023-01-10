#include <string>
#include <map>
#include <iostream>
#include <vector>

struct Files {
	Files(void) {
		filenames.push_back("file1");
		filenames.push_back("file2");
		filenames.push_back("file3");
	}
	~Files(void) {
	}
	std::vector<std::string> filenames;
};

std::ostream &operator<<(std::ostream &n, Files &files) {
	for (std::vector<std::string>::iterator it = files.filenames.begin(); it != files.filenames.end(); it++) {
		n << *it << std::endl;
	}
	return n;
}

int main(void) {
	std::map<std::string, Files> test;
	Files file;
	std::cout << "First test" << test["hello"] << std::endl;
	test["hello"] = file;
	std::cout << test["hello"] << std::endl;

	file.filenames.clear();
	file.filenames.push_back("test1");
	
	test["panda"] = file;
	std::cout << test["hello"] << std::endl;
	std::cout << test["panda"] << std::endl;
}
