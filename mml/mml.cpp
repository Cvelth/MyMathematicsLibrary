#include "mml.hpp"

#include <string>
#include <sstream>
char const* mgl::get_version() {
	std::ostringstream s;
	s << Version_Major << '.' << Version_Minor << '.' << Version_Patch << '(' << Version_Build << ')';
	return s.str().c_str();
}