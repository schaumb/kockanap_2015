#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "gameitem.hpp"
#include <string>

class Player : public GameItem
{
		std::string nickName;
		int bombSize;
		int dx;
		int dy;
    public:
    	static constexpr const int typeId = 1;

		virtual int type() override {
			return typeId;
		}

    	Player(const pugi::xml_node& info) : GameItem(info),
    		nickName(get("NickName")),
	    	bombSize(get_int("BombSize")),
	    	dx(get_int("DX")),
	    	dy(get_int("DY"))
    	{
    	
    	}

		std::string getNickName() {
			return nickName;
		}

        float getWalkSpeed() {
            return get_float("WalkSpeed");
        }

        int getBombsAllowed() {
			return get_int("BombsAllowed");
		}

		int getBombSize() {
			return bombSize;
		}

		int getBombsDetonated() {
			return get_int("BombsDetonated");
		}

		int getSecretsCollected() {
			return get_int("SecretsCollected");
		}

		int getNoClipStepsRemaining() {
			return get_int("NoClipStepsRemaining");
		}

		int getArmorStepsRemaining() {
			return get_int("ArmorStepsRemaining");
		}

		int getSpeedUpStepsRemaining() {
			return get_int("SpeedUpStepsRemaining");
		}

		int getNoBombStepsRemaining() {
			return get_int("NoBombStepsRemaining");
		}

		int getSpeedDownStepsRemaining() {
			return get_int("SpeedDownStepsRemaining");
		}

		int getAlwaysBombStepsRemaining() {
			return get_int("AlwaysBombStepsRemaining");
		}

		bool getMustStop() {
			return get_bool("MustStop");
		}

		std::string getState() {
            return get("State");
		}

		int getBombsLeft() {
			return get_int("BombsLeft");
		}

		auto getDX() {
			return dx;
		}

		auto getDY() {
			return dy;
		}
		
		auto getD() {
			return std::make_tuple(getDX(), getDY());
		}

		int getDied() {
			return get_int("Died");
		}

		int getKills() {
			return get_int("Kills");
		}
};

#endif // PLAYER_HPP
