#ifndef COMMANDS_HPP
#define COMMANDS_HPP


enum class Direction : unsigned char {
	UP = 'u',
	DOWN = 'd',
	LEFT = 'l',
	RIGHT = 'r',
};

std::ostream& operator << (std::ostream& out, const Direction& dir) {
	return out << std::static_cast<char>(dir);
}

struct Commands {

	std::string login() {
		static std::string userName = "";
		static std::string password = "";
		return "/LOGIN " + userName + " " + password;
	}
	std::string left() {
		return "/LEFT";
	}
	std::string right() {
		return "/RIGHT";
	}
	std::string up() {
		return "/UP";
	}
	std::string down() {
		return "/DOWN";
	}
	std::string reverse() {
		return "/REVERSE";
	}
	std::string stop() {
		return "/STOP";
	}
	std::string bomb() {
		return "/BOMB";
	}
	std::string bombSize(int exp) {
		return "/BOMBSIZE " + std::to_string(exp);
	}
	std::string bombNum(int num) {
		return "/BOMBNUM " + std::to_string(num);
	}
	std::string speed(double sp) {
		return "/SPEED " + std::to_string(sp);
	}


	std::string move(const Direction& dir) {
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
			return "";
		}
	}
};

#endif // COMMANDS_HPP
