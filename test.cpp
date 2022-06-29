
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <utility>

class A
{
private:
	int a;
public:
	A() : a(42) { std::cout << "A default constructed = " << a << std::endl; }
	A(int a) : a(a) { std::cout << "A constructed = " << a << std::endl; }
	A(A const &other)
	{
		*this = other;
		std::cout << "A copy constructed = " << a << std::endl;
	}
	A &operator=(A const &other)
	{
		a = other.a;
		return *this;
	}
	~A() { std::cout << "A destroyed = " << a << std::endl; }
};

int main()
{
	A *a1 = new A(1);
	A *a2 = new A(2);
	std::map<char,A> mappa;

	mappa.insert(std::pair<char,A>('1', *a1));
	mappa.insert(std::pair<char,A>('2', *a2));
	mappa.erase('1');
	// mappa.clear();
	std::cout << "fine" << std::endl;
}
