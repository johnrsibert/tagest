#include <string>
#include "TagestFacade.h"

/*
#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/helpers/exception.h>
#include <log4cxx/patternlayout.h>
#include <log4cxx/fileappender.h>

using namespace log4cxx;
using namespace log4cxx::helpers;
*/

int main()
{
/*
	PatternLayoutPtr patternlayout(new PatternLayout(std::string("%5p[%F:%L] %m%n")));
	FileAppenderPtr fileappender(new FileAppender(patternlayout, std::string("log4cxx.tagest.log"), false));
	BasicConfigurator::configure(fileappender);
*/

	TagestFacade tagest;
	tagest.minimize();

	return 0;
}
