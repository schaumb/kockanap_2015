#ifndef SERVEROPTIONS_HPP
#define SERVEROPTIONS_HPP

#include <boost/program_options.hpp>

struct Options {
	std::string serverName;
	unsigned short port;
	std::size_t timeoutMillisec;
	std::size_t tryNToConect;
};

inline boost::program_options::options_description description(Options& options) {
	using namespace boost::program_options;
	
	options_description description("Client");
	description.add_options()
		("serverName,s", value<std::string>(&options.serverName)->default_value("224.5.6.7"), "Server name.")
		("port,p", value<unsigned short>(&options.port)->default_value(6666), "Server port.")
		("timeout,t", value<std::size_t>(&options.timeoutMillisec)->default_value(2000), "Server connection timeout.")
		("tries,c", value<std::size_t>(&options.tryNToConect)->default_value(20), "Server connection tries.");
	return description;
}

#include "optionParser.hpp"

#endif // SERVEROPTIONS_HPP
