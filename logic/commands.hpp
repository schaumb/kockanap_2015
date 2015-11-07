#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include "moves.hpp"

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

std::ostream& operator << (std::ostream& out, const Direction& dir) {
	return out << static_cast<char>(dir);
}



struct Commands {
	static const char we[];
	static std::string login() {
		static std::string password = "Vegrex";
		return std::string("/LOGIN ") + we + " " + password;
	}
	static std::string left() {
		return "/LEFT";
	}
	static std::string right() {
		return "/RIGHT";
	}
	static std::string up() {
		return "/UP";
	}
	static std::string down() {
		return "/DOWN";
	}
	static std::string reverse() {
		return "/REVERSE";
	}
	static std::string stop() {
		return "/STOP";
	}
	static std::string bomb() {
		return "/BOMB";
	}
	static std::string bombSize(int exp) {
		return "/BOMBSIZE " + std::to_string(exp);
	}
	static std::string bombNum(int num) {
		return "/BOMBNUM " + std::to_string(num);
	}
	static std::string speed(double sp) {
		return "/SPEED " + std::to_string(sp);
	}

	static std::string move(const Direction& dir = {}) {
		switch(dir) {
		case Direction::UP :
			return up();
		case Direction::DOWN :
			return down();
		case Direction::LEFT :
			return left();
		case Direction::RIGHT :
			return right();
		default:
			std::cerr << "no exist direction" << std::endl;
			return stop();
		}
	}
};

const char Commands::we[] = "aludjunkRá"; 

#endif // COMMANDS_HPP
