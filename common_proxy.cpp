#include "common_socket.h"
#include "common_proxy.h"
#include <arpa/inet.h>
#include <strings.h>
#include <string.h>

void Proxy::enviar(Socket& socket, uint32_t entero, size_t cantidad){
	char buff[5];
	bzero(buff,5);
	size_t enteroEnviar = entero;
	// no enconte forma de hacerlo generica,
	// si cuando mando el comando le hago htonl
	// me crea un numero enorme.
	if (cantidad == 4){
		enteroEnviar = htonl(entero);
	}
	memcpy(buff,&enteroEnviar ,cantidad);
	socket.send(buff,cantidad);
}

int Proxy::recibir(Socket& socket,size_t cantidad){
	char respuestaComando[5];
	if (socket.receive(respuestaComando,cantidad) < 0){
		return -1;
	}
	uint32_t ui=0;
	memcpy(&ui, respuestaComando,cantidad);
	// no pude hacerla generica ya que en ese tenia que hacer ntohl 
	// de un entero y me devolvia un cualquier cosa. Si es solo un byte
	// no lo convierto.
	if (cantidad == 1){
		return ui;
	}
	return ntohl(ui);
}
