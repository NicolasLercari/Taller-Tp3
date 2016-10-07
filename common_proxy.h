#include "common_socket.h"
#ifndef PROXY_H_
#define PROXY_H_

class Proxy {
	public:
		Proxy(){}

		int recibir(Socket& socket, size_t cantidad);
		void enviar(Socket& socket,uint32_t entero, size_t cantidad);

		virtual ~Proxy(){}
};

#endif
