#ifndef XTHREAD_H
#define XTHREAD_H

#include <list>
#include <mutex>
class XTask;
class XThread
{
public:

	//�����߳�
	void Start();

	//�߳���ں���
	void Main();

	//��װ�̣߳���ʼ��event_base�͹ܵ������¼����ڼ���
	bool Setup();

	//�յ����̷߳����ļ�����Ϣ���̳߳صķַ���
	void Notify(int fd, short which);

	//�̼߳���
	void Activate();

	//��Ӵ��������һ���߳�ͬʱ���Դ��������񣬹���һ��event_base
	void AddTask(XTask *t);
	XThread();
	~XThread();

	//�̱߳��
	int id = 0;
private:
	int notify_send_fd_ = 0;
	struct event_base *base_ = 0;

	//�����б�
	std::list<XTask*> tasks_;
	//�̰߳�ȫ ����
	std::mutex tasks_mutex_;

};

#endif
