#include "../Inc/Buffer.h"

namespace tanfy
{

Buffer::Buffer(size_t maxsize)
	:mutex_(),
	 notFull_(mutex_),
	 notEmpty_(mutex_),
	 maxsize_(maxsize),
	 ifWakeEmpty_(false)
{
}

bool Buffer::empty()
{
	return (0 == que_.size());
}

bool Buffer::full()
{
	return (maxsize_ == que_.size());
}

void Buffer::push(Task *mytask)
{
	mutex_.lock();
	
	while(full())
	{	
		notFull_.wait();
	}
	que_.push(mytask);
	notEmpty_.notify();

	mutex_.unlock();
}

Task *Buffer::pop()
{
	mutex_.lock();

	while(empty() && !ifWakeEmpty_)
	{
		notEmpty_.wait();
	}
	Task *mytask = NULL;
	if(!ifWakeEmpty_)
	{
		mytask = que_.front();
		que_.pop();
		notFull_.notify();
	}

	mutex_.unlock();
	return mytask;
}

void Buffer::wakeup_empty()
{
	notEmpty_.notifyall();
	ifWakeEmpty_ = true;
}

}//end of namespace
