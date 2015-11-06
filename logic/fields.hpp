#ifndef FIELDS_HPP
#define FIELDS_HPP

#include "../pugixml/pugixml.hpp"
#include "player.hpp"
#include "bomb.hpp"
#include "commands.hpp"
#include "moves.hpp"
#include <boost/optional.hpp>
#include <boost/none.hpp>



class Fields
{
	std::vector<std::shared_ptr<GameItem>> fields[20][20];
	std::vector<std::shared_ptr<GameItem>> otherItems;
	std::vector<std::shared_ptr<Bomb>> bombs;
	std::vector<std::shared_ptr<Player>> players;
	Coordinate ourCoord;
public:
	void parse(pugi::xml_document& doc) {
		for(auto& it : fields) {
			for(auto& field : it) {
				field.clear();
			}
		}
		otherItems.clear();
		bombs.clear();
		players.clear();

		auto items = doc.child("GameItems");
		auto count = items.attribute("objects").as_int();
		std::cout << "Item count " << count << std::endl;

		for (pugi::xml_node tool: items.children("Player")) {
			
			auto player = std::make_shared<Player>(tool);
			
			auto& shr = fields[player->getPosX()][player->getPosY()];

			shr.push_back(player);
			players.push_back(player);
		}
		for (pugi::xml_node tool: items.children("Bomb")) {
			auto bomb = std::make_shared<Bomb>(tool);
			
			auto& shr = fields[bomb->getPosX()][bomb->getPosY()];
			shr.push_back(bomb);
			bombs.push_back(bomb);
		}
		for (pugi::xml_node tool: items.children("GameItem")) {
			auto item = std::make_shared<GameItem>(tool);
			
			auto& shr = fields[item->getPosX()][item->getPosY()];

			shr.push_back(item);
			otherItems.push_back(item);
		}
		
		for(auto& it : fields) {
			for(auto& itemX : it) {
				auto& item = itemX.front();
				if(item) {
					char c = item->getTileChar();
					if(item->type() == Player::typeId) {
						if(dynamic_cast<Player*>(item.get())->getNickName() == Commands::we) {
							std::cout << '*';
							ourCoord = item->getPos();
							continue;
						}
					}
					std::cout << (c == 'Z' ? ' ' : c);
				}
				else {
					std::cout << " ";
				}
			}
			std::cout << std::endl;
		}
	}
	
	std::vector<std::string> moves() {
		std::set<Coordinate> dangerousZone;
		
		static char arr[] = {'u', ' ', 'd', ' ', 'l', ' ', 'r', ' '};
		static int i = 0;
		
		return {Commands::move(static_cast<Direction>(arr[++i %= 8])), Commands::speed(0.5)};
	}
};

#endif // FIELDS_HPP


