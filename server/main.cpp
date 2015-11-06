#include "../logic/logic.hpp"
#include "options.hpp"

int main(int argc, char const* const* argv) {
	auto options = OptionParser<Options>{argc, argv}.getOptions();
	
	Logic{ options }.run();
}
