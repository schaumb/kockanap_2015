#ifndef FIELDS_HPP
#define FIELDS_HPP

#include "../pugixml/pugixml.hpp"
#include "player.hpp"
#include "bomb.hpp"
#include "commands.hpp"
#include <boost/optional.hpp>

class Fields
{
	boost::optional<GameItem> items[20][20];
	
	
public:
	void parse(pugi::xml_document& doc) {
		auto items = doc.child("GameItems");
		auto count = items.attribute("objects").as_int();
		std::cout << "Item count " << count << std::endl;

		for (pugi::xml_node tool: items.children("Player")) {
			Player p(tool);
			std::cout << "Player " << p.getId() << " is enemy: " << p.getIsKiller() << std::endl;
		}
		for (pugi::xml_node tool: items.children("Bomb")) {
			Bomb p(tool);
			std::cout << "Bomb " << p.getId() << " is enemy: " << p.getIsKiller() << std::endl;
		}
		for (pugi::xml_node tool: items.children("GameItem")) {
			GameItem p(tool);
			std::cout << "GameItem " << p.getId() << " is enemy: " << p.getIsKiller() << std::endl;
		}
	}
	
	std::vector<std::string> moves() {
		return {};
	}
};

#endif // FIELDS_HPP


