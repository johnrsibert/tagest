//$Id: readERSSTv2File.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include <iostream>
#include <iomanip>
#include <fstream>
#include <strstream>
#include <string>

int readERSSTv2File(const char* filename,
		    const int beginYear,
		    const int beginMonth,
		    const int beginLongitude,
		    const int beginLatitude,
		    const int endYear,
		    const int endMonth,
		    const int endLongitude,
		    const int endLatitude)
{
	std::ifstream ifs(filename);
	if (ifs.is_open() == false) {
		return -1;
	}
	using std::string;	
	string s(filename);
	const int length = s.length();
	if (s.compare(length - 22, 10, "ersst.v2.1") != 0 || s.compare(length - 4, 4, ".asc") != 0) {
		return -2;
	}
	int fullBeginYear = 0;
	int fullEndYear = 0;
	using std::istrstream;
	istrstream istrFullBeginYear(s.substr(length - 13, 4).c_str());
	istrFullBeginYear >> fullBeginYear;
	istrstream istrFullEndYear(s.substr(length - 8, 4).c_str());
	istrFullEndYear >> fullEndYear;
	if (fullBeginYear < 1854 || fullEndYear < 1859) {
		return -3;
	}
	if (beginYear > endYear 
	    || (fullBeginYear <= beginYear && beginYear <= fullEndYear) == false
	    || (fullBeginYear <= endYear && endYear <= fullEndYear) == false
	    || (1 <= beginMonth && beginMonth <= 12) == false
	    || (1 <= endMonth && endMonth <= 12) == false
	    || (beginYear == endYear && beginMonth > endMonth) == true) {
		return -4;
	}
	if (beginLongitude > endLongitude
	    || (0 <= beginLongitude && beginLongitude < 360) == false
	    || (0 <= endLongitude && endLongitude < 360) == false) {
		return -5;
	}
	if (beginLatitude > endLatitude
	    || (-90 <= beginLatitude && beginLatitude <= 90) == false
	    || (-90 <= endLatitude && endLatitude <= 90) == false) {
		return -6;
	}
	const int pointsZonal = 180;
	//const int pointsMeridional = 89;
	string record;
	int year = fullBeginYear;
	for (; year < beginYear; year++) {
		for (int month = 1; month <= 12; month++) {
			for (int i = 1; i <= pointsZonal; i++) {
				getline(ifs, record);	
			}
		}
	}
	int month = 1;
	for (; month < beginMonth; month++) {
		for (int i = 1; i <= pointsZonal; i++) {
			getline(ifs, record);	
		}
	}

	//All fields were computed on the 2-degree grid.
	//  -> coarse
	//The grid has 180 zonal points and 89 meridional points.
	//The 2-degree cells are centered on 0E, 2E, ...,and 88S,86S ....
	//  -> uses north west corner
	//Over land a value of -99.99 is assigned. 
	//  -> don't output
	//In addition any SST mean value less than -1.8C was set to -1.8C.
	//  -> good for testing
	
	const int minLongitude = (beginLongitude % 2) == 0 ? beginLongitude : beginLongitude + 1; 
	const int maxLongitude = (endLongitude % 2) == 0 ? endLongitude : endLongitude + 1; 
	const int minLatitude = (beginLatitude % 2) == 0 ? beginLatitude : beginLatitude + 1; 
	const int maxLatitude = (endLatitude % 2) == 0 ? endLatitude : endLatitude + 1; 
	int totalPoints = 0;
	for (; year <= endYear; year++) {
		for (; month <= (year == endYear ? endMonth : 12); month++) {
			for (int longitude = 0; longitude < 360; longitude += 2) {
				if (minLongitude <= longitude && longitude <= maxLongitude) {
					for (int latitude = -90; latitude <= 90; latitude += 2) {
						int value = 0;
						ifs >> value;
						if (minLatitude <= latitude && latitude <= maxLatitude) {
							if (value != -9999) {
								const double v = value / 100.0;
								std::cout << year << '\t' << month << '\t' << longitude << '\t' << latitude << '\t' << v << std::endl;
								std::cout << year << '\t' << month << '\t' << longitude - 1 << '\t' << latitude << '\t' << v << std::endl;
								std::cout << year << '\t' << month << '\t' << longitude - 1 << '\t' << latitude - 1 << '\t' << v << std::endl;
								std::cout << year << '\t' << month << '\t' << longitude << '\t' << latitude - 1 << '\t' << v << std::endl;
							}
							totalPoints++;
						}
					}
				} else {
					getline(ifs, record);	
				}
			}
		}
		month = 1;
	}
	ifs.close();
	return totalPoints;
}
int readERSSTv2File(const char* filename)
{
	const int beginYear = 0;
	const int beginMonth= 0;
	const int beginLongitude = 0;
	const int beginLatitude = 0;
	const int endYear = 0;
	const int endMonth= 0;
	const int endLongitude = 0;
	const int endLatitude = 0;
	return readERSSTv2File(filename,
			       beginYear,
			       beginMonth,
			       beginLongitude,
			       beginLatitude,
			       endYear,
			       endMonth,
			       endLongitude,
			       endLatitude);
}
