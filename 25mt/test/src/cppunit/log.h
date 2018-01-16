#ifndef __log_h__
#define __log_h__
/*
#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/helpers/exception.h>
using namespace log4cxx;
using namespace log4cxx::helpers;

#define LOG_DEBUG(logger, message) { \
	if (LOG4CXX_UNLIKELY(logger->isDebugEnabled())) { \
		ostringstream ostr; \
		ostr << message; \
		logger->forcedLog(::log4cxx::Level::getDebug(), ostr.str(), LOG4CXX_LOCATION); \
	}\
}
#define LOG_INFO(logger, message) { \
	if (logger->isInfoEnabled()) { \
		ostringstream ostr; \
		ostr << message; \
		logger->forcedLog(::log4cxx::Level::INFO, ostr.str(), LOG4CXX_LOCATION); \
	}\
}
#define LOG_WARN(logger, message) { \
	if (logger->isWarnEnabled()) { \
		ostringstream ostr; \
		ostr << message; \
		logger->forcedLog(::log4cxx::Level::WARN, ostr.str(), LOG4CXX_LOCATION); \
	}\
}
#define LOG_ERROR(logger, message) { \
	if (logger->isErrorEnabled()) { \
		ostringstream ostr; \
		ostr << message; \
		logger->forcedLog(::log4cxx::Level::getError(), ostr.str(), LOG4CXX_LOCATION); \
	}\
}
#define LOG_ASSERT(logger, condition, message) { \
	if (!condition && logger->isErrorEnabled()) { \
		ostringstream ostr; \
		ostr << message; \
		logger->forcedLog(::log4cxx::Level::getError(), ostr.str(), LOG4CXX_LOCATION); \
	}\
}
#define LOG_FATAL(logger, message) { \
	if (logger->isFatalEnabled()) { \
		ostringstream ostr; \
		ostr << message; \
		logger->forcedLog(::log4cxx::Level::FATAL, ostr.str(), LOG4CXX_LOCATION); \
	}\
}
*/
#endif
