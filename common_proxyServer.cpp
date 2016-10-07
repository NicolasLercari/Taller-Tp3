#include "common_proxyServer.h"
#include "common_proxy.h"
#include "common_socket.h"
#include <arpa/inet.h>
#include <strings.h>
#include <string.h>


ProxyServer::ProxyServer(const char* hostname, const unsigned int puerto){
	sktCliente.connect(hostname,puerto);
}

void ProxyServer::enviar(uint32_t entero, size_t cantidad){
	Proxy::enviar(sktCliente, entero, cantidad);
}

int ProxyServer::recibir(size_t cantidad){
	return Proxy::recibir(sktCliente,cantidad);
}

ProxyServer::~ProxyServer(){
	sktCliente.shutdown();
}
