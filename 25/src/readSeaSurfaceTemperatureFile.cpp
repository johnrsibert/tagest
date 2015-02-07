//$Id: readSeaSurfaceTemperatureFile.cpp 3127 2013-12-05 22:40:36Z jsibert $
#include <iostream>
#include <string>
//#include <strstream>
#include <sstream>
#include <fvar.hpp>
#include <adstring.hpp>

int readSeaSurfaceTemperatureFile(const adstring sstfile, const int year, const int month, const int originLongitude, const int originLatitude, dmatrix& sst)
{
	using std::string;

	ifstream ifs(sstfile);
	if (ifs.is_open() == false) {
		return -1;
	}
	string MMM;
	switch(month) {
		case 1:
			MMM="Jan";
			break;
		case 2:
			MMM="Feb";
			break;
		case 3:
			MMM="Mar";
			break;
		case 4:
			MMM="Apr";
			break;
		case 5:
			MMM="May";
			break;
		case 6:
			MMM="Jun";
			break;
		case 7:
			MMM="Jul";
			break;
		case 8:
			MMM="Aug";
			break;
		case 9:
			MMM="Sep";
			break;
		case 10:
			MMM="Oct";
			break;
		case 11:
			MMM="Nov";
			break;
		case 12:
			MMM="Dec";
			break;
		default:
			break;
	}
	string s;
	getline(ifs, s); //skip header
	int records = 0;
	while (ifs.eof() == false) {
		getline(ifs, s);
		//std::istrstream istr(s.c_str());
		std::istringstream istr(s.c_str());

		int y = 0;
		string m;
		double longitude = 0, latitude = 0, temperature = 0;
		istr >> y >> m >> longitude >> latitude >> temperature;	
		if (y == year && m == MMM) {
			const int i = int(longitude) - originLongitude + 1;
			const int j = int(latitude) - originLatitude + 1;
			if (sst.rowmin() <= i && i <= sst.rowmax()
			    && sst(i).indexmin() <= j && j <= sst(i).indexmax()) {
				sst(i, j) = temperature;
			}
		}
		records++;
		ifs.peek();
	}

	return records;
}
