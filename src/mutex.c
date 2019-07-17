#include "sys_inc.h"
#include "log_trace.h"
#include "mutex.h"

/*
* 函数: mutex_create
* 功能: 动态创建锁
* 参数: 无
* 返回: mutex_t *
* 说明: 
*/
mutex_t *mutex_create()
{
	mutex_t *mutex = calloc(sizeof(mutex_t), 1);
	if (mutex == NULL)
	{
		LOG_DEBUG_PERROR("calloc error !\n");
		return NULL;
	}

	pthread_mutex_init(&mutex->lock, NULL);
	return mutex;
}

/*
* 函数: mutex_destroy
* 功能: 销毁创建的锁
* 参数: mutex			互斥锁
* 返回: bool
*		- false		失败
* 说明: 
*/
bool mutex_destroy(mutex_t *mutex)
{
	if (mutex == NULL)
	{
		return false;
	}

	pthread_mutex_destroy(&mutex->lock);
	memset(mutex, 0, sizeof(mutex_t));
	free(mutex);
	return true;
}

/*
* 函数: spinlock_create
* 功能: 动态创建自旋锁
* 参数: 无
* 返回: mutex_t *
* 说明: 
*/
spinlock_t *spinlock_create()
{
	spinlock_t *spinlock = calloc(sizeof(spinlock_t), 1);
	if (spinlock == NULL)
	{
		LOG_DEBUG_PERROR("calloc error !\n");
		return NULL;
	}

	pthread_spin_init(&spinlock->lock, PTHREAD_PROCESS_PRIVATE);
	return spinlock;
}

/*
* 函数: spinlock_destroy
* 功能: 销毁创建的锁
* 参数: spinlock		自旋锁
* 返回: bool
*		- false		失败
* 说明: 
*/
bool spinlock_destroy(spinlock_t *spinlock)
{
	if (spinlock == NULL)
	{
		return false;
	}

	pthread_spin_destroy(&spinlock->lock);
	memset(spinlock, 0, sizeof(spinlock_t));
	free(spinlock);
	return true;
}

