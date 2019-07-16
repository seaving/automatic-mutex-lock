#include "sys_inc.h"
#include "mutex.h"
#include "log_trace.h"

//静态初始化(类似于pthread_mutex_t _mutex = PTHREAD_MUTEX_INITIALIZER)
static mutex_t _mutex = MUTEX_INITIALIZER;

//竞争资源
#define RESOURCE_BUF_SIZE		128
static char *_resource = NULL;

/******************** 返回值是非void的锁的使用 ************************/
static void *_mutex_test_thread1(void *arg)
{
	pthread_detach(pthread_self());

	for ( ; ; )
	{
		/*
			因为函数返回类型是void *，所以要使用
			mutex_sync_return_type_not_void，并且指定
			returnType为void *

			有点类似java:
			synchronized(this) {
				//代码
			}
		*/
		mutex_sync_return_type_not_void(
			void *, &_mutex, {
				/* 
					此处写代码块，代码块会自动被上锁保护，
					代码块中写法和平时写代码一样，可以直
					接在此处return，锁一样会被自动释放
				*/
				if (_resource == NULL)
				{
					return NULL;
				}

				snprintf(_resource, RESOURCE_BUF_SIZE, "Hello, I am thread 1");
				printf("%s\n", _resource);
			}
		);

		usleep(10 * 1000);
	}

	return NULL;
}

/******************** 返回值void的锁的使用 ************************/
static inline void _change_resource()
{
	/*
		因为函数返回类型是void，所以要使用
		mutex_sync_return_type_is_void
	
		有点类似java:
		synchronized(this) {
			//代码
		}
	*/
	mutex_sync_return_type_is_void(
		&_mutex, {
			/* 
				此处写代码块，代码块会自动被上锁保护，
				代码块中写法和平时写代码一样，可以直
				接在此处return，锁一样会被自动释放
			*/
			if (_resource == NULL)
			{
				return;
			}

			snprintf(_resource, RESOURCE_BUF_SIZE, "I modify it in the function.");
			printf("%s\n", _resource);
		}
	);
}

static void *_mutex_test_thread2(void *arg)
{
	pthread_detach(pthread_self());

	for ( ; ; )
	{
		_change_resource();

		usleep(10 * 1000);
	}

	return NULL;
}
/******************** 死锁的例子 ************************/
static inline void _printf_resource()
{
	mutex_sync_return_type_is_void(
		&_mutex, {

			if (_resource == NULL)
			{
				return;
			}

			printf("%s\n", _resource);
		}
	);
}

static void *_mutex_test_thread3(void *arg)
{
	pthread_detach(pthread_self());

	for ( ; ; )
	{
		mutex_sync_return_type_not_void(
			void *, &_mutex, {

				if (_resource == NULL)
				{
					return NULL;
				}
		
				snprintf(_resource, RESOURCE_BUF_SIZE, "12343455t45654435435435345");

				/*
					有时候，我们会遇到调用某个函数，而该函数中也使用了同一把锁
					导致死锁，我这里演示的也会造成死锁，但是mutex_sync_xx中会检测
					是否被死锁，并打印出死锁在哪里，这样程序员就很容易通过打印知道
					是锁在哪个函数，从而顺着这个函数排查到问题
				*/
				_printf_resource();
			}
		);

		usleep(10 * 1000);
	}

	return NULL;
}
/********************************************************/

//函数类型是非void时的使用方法
int main()
{
	pthread_t thd1, thd2, thd3;

	_resource = calloc(1, RESOURCE_BUF_SIZE);

	pthread_create(&thd1, NULL, _mutex_test_thread1, NULL);
	pthread_create(&thd2, NULL, _mutex_test_thread2, NULL);
	pthread_create(&thd3, NULL, _mutex_test_thread3, NULL);

	for ( ; ; )
	{
		/*
			因为函数返回类型是int，所以要使用
			mutex_sync_return_type_not_void，并且指定
			returnType为void *
		
			有点类似java:
			synchronized(this) {
				//代码
			}
		*/
		mutex_sync_return_type_not_void(
			int, &_mutex, {

				/* 
					此处写代码块，代码块会自动被上锁保护，
					代码块中写法和平时写代码一样，可以直
					接在此处return，锁一样会被自动释放
				*/
				if (_resource == NULL)
				{
					return -1;
				}

				snprintf(_resource, RESOURCE_BUF_SIZE, "I modify it in the main function.");
				printf("%s\n", _resource);				
			}
		);

		usleep(10 * 1000);
	}

	return 0;
}

