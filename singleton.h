#ifndef __SINGLETON_H__
#define __SINGLETON_H__

#include <mutex>
#include <memory>


namespace zy{

template<typename T>
class Singleton
{
public:
	struct Initializer
	{
		Initializer() {
			Singleton<T>::get_once_flag();
			Singleton<T>::get_instance();
		}
		void do_nothing() const {}
	};
	static Initializer I;

public:
	static std::shared_ptr<T> instance()
	{
		I.do_nothing();
		std::call_once(get_once_flag(), []{get_instance().reset(new T); });
		return get_instance();
	}

private:
	static std::once_flag& get_once_flag()
	{
		static std::once_flag once_flag;
		return once_flag;
	}

	static std::shared_ptr<T>& get_instance()
	{
		static std::shared_ptr<T> instance_;
		return instance_;
	}
};

template<typename T>
typename Singleton<T>::Initializer Singleton<T>::I;

} /*namespace zy*/

#endif /*__SINGLETON_H__*/