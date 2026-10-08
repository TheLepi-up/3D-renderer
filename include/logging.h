#include <string>
#include <sys/types.h>
extern const std::string LogLevels[5];

void log(const std::string &logLevel, const std::string &file, uint line, const std::string &msg, const std::string& component = "");

#define FATAL 4
#define ERROR 3
#define WARNING 2
#define INFO 1
#define TRACE 0

#define LOGLEVEL INFO

#define EXPAND(x) x
#define GET_MACRO(_1, _2, _3, _4, name, ...) name

#define logFatal(...) EXPAND(GET_MACRO(__VA_ARGS__, logFatal4, logFatal3, logFatal2, logFatal1)(__VA_ARGS__))
#define logFatal1(msg) log(LogLevels[0], __FILE__, __LINE__, msg, __func__);
#define logFatal2(msg, component) log(LogLevels[0], __FILE__, __LINE__, msg, component)

#if LOGLEVEL <= ERROR
#define logError(...) EXPAND(GET_MACRO(__VA_ARGS__, logError4, logError3, logError2, logError1)(__VA_ARGS__))
#define logError1(msg) log(LogLevels[1], __FILE__, __LINE__, msg, __func__);
#define logError2(msg, component) log(LogLevels[1], __FILE__, __LINE__, msg, component)
#else
#define logError(...)
#endif

#if LOGLEVEL <= WARNING
#define logWarning(...) EXPAND(GET_MACRO(__VA_ARGS__, logWarning4, logWarning3, logWarning2, logWarning1)(__VA_ARGS__))
#define logWarning1(msg) log(LogLevels[2], __FILE__, __LINE__, msg, __func__);
#define logWarning2(msg, component) log(LogLevels[2], __FILE__, __LINE__, msg, component)
#else
#define logWarning(...)
#endif

#if LOGLEVEL <= INFO
#define logInfo(...) EXPAND(GET_MACRO(__VA_ARGS__, logInfo4, logInfo3, logInfo2, logInfo1)(__VA_ARGS__))
#define logInfo1(msg) log(LogLevels[3], __FILE__, __LINE__, msg, __func__);
#define logInfo2(msg, component) log(LogLevels[3], __FILE__, __LINE__, msg, component)
#else
#define logInfo(...)
#endif

#if LOGLEVEL <= TRACE
#define logTrace(...) EXPAND(GET_MACRO(__VA_ARGS__, logTrace4, logTrace3, logTrace2, logTrace1)(__VA_ARGS__))
#define logTrace1(msg) log(LogLevels[4], __FILE__, __LINE__, msg, __func__);
#define logTrace2(msg, component) log(LogLevels[4], __FILE__, __LINE__, msg, component)
#else
#define logTrace(...)
#endif