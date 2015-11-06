#include "connection.hpp"
#include "options.hpp"

int main(int argc, char const* const* argv) {
	auto options = OptionParser<Options>{argc, argv}.getOptions();
}
