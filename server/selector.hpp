#ifndef SELECTOR_HPP
#define SELECTOR_HPP

#include <SFML/Network.hpp>
#include <map>
#include <functional>

class Selector
{
	using CallBack = std::function<void()>;
public:
	
	void run() {
		running = true;
		while (running) {
			if (selector.wait(waitFor())) {
				receive();
			}
			else {
				timeout();
			}
		}
	}
	
	virtual ~Selector() = default;
protected:

	void addSocket(sf::Socket& socket, CallBack function) {
		sockets[&socket] = function;
		selector.add(socket);
	}

	void removeSocket(sf::Socket& socket) {
		selector.remove(socket);
		sockets.erase(&socket);
	}

	virtual void timeout() {}
	
	virtual inline sf::Time waitFor() {
		return sf::Time::Zero;
	}
	
	bool running = false;
private:
	void receive() {
		auto socketIt = std::begin(sockets);
		while (socketIt != std::end(sockets)) {
			auto nextSocketIt = std::next(socketIt);

			if (selector.isReady(*socketIt->first)) {
				socketIt->second();
			}
			
			socketIt = nextSocketIt;
		}
	}
	
	sf::SocketSelector selector;
	std::map<sf::Socket*, CallBack> sockets; 
};

#endif // SELECTOR_HPP
