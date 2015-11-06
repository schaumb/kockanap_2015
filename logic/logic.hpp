#ifndef LOGIC_HPP
#define LOGIC_HPP

#include "../server/connection.hpp"
#include "../server/options.hpp"

class Logic : Connection
{

public:
	Logic(const Options& options) : Connection(options) {
		
	}

	using Selector::run;
	virtual void preRun() override {
		
	}
	
	virtual void readable() override {
		
	}
};

#endif // LOGIC_HPP
