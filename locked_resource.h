#ifndef __LOCKED_RESOURCE_H__
#define __LOCKED_RESOURCE_H__

#include <mutex>
#include <assert.h>

namespace zy{

template<typename T>
class ResourceHolder;


template<typename T>
class LockedResource
{
public:
	template<typename ...Args>
	LockedResource(Args&& ...args) : resource(std::forward<Args>(args)...)
	{ }

	ResourceHolder<T> get_resource_holder()
	{
		return ResourceHolder<T>(&lock, &resource);
	}

protected:
	std::mutex lock;
	T resource;
};


template<typename T>
class ResourceHolder
{
public:
	T* operator ->(){ return resource; }

public:
	ResourceHolder(std::mutex* _lock, T* _resource) :
		lock(_lock), resource(_resource)
	{
		assert(lock);
		assert(resource);

		lock->lock();
	}
	~ResourceHolder()
	{
		if (lock)
			lock->unlock();
	}

	ResourceHolder(const ResourceHolder&) = delete;
	ResourceHolder& operator=(const ResourceHolder&) = delete;
	ResourceHolder(ResourceHolder&& rhs)
	{
		*this = std::move(rhs);
	}

	ResourceHolder& operator=(ResourceHolder&& rhs)
	{	
		this->lock = rhs.lock;
		rhs.lock = nullptr;
		this->resource = rhs.resource;
		rhs.resource = nullptr;

		return *this;
	}
protected:
	std::mutex* lock;
	T* resource;
};


template<typename T>
class LockedVariable
{
public:
	T get()
	{
		std::lock_guard<std::mutex> lock_guard(lock);
		return value;
	}

	template<typename U,
	typename = typename std::enable_if<std::is_convertible<U, T>::value>::type>
	void set(U&& _value)
	{
		std::lock_guard<std::mutex> lock_guard(lock);
		value = std::forward<U>(_value);
	}

protected:
	T value;
	std::mutex lock;
};

} /*namespace zy*/

#endif /*__LOCKED_RESOURCE_H__*/
