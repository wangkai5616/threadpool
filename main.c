#include "threadpool.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
 
void* mytask(void *arg)
{
    printf("thread %d is working on task %d\n", (int)pthread_self(), *(int*)arg);
    //作用是模拟执行任务
    sleep(1);
    free(arg);
    return NULL;
}
 
//测试代码
int main(void)
{
	//创建一个线程池
    threadpool_t pool;
    //初始化线程池，最多三个线程
    threadpool_init(&pool, 3);
    int i;
    //主线程（不是线程池里的线程）创建十个任务
    for(i=0; i < 10; i++)
    {
        int *arg = malloc(sizeof(int));
        *arg = i;
        threadpool_add_task(&pool, mytask, arg);
        
    }
 
    threadpool_destroy(&pool);
    return 0;
}
