#include <iostream>

#ifndef SOCKET_H_
#define SOCKET_H_

class Socket{
private:
	int skt;

public:
	Socket();
	int bindAndListen(unsigned short port);
	int connect(const char* host_name, unsigned short port);
	Socket* accept();
	int send(const char* buffer, size_t length);
	int receive(char* buffer, size_t length);
	void shutdown();
	bool invalido();
	~Socket();

	Socket(Socket&& other) {
		this->skt = std::move(other.skt);
	}

	Socket& operator=(Socket&& other) {
		this->skt = std::move(other.skt);
		return *this;
	}
	
private:
	Socket(const Socket&) = delete;
	Socket& operator=(const Socket&) = delete;

};

#endif