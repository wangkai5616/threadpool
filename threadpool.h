#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

//线程池头文件

#include "condition.h"

//任务结构体，将任务放入队列由线程池中的线程来执行
typedef struct task
{
    void *(*run)(void *args);  //任务回调函数
    void *arg;              //参数
    struct task *next;      //任务队列中下一个任务
}task_t;


//下面是线程池结构体
typedef struct threadpool
{
    condition_t ready;    //任务准备就绪或者线程池销毁通知
    task_t *first;       //任务队列中第一个任务
    task_t *last;        //任务队列中最后一个任务
    int counter;         //线程池中当前线程数
    int idle;            //线程池中空闲线程数
    int max_threads;     //线程池最大允许线程数
    int quit;            //是否退出标志，1表示退出
}threadpool_t;


//线程池初始化
void threadpool_init(threadpool_t *pool, int threads);

//往线程池中加入任务
void threadpool_add_task(threadpool_t *pool, void *(*run)(void *arg), void *arg);

//摧毁线程池
void threadpool_destroy(threadpool_t *pool);

#endif