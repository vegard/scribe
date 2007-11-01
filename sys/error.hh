#ifndef SYS_ERROR_HH
#define SYS_ERROR_HH

#include <stdexcept>

namespace sys {

class error:
	public std::runtime_error
{
public:
	error(int errno_saved);

private:
	int _errno;
};

}

#endif
