#pragma once
#include <string>
#include <stdexcept>
#include <iostream>
namespace myopengl{
#define src_location() std::string(std::string("[") + __FILE__ + ":" + std::to_string(__LINE__) + "]")
inline void throw_exception_(const std::string& msg){
	throw std::runtime_error(msg);
}
#define throw_exception(msg) do{::myopengl::throw_exception_(src_location() + msg);}while(0);

}
