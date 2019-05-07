#include "threadpool.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>
 
//创建的线程执行
void *thread_routine(void *arg)
{
    struct timespec abstime;
    int timeout;
    printf("thread %d is starting\n", (int)pthread_self());
    threadpool_t *pool = (threadpool_t *)arg;
    while(1)
    {
		//是否超时标记
        timeout = 0;
        //访问线程池之前需要加锁
        condition_lock(&pool->ready);
        //既然处于等待状态，空闲线程数增加一个
        pool->idle++;
        //等待队列有任务到来 或者 收到线程池销毁通知，如果不满足，就循环
        while(pool->first == NULL && !pool->quit)
        {
            //否则线程阻塞等待
            printf("thread %d is waiting\n", (int)pthread_self());  
            //获取从当前时间，并加上等待时间， 设置进程的超时睡眠时间
            clock_gettime(CLOCK_REALTIME, &abstime);
			//表示超时时间是2s
            abstime.tv_sec += 2;
            int status;
			//等待超时函数，如果超时，线程池中没有任务了，线程可以销毁了
            status = condition_timedwait(&pool->ready, &abstime);  //该函数会解锁，允许其他线程访问，当被唤醒时，加锁
            if(status == ETIMEDOUT)
            {
                printf("thread %d wait timed out\n", (int)pthread_self());
                timeout = 1;
                break;
            }
        }
        
		//等待到条件，处于工作状态
        pool->idle--;
		//等待到了任务
        if(pool->first != NULL)
        {
            //取出等待队列最前的任务，移除任务，并执行任务
            task_t *t = pool->first;
            pool->first = t->next;
            //由于任务执行需要消耗时间，先解锁让其他线程访问线程池
            condition_unlock(&pool->ready);
            //执行任务
            t->run(t->arg);
            //执行完任务释放内存
            free(t);
            //重新加锁
            condition_lock(&pool->ready);
        }
        
        //等待到线程池销毁通知（通过在threadpool_destroy函数中设置 pool->quit = 1;），且任务执行完毕
        if(pool->quit && pool->first == NULL)
        {
            pool->counter--;//当前工作的线程数-1
            //若线程池中没有线程，通知等待线程（主线程）全部任务已经完成
            if(pool->counter == 0)
            {
                condition_signal(&pool->ready);
            }
            condition_unlock(&pool->ready);
            break;
        }
        //超时，跳出销毁线程
        if(timeout == 1)
        {
            pool->counter--;//当前工作的线程数-1
            condition_unlock(&pool->ready);
            break;
        }
        
        condition_unlock(&pool->ready);
    }
    
    printf("thread %d is exiting\n", (int)pthread_self());
    return NULL;
    
}
 
 
//线程池初始化
void threadpool_init(threadpool_t *pool, int threads)
{
    
    condition_init(&pool->ready);
    pool->first = NULL;
    pool->last =NULL;
    pool->counter =0;
    pool->idle =0;
    pool->max_threads = threads;
    pool->quit =0;
    
}
 
 
//增加一个任务到线程池
void threadpool_add_task(threadpool_t *pool, void *(*run)(void *arg), void *arg)
{
    //产生一个新的任务
    task_t *newtask = (task_t *)malloc(sizeof(task_t));
    newtask->run = run;
    newtask->arg = arg;
    newtask->next=NULL;//新加的任务放在队列尾端
    
    //线程池的状态被多个线程共享，操作前需要加锁
    condition_lock(&pool->ready);
    
    if(pool->first == NULL)//第一个任务加入
    {
        pool->first = newtask;
    }        
    else    
    {
        pool->last->next = newtask;
    }
    pool->last = newtask;  //队列尾指向新加入的线程
    
    //线程池中有线程空闲，唤醒
    if(pool->idle > 0)
    {
		//唤醒至少一个阻塞在条件变量上的线程，卡在这好久，去看condition.c中的相关函数
        condition_signal(&pool->ready);
    }
    //当前线程池中线程个数没有达到设定的最大值，创建一个新的线性
    else if(pool->counter < pool->max_threads)
    {
        pthread_t tid;
		//创建出来的线程执行函数thread_routine，该函数执行结束，线程也就结束
        pthread_create(&tid, NULL, thread_routine, pool);
        pool->counter++;
    }
    //结束，访问
    condition_unlock(&pool->ready);
}
 
//线程池销毁
//等待线程将所有任务执行完毕之后在销毁
void threadpool_destroy(threadpool_t *pool)
{
    //如果已经调用销毁，直接返回
    if(pool->quit)
    {
    return;
    }
    //加锁
    condition_lock(&pool->ready);
    //设置销毁标记为1
    pool->quit = 1;
    //线程池中当前线程个数大于0
    if(pool->counter > 0)
    {
        if(pool->idle > 0)
        {
            condition_broadcast(&pool->ready);
        }
        //处于执行任务状态中的线程，不会收到广播
		//线程池中需要等待执行任务状态中的线程全部退出
        while(pool->counter)
        {
			//现在处于等待，那么谁发起通知呢？在第66行
            condition_wait(&pool->ready);
        }
    }
    condition_unlock(&pool->ready);
    condition_destroy(&pool->ready);
}
