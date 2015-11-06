#ifndef LOGIC_HPP
#define LOGIC_HPP

#include "connection.hpp"
#include "options.hpp"

class Logic : Connection
{

public:
	Logic(const Options& options) : Connection(options) {
		
	}

	using Selector::run;
	
	virtual void readable() override {

	}
};

#endif // LOGIC_HPP
