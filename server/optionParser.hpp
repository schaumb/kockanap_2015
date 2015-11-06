#ifndef OPTIONPARSER_HPP
#define OPTIONPARSER_HPP

#include <boost/program_options.hpp>

/*
	Need a struct, and a free description function with one parameter

	example:
	
	#include <boost/program_options.hpp>
	
	struct Stg {
		int num;
		std::string str;
		bool is_true;
	};

	boost::program_options::options_description description(Stg& stg) {
		using namespace boost::program_options;
		
		options_description description("ProgramName");
		description.add_options()
			("num,n", value<int>(&stg.num)->default_value(11162), "Some information.")
			("str,s", value<std::string>(&stg.str)->required(),	"String info.")
			("bool,b", bool_switch(&stg.is_true)->default_value(false), "Some bool info");
		return description;
	}

	#include <general/optionParser.hpp>


	Using:

	Stg opt = general::OptionParser<Stg>{argc, argv}.getOptions();
	
*/

template<class OptionsStruct>
class OptionParser {
public:
	OptionParser(int argc, const char* const * argv) :
		argc(argc), argv(argv), options(), 
		optionDescription(::description(options))
	{
		using namespace boost::program_options;
		bool help = false;
		optionDescription.add_options()
			("help,h", bool_switch(&help), "Print this help message.");

		try {
			parsed_options parsed = 
				command_line_parser(argc, argv).
					options(optionDescription).run();
		
			store(parsed, variableMap);
			notify(variableMap);
		}
		catch (std::exception & e){		
		    std::cerr << e.what() << std::endl << std::endl;
		    help = true;
		}
		
		if(help)
		{
			optionDescription.print(std::cerr);
			std::exit(1);
		}
	}

	const OptionsStruct& getOptions() const {
		return options;
	}

private:
	const int argc;
	const char* const * argv;
	OptionsStruct options;
	boost::program_options::variables_map variableMap;
	boost::program_options::options_description optionDescription;
};

#endif // OPTIONPARSER_HPP
