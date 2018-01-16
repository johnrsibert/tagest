/**
 * $Id: TestMain.cpp 2737 2010-08-05 22:14:06Z johnoel $
 * Copyright 2005 University of Hawaii, Pelagic Fisheries Research Program
 */
/*
#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/helpers/exception.h>
#include <log4cxx/patternlayout.h>
#include <log4cxx/fileappender.h>

using namespace log4cxx;
using namespace log4cxx::helpers;
*/

#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

int main(int argc, char* argv[])
{
/*
  PatternLayoutPtr patternlayout(new PatternLayout(std::string("%5p[%F:%L] %m%n")));
  FileAppenderPtr fileappender(new FileAppender(patternlayout, std::string("log4cxx.tagest.log"), false));
  BasicConfigurator::configure(fileappender);
*/

  // Get the top level suite from the registry
  CppUnit::Test *suite = CppUnit::TestFactoryRegistry::getRegistry().makeTest();

  // Adds the test to the list of test to run
  CppUnit::TextUi::TestRunner runner;
  runner.addTest( suite );

  // Change the default outputter to a compiler error format outputter
  runner.setOutputter( new CppUnit::CompilerOutputter( &runner.result(),
                                                       std::cerr ) );
  // Run the tests.
  bool wasSucessful = runner.run();

  // Return error code 1 if the one of test failed.
  return wasSucessful ? 0 : 1;
}
