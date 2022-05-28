#pragma once

#include "socket.hpp"

class Base
{
protected:
	// attributes
	Socket	*socket;

public:
	// destructor
	virtual ~Base() {}
};