#include <stdio.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <syslog.h>
#include <errno.h>

#include "common_socket.h"

#define CANT_MAX_CLIENT 5
#define PAQ_SOBRANTE_LEN 100
#define ERROR_DE_CONEXION -1
#define SIN_ERRORES 1

Socket::Socket(){
    this -> skt = socket(AF_INET, SOCK_STREAM, 0);
    if (this -> skt == -1){
    	throw -1;
    }
    int val = 1,sktAux = this->skt;
    int s = setsockopt(sktAux, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
    if (s == -1) {
   		::shutdown(this -> skt, SHUT_RDWR);
    }
}

Socket::~Socket(){
	if (this -> skt >= 0){
		close(this->skt);
	}
}

Socket::Socket(Socket&& other) {
	this->skt = std::move(other.skt);
	other.skt = -1;
}

Socket& Socket::operator=(Socket&& other) {
	this->skt = std::move(other.skt);
	other.skt = -1;
	return *this;
}


int Socket::bindAndListen(unsigned short port){
	struct sockaddr_in serv_addr;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	bzero(&(serv_addr.sin_zero),sizeof(serv_addr.sin_zero));
	if(bind(this->skt,(struct sockaddr *)&serv_addr,
		(socklen_t)sizeof(struct sockaddr))==-1){
		return ERROR_DE_CONEXION;
	}
	if(listen(this->skt,CANT_MAX_CLIENT) == -1){
		return ERROR_DE_CONEXION;	
	}
	return SIN_ERRORES;
}

int Socket::connect(const char* host_name, unsigned short port){
	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = inet_addr(host_name);
    bzero(address.sin_zero,sizeof(address.sin_zero)); 
	if(::connect(this->skt, (struct sockaddr*) &address,
		(socklen_t)sizeof(struct sockaddr)) == -1){
		return ERROR_DE_CONEXION;	
	}
	return SIN_ERRORES;
}

Socket Socket::accept(){
	Socket accepted;
	accepted.skt = ::accept(this->skt,nullptr, nullptr);
	if (accepted.invalido()){
		throw std::exception();
	}
	return accepted;
}


int Socket::send(const char* buffer, size_t length){
	size_t sent = 0;
	int s = 0;
	bool is_the_socket_valid = true;

	while (sent < length && is_the_socket_valid) {
		s = ::send(this->skt, &buffer[sent], length-sent, MSG_NOSIGNAL);

		if (s == 0) 
		{
			is_the_socket_valid = false;
		} else if (s < 0) {
			is_the_socket_valid = false;
		} else {
			sent += s;
		}
	}

	if (is_the_socket_valid) 
	{
		return sent;
	} else {
		return ERROR_DE_CONEXION;
	}
	return SIN_ERRORES;
}

int Socket::receive(char* buffer, size_t length){
   size_t received = 0;
   int s = 0;
   bool is_the_socket_valid = true;

   while (received < length && is_the_socket_valid) {
   		s = recv(this->skt,&buffer[received], length-received, MSG_NOSIGNAL);
      
    	if (s == 0) { 
         	is_the_socket_valid = false;
      	} else if (s < 0) { 
         	is_the_socket_valid = false;
      	} else {
         	received += s;
      	}
   }

   if (is_the_socket_valid || received) {
   		return received;
   } else {
      	return ERROR_DE_CONEXION;
   }
}

void Socket::shutdown(){
	this -> skt = ::shutdown(this -> skt, SHUT_RDWR);
}

bool Socket::invalido(){
	return (this->skt <= 0);
}
