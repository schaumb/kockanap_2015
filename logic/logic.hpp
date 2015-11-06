#ifndef LOGIC_HPP
#define LOGIC_HPP

#include "../server/connection.hpp"
#include "../server/options.hpp"
#include "commands.hpp"
#include "fields.hpp"

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
		std::cerr << "has readable data" << std::endl;
		pugi::xml_document doc;
		
		this->read(doc);
		
		Fields f;
		f.parse(doc);
		
		
		this->write(f.moves());
		static char arr[] = {'u', 'd', 'l', 'r'};
		static int i = 0;
		this->write({{commands.move(static_cast<Direction>(arr[++i %= 4]))}});
	}
};

#endif // LOGIC_HPP
