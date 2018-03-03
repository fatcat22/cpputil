#ifndef __TYPE_CLASSIFY_H__
#define __TYPE_CLASSIFY_H__

#include <type_traits>

namespace zy{

enum class type_category
{
	number,  //integral, float point
	enumeration, //enum type
	//std_string, //std::string

	unknown,
};

template<type_category _V>
using type_category_t = std::integral_constant<type_category, _V>;

typedef type_category_t<type_category::number> number_t; 
typedef type_category_t<type_category::enumeration> enumeration_t;
//typedef type_category_t<type_category::std_string> std_string_t;
typedef type_category_t<type_category::unknown> unknown_t;



template<int TypeCode, typename T>
struct __type_classify : unknown_t
{};

//integral or floating point
template<typename T>
struct __type_classify<1, T> : number_t
{};

//enum
template<typename T>
struct __type_classify<2, T> : enumeration_t
{};

/*
template<>
struct __type_classify<0, std::string> : std_string_t
{};
*/




////////////////////////////////////////////////////////
//
template<typename T>
struct _type_classify : __type_classify<
	((int)std::is_arithmetic<T>::value) | 
	((int)std::is_enum<T>::value) << 1,
	T >
{};


template<typename T>
struct type_classify : _type_classify<typename std::decay<T>::type>
{
};



} /*namespace zy*/


#endif /*__TYPE_CLASSIFY_H__*/