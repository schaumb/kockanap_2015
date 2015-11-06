#ifndef FIELDS_HPP
#define FIELDS_HPP

#include "../pugixml/pugixml.hpp"

class Fields
{
	struct Bomb {
		pugi::xml_node info;
		std::string get( std::string str) {
		
		}
		
		int get_int(std::string str) {
			return std::stoi(get(str));
		}
		
		int getId() {
			return get_int("ID");
		}
	};
public:
	void parse(pugi::xml_document& doc) {
		auto items = doc.child("GameItems");
		auto count = items.attribute("objects").as_int();
		std::cout << "Item count " << count << std::endl;
		
		for (pugi::xml_node tool: items.children("Player")) {
			std::cout << "player id: " << tool.children("ID").child_value() << std::endl;
		}
		
	}
};

#endif // FIELDS_HPP
