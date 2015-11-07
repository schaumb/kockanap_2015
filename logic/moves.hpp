#ifndef MOVES_HPP
#define MOVES_HPP

#include <tuple>
#include <iostream>


using Coordinate = std::tuple<int, int>;

std::ostream& operator<<(std::ostream& o, const Coordinate& c) {
	return o << "{" << std::get<0>(c) <<"," << std::get<1>(c) << "}";
}

template<class Container>
auto& get(Container&& c, Coordinate coord) {
	return c[std::get<1>(coord)][std::get<0>(coord)];
}


enum class Direction : unsigned char {
	UP = 'u',
	DOWN = 'd',
	LEFT = 'l',
	RIGHT = 'r',
};

Coordinate getNext(Coordinate coord, Direction dir, int count = 1) {
	switch(dir) {
	case Direction::UP:
		count = -count;
	case Direction::DOWN:
		if(0 <= std::get<1>(coord) + count && std::get<1>(coord) + count < 20) {
			std::get<1>(coord) += count;
		}
		break;
	case Direction::LEFT:
		count = -count;
	case Direction::RIGHT:
		if(0 <= std::get<0>(coord) + count && std::get<0>(coord) + count < 20) {
			std::get<0>(coord) += count;
		}
		break;
	default:
		break;
	}
	return coord;
}

bool isSymmetric(Direction dir1, Direction dir2) {
	switch(dir1) {
	case Direction::UP:
		return dir2 == Direction::DOWN;
	case Direction::DOWN:
		return dir2 == Direction::UP;
	case Direction::LEFT:
		return dir2 == Direction::RIGHT;
	case Direction::RIGHT:
		return dir2 == Direction::LEFT;	
	}
	return false;
}

Direction getDir(Coordinate coord) {
	switch(std::get<0>(coord)) {
	case 1 : 
		return Direction::RIGHT;
	case -1:
		return Direction::LEFT;
	default:
		switch(std::get<1>(coord)) {
		case 1:
			return Direction::DOWN;
		case -1:
			return Direction::UP;
		default:
			break;
		}
	}
	return {};
}

std::ostream& operator << (std::ostream& out, const Direction& dir) {
	return out << static_cast<char>(dir);
}

class Moves {
	
public:
	template<class Fields>
	static std::set<Direction> directions(Fields&& fields, Coordinate position, std::set<char> skip = {'W', 'V', 'X', 'Y'}) {
		std::set<Direction> dirs;
		for(auto dir : {Direction::UP, Direction::DOWN, Direction::LEFT, Direction::RIGHT}) {
			auto npos = getNext(position, dir);

			bool hasSkipped = false;
			for(auto ptr : get(fields, npos)) {
				if(skip.count(ptr->getTileChar())) {
					hasSkipped = true;
					break;
				}
			}
			if(!hasSkipped) {
				dirs.insert(dir);
			}
		}
		return dirs;
	}
	
	template<class Fields>
	static bool deadEnd(Fields&& fields, Coordinate from, Direction to, std::set<char> skip = {'W', 'V', 'X', 'Y'}) {
		if(directions(fields, from, skip).count(to)) {
			Direction dir = to;
			Coordinate next = getNext(from, to);
			auto nextDirs = directions(fields, next);
			std::cerr << "Deadend?" << from << " " << to << " " << nextDirs.size() << std::endl;
			switch(nextDirs.size()) {
			case 1: 
				return true;
			case 4:
			case 3:
				return false;
			case 2:
				if(nextDirs.count(to)) {
					return deadEnd(fields, next, to, skip);
				}
				else {
					for(auto dirx : nextDirs) {
						if(!isSymmetric(dir, dirx)) {
							return deadEnd(fields, next, dirx, skip);
						}
					}
				}
			}
		}
		return false;
	}
};

#endif // MOVES_HPP


