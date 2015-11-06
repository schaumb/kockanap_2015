#ifndef BOMB_HPP
#define BOMB_HPP

#include "gameitem.hpp"
#include "player.hpp"

class Bomb : public GameItem
{
    public:
    	static constexpr const int typeId = 2;

		virtual int type() override {
			return typeId;
		}

    	Bomb(pugi::xml_node& info) : GameItem(info) {}

        int getTimeLeft() {
			return get_int("TimeLeft");
		}

		/*Player getOwner() {

		}*/

};

#endif // BOMB_HPP
