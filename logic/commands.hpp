#ifndef COMMANDS_HPP
#define COMMANDS_HPP


enum class Direction : unsigned char {
	UP = 'u',
	DOWN = 'd',
	LEFT = 'l',
	RIGHT = 'r',
};

std::ostream& operator << (std::ostream& out, const Direction& dir) {
	return out << static_cast<char>(dir);
}

struct Commands {
	static const char we[11]; 
	std::string login() {
		static std::string password = "Vegre";
		return std::string("/LOGIN ") + we + " " + password;
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

const char Commands::we[11] = "aludjunkRa"; 

#endif // COMMANDS_HPP
