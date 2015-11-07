#ifndef MOVES_HPP
#define MOVES_HPP

#include <tuple>

using Coordinate = std::tuple<int, int>;

template<class Container>
auto& get(Container&& c, Coordinate coord) {
	return c[std::get<1>(coord)][std::get<0>(coord)];
}

class Moves {
	
public:
};

#endif // MOVES_HPP


