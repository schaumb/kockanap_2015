#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <SFML/Network.hpp>
#include <climits>
#include <iostream>
#include <fstream>
#include "selector.hpp"
#include "options.hpp"
#include "../pugixml/pugixml.hpp"

typedef std::array<unsigned char, 4> ProtoSize;

template<class T>
ProtoSize encodeSize(T size) {
	ProtoSize sizeBytes = { { static_cast<unsigned char>((size >> 0) & 0xff),
			static_cast<unsigned char>((size >> 8) & 0xff),
			static_cast<unsigned char>((size >> 16) & 0xff),
			static_cast<unsigned char>((size >> 24) & 0xff) } };
	return sizeBytes;
}

template<class T>
uint32_t decodeSize(const T& sizeBytes) {
	uint32_t size = 0;
	for (size_t i = 0; i < sizeof(size); ++i) {
		size <<= CHAR_BIT;
		size |= (uint32_t) sizeBytes[sizeof(size) - i - 1];
	}
	return size;
}

class Connection : protected virtual Selector, sf::TcpSocket {
	Options opt;
	sf::UdpSocket udp;
	sf::TcpSocket tcp;
public:

	std::string getUdpString() {
		return sf::IpAddress::getLocalAddress().toString() + " " + std::to_string(udp.getLocalPort());
	}
	
	Connection(const Options& options) : opt(options) {
		Status status;
		std::size_t nthTry = options.tryNToConect;
		
		auto timeout = sf::milliseconds(options.timeoutMillisec);
		
		do {
			if(--nthTry == -1UL) {
				std::cerr << "Nem sikerult csatlakozni - exit" << std::endl;
				std::exit(1);
			}
			status = tcp.connect(options.serverName, options.port, timeout);
			
			if(status != Done) {
				std::cerr << "nem sikerult " << static_cast<int>(status) << " miatt csatlakozni." << nthTry << std::endl; 
			}
		} while(status != Done);

		this->addSocket(tcp, [this]{
			this->readTcp();
		});
		std::cerr << "Sikeres csatlakozas 1!" << std::endl;

		do {
			if(--nthTry == -1UL) {
				std::cerr << "Nem sikerult csatlakozni - exit" << std::endl;
				std::exit(1);
			}
			status = udp.bind(opt.port2);
			
			if(status != Done) {
				std::cerr << "nem sikerult bindolni " << static_cast<int>(status) << " miatt csatlakozni." << nthTry << std::endl;
				++opt.port2; 
			}
		} while(status != Done);
		std::cerr << "Sikeres csatlakozas 2!" << std::endl;

		this->addSocket(udp, [this]{ this->readable(); });
	}

	void readTcp() {
		ProtoSize sizeBytes;
		std::size_t readed;
		tcp.receive(reinterpret_cast<void*>(sizeBytes.data()), sizeBytes.size(), readed);

		if(sizeBytes.size() != readed) {
			std::cerr << "tcp read not enough size" << std::endl;
		}
		uint32_t size = decodeSize(sizeBytes);
		
		std::string str;
		str.resize(size);
		
		if(size > 0) {
			tcp.receive(reinterpret_cast<void*>(&str[0]), size, readed);
			if(size != readed) {
				std::cerr << "tcp read not enough message" << std::endl;
			}
			
		}
		std::cerr << "Got from tcp: " << str << std::endl;
	}
	void write(const std::vector<std::string>& messages) {
		std::string str;
		for(const std::string& message: messages) {
			uint32_t size = message.size();
			ProtoSize sizeBytes = encodeSize(size);
			
			std::cerr << "Kuldjuk: \"" << message << "\" meretjelzovel: ";
			for(std::size_t i = 0; i < sizeBytes.size(); ++i) {
				std::cerr << (int)sizeBytes[i] << " ";
			}
			std::cerr << std::endl;
			
			Status result = tcp.send(reinterpret_cast<const void*>(sizeBytes.data()), sizeBytes.size());
			
			if(result != Done) {
				std::cerr << "not sent properly size" << std::endl;
			}

			result = tcp.send(reinterpret_cast<const void*>(message.data()), message.size());

			if(result != Done) {
				std::cerr << "not sent properly message" << std::endl;
			}
		}
	}
	
	struct Message {
		std::map<int, std::string> packages;
		std::size_t _max;
		std::size_t _sendIndex;
		bool first = true;

		void setMax(std::size_t sendIndex, std::size_t max) {
			_max = max;
			_sendIndex = sendIndex;
			first = false;
		}
		
		void setPackage(std::size_t package, const char* message, std::size_t size) {
			if(package > _max) {
				std::cerr << "nagyobb a package, mint a max" << std::endl;
			}
			if(packages.count(package)) {
				std::cerr << "ERROR same package" << package << std::endl;
			}
			packages[package] = std::string(message, size);
		}
		
		bool checkSetMax(std::size_t sendIndex, std::size_t pmax) {
			if(first) {
				setMax(sendIndex, pmax);
				return true;
			}
			
			if(sendIndex != _sendIndex) {
				std::cerr << "nem egyenlo a sendIndex" << std::endl;
				return false;
			}
			if(pmax != _max) {
				std::cerr << "nem egyenlo a max" << std::endl;
			}
			return pmax == _max;
		}
		
		bool isDone() {
			return _max == packages.size();
		}
		
		std::string getData() {
			std::string res;
			for(auto& p : packages) {
				std::string& str = p.second;
				for(char& c : str) {
					if(c == '\0') c = ' ';
				}
				res += str.c_str();
			}
			return res;
		}
	};
	
	
	
	bool read(pugi::xml_document& doc) {
		Message message;


		char reader[1012];
		std::size_t received;
		sf::IpAddress ip;
		unsigned short port;
		do {
			udp.receive(reinterpret_cast<void*>(reader),1012, received, ip, port);
		
			if(received != 1012) {
				std::cerr << "Nem eleget olvasott be!!" << std::endl;
			}

			std::int32_t sendIndex = *reinterpret_cast<std::int32_t*>(reader);
			std::int32_t packetIndex = *reinterpret_cast<std::int32_t*>(reader + 4);
			std::int32_t packetCount = *reinterpret_cast<std::int32_t*>(reader + 8);

			if(!message.checkSetMax(sendIndex, packetCount)) {
				std::cerr << "checkSetMax failed" << std::endl;
			}
			
			message.setPackage(packetIndex, reader + 12, 1000);

		} while(!message.isDone());
		
		std::string source = message.getData();
		doc.reset();
		pugi::xml_parse_result result = doc.load_string(source.c_str());
		
		
		if (result)
		{
			std::cerr << "Parse GOOD\n\n";
		}
		else
		{
			std::cerr << "Error description: " << result.description() << std::endl;
			std::cerr << "Error offset: " << result.offset << " (error at [..." << std::string(source.data() + result.offset, 20) << "] ->" << source.data() + result.offset - 300 << std::endl;

 			std::cerr << "BAD PARSE" << source << std::endl << std::endl;
		}
		return result;
	}
	
	
	virtual void readable() = 0;

};

#endif // CONNECTION_HPP
