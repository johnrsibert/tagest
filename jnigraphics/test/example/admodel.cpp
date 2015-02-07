#include <iostream>
//#define __GNUDOS__
//#define OPTLIB
using namespace std;
#include <fvar.hpp>


int main(int argc, char** argv)
{
	dmatrix dm(1, 4, 1, 9);
	std::cout << dm << std::endl;

	return 0;
}
