#include <iostream>
#include <string>

class StringPointer {
private:
	bool constructed;
	std::string* p;
public:
	StringPointer(std::string* p) : p(p), constructed(false) {};
	~StringPointer() {
		if (constructed && p)
			delete p;
	}
	std::string* operator->() { 
		if (!p) {
			constructed = true;
			p = new std::string();
		}
		return p;
	}
	std::string operator*() { 
		if (!p) {
			constructed = true;
			p = new std::string();
		}
		return *p;
	}
};

int main(int argc, char *argv[]) {

	std::string s1 = "Hello, world!";

	StringPointer sp1(&s1);
	StringPointer sp2(NULL);

	std::cout << sp1->length() << std::endl;
	std::cout << *sp1 << std::endl;
	std::cout << sp2->length() << std::endl;
	std::cout << *sp2 << std::endl;

	return 0;
}