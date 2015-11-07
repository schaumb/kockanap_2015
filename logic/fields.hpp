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
	int howDanger[20][20];
	std::vector<std::shared_ptr<GameItem>> otherItems;
	std::vector<std::shared_ptr<Bomb>> bombs;
	std::vector<std::shared_ptr<Player>> players;
	std::shared_ptr<Player> we;
public:

	void parse(pugi::xml_document& doc) {
		for(auto& it : fields) {
			for(auto& field : it) {
				field.clear();
			}
		}
		we = nullptr;
		otherItems.clear();
		bombs.clear();
		players.clear();

		auto items = doc.child("GameItems");

		for (pugi::xml_node tool: items.children("Player")) {
			
			auto player = std::make_shared<Player>(tool);
			
			auto& shr = get(fields, player->getPos());

			shr.push_back(player);
			players.push_back(player);
		}
		for (pugi::xml_node tool: items.children("Bomb")) {
			auto bomb = std::make_shared<Bomb>(tool);
			
			auto& shr = get(fields, bomb->getPos());
			shr.push_back(bomb);
			bombs.push_back(bomb);
		}
		for (pugi::xml_node tool: items.children("GameItem")) {
			auto item = std::make_shared<GameItem>(tool);
			
			auto& shr = get(fields, item->getPos());

			shr.push_back(item);
			otherItems.push_back(item);
		}
		
		for(auto& it : fields) {
			for(auto& itemX : it) {
				if(itemX.empty()) break;
				auto item = itemX.front();
				if(item) {
					char c = item->getTileChar();
					if(item->type() == Player::typeId) {
						if(dynamic_cast<Player*>(item.get())->getNickName() == Commands::we) {
							std::cout << '*';
							we = std::dynamic_pointer_cast<Player>(item);
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

	void setDangerZones() {
		for(auto bombptr : bombs) {
			int radius = bombptr->getRadius();
			auto coord = bombptr->getPos();
			for(int i = -radius; i < radius; ++i) {
				auto ncoord = getNext(coord, Direction::DOWN, i);
				get(howDanger, ncoord) += 10 - bombptr->getTimeLeft();
				
				ncoord = getNext(coord, Direction::RIGHT, i);
				get(howDanger, ncoord) += 10 - bombptr->getTimeLeft();
			}
		}
	}
	std::set<Direction> directions() {
		std::set<Direction> dirs;
		if(we) {
			auto wePos = we->getPos();
			for(auto dir : {Direction::UP, Direction::DOWN, Direction::LEFT, Direction::RIGHT}) {
				auto npos = getNext(wePos, dir);
				bool hasNotSolid = false;
				for(auto ptr : get(fields, npos)) {
					if(!ptr->getIsSolid()) {
						hasNotSolid = true;
						break;
					}
				}
				if(!hasNotSolid) {
					dirs.insert(dir);
				}
			}
		}
		return dirs;
	}
	
	std::vector<std::string> moves(int& millisecToNext) {
		std::vector<std::string> result;

		setDangerZones();
		
		if(we) {
			//auto wePos = we->getPos();
			if(we->getState() == "Standing") {
				auto dirs = directions();
				if(dirs.size()) {
					std::cerr << dirs.size() << " " << *dirs.begin() << std::endl;
					result.push_back(Commands::move(*dirs.begin()));
				}
			}
			else {
				result.push_back(Commands::stop());
			}
		}
		std::cout << we << "RESULT SIZE" << result.size() << std::endl;
		millisecToNext = 500;
		return result;
	}
};

#endif // FIELDS_HPP


