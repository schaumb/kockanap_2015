#ifndef MOVES_HPP
#define MOVES_HPP

#include <tuple>
#include <iostream>
#include <deque>



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

int maxThere(Coordinate c, Direction d) {
	switch(d) {
	case Direction::UP:
		return std::get<1>(c);
		break;
	case Direction::LEFT:
		return std::get<0>(c);
		break;
	case Direction::RIGHT:
		return 19 - std::get<0>(c);
		break;
	case Direction::DOWN:
		return 19 - std::get<1>(c);
		break;
	}
	return 100;
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

Direction getVectorDir(Coordinate from, Coordinate to) {
	return getDir(Coordinate{std::get<0>(to) - std::get<0>(from), std::get<1>(to) - std::get<1>(from)});
}

bool neigbours(Coordinate coord1, Coordinate coord2) {
	if(coord1 == coord2) return false;
	for(auto dir : {Direction::UP, Direction::DOWN, Direction::LEFT, Direction::RIGHT}) {
		if(getNext(coord1, dir) == coord2) {
			return true;	
		}
	}
	return false;
}

bool closeTo(Coordinate c1, Coordinate c2, int radius = 1) {
	if(std::get<0>(c1) == std::get<0>(c2)) {
		return std::abs(std::get<1>(c1) - std::get<1>(c2)) <= radius;
	}

	if(std::get<1>(c1) == std::get<1>(c2)) {
		return std::abs(std::get<0>(c1) - std::get<0>(c2)) <= radius;
	}
	return false;
}

std::ostream& operator << (std::ostream& out, const Direction& dir) {
	return out << static_cast<char>(dir);
}

class Moves {
	
public:
	
	static auto directions(Coordinate position, std::set<Coordinate> solids) {
		std::vector<Direction> dirs;
		for(auto dir : {Direction::UP, Direction::DOWN, Direction::LEFT, Direction::RIGHT}) {
			auto npos = getNext(position, dir);

			if(!solids.count(npos)) {
				dirs.push_back(dir);
			}
		}
		std::sort(dirs.begin(), dirs.end(), [&position](Direction d1, Direction d2) {
			return maxThere(position, d1) < maxThere(position, d2);
		});
		return dirs;
	}
	
	static std::deque<Coordinate> routeTo(Coordinate from, Coordinate to, std::set<Coordinate> solids) {
		std::map<Coordinate, Coordinate> fater;
		
		fater[from] = from;
		
		std::deque<Coordinate> coords;
		coords.push_back(from);
		
		while(coords.size()) {
			Coordinate now = coords.front();
			coords.pop_front();
			
			for(auto dir : directions(now, solids)) {
				auto nextCoord = getNext(now, dir);
				if(!fater.count(nextCoord)) {
					fater[nextCoord] = now;
					if(nextCoord == to) {
						coords.clear();
						break;
					}
					else {
						coords.push_back(nextCoord);
					}
				}
			}
		}
		if(fater.count(to)) {
			std::deque<Coordinate> result;
			result.push_front(to);
			
			Coordinate next = fater[to];
			
			while(next != fater[next]) {
				result.push_front(next);
				next = fater[next];
			}
			return result;
		}
		else {
			return {};
		}
		
	}

	static std::vector<std::tuple<int, Coordinate, Direction>> reachableCoords(Coordinate position, std::set<Coordinate> solids) {
		std::set<Coordinate> used;
		std::vector<std::tuple<int, Coordinate, Direction>> elems;
		elems.reserve(300);

		used.insert(position);

		for(auto dir : directions(position, solids)) {
			auto next = getNext(position, dir);
			used.insert(next);
			elems.emplace_back(1, next, dir);
		}

		std::size_t where = 0;
		while(where != elems.size()) {
			for(auto dir : directions(std::get<1>(elems[where]), solids)) {
				auto next = getNext(std::get<1>(elems[where]), dir);
				if(!used.count(next)) {
					used.insert(next);
					elems.emplace_back(std::get<0>(elems[where]) + 1, next, std::get<2>(elems[where]));
				}
			}
			++where;
		}
		
		return elems;
	}
	
	static std::set<Coordinate> deadEnd(Coordinate from, Direction to, std::set<Coordinate> solids) {
		auto dirs = directions(from, solids);
		if(std::count(dirs.begin(), dirs.end(), to)) {
			Direction dir = to;
			Coordinate next = getNext(from, to);
			auto nextDirs = directions(next, solids);

			switch(nextDirs.size()) {
			case 1:
				return {from};
			case 4:
			case 3:
				return {};
			case 2:
				if(std::count(nextDirs.begin(), nextDirs.end(), to)) {
					return deadEnd(next, to, solids);
				}
				else {
					for(auto dirx : nextDirs) {
						if(!isSymmetric(dir, dirx)) {
							return deadEnd(next, dirx, solids);
						}
					}
				}
			}
		}
		return {from};
	}
};

#endif // MOVES_HPP


