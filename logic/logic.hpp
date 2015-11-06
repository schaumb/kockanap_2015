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
		this->write({{commands.login() + " " + getUdpString() }});
	}
	
	virtual void readable() override {
		static int i = 0;
		this->write({{commands.move(static_cast<Direction>(++i %= 4))}});
	}
};

#endif // LOGIC_HPP
