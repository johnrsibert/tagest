//$Id: mainReadERSSTv2File.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include <iostream>

//int readERSSTv2File(const char* filename);
int readERSSTv2File(const char* filename,
                    const int beginYear,
                    const int beginMonth,
                    const int beginLongitude,
                    const int beginLatitude,
                    const int endYear,
                    const int endMonth,
                    const int endLongitude,
                    const int endLatitude);

int main(int argc, char** argv)
{
	if (argc <= 1) {
		std::cerr << "Usage: " << argv[0] << " <ersst.v2 filename>\n";
		return 0;
	}
	readERSSTv2File("ersst.v2.1970.1979.asc",
			1977,
			10,
			130,
			-25,
			1979,
			12,
			190,
			5);
	readERSSTv2File("ersst.v2.1980.1989.asc",
                   	1980,
			1,
			130,
			-25,
			1982,
			9,
			190,
			5);
	return 0;
}
