#ifndef LOGIC_HPP
#define LOGIC_HPP

#include "../server/connection.hpp"
#include "../server/options.hpp"
#include "commands.hpp"
#include "fields.hpp"

class Logic : Connection
{
	Commands commands;
	Fields f;
	int millisecToNext = 300;
	pugi::xml_document doc;
public:
	Logic(const Options& options) : Connection(options) {
	}

	using Selector::run;
	virtual void preRun() override {
		this->write({{commands.login() + " " + getUdpString() }});
	}
	
	virtual inline sf::Time waitFor() override {
		return sf::milliseconds(millisecToNext);
	}
	virtual void timeout() {
		this->write(f.moves(millisecToNext));
	}
	
	virtual void readable() override {
		doc.reset();
		
		this->read(doc);
		
		f.parse(doc);
		timeout();
	}
};

#endif // LOGIC_HPP
