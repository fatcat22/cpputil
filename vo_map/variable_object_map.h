#ifndef __VARIABLE_OBJECT_MAP_H__
#define __VARIABLE_OBJECT_MAP_H__

#include "ScopeGuard.hpp"
#include "type_classify.h"
#include <type_traits>
#include <stdint.h>
#include <unordered_map>
#include <memory>


namespace zy{
	
class numberic_t
{
public:
	template<typename T,
		typename = typename std::enable_if<std::is_integral<T>::value>::type
	>
	numberic_t(T value){
		data.ulonglong = value;
	}

	numberic_t(float value){
		data.single_float = value;
	}

	numberic_t(double value){
		data.double_float = value;
	}

	numberic_t(bool value){
		data.boolean_value = value;
	}

	template<typename T>
	T get()
	{
		static_assert(std::is_integral<T>::value, "T must integral type.");
		return get_spec(T{});
	}

private:
	template<typename T>
	T get_spec(T)
	{
		return static_cast<T>(data.ulonglong);
	}

	double get_spec(double)
	{
		return data.double_float;
	}

	float get_spec(float)
	{
		return data.single_float;
	}

	bool get_spec(bool)
	{
		return data.boolean_value;
	}

private:
	union{
		bool boolean_value;
		unsigned long long ulonglong;
		double double_float;
		float single_float;
	} data;
};


template<typename IdType>
class VariableObjectMap
{
public:
	template<typename T> 
	void add_object(const IdType& id, T&& value)
	{
		add_object(id, std::forward<T>(value), zy::type_classify<T>());
	}

	template<typename T>
	bool get_object(const IdType& id, T& value)
	{
		return get_object(id, value, zy::type_classify<T>());
	}

	void rm_object(const IdType& id)
	{
		if (number_object_map.erase(id) == 0)
			void_object_map.erase(id);
	}

private:
	template<typename T>
	void add_object(const IdType& id, T&& value, zy::number_t)
	{
		number_object_map.emplace(id, numberic_t(value));
	}

	template<typename T>
	void add_object(const IdType& id, T&& value, zy::enumeration_t)
	{
		add_object(id, static_cast<typename std::underlying_type<typename std::decay<T>::type>::type>(value), zy::number_t());
	}

	template<typename T>
	void add_object(const IdType& id, T&& value, zy::unknown_t)
	{
		typename std::decay<T>::type *object = nullptr;
		ScopeGuard scope_guard([&object] {if (object) delete object; });

		object = new typename std::decay<T>::type(std::forward<T>(value));
		void* void_ptr = static_cast<void*>(object);

		scope_guard.dismiss();
		std::shared_ptr<void> shared_obj(void_ptr, [object](void*) {VariableObjectMap::deletor<typename std::decay<T>::type>(object); });
		void_object_map.emplace(id, std::move(shared_obj));
	}

private:
	template<typename T>
	bool get_object(const IdType& id, T& value, zy::number_t)
	{
		auto it = number_object_map.find(id);
		if (it == number_object_map.end())
			return false;

		numberic_t& tmp = it->second;
		value = tmp.get<typename std::decay<T>::type>();
		return true;
	}

	template<typename T>
	bool get_object(const IdType& id, T& value, zy::enumeration_t)
	{
		typename std::underlying_type<typename std::decay<T>::type>::type numberic_value;

		if (get_object(id, numberic_value, zy::number_t()) != true)
			return false;

		value = static_cast<T>(numberic_value);
		return true;
	}

	template<typename T>
	bool get_object(const IdType& id, T& value, zy::unknown_t)
	{
		auto it = void_object_map.find(id);
		if (it == void_object_map.end())
			return false;

		value = *(static_cast<typename std::decay<T>::type *>(it->second.get()));
		return true;
	}

private:
	template<typename T>
	static void deletor(void* buffer)
	{
		auto data = static_cast<T*>(buffer);
		delete data;
	}

private:
	std::unordered_map<IdType, numberic_t> number_object_map;
	std::unordered_map<IdType, std::shared_ptr<void>> void_object_map;
};

} /*namespace zy*/

#endif /*__VARIABLE_OBJECT_MAP_H__*/
