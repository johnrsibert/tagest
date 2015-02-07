//$Id: readSouthernOscillationIndexFile.cpp 3127 2013-12-05 22:40:36Z jsibert $
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <fvar.hpp>
#include <adstring.hpp>

int readSouthernOscillationIndexFile(const adstring& soiFile, const int beginYear, const int endYear, dmatrix& soidata)
{
	if (soiFile.size() < 1) {
		return -1;
	} 
	const int minimumYear = 1951;
	const int maximumYear = 2006;
	if (beginYear > endYear
	    || minimumYear > beginYear
	    || endYear > maximumYear) {
			return -4;
	}
	ifstream ifs(soiFile);
	if (ifs.is_open() == false) {
		return -2;
	}
	soidata.allocate(beginYear, endYear, 1, 12);
	std::string s;
	for (int header = 1; header <= 5; header++) {
		getline(ifs, s);
	}
	for (int year = minimumYear; year < beginYear; year++) {
		getline(ifs, s);
	}
	for (int year = beginYear; year <= endYear; year++) {
		getline(ifs, s);

		//std::istrstream istr(s.c_str());
		std::istringstream istr(s.c_str());
		int y = 0;
		istr >> y;
		if (y != year) {
			ifs.close();
			return -3;
		}
		for (int month = 1; month <= 12; month++) {
			double v(0);
			istr >> v;
			soidata(year, month) = v;
		}
	}
	ifs.close();

	return true;
}
int readSouthernOscillationIndexFile(const int beginYear, const int endYear, dmatrix& soidata)
{
	const adstring soiFile = "../../../data/soi.txt";
	return readSouthernOscillationIndexFile(soiFile, beginYear, endYear, soidata);
}
