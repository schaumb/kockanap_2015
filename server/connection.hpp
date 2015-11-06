#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <SFML/Network.hpp>
#include <climits>
#include <iostream>
#include "selector.hpp"
#include "options.hpp"

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
		return write(str);
	}

	Status write(const std::string& message) {
		Status result = tcp.send(reinterpret_cast<const void*>(message.data()), message.size());
		
		if(result != Done) {
			std::cerr << "Nem irta ki az uzenetet" << std::endl;
		}
		return result;
	}
	
	
	
	virtual void readable() = 0;
};

#endif // CONNECTION_HPP
