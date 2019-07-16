#ifndef __MUTEX_H__
#define __MUTEX_H__

#include "sys_inc.h"

typedef struct __mutex__
{
	pthread_mutex_t lock;		//互斥锁
	pid_t	pid;				//持有锁的进程ID
	pthread_t thread_id;		//持有锁的线程ID
	char *fun_name;				//持有锁的函数
	int fun_line;				//持有锁的函数行号
	bool locked;				//是否上锁标记
	bool codeBlock_noReturn;	//代码块中是否return标记
} mutex_t;

/*
* 宏: MUTEX_INITIALIZER
* 功能: 静态初始化 
* 参数: 无
* 返回: 无
* 说明: 使用举例:
*		mutex_t mutex = MUTEX_INITIALIZER;
*/
#define MUTEX_INITIALIZER	{ \
	.lock = PTHREAD_MUTEX_INITIALIZER, \
	.pid = 0, \
	.thread_id = 0, \
	.locked = false, \
}

/*
* 函数: mutex_sync_return_type_not_void
* 功能: 代码块同步
* 参数: returnType		调用该宏的函数返回值类型
*		_pMutex			mutex类型互斥锁
*		_codeBlock		代码块
*		fun				函数名
*		line			行数
* 返回: 无
* 说明: 适用于函数返回值类型不是void
*/
#define __mutex_sync_return_type_not_void(returnType, _pMutex, _codeBlock, fun, line)	\
	do { \
		/*const char *__mutex_sync_fun_name = fun;*/ \
		/*const int __mutex_sync_fun_line = line;*/ \
		returnType _mutex_sync_##fun##line##_ret;\
		if ((_pMutex)->locked == true \
			&& pthread_self() == (_pMutex)->thread_id \
			&& getpid() == (_pMutex)->pid) \
		{ \
			LOG_DEBUG_TRACE("!!!!!! WARN DeadLock (Holding lock fun: %s, line: %d) !!!!!!\n", \
				(_pMutex)->fun_name, (_pMutex)->fun_line); \
		} \
		/*LOG_DEBUG_TRACE("pid=%lu, threadid=%lu, locked=%d\n", */ \
			/*(_pMutex)->pid, (_pMutex)->thread_id, (_pMutex)->locked); */ \
		pthread_mutex_lock(&((_pMutex)->lock)); \
		(_pMutex)->pid = getpid(); \
		(_pMutex)->thread_id = pthread_self(); \
		(_pMutex)->locked = true; \
		(_pMutex)->fun_name = (char *) fun; \
		(_pMutex)->fun_line = (int) line; \
		\
		returnType _mutex_sync_##fun##line() { \
			/*LOG_DEBUG_TRACE("<<< source fun: %s, source line: %d >>>\n",*/ \
				/*	__mutex_sync_fun_name, __mutex_sync_fun_line);*/ \
			(_pMutex)->codeBlock_noReturn = false; \
			_codeBlock \
			(_pMutex)->codeBlock_noReturn = true; \
			return (returnType) 0; \
		} \
		/*LOG_DEBUG_TRACE("execut code block function >>> \n");*/ \
		_mutex_sync_##fun##line##_ret = _mutex_sync_##fun##line();\
		\
		(_pMutex)->pid = 0; \
		(_pMutex)->thread_id = 0; \
		(_pMutex)->locked = false; \
		(_pMutex)->fun_name = ""; \
		(_pMutex)->fun_line = 0; \
		pthread_mutex_unlock(&((_pMutex)->lock)); \
		if ((_pMutex)->codeBlock_noReturn == false) \
		{ \
			/*LOG_DEBUG_TRACE("return ==> \n");*/ \
			return _mutex_sync_##fun##line##_ret;\
		} \
	} while (0);

/*
* 函数: mutex_sync_return_type_is_void
* 功能: 代码块同步
* 参数: returnType		调用该宏的函数返回值类型
*		_pMutex 		mutex类型互斥锁
*		_codeBlock		代码块
*		fun				函数名
*		line			行数
* 返回: 无
* 说明: 适用于函数返回值类型void
*/
#define __mutex_sync_return_type_is_void(/*returnType, */_pMutex, _codeBlock, fun, line)	\
	do { \
		/*const char *__mutex_sync_fun_name = fun;*/ \
		/*const int __mutex_sync_fun_line = line;*/ \
		if ((_pMutex)->locked == true \
			&& pthread_self() == (_pMutex)->thread_id \
			&& getpid() == (_pMutex)->pid) \
		{ \
			LOG_DEBUG_TRACE("!!!!!! WARN DeadLock (Holding lock fun: %s, line: %d) !!!!!!\n", \
				(_pMutex)->fun_name, (_pMutex)->fun_line); \
		} \
		pthread_mutex_lock(&((_pMutex)->lock)); \
		(_pMutex)->pid = getpid(); \
		(_pMutex)->thread_id = pthread_self(); \
		(_pMutex)->locked = true; \
		(_pMutex)->fun_name = (char *)fun; \
		(_pMutex)->fun_line = (int) line; \
		\
		void _mutex_sync_##fun##line() { \
			/*LOG_DEBUG_TRACE("<<< source fun: %s, source line: %d >>>\n",*/ \
					/*__mutex_sync_fun_name, __mutex_sync_fun_line);*/ \
			(_pMutex)->codeBlock_noReturn = false; \
			_codeBlock \
			(_pMutex)->codeBlock_noReturn = true; \
		} \
		/*LOG_DEBUG_TRACE("execut code block function >>> \n");*/ \
		_mutex_sync_##fun##line();\
		\
		(_pMutex)->pid = 0; \
		(_pMutex)->thread_id = 0; \
		(_pMutex)->locked = false; \
		(_pMutex)->fun_name = ""; \
		(_pMutex)->fun_line = 0; \
		pthread_mutex_unlock(&((_pMutex)->lock)); \
		if ((_pMutex)->codeBlock_noReturn == false) \
		{ \
			/*LOG_DEBUG_TRACE("return ==> \n");*/ \
			return; \
		} \
	} while (0);

/*
* 函数: mutex_sync_return_type_not_void
* 功能: 代码块同步
* 参数: returnType		调用该宏的函数返回值类型
*		_pMutex 		mutex类型互斥锁
*		_codeBlock		代码块
* 返回: 无
* 说明: 适用于函数返回值类型不是void
*/
#define mutex_sync_return_type_not_void(returnType, _pMutex, _codeBlock) \
	__mutex_sync_return_type_not_void(returnType, _pMutex, _codeBlock, __FUNCTION__, __LINE__)

/*
* 函数: mutex_sync_return_type_not_void
* 功能: 代码块同步
* 参数: returnType		调用该宏的函数返回值类型
*		_pMutex 		mutex类型互斥锁
*		_codeBlock		代码块
* 返回: 无
* 说明: 适用于函数返回值类型不是void
*/
#define mutex_sync_return_type_is_void(/*returnType, */_pMutex, _codeBlock) \
	__mutex_sync_return_type_is_void(/*returnType, */_pMutex, _codeBlock, __FUNCTION__, __LINE__)

//动态初始化
mutex_t *mutex_create();

//释放锁资源
bool mutex_destroy(mutex_t *mutex);

#endif

