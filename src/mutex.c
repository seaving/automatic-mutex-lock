#include "sys_inc.h"
#include "log_trace.h"
#include "mutex.h"

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

