#include <cstring>

#include "sys/error.hh"

using namespace sys;

error::error(int errno_saved):
	std::runtime_error(std::strerror(errno_saved)),
	_errno(errno_saved)
{
}
