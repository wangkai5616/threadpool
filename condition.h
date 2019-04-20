#ifndef _CONDITION_H_
#define _CONDITION_H_

#include <pthread.h>

//封装一个互斥量和条件变量作为状态
typedef struct condition
{
	//条件变量总是和互斥锁配合使用
    pthread_mutex_t pmutex;
    pthread_cond_t pcond;
}condition_t;

//对状态的操作函数

//初始化一个条件变量
int condition_init(condition_t *cond);
//对互斥锁锁定
int condition_lock(condition_t *cond);
//对互斥锁解锁
int condition_unlock(condition_t *cond);
//在条件变量之上等待条件
int condition_wait(condition_t *cond);
//具有超时的等待功能
int condition_timedwait(condition_t *cond, const struct timespec *abstime);
//向等待线程发起通知
int condition_signal(condition_t* cond);
//向等待广播
int condition_broadcast(condition_t *cond);
//销毁条件变量
int condition_destroy(condition_t *cond);

#endif
