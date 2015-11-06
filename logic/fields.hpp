#ifndef FIELDS_HPP
#define FIELDS_HPP

#include "../pugixml/pugixml.hpp"

class Fields
{
	
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


