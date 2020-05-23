#ifndef XTHREAD_POOL_H
#define XTHREAD_POOL_H

#ifdef _WIN32
#ifdef XCOM_EXPORTS
#define XCOM_API __declspec(dllexport)
#else
#define XCOM_API __declspec(dllimport)
#endif
#else
#define XCOM_API
#endif
#include <vector>
class XThread;
class XTask;
class XCOM_API XThreadPool
{
public:
    ///////////////////////////////////////////////////////////////////////////
    /// @brief 获取XThreadPool的静态对象 （静态函数）
    /// @return XThreadPool 静态对象的指针
    ///////////////////////////////////////////////////////////////////////////
	static XThreadPool* Get()
	{
		static XThreadPool p;
		return &p;
	}
    ///////////////////////////////////////////////////////////////////////////
    /// @brief 初始化所有线程并启动线程，创建号event_base ,并在线程中开始接收消息
	void Init(int thread_count);

    ///////////////////////////////////////////////////////////////////////////
    /// @brief 分发任务到线程中执行，会调用task的Init进行任务初始化
    ///        任务会轮询分发到线程池中的各个线程
    /// @param task 任务接口对象，XTask需要用户自己继承并重载Init函数
	void Dispatch(XTask *task);

    XThreadPool() {};
private:
	///线程数量
	int thread_count_ = 0;

    ///上一次分发得到线程，用于轮询
	int last_thread_ = -1;

	///程池线程队列
	std::vector<XThread *>threads_;
	

};

#endif