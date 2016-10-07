#include "common_proxy.h"
#include "common_socket.h"

#ifndef PROXYSERVER_H_
#define PROXYSERVER_H_

class ProxyServer : public Proxy {
	private:
		Socket sktCliente;
	public:
		ProxyServer(const char* hostname, const unsigned int puerto);
		int recibir(size_t cantidad);
		void enviar(uint32_t entero, size_t cantidad);
		~ProxyServer();
};

#endif
