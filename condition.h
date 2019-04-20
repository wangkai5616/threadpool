#ifndef _CONDITION_H_
#define _CONDITION_H_

#include <pthread.h>

//��װһ��������������������Ϊ״̬
typedef struct condition
{
	//�����������Ǻͻ��������ʹ��
    pthread_mutex_t pmutex;
    pthread_cond_t pcond;
}condition_t;

//��״̬�Ĳ�������

//��ʼ��һ����������
int condition_init(condition_t *cond);
//�Ի���������
int condition_lock(condition_t *cond);
//�Ի���������
int condition_unlock(condition_t *cond);
//����������֮�ϵȴ�����
int condition_wait(condition_t *cond);
//���г�ʱ�ĵȴ�����
int condition_timedwait(condition_t *cond, const struct timespec *abstime);
//��ȴ��̷߳���֪ͨ
int condition_signal(condition_t* cond);
//��ȴ��㲥
int condition_broadcast(condition_t *cond);
//������������
int condition_destroy(condition_t *cond);

#endif
