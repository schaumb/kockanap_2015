#ifndef FIELDS_HPP
#define FIELDS_HPP

#include "../pugixml/pugixml.hpp"
#include "player.hpp"
#include "bomb.hpp"
#include "commands.hpp"
#include <boost/optional.hpp>
#include <boost/none.hpp>

class Fields
{
	std::shared_ptr<GameItem> fields[20][20];
	
	
public:
	void parse(pugi::xml_document& doc) {
	
		for(auto& it : fields) {
			for(auto& item : it) {
				item = nullptr;
			}
		}
		
		auto items = doc.child("GameItems");
		auto count = items.attribute("objects").as_int();
		std::cout << "Item count " << count << std::endl;

		for (pugi::xml_node tool: items.children("Player")) {
			
			auto player = std::make_shared<Player>(tool);
			
			auto& shr = fields[player->getPosX()][player->getPosY()];

			shr = player;
		}
		for (pugi::xml_node tool: items.children("Bomb")) {
			auto bomb = std::make_shared<Bomb>(tool);
			
			auto& shr = fields[bomb->getPosX()][bomb->getPosY()];
			
			shr = bomb;
		}
		for (pugi::xml_node tool: items.children("GameItem")) {
			auto item = std::make_shared<GameItem>(tool);
			
			auto& shr = fields[item->getPosX()][item->getPosY()];
			
			shr = item;

		}
		
		for(auto& it : fields) {
			for(auto& item : it) {
				if(item) {
					char c = item->getTileChar();
					if(item->type() == Player::typeId) {
						if(dynamic_cast<Player*>(item.get())->getNickName() == Commands::we) {
							std::cout << '*';
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
		return {};
	}
};

#endif // FIELDS_HPP


