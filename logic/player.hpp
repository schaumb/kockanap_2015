#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "gameitem.hpp"

class Player : public GameItem
{
    public:
    	static constexpr const int typeId = 1;

		virtual int type() override {
			return typeId;
		}

    	Player(pugi::xml_node& info) : GameItem(info) {}

		std::string getNickName() {
			return get("NickName");
		}

        float getWalkSpeed() {
            return get_float("WalkSpeed");
        }

        int getBombsAllowed() {
			return get_int("BombsAllowed");
		}

		int getBombSize() {
			return get_int("BombSize");
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

		int getDX() {
			return get_int("DX");
		}

		int getDY() {
			return get_int("DY");
		}

		int getDied() {
			return get_int("Died");
		}

		int getKills() {
			return get_int("Kills");
		}
};

#endif // PLAYER_HPP
