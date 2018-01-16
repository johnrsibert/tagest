#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
using std::cerr;
using std::ifstream;
using std::string;
using std::istringstream;

#include <fvar.hpp>
int sstGroupByLongitudeLatitude(const char* file,
				const int beginLongitude,
				const int endLongitude,
				const int beginLatitude,
				const int endLatitude)
{
	if (beginLongitude > endLongitude
	    || beginLatitude > endLatitude
	    || beginLongitude < 0 
	    || beginLongitude > 360 
	    || endLongitude > 360 
	    || beginLatitude < -90 
	    || beginLatitude > 90 
	    || endLatitude > 90 
	    ) {
		return -3;
	}
	if (file == 0) {
		return -1;
	}
	ifstream ifs(file);
	if (ifs.is_open() == false) {
		return -2;
	}

	dmatrix sum(beginLongitude,
		    endLongitude,
		    beginLatitude,
		    endLatitude);
	sum.initialize();

	imatrix count(beginLongitude,
		      endLongitude,
		      beginLatitude,
		      endLatitude);
	count.initialize();

	string s;
	getline(ifs, s);

	int totalRecords = 0;
	int year = -1, currentYear = -1;
	string month = "XXX", currentMonth = "XXX";
	double longitude = -1, currentLongitude = -1;
	double latitude = -1, currentLatitude = -1;
	double temperature = -1, currentTemperature = -1; //degrees celcius
	while (ifs.eof() == false) {
		getline(ifs, s);
		std::istringstream istr(s);
		istr >> currentYear >> currentMonth >> currentLongitude >> currentLatitude >> currentTemperature;
		if (year == -1) {
			year = currentYear;
			month = currentMonth;
			longitude = currentLongitude;
			latitude = currentLatitude;
			temperature = currentTemperature;
		}
		const int longitudeDegree = int(currentLongitude);
		const int latitudeDegree = int(currentLatitude);
		if (year == currentYear && month == currentMonth) {
			if (beginLongitude <= longitudeDegree && longitudeDegree <= endLongitude
			    && beginLatitude <= latitudeDegree && latitudeDegree <= endLatitude) {
				sum(longitudeDegree, latitudeDegree) += currentTemperature;
				count(longitudeDegree, latitudeDegree) += 1;
			}
		} else {
			for (int i = beginLongitude; i <= endLongitude; i++) {
				for (int j = beginLatitude; j <= endLatitude; j++) {
					double average = 0;
					int c = count(i, j);
					if (c > 0) {
						average = sum(i, j) / double(c);
					}
					//cout << average;
				}
				//cout << endl;
			}

			sum.initialize();
			count.initialize();
			if (beginLongitude <= longitudeDegree && longitudeDegree <= endLongitude
			    && beginLatitude <= latitudeDegree && latitudeDegree <= endLatitude) {
				sum(longitudeDegree, latitudeDegree) = currentTemperature;
				count(longitudeDegree, latitudeDegree) = 1;
			}
			year = currentYear;
			month = currentMonth;
			longitude = currentLongitude;
			latitude = currentLatitude;
			temperature = currentTemperature;
		}
		totalRecords++;
		ifs.peek();
	}

	ifs.close();
	return totalRecords;
}
int sstGroupByLongitudeLatitude(const char* file)
{
	return sstGroupByLongitudeLatitude(file,
	 			           0,
				           0,
				           0,
				           0);
}
