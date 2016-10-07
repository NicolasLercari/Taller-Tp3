#include "common_socket.h"
#include "common_proxyClient.h"
#include <arpa/inet.h>
#include <strings.h>
#include <string.h>


ProxyClient::ProxyClient(const unsigned int puerto){
	server.bindAndListen(puerto);
}

ProxyClient::ProxyClient(Socket&& socket){
	this->server = std::move(socket);
}

ProxyClient::ProxyClient(){}

ProxyClient::ProxyClient(ProxyClient&& other) {
	this->server = std::move(other.server);
}

ProxyClient& ProxyClient::operator=(ProxyClient&& other) {
	this->server = std::move(other.server);
	return *this;
}

ProxyClient ProxyClient::aceptarCliente(){
	Socket socket = server.accept();
	
	ProxyClient proxyClientAceptado(std::move(socket));
	return proxyClientAceptado;
}

bool ProxyClient::hayConexion(){
	return !(server.invalido());
}

void ProxyClient::cerrarConexion(){
	server.shutdown();
}

void ProxyClient::enviar(uint32_t entero, size_t cantidad){
	Proxy::enviar(server, entero, cantidad);
}

int ProxyClient::recibir(size_t cantidad){
	return Proxy::recibir(server,cantidad);
}

ProxyClient::~ProxyClient(){
	server.shutdown();
}
