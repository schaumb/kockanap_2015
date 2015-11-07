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
	std::set<Coordinate> solids;
	std::set<Coordinate> dangerous;
	std::set<Coordinate> pickable;
	std::set<Coordinate> explodable;
	std::shared_ptr<Player> we;
	std::vector<std::shared_ptr<Bomb>> bombs;
	Coordinate reach = {};
	int cel = 0;
public:
	void insertStuffs(std::shared_ptr<GameItem> ptr) {
		if(ptr->getIsSolid()) {
			solids.insert(ptr->getPos());
		}
		if(ptr->getIsPickable()) {
			pickable.insert(ptr->getPos());
		}
		if(ptr->getIsExplodable()) {
			explodable.insert(ptr->getPos());
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
				
				if(solids.count(ujpos)) {
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
		solids.clear();
		dangerous.clear();
		bombs.clear();
		pickable.clear();
		explodable.clear();

		auto items = doc.child("GameItems");

		for (pugi::xml_node tool: items.children("Player")) {
			auto player = std::make_shared<Player>(tool);

			if(player->getNickName() != Commands::we) {
				insertStuffs(player);
			}
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
				else if(solids.count(item->getPos())) {
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
			return Moves::directions(we->getPos(), solids);
		}
		return {};
	}
	
	bool isDeadEnd() {
		if(we) {
			auto dir = getDir(we->getD());
			auto endx = Moves::deadEnd(getNext(we->getPos(), dir), getDir(we->getD()), solids);
			if(endx.size()) {
				std::cerr << getNext(we->getPos(), dir) << " to " << getDir(we->getD()) << " is deadended in " << *endx.begin() << std::endl;
			}
			return endx.size();
		}
		return false;
	}
	
	Direction closestNotDangerRoute(Coordinate from) {
		for(auto coord : Moves::reachableCoords(from, solids)) {
			if(!dangerous.count(std::get<1>(coord))) {
				reach = std::get<1>(coord);
				return std::get<2>(coord);
			}
		}
		return {};
	}
	
	Direction closestPickableRoute(Coordinate from) {
		std::set<Coordinate> notJustSolid = solids;
		notJustSolid.insert(dangerous.begin(), dangerous.end());
		for(auto coord : Moves::reachableCoords(from, notJustSolid)) {
			if(pickable.count(std::get<1>(coord))) {
				reach = std::get<1>(coord);
				return std::get<2>(coord);
			}
		}
		return {};
	}
	
	Direction closestAttack(Coordinate from) {
		std::set<Coordinate> notJustSolid = solids;
		notJustSolid.insert(dangerous.begin(), dangerous.end());
		for(auto coord : Moves::reachableCoords(from, notJustSolid)) {
			if(explodable.count(std::get<1>(coord))) {
				reach = std::get<1>(coord);
				return std::get<2>(coord);
			}
		}
		return {};
	}	
	std::vector<std::string> moves(int& millisecToNext) {
		std::vector<std::string> result;
		if(we) {
			std::cerr << "State " << we->getState() << " ";
			if(we->getState() == "Standing") {
				std::cerr << "We are standing ";
				if(dangerous.count(we->getPos())) {
					auto dir = closestNotDangerRoute(we->getPos());

					std::cerr << "it's dangerous " << std::endl;
					if(dir != Direction{}) {
						std::cerr << "so move to " << dir << std::endl;
						cel = 1;
						result.push_back(Commands::move(dir));
					}
					else {
						std::cerr << "but not seeing not dangerous place" << std::endl;
					}
				}

				if(result.empty()){
					std::cerr << "let's try with some pickable ";
					auto dir = closestPickableRoute(we->getPos());
					if(dir != Direction{}) {
						cel = 2;
						std::cerr << "so go to " << dir << std::endl;
						result.push_back(Commands::move(dir));
					}
					else {
						std::cerr << "not seeing anyone" << std::endl;
					}
				}

				if(result.empty()){
					std::cerr << "let's try with attack ";
					auto dir = closestAttack(we->getPos());
					if(dir != Direction{}) {
						std::cerr << "found!! " << std::endl;;
						cel = 3;
						result.push_back(Commands::move(dir));
					}
					else {
						std::cerr << "nowhere nothing" << std::endl;;
					}
				}
			}
			else {
				switch(cel) {
				case 1:
					if(!dangerous.count(we->getPos())) {
						std::cerr << "We reached safety" << std::endl;
						cel = 0;
					}
					else if(closestNotDangerRoute(getNext(we->getPos(), getDir(we->getD()))) != getDir(we->getD())) {
						std::cerr << "We have to change the route to safety" << std::endl;
						result.push_back(Commands::stop());
					}
					else {
						std::cerr << "We can go on the route to safety" << std::endl;
					}
					break;
				case 2:
					if(reach == we->getPos()) {
						std::cerr << "We reached the pickable" << std::endl;
						cel = 0;
					}
					else if(closestPickableRoute(getNext(we->getPos(), getDir(we->getD()))) != getDir(we->getD())) {
						std::cerr << "We have to change the route to pickable" << std::endl;
						result.push_back(Commands::stop());
					}
					else {
						std::cerr << "We can go on the route to pickable" << std::endl;
					}
					break;
				case 3:
					if((reach == we->getPos() || neigbours(reach, we->getPos())) && we->getBombSize()) 
					{
						cel = 0;
						result.push_back(Commands::bomb());
					}
					else if(closestAttack(getNext(we->getPos(), getDir(we->getD()))) != getDir(we->getD())) {
						std::cerr << "We have to change the route to attack" << std::endl;
						result.push_back(Commands::stop());
					}
					else {
						std::cerr << "We can go on the route to attack" << std::endl;
					}
					break;
				default:
					if(dangerous.count(we->getPos())) {
						std::cerr << "We have some dangerous place. We need to think that we change the route" << std::endl;
						result.push_back(Commands::stop());
					}
				}
			}
			
			if(result.empty() && we->getBombSize()) {
				std::cerr << "We dont any command. Try to put a bomb" << std::endl;
				for(auto exp : explodable) {
					if(we->getPos() == exp || neigbours(we->getPos(), exp)) {
						std::cerr << "Some explodable in close" << std::endl;
						result.push_back(Commands::bomb());	
						break;
					}
				}
				cel = 0;
			}
			
			if(result.empty()) {
				std::cerr << "Nothing to do" << std::endl;
			}
		}
		millisecToNext = 100000;
		std::cout << we << " Result size " << result.size() << std::endl;
		return result;
	}
};

#endif // FIELDS_HPP


