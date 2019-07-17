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

typedef struct __spinlock__
{
	pthread_spinlock_t lock;	//自旋锁
	pid_t	pid;				//持有锁的进程ID
	pthread_t thread_id;		//持有锁的线程ID
	char *fun_name;				//持有锁的函数
	int fun_line;				//持有锁的函数行号
	bool locked;				//是否上锁标记
	bool codeBlock_noReturn;	//代码块中是否return标记
} spinlock_t;

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
* 函数: _lock_sync_return_type_not_void
* 功能: 代码块同步
* 参数: _lock_fun			上锁函数
*		_unlock_fun		解锁函数
*		returnType		调用该宏的函数返回值类型
*		_pLock			锁
*		_codeBlock		代码块
*		fun				函数名
*		line			行数
* 返回: 无
* 说明: 适用于函数返回值类型不是void
*/
#define __lock_sync_return_type_not_void(_lock_fun, _unlock_fun, returnType, _pLock, _codeBlock, fun, line)	\
	do { \
		/*const char *__lock_sync_fun_name = fun;*/ \
		/*const int __lock_sync_fun_line = line;*/ \
		returnType _lock_sync_##fun##line##_ret;\
		if ((_pLock)->locked == true \
			&& pthread_self() == (_pLock)->thread_id \
			&& getpid() == (_pLock)->pid) \
		{ \
			LOG_DEBUG_TRACE("!!!!!! WARN DeadLock (Holding lock fun: %s, line: %d) !!!!!!\n", \
				(_pLock)->fun_name, (_pLock)->fun_line); \
		} \
		/*LOG_DEBUG_TRACE("pid=%lu, threadid=%lu, locked=%d\n", */ \
			/*(_pLock)->pid, (_pLock)->thread_id, (_pLock)->locked); */ \
		_lock_fun(&((_pLock)->lock)); \
		(_pLock)->pid = getpid(); \
		(_pLock)->thread_id = pthread_self(); \
		(_pLock)->locked = true; \
		(_pLock)->fun_name = (char *) fun; \
		(_pLock)->fun_line = (int) line; \
		\
		returnType _lock_sync_##fun##line() { \
			/*LOG_DEBUG_TRACE("<<< source fun: %s, source line: %d >>>\n",*/ \
				/*	__lock_sync_fun_name, __lock_sync_fun_line);*/ \
			(_pLock)->codeBlock_noReturn = false; \
			_codeBlock \
			(_pLock)->codeBlock_noReturn = true; \
			return (returnType) 0; \
		} \
		/*LOG_DEBUG_TRACE("execut code block function >>> \n");*/ \
		_lock_sync_##fun##line##_ret = _lock_sync_##fun##line();\
		\
		(_pLock)->pid = 0; \
		(_pLock)->thread_id = 0; \
		(_pLock)->locked = false; \
		(_pLock)->fun_name = ""; \
		(_pLock)->fun_line = 0; \
		_unlock_fun(&((_pLock)->lock)); \
		if ((_pLock)->codeBlock_noReturn == false) \
		{ \
			/*LOG_DEBUG_TRACE("return ==> \n");*/ \
			return _lock_sync_##fun##line##_ret;\
		} \
	} while (0)

/*
* 函数: __lock_sync_return_type_is_void
* 功能: 代码块同步
* 参数: _lock_fun			上锁函数
*		_unlock_fun		解锁函数
*		returnType		调用该宏的函数返回值类型
*		_pLock 			锁
*		_codeBlock		代码块
*		fun				函数名
*		line			行数
* 返回: 无
* 说明: 适用于函数返回值类型void
*/
#define __lock_sync_return_type_is_void(_lock_fun, _unlock_fun, /*returnType, */_pLock, _codeBlock, fun, line)	\
	do { \
		/*const char *__lock_sync_fun_name = fun;*/ \
		/*const int __lock_sync_fun_line = line;*/ \
		if ((_pLock)->locked == true \
			&& pthread_self() == (_pLock)->thread_id \
			&& getpid() == (_pLock)->pid) \
		{ \
			LOG_DEBUG_TRACE("!!!!!! WARN DeadLock (Holding lock fun: %s, line: %d) !!!!!!\n", \
				(_pLock)->fun_name, (_pLock)->fun_line); \
		} \
		_lock_fun(&((_pLock)->lock)); \
		(_pLock)->pid = getpid(); \
		(_pLock)->thread_id = pthread_self(); \
		(_pLock)->locked = true; \
		(_pLock)->fun_name = (char *)fun; \
		(_pLock)->fun_line = (int) line; \
		\
		void _lock_sync_##fun##line() { \
			/*LOG_DEBUG_TRACE("<<< source fun: %s, source line: %d >>>\n",*/ \
					/*__lock_sync_fun_name, __lock_sync_fun_line);*/ \
			(_pLock)->codeBlock_noReturn = false; \
			_codeBlock \
			(_pLock)->codeBlock_noReturn = true; \
		} \
		/*LOG_DEBUG_TRACE("execut code block function >>> \n");*/ \
		_lock_sync_##fun##line();\
		\
		(_pLock)->pid = 0; \
		(_pLock)->thread_id = 0; \
		(_pLock)->locked = false; \
		(_pLock)->fun_name = ""; \
		(_pLock)->fun_line = 0; \
		_unlock_fun(&((_pLock)->lock)); \
		if ((_pLock)->codeBlock_noReturn == false) \
		{ \
			/*LOG_DEBUG_TRACE("return ==> \n");*/ \
			return; \
		} \
	} while (0)

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
	__lock_sync_return_type_not_void(pthread_mutex_lock, pthread_mutex_unlock, \
			returnType, _pMutex, _codeBlock, __FUNCTION__, __LINE__)

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
	__lock_sync_return_type_is_void(pthread_mutex_lock, pthread_mutex_unlock, \
			/*returnType, */_pMutex, _codeBlock, __FUNCTION__, __LINE__)

/*
* 函数: spinlock_sync_return_type_not_void
* 功能: 代码块同步
* 参数: returnType		调用该宏的函数返回值类型
*		_pSpinLock 		spinlock类型互斥锁
*		_codeBlock		代码块
* 返回: 无
* 说明: 适用于函数返回值类型不是void
*/
#define spinlock_sync_return_type_not_void(returnType, _pSpinLock, _codeBlock) \
	__lock_sync_return_type_not_void(pthread_mutex_lock, pthread_mutex_unlock, \
			returnType, _pSpinLock, _codeBlock, __FUNCTION__, __LINE__)

/*
* 函数: spinlock_sync_return_type_is_void
* 功能: 代码块同步
* 参数: returnType		调用该宏的函数返回值类型
*		_pSpinLock 		spinlock类型互斥锁
*		_codeBlock		代码块
* 返回: 无
* 说明: 适用于函数返回值类型不是void
*/
#define spinlock_sync_return_type_is_void(/*returnType, */_pSpinLock, _codeBlock) \
	__lock_sync_return_type_is_void(pthread_spin_lock, pthread_spin_unlock, \
			/*returnType, */_pSpinLock, _codeBlock, __FUNCTION__, __LINE__)

/*
* 函数: mutex_create
* 功能: 动态创建锁
* 参数: 无
* 返回: mutex_t *
* 说明: 
*/
mutex_t *mutex_create();

/*
* 函数: mutex_destroy
* 功能: 销毁创建的锁
* 参数: mutex			互斥锁
* 返回: bool
*		- false		失败
* 说明: 
*/
bool mutex_destroy(mutex_t *mutex);

/*
* 函数: spinlock_create
* 功能: 动态创建自旋锁
* 参数: 无
* 返回: mutex_t *
* 说明: 
*/
spinlock_t *spinlock_create();

/*
* 函数: spinlock_destroy
* 功能: 销毁创建的锁
* 参数: spinlock		自旋锁
* 返回: bool
*		- false		失败
* 说明: 
*/
bool spinlock_destroy(spinlock_t *spinlock);

#endif

