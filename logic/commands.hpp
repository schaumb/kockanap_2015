#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include "moves.hpp"


struct Commands {
	static std::string we;
	static std::string login() {
		static std::string password = "Vegre";
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

std::string Commands::we = "aludjunkRa"; 

#endif // COMMANDS_HPP
