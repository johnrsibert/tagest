#include <iostream>
#include <fvar.hpp>
#include <cifstrem.h>
#include <adstring.hpp>

void binaryHabitatDataToXml(char* binaryHabitatFilename)
{
	cifstream cifs("file_nam.dat");
	if (!cifs) {
		cerr << "Error: unable to open \"file_nam.dat\"\n";
		return;
	}
	adstring filename;
	cifs >> filename;
	cifs.close();

	cifstream prn(filename + ".prn");
	if (!prn) {
		cerr << "Error: unable to open \"" << filename << ".prn\"\n";
		return;
	}
	int m, n;
	prn >> m >> n;
	prn.close();

	dmatrix record(1, m, 1, n); record.initialize();
	uistream ui(binaryHabitatFilename);
	if (!ui) {
		cerr << "Error: unable to open \"" << binaryHabitatFilename << "\"\n";
		return;
	}
	int year, month;
	ui >> year >> month;
	std::cout << "<data year=\"" << year << "\" month=\"" << month << "\">"<< std::endl;
	int time = 0;
	while (ui) {
		std::cout << "<record time=\"" << time << "\">" << std::endl;
		ui >> record;
		std::cout << record << std::endl;
		std::cout << "</record>" << std::endl;
		time++;
	}
	ui.close();
	std::cout << "</data>" << std::endl;
}
int main(int argc, char** argv)
{
	if (argc > 1) {
		for (int i = 1; i < argc; i++) {
			binaryHabitatDataToXml(argv[i]);
		}
	}
	return 0;
}
