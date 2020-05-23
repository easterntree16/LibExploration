#include "xthread_pool.h"
#include "xthread.h"
#include <thread>
#include <iostream>
#include "xtask.h"
using namespace std;

//�ַ��߳�
void XThreadPool::Dispatch(XTask *task)
{
	//��ѯ
	if (!task)return;
	int tid = (last_thread_ + 1) % thread_count_;
	last_thread_ = tid;
	XThread *t = threads_[tid];

	t->AddTask(task);

	//�����߳�
	t->Activate();


}
//��ʼ�������̲߳������߳�
void XThreadPool::Init(int thread_count)
{
	this->thread_count_ = thread_count;
	this->last_thread_ = -1;
	for (int i = 0; i < thread_count; i++)
	{
		XThread *t = new XThread();
		t->id = i + 1;
		cout << "Create thread " << i << endl;
		//�����߳�
		t->Start();
		threads_.push_back(t);
		this_thread::sleep_for(10ms);
	}
}