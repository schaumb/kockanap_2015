#ifndef BOMB_HPP
#define BOMB_HPP

#include "gameitem.hpp"
#include "player.hpp"

class Bomb : public GameItem
{
		int timeLeft;
		Player owner;
    public:
    	static constexpr const int typeId = 2;

		virtual int type() override {
			return typeId;
		}

    	Bomb(const pugi::xml_node& info) : GameItem(info), 
    		owner(info.child("Owner")) {
    		timeLeft = get_int("TimeLeft");
    	}

        int getTimeLeft() {
			return timeLeft;
		}

		Player getOwner() {
			return owner;
		}
		
		int getRadius() {
			return getOwner().getBombSize();
		}

};

#endif // BOMB_HPP
