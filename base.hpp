#pragma once

#include "socket.hpp"

class Base
{
protected:
	// attributes
	Socket	*socket;

public:
	// constructor
	Base()
	{
		socket = nullptr;
	}

	// destructor
	virtual ~Base()
	{
		if (socket)
		{
			socket->~Socket();
			delete socket;
		}
	}
};