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
	std::set<Coordinate> solid;
	std::set<Coordinate> dangerous;
	std::shared_ptr<Player> we;
	std::vector<std::shared_ptr<Bomb>> bombs;
public:
	void insertStuffs(std::shared_ptr<GameItem> ptr) {
		if(ptr->getIsSolid()) {
			solid.insert(ptr->getPos());
		}
		if(ptr->getTileChar() == 'X' || ptr->getTileChar() == 'Y') {
			dangerous.insert(ptr->getPos());
		}
	}
	
	void insertDanger(std::shared_ptr<Bomb> bptr) {
		int radius = bptr->getRadius();
		auto pos = bptr->getPos();
		for(auto dir : {Direction::UP, Direction::DOWN, Direction::LEFT, Direction::RIGHT}) {
			for(int i = 1; i <= radius; ++i) {
				auto ujpos = getNext(pos, dir, i);
				
				if(solid.count(ujpos)) {
					break;
				}
				dangerous.insert(ujpos);
			}
		}
	}


	void parse(pugi::xml_document& doc) {
		for(auto& it : fields) {
			for(auto& field : it) {
				field.clear();
			}
		}
		we = nullptr;
		solid.clear();
		dangerous.clear();
		bombs.clear();

		auto items = doc.child("GameItems");

		for (pugi::xml_node tool: items.children("Player")) {
			auto player = std::make_shared<Player>(tool);

			insertStuffs(player);		
			auto& shr = get(fields, player->getPos());
			shr.push_back(player);
		}
		for (pugi::xml_node tool: items.children("Bomb")) {
			auto bomb = std::make_shared<Bomb>(tool);
			
			insertStuffs(bomb);
			auto& shr = get(fields, bomb->getPos());
			shr.push_back(bomb);
			bombs.push_back(bomb);
		}
		for (pugi::xml_node tool: items.children("GameItem")) {
			auto item = std::make_shared<GameItem>(tool);
			
			insertStuffs(item);		
			auto& shr = get(fields, item->getPos());
			
			shr.push_back(item);
		}
		
		for(auto bomb : bombs) {
			insertDanger(bomb);
		}
		
		for(auto& it : fields) {
			for(auto& itemX : it) {
				std::cout << "|";
				if(itemX.empty()) break;
				auto item = itemX.front();
				if(item) {
					char c = item->getTileChar();
					if(item->type() == Player::typeId && dynamic_cast<Player*>(item.get())->getNickName() == Commands::we) {
						std::cout << '*';
						we = std::dynamic_pointer_cast<Player>(item);
					}
					else {
						std::cout << (c == 'Z' ? ' ' : c);
					}
				}
				else {
					std::cout << " ";
				}
				
				if(dangerous.count(item->getPos())) {
					std::cout << "!";
				}
				else if(solid.count(item->getPos())) {
					std::cout << "#";
				}
				else {
					std::cout << " ";
				}
				
			}
			std::cout << std::endl;
		}
	}

	std::set<Direction> directions() {
		if(we) {
			return Moves::directions(fields, we->getPos());
		}
		return {};
	}
	
	bool isDeadEnd() {
		if(we) {
			auto dir = getDir(we->getD());
			return Moves::deadEnd(fields, getNext(we->getPos(), dir), getDir(we->getD()));
		}
		return false;
	}
	
	std::vector<std::string> moves(int& millisecToNext) {
		std::vector<std::string> result;
		if(we) {
			
			if(we->getState() == "Standing") {
				auto dirs = directions();

				for(auto dir : dirs) {
					if(!Moves::deadEnd(fields, we->getPos(), dir)) {
						result.push_back(Commands::move(dir));
						break;
					}
					else {
						std::cerr << "Dead end" << we->getPos() << " to " << dir << std::endl;
					}
				}
				if(result.empty() && dirs.size()) {
					for(auto dir : dirs) {
						if(!dangerous.count(getNext(we->getPos(), dir))) {
							result.push_back(Commands::move(dir));
							break;
						}
					}
				}
			}
			else if(isDeadEnd()) {
				result.push_back(Commands::stop());
			}
			else if(we->getBombSize()) {
				result.push_back(Commands::bomb());
				
				std::cerr << std::get<0>(we->getD()) << " " << std::get<1>(we->getD()) << " dir:" << getDir(we->getD()) << std::endl;
			}
		}
		std::cout << we << "RESULT SIZE" << result.size() << std::endl;
		millisecToNext = 100000;
		return result;
	}
};

#endif // FIELDS_HPP


