#ifndef GAMEITEM_HPP
#define GAMEITEM_HPP

class GameItem {
		pugi::xml_node info;
    protected:
    
		std::string get(const std::string& str) {
			return info.child_value(str.c_str());
		}

		int get_int(std::string str) {
			return std::stoi(get(str));
		}

		bool get_bool(std::string str) {
			return get(str)=="true";
		}

		float get_float(std::string str) {
            return std::stof(get(str));
		}

    public:
    	GameItem(pugi::xml_node& info) : info(info) {}

		int getId() {
			return get_int("ID");
		}

		char getTileChar() {
			return get_int("TileChar");
		}

		bool getIsKiller() {
			return get_bool("IsKiller");
		}

		bool getIsPickable() {
			return get_bool("IsPickable");
		}

		bool getIsExplodable() {
			return get_bool("IsExplodable");
		}

		bool getIsSolid() {
			return get_bool("IsSolid");
		}

		int getPosX() {
			return get_int("PosX");
		}

		int getPosY() {
			return get_int("PosY");
		}

	};

#endif // GAMEITEM_HPP
