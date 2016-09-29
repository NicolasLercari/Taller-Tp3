#include <iostream>
#include <sstream>      
#include <fstream>
#include <string.h>
#include <arpa/inet.h>
#include <string>
#include "common_socket.h"
#include "common_thread.h"
#include <mutex>
#include "common_paradas.h"
#include <thread>
#include <vector>
#define RESPUESTACA 0
#define RESPUESTACF 2 
#define RESPUESTACL 3 
#define RESPUETACR 4 
#define RESPUESTAERROR 255

using namespace std;

uint32_t recibirUint32_t(Socket* socket){
	char buff[5];
	bzero(buff,5);

	socket->receive(buff,sizeof(buff)-1);

	uint32_t cs = 0;

	memcpy ( &cs, buff, sizeof(cs) );

	return ntohl(cs);
}

void enviarUint32_t(Socket* socket, uint32_t entero){

	uint32_t ui = htonl(entero);

	char buff[5];

	memcpy ( buff, &ui, sizeof(buff)-1 );

	socket->send(buff,sizeof(buff)-1);
}
void enviarByte(Socket* socket, uint32_t respuesta ){
		char buff[1];
		memcpy(buff,&respuesta,1);
		socket->send(buff,1);
}

/*void procesarCliente(Socket&& socket, Paradas& paradas){

	char comando;
	while ( socket.receive(&comando,sizeof(comando)) > 0){
		
		std::cerr << "Comando " << comando << " recibido." << std::endl;

		uint32_t unixTime = recibirUint32_t(socket);

		switch (comando){
			case 'A':{
				uint32_t numLinea = recibirUint32_t(socket);
				if (paradas.agregarPartidaBondi(numLinea,unixTime) == 0){
					enviarByte(socket, 0x00);
					enviarUint32_t(socket,numLinea);
				} else {
					enviarByte(socket, 0xFF);
				}
				break;
			}
			case 'F':{
				uint32_t numLinea = recibirUint32_t(socket);
				uint32_t numParada = recibirUint32_t(socket);
				int cuantoFalta = paradas.cuantoFaltaParaLlegar(unixTime, numLinea, numParada);
				
				if (cuantoFalta>=0){
					enviarByte(socket, 0x02);
					enviarUint32_t(socket,cuantoFalta);
				} else {
					enviarByte(socket,0xFF);
				}
				break;
			}
			case 'L':{
				uint32_t paradaUno,paradaDos;
				paradaUno = recibirUint32_t(socket);
				paradaDos = recibirUint32_t(socket);
				std::pair<size_t,size_t> mejorBondi = paradas.bondiMasRapido(paradaUno,paradaDos);
				
				enviarByte(socket,0x03);
				enviarUint32_t(socket,mejorBondi.first);
				enviarUint32_t(socket,mejorBondi.second);
				break;
			}
			case 'R':{

				uint32_t paradaUno=recibirUint32_t(socket);
				uint32_t paradaDos=recibirUint32_t(socket);
				std::pair<size_t,size_t> bondiTiempo = paradas.bondiRecomendado(unixTime,paradaUno,paradaDos);
				enviarByte(socket,0x04);
				enviarUint32_t(socket,bondiTiempo.first);
				enviarUint32_t(socket,bondiTiempo.second);
				break;
			}
		}
	}
}*/ 	

class manejadorCliente : public Thread {
private:
	Socket* socket;
	Paradas& paradas;

public:
	manejadorCliente(Socket* socket, Paradas& paradas) : socket(socket),paradas(paradas){
		
	}

	virtual void run(){
		cerr << "Cliente conectado." << endl;
		char comando;
		
		while ( socket->receive(&comando,sizeof(comando)) > 0){
			
			std::cerr << "Comando " << comando << " recibido." << std::endl;

			uint32_t unixTime = recibirUint32_t(socket);

			switch (comando){
				case 'A':{
					uint32_t numLinea = recibirUint32_t(socket);
					if (paradas.agregarPartidaBondi(numLinea,unixTime) == 0){
						enviarByte(socket, 0x00);
						enviarUint32_t(socket,numLinea);
					} else {
						enviarByte(socket, 0xFF);
					}
					break;
				}
				case 'F':{
					uint32_t numLinea = recibirUint32_t(socket);
					uint32_t numParada = recibirUint32_t(socket);
					int cuantoFalta = paradas.cuantoFaltaParaLlegar(unixTime, numLinea, numParada);
					
					if (cuantoFalta>=0){
						enviarByte(socket, 0x02);
						enviarUint32_t(socket,cuantoFalta);
					} else {
						enviarByte(socket,0xFF);
					}
					break;
				}
				case 'L':{
					uint32_t paradaUno,paradaDos;
					paradaUno = recibirUint32_t(socket);
					paradaDos = recibirUint32_t(socket);
					std::pair<size_t,size_t> mejorBondi = paradas.bondiMasRapido(paradaUno,paradaDos);
					
					enviarByte(socket,0x03);
					enviarUint32_t(socket,mejorBondi.first);
					enviarUint32_t(socket,mejorBondi.second);
					break;
				}
				case 'R':{

					uint32_t paradaUno=recibirUint32_t(socket);
					uint32_t paradaDos=recibirUint32_t(socket);
					std::pair<size_t,size_t> bondiTiempo = paradas.bondiRecomendado(unixTime,paradaUno,paradaDos);
					enviarByte(socket,0x04);
					enviarUint32_t(socket,bondiTiempo.first);
					enviarUint32_t(socket,bondiTiempo.second);
					break;
				}
			}
		}
		socket->shutdown();
		delete socket;
		cerr << "Cliente desconectado." << endl;
	}

};

class espearaQ :  public Thread{ 
    private:
    	Socket& skt;

    public:
        espearaQ(Socket& skt) : skt(skt){}
        
		virtual void run() {
            string quit;
            for (;;){
				    
				cin >> quit;
            	if (quit == "q"){
            		skt.shutdown();
            		return;
            	}
            }
        }
};

class hola :  public Thread{ 
    private:
    	Socket& skt;
    	Paradas& paradas;
    public:
        hola(Socket& skt, Paradas& paradas): skt(skt), paradas(paradas) {}
        
		virtual void run() {            
			std::vector<Thread*> threads;
		    //Socket socket;
			for (;;){
				Socket* socket = skt.accept();
					
				if (socket->invalido()){
					delete socket;
					break;
				}

				//cerr << "Cliente conectado." << endl;

				Thread* th = new manejadorCliente(socket,paradas);

				th->start();

				//th -> join();
				threads.push_back(th);

				//procesarCliente(socket,paradas);

				//cerr << "Cliente desconectado." << endl;		
				
				//socket.shutdown();
				
			}
			
			/*for(unsigned int i=0; i < threads.size(); i++){
				threads[i]->start();
			}*/

			for(unsigned int i=0; i < threads.size(); i++){
				threads[i]->join();

				delete threads[i];
			}
		}
};

/*std::thread aceptar_un_cliente(Socket &aceptador) {
	Socket skt_cliente = aceptador.accept();
	// copia de un socket, error!
	// std::thread t {manejador_del_cliente,
	// skt_cliente};
	// movimiento de un socket, todo ok
	std::thread t {manejador_del_cliente, std::move(skt_cliente)};
	return t; // movemos el hilo, no hay copia
}*/

int main(int argc, char *argv[]) {

	Paradas paradas;

    std::vector<Thread*> threads;
  
	Socket skt;

	skt.bindAndListen(8080);

    threads.push_back(new espearaQ(skt));

	threads.push_back(new hola(skt,paradas));

    for (int i=0; i <= 1; i++){

    	threads[i]->start();
	}

    for (int i=0; i <= 1; i++){

    	threads[i]->join();
		delete threads[i];
	}

	return 0;
}
