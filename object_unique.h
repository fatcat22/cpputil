#ifndef __ZY_OBJECT_UNIQUE_H__
#define __ZY_OBJECT_UNIQUE_H__

namespace zy{

template<typename T>
class ObjectUnique
{
	typedef std::function<void(T)> Deleter;

public:
	ObjectUnique(T&& obj, Deleter&& d)
	{
		_obj = std::forward<T>(obj);
		_deleter = std::move(d);
		_valid = true;
	}

	explicit ObjectUnique(T&& obj = T())
	{
		_obj = std::move(obj);
		_valid = false;
	}

	~ObjectUnique()
	{
		clear();
	}

public:
	const T& get()
	{
		return _obj;
	}

	void clear()
	{
		if (true != _valid)
			return;

		_deleter(_obj);
		_valid = false;
	}

public:
	ObjectUnique(ObjectUnique<T>&& rhs)
	{
		*this = std::move(rhs);
	}

	ObjectUnique& operator=(ObjectUnique<T>&& rhs)
	{
		this->_obj = std::move(rhs._obj);
		this->_deleter = std::move(rhs._deleter);

		this->_valid = true;
		rhs._valid = false;

		return *this;

	}

	explicit ObjectUnique(const ObjectUnique<T>&) = delete;
	ObjectUnique<T>& operator=(const ObjectUnique<T>&) = delete;

private:
	bool _valid = false;
	T _obj;
	Deleter _deleter;
};

template<typename T, typename Deleter>
ObjectUnique<T>
	make_object_unique(T&& obj, Deleter&& d)
{
	return ObjectUnique<T>(std::forward<T>(obj), std::forward<Deleter>(d));
}


}/*namespace zy*/

#endif /*__ZY_OBJECT_UNIQUE_H__*/
