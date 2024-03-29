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
	std::map<Coordinate, int> dangerous;
	std::set<Coordinate> pickable;
	std::set<Coordinate> explodable;
	std::shared_ptr<Player> we;
	std::vector<std::shared_ptr<Bomb>> bombs;
	Coordinate reach = {};
	int cel = 0;
public:
	static std::string login;
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
			++dangerous[ptr->getPos()];
		}
	}
	
	void insertDanger(std::shared_ptr<Bomb> bptr) {
		if(bptr->getTileChar() == 'X') return; 
		int radius = bptr->getRadius();
		auto pos = bptr->getPos();
		for(auto dir : {Direction::UP, Direction::DOWN, Direction::LEFT, Direction::RIGHT}) {
			for(int i = 1; i <= radius; ++i) {
				auto ujpos = getNext(pos, dir, i);
				
				if(solids.count(ujpos)) {
					break;
				}
				++dangerous[ujpos];
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
			else {
				we = player;
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

	auto directions() {
		if(we) {
			return Moves::directions(we->getPos(), solids);
		}
		return decltype(Moves::directions(we->getPos(), solids)){};
	}
	
	bool isDeadEndFromNext(Coordinate from, Direction dir) {
		auto endx = Moves::deadEnd(getNext(from, dir), dir, solids);
		if(endx.size()) {
			std::cerr << getNext(we->getPos(), dir) << " to " << getDir(we->getD()) << " is deadended in " << *endx.begin() << std::endl;
		}
		return endx.size();
	}
	
	Direction closestNotDangerRoute(Coordinate from) {
		bool release_if_same_root = true;
		bool release_reachables = true;
		bool best_minimal_route = false;
		bool first = true;
		Direction now = getDir(we->getD());
		Direction d = {};
		int count;

		auto solids_ = solids; 
		if(release_reachables) {
			for(auto bomb : bombs) {
				if(bomb->getTileChar() == 'X') {
					solids_.insert(bomb->getPos());
				}
			}
		}
		for(auto coord : Moves::reachableCoords(from, solids_)) {
			if(!dangerous.count(std::get<1>(coord))) {
				if(!release_if_same_root) {
					reach = std::get<1>(coord);
					return std::get<2>(coord);
				} else if (first) {
					first = false;
					std::tie(count, reach, d) = coord;
				} 
				else if(best_minimal_route) {
				
				}
				else  {
					if(std::get<0>(coord) <= count + 2 && std::get<2>(coord) == now) {
						reach = std::get<1>(coord);
						return now;
					}
				}
			}
		}
		return d;
	}
	
	Direction closestPickableRoute(Coordinate from) {
		std::set<Coordinate> notJustSolid = solids;
		for(auto danger : dangerous) {
			notJustSolid.insert(danger.first);
		}
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
		for(auto danger : dangerous) {
			notJustSolid.insert(danger.first);
		}
		for(auto coord : Moves::reachableCoords(from, notJustSolid)) {
			if(explodable.count(std::get<1>(coord))) {
				reach = std::get<1>(coord);
				return std::get<2>(coord);
			}
		}
		return {};
	}
	
	bool inDangerous(Coordinate from) {
		std::set<Coordinate> notJustSolid = solids;
		for(auto danger : dangerous) {
			notJustSolid.insert(danger.first);
		}
		Coordinate coordx;
		for(auto coord : Moves::reachableCoords(from, notJustSolid)) {
			if(coordx == Coordinate{}) {
				coordx = std::get<1>(coord);
			}
			else {
				if(std::get<0>(coordx) != std::get<0>(std::get<1>(coord))) {
					std::get<0>(coordx) = -1;
				}
				if(std::get<1>(coordx) != std::get<1>(std::get<1>(coord))) {
					std::get<1>(coordx) = -1;
				}
				if(coordx == Coordinate{-1, -1}) {
					return false;
				}
			}
		}
		return true;
	}
	void slowmove(int& millisecToNext, std::vector<std::string>& res) {
		cel += 10;
		millisecToNext = 400;
		res.push_back(Commands::speed(0.17));
	}
		
	std::vector<std::string> moves(int& millisecToNext) {
		std::vector<std::string> result;
		millisecToNext = 3000;
		
		if(we) {
			std::cerr << "State " << we->getState() << " ";
			if(cel >= 10) {
				std::cerr << " slowmove-> stop" << std::endl;
				result.push_back(Commands::stop());
				cel-= 10;
				result.push_back(Commands::speed(0.25));
			}
			else if(cel == 7) {
				result.push_back(Commands::bomb());
				cel = 0;
			}
			else if(we->getState() == "Standing") {
				std::cerr << "We are standing ";
				if(dangerous.count(we->getPos())) {
					auto dir = closestNotDangerRoute(we->getPos());

					std::cerr << "it's dangerous " << std::endl;
					if(dir != Direction{}) {
						std::cerr << "so move to " << dir ;
						cel = 1;
						if(closestNotDangerRoute(getNext(we->getPos(), dir)) != dir) {
							slowmove(millisecToNext, result);
							std::cerr << " with slow";
						}
						else {
							//result.push_back(Commands::speed(0.3));
						}
						std::cerr << std::endl;
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
						std::cerr << "so go to " << dir;
						if(closestPickableRoute(getNext(we->getPos(), dir)) != dir) {
							slowmove(millisecToNext, result);
							std::cerr << " with slow";
						}
						std::cerr << std::endl;
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
						std::cerr << "found!! ";
						cel = 3;
						if(closestAttack(getNext(we->getPos(), dir)) != dir) {
							slowmove(millisecToNext, result);
							std::cerr << " with slow";
						}
						std::cerr << std::endl;
						result.push_back(Commands::move(dir));
					}
					else {
						std::cerr << "nowhere nothing" << std::endl;;
					}
				}
			}
			else {
				bool safety_enabled = true;
				if(safety_enabled && !dangerous.count(we->getPos())) {
					auto next = getNext(we->getPos(), getDir(we->getD()));
					auto afternext = getNext(we->getPos(), getDir(we->getD()), 2);
					auto afterafternext = getNext(we->getPos(), getDir(we->getD()), 3);
					if(dangerous.count(next) || dangerous.count(afternext) || dangerous.count(afterafternext)) 
					{
						std::cerr << "Watch OUT" << std::endl;
						result.push_back(Commands::stop());
					}					
				}
				
				switch(cel) {
				case 1:
					if(!dangerous.count(we->getPos())) {
						std::cerr << "We reached safety" << std::endl;
						cel = 0;
					}
					else if(
						closestNotDangerRoute(getNext(we->getPos(), getDir(we->getD()), 0)) != getDir(we->getD()) ||
						closestNotDangerRoute(getNext(we->getPos(), getDir(we->getD()))) != getDir(we->getD())) {
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
					if(closeTo(reach, we->getPos(), we->getBombSize()) && we->getBombsLeft()) 
					{
						cel = 7;
						result.push_back(Commands::stop());
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
						
						if(closestNotDangerRoute(getNext(we->getPos(), getDir(we->getD()))) != getDir(we->getD()) &&
							closestNotDangerRoute(getNext(we->getPos(), getDir(we->getD()), 0)) != getDir(we->getD())) {
							std::cerr << "We have to change the route to safety" << std::endl;
							result.push_back(Commands::stop());
						}
					} 
					else {
						std::cerr << "We have no cel and we ar in safe place, we need to rehink what we want" << std::endl;
						result.push_back(Commands::stop());
					}
				}
			}
			
			if(result.empty() && we->getBombsLeft()) {
				std::cerr << "We dont any command. Try to put a bomb" << std::endl;
				if(!inDangerous(we->getPos())) {
					if(!isDeadEndFromNext(we->getPos(), getDir(we->getD()))) {
						bool getDoIt = false;	
						for(auto exp : explodable) {
							if(closeTo(exp, we->getPos(), we->getBombSize())) {
								std::cerr << "Some explodable in close" << std::endl;
								getDoIt = true;
								break;
							}
						}
						
						if(getDoIt) {
							for(auto bomb : bombs) {
								if(bomb->getTileChar() == 'Y') {
									if(closeTo(bomb->getPos(), we->getPos(), bomb->getRadius()) ||
										dangerous.count(we->getPos())) {
										std::cerr << "NEM JO OTLET, kozelben bomba van" << std::endl;
										getDoIt = false;
										break;
									}
								}
							}
						}
						
						if(getDoIt) {
							result.push_back(Commands::bomb());
							if(we->getState() == "Standing") {
								for(auto c : Moves::reachableCoords(we->getPos(), solids)) {
									if(std::get<0>(c) > we->getBombSize()) {
										break;
									}
									if(std::get<0>(std::get<1>(c)) == std::get<0>(we->getPos()) ||
										std::get<1>(std::get<1>(c)) == std::get<1>(we->getPos())) 
									{
										++dangerous[std::get<1>(c)];
									}
								}
								millisecToNext = 1;
							}
						}
					}
				}
			}
			
			if(result.empty()) {
				std::cerr << "Nothing to do" << std::endl;
			}

			std::cout << "DIED: " << we->getDied() << " KILL:" << we->getKills() << std::endl;
		}
		else {
			result.push_back(Commands::left());
		}
		std::cout << we << " Result size " << result.size() << std::endl;
		return result;
	}
};

std::string Fields::login;

#endif // FIELDS_HPP


