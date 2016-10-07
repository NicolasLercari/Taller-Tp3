
#include "common_socket.h"
#include "common_proxy.h"

#ifndef PROXYCLIENT_H_
#define PROXYCLIENT_H_

class ProxyClient : public Proxy {
	protected:
		Socket server;
	public:
		explicit ProxyClient(const unsigned int puerto);
		explicit ProxyClient(Socket&& socket);
		ProxyClient();
		ProxyClient aceptarCliente();
		bool hayConexion();
		void cerrarConexion();
		void enviar(uint32_t entero, size_t cantidad);
		int recibir(size_t cantidad);
		~ProxyClient();

		ProxyClient(ProxyClient&& other);
		ProxyClient& operator=(ProxyClient&& other);
};

#endif
