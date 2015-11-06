#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <SFML/Network.hpp>
#include <climits>
#include <iostream>
#include "selector.hpp"
#include "options.hpp"
#include "../pugixml/pugixml.hpp"

typedef std::array<unsigned char, 4> ProtoSize;

template<class T>
ProtoSize encodeSize(T size) {
	// instead of htonl:
	ProtoSize sizeBytes = { { static_cast<unsigned char>((size >> 24) & 0xff),
			static_cast<unsigned char>((size >> 16) & 0xff),
			static_cast<unsigned char>((size >> 8) & 0xff),
			static_cast<unsigned char>(size & 0xff) } };
	return sizeBytes;
}

template<class T>
uint32_t decodeSize(const T& sizeBytes) {
	uint32_t size = 0;
	// instead of ntohl:
	for (size_t i = 0; i < sizeof(size); ++i) {
		size <<= CHAR_BIT;
		size |= (uint32_t) sizeBytes[i];
	}
	return size;
}

class Connection : protected virtual Selector, sf::TcpSocket {
	Options opt;
	sf::UdpSocket udp;
	sf::TcpSocket tcp;
public:
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
		} while(status != Done);

		std::cerr << "Sikeres csatlakozas 1!" << std::endl;

		do {
			if(--nthTry == -1UL) {
				std::cerr << "Nem sikerult bindolni udp - exit" << std::endl;
				std::exit(1);
			}
			status = udp.bind(0);
		} while(status != Done);
		
		std::cerr << "Sikeres bindolas! " << udp.getLocalPort() << std::endl;
		this->addSocket(udp, [this]{ this->readable(); });
	}
	
	~Connection() {
		udp.unbind();
	}

	Status write(const std::vector<std::string>& messages) {
		std::string str;
		for(const std::string& message: messages) {
			str += message;
			str += "\n";
		}
		return writeM(str);
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
			if(package >= _max) {
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
				res += std::move(p.second);
			}
			return res;
		}
	};
	
	bool read(pugi::xml_document& doc) {
		Message message;


		char reader[1012];
		std::size_t received;
		sf::IpAddress address;
		unsigned short port;

		do {
			udp.receive(reinterpret_cast<void*>(reader),1012, received, address, port);
		
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
		pugi::xml_parse_result result = doc.load_string(source.c_str());
		
		
		if (result)
		{
			std::cerr << "Parse GOOD\n\n";
		}
		else
		{
			std::cerr << "BAD PARSE" << source << std::endl << std::endl;
			std::cerr << "Error description: " << result.description() << "\n";
			std::cerr << "Error offset: " << result.offset << " (error at [..." << *(source.data() + result.offset) << "]\n\n";
		}
		return result;
	}
	
	
	virtual void readable() = 0;
private:
	Status writeM(const std::string& message) {
		Status result = tcp.send(reinterpret_cast<const void*>(message.data()), message.size());
		
		if(result != Done) {
			std::cerr << "Nem irta ki az uzenetet" << std::endl;
		}
		return result;
	}

};

#endif // CONNECTION_HPP
