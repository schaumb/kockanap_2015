#ifndef LOGIC_HPP
#define LOGIC_HPP

#include "../server/connection.hpp"
#include "../server/options.hpp"
#include "commands.hpp"

class Logic : Connection
{
	Commands commands;
public:
	Logic(const Options& options) : Connection(options) {
		
	}

	using Selector::run;
	virtual void preRun() override {
		this->write({{commands.login()}});
	}
	
	virtual void readable() override {
		
	}
};

#endif // LOGIC_HPP
