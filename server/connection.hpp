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
	
public:
	using sf::TcpSocket::Status;
	using sf::TcpSocket::receive;
	using sf::TcpSocket::send;
	Connection(const Options& options) {
		Status status;
		std::size_t nthTry = options.tryNToConect;
		
		auto timeout = sf::milliseconds(options.timeoutMillisec);
		
		do {
			if(--nthTry == -1UL) {
				std::cerr << "Nem sikerult csatlakozni - exit" << std::endl;
				std::exit(1);
			}
			status = this->connect(options.serverName, options.port, timeout);
		} while(status != Done);

		std::cerr << "Sikeres csatlakozas!" << std::endl;
		this->addSocket(*this, [this]{ this->readable(); });
	}
	

	Status write(const std::string& message) {
		Status result = this->send(reinterpret_cast<const void*>(message.data()), message.size());
		
		if(result != Done) {
			std::cerr << "Nem irta ki az uzenetet" << std::endl;
		}
		return result;
	}
	
	Status writeWithSize(const std::string& message) {
		ProtoSize sizeBytes = encodeSize(message.size());

		Status result = this->send(reinterpret_cast<const void*>(sizeBytes.data()), sizeBytes.size());
		
		if(result != Done) {
			std::cerr << "Nem irta ki az uzenet meretet" << std::endl;
			return result;
		}
		return write(message);
	}
	
	Status read(std::string& inputMessage, std::size_t maxReceive = 500, bool exactly = false) {
		inputMessage.resize(maxReceive);
		std::size_t received;
		Status result = this->receive(reinterpret_cast<void*>(&inputMessage[0]), maxReceive, received);

		if(received == 0) {
			std::cerr << "Fogadott bajtok az uzenetnel: 0" << std::endl;
		}

		if(exactly && received != maxReceive) {
			std::cerr << "nem olvasta be pontosan az uzenetet" << std::endl;
		}

		if(result != Done) {
			std::cerr << "Nem olvasta be az uzenetet" << std::endl;
			return result;
		}

		inputMessage.resize(maxReceive);
		return result;
	}
	
	Status readWithSize(std::string& inputMessage) {
		ProtoSize sizeBytes;
		std::size_t received;
		Status result = this->receive(reinterpret_cast<void*>(&sizeBytes[0]), sizeBytes.size(), received);

		if(received == 0) {
			std::cerr << "Fogadott bajtok az uzenetnel: 0" << std::endl;
		}

		if(received != sizeBytes.size()) {
			std::cerr << "Nem olvasta be teljesen az uzenet meretet" << std::endl;
		}
		
		if(result != Done) {
			std::cerr << "Nem sikerult a meret beolvasasa" << std::endl;
			return result;
		}

		return read(inputMessage, received, true);
	}
	
	virtual void readable() = 0;
};

#endif // CONNECTION_HPP
