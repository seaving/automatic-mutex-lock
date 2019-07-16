#ifndef __LOG_DEBUG_TRACE_H__
#define __LOG_DEBUG_TRACE_H__

#ifndef MODULE_NAME
#define MODULE_NAME	""
#endif

#ifndef SYSTEM_SEC
#define SYSTEM_SEC time(NULL)
#endif

#define LOG_DEBUG_TRACE_ENABLE
#if 0
#define DBG_PRINT	daemon_log
#define DBG_LEVEL	LOG_INFO,
#else
#define DBG_PRINT	printf
#define DBG_LEVEL	""
#endif

#if defined(LOG_DEBUG_TRACE_ENABLE)
#define LOG_DEBUG_TRACE_DAEMON(format, args...) \
	do { \
		DBG_PRINT(DBG_LEVEL "%s %lu [%s]:%d " format, \
			MODULE_NAME, SYSTEM_SEC, __FUNCTION__, __LINE__, ## args); \
	} while (0)
#else
#define LOG_DEBUG_TRACE_DAEMON(format, args...)
#endif

#if defined(LOG_DEBUG_TRACE_ENABLE)
#define LOG_DEBUG_TRACE_PERROR(format, args...) \
	do { \
		DBG_PRINT(DBG_LEVEL "%s %lu [%s]:%d [error info(errno=%d): %s] -> " format, \
			MODULE_NAME, SYSTEM_SEC, __FUNCTION__, __LINE__, errno, strerror(errno), ## args); \
	} while (0)
#else
#define LOG_DEBUG_TRACE_PERROR(format, args...)
#endif

#define LOG_DEBUG_TRACE			LOG_DEBUG_TRACE_DAEMON
#define LOG_DEBUG_PERROR		LOG_DEBUG_TRACE_PERROR
#define DEBUG_POINT				LOG_DEBUG_TRACE("\n");


#endif

