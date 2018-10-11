#pragma once
#include <exception>
namespace mml::Exceptions {
	class MMLException : std::exception {
	public: using std::exception::exception;
	};
}
#define DefineNewMMLException(name) namespace mml::Exceptions {class name : public mml::Exceptions::MMLException {public: using MMLException::MMLException;};}