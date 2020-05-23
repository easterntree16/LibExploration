#include "xthread_pool.h"
#include "xthread.h"
#include <thread>
#include <iostream>
#include "xtask.h"
using namespace std;

//分发线程
void XThreadPool::Dispatch(XTask *task)
{
	//轮询
	if (!task)return;
	int tid = (last_thread_ + 1) % thread_count_;
	last_thread_ = tid;
	XThread *t = threads_[tid];

	t->AddTask(task);

	//激活线程
	t->Activate();


}
//初始化所有线程并启动线程
void XThreadPool::Init(int thread_count)
{
	this->thread_count_ = thread_count;
	this->last_thread_ = -1;
	for (int i = 0; i < thread_count; i++)
	{
		XThread *t = new XThread();
		t->id = i + 1;
		cout << "Create thread " << i << endl;
		//启动线程
		t->Start();
		threads_.push_back(t);
		this_thread::sleep_for(10ms);
	}
}