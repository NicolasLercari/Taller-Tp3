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
#include "common_proxyClient.h"
#include <thread>
#include <vector>
#include <utility>

#define RESPUESTACA 0x00
#define RESPUESTACF 0x02 
#define RESPUESTACL 0x03 
#define RESPUETACR 0x04 
#define RESPUESTAERROR 0xFF


class  manejadorCliente : public Thread {
private:
	ProxyClient proxyClient;
	Paradas& paradas;

public:
	manejadorCliente(ProxyClient&& proxyClient, Paradas& paradas) : 
			proxyClient(std::move(proxyClient)), paradas(paradas) {}

	virtual void run(){
		std::cerr << "Cliente conectado." << std::endl;
		int comando;
	
		while ( (comando = proxyClient.recibir(sizeof(char))) > 0 ){		
			std::cerr << "Comando " << (char)comando << " recibido." << std::endl;

			uint32_t unixTime = proxyClient.recibir(sizeof(uint32_t));

			switch (comando){
				case 'A':{
					uint32_t numLinea = proxyClient.recibir(sizeof(uint32_t));
					if (paradas.agregarPartidaBondi(numLinea,unixTime) == 0){
						proxyClient.enviar(RESPUESTACA,sizeof(char));
						proxyClient.enviar(numLinea,sizeof(uint32_t));
					} else {
						proxyClient.enviar(RESPUESTAERROR,sizeof(char));
					}
					break;
				}
				case 'F':{
					uint32_t numLinea = proxyClient.recibir(sizeof(uint32_t));
					uint32_t numParada = proxyClient.recibir(sizeof(uint32_t));

					int cuantoFalta = paradas.cuantoFaltaParaLlegar
										(unixTime, numLinea, numParada);
					
					if (cuantoFalta>=0){
						proxyClient.enviar(RESPUESTACF,sizeof(char));
						proxyClient.enviar(cuantoFalta,sizeof(uint32_t));

					} else {
						proxyClient.enviar(RESPUESTAERROR,sizeof(char));
					}
					break;
				}
				case 'L':{
					uint32_t paradaUno,paradaDos;
					paradaUno = proxyClient.recibir(sizeof(uint32_t));
					paradaDos = proxyClient.recibir(sizeof(uint32_t));
					std::pair<size_t,size_t> mejorBondi; 
					mejorBondi = paradas.bondiMasRapido(paradaUno,paradaDos);
					proxyClient.enviar(RESPUESTACL, sizeof(char));
					proxyClient.enviar(mejorBondi.first, sizeof(uint32_t));
					proxyClient.enviar(mejorBondi.second, sizeof(uint32_t));
					break;
				}
				case 'R':{
				    uint32_t paradaUno,paradaDos;
					paradaUno = proxyClient.recibir(sizeof(uint32_t));
					paradaDos = proxyClient.recibir(sizeof(uint32_t));
					std::pair<size_t,size_t> mejorBondi;
					mejorBondi = paradas.bondiRecomendado(unixTime,paradaUno,paradaDos);
					proxyClient.enviar(RESPUETACR, sizeof(char));
					proxyClient.enviar(mejorBondi.first, sizeof(uint32_t));
					proxyClient.enviar(mejorBondi.second, sizeof(uint32_t));
					break;
				}
			}
		}
		proxyClient.cerrarConexion();
		std::cerr << "Cliente desconectado." << std::endl;
	}
};

class espearaQ :  public Thread{ 
    private:
    	ProxyClient& proxyClient;

    public:
        explicit espearaQ(ProxyClient& proxyClient) : proxyClient(proxyClient){}
        
		virtual void run() {
            std::string quit;
            for (;;){
				std::cin >> quit;
            	if (quit == "q"){
            		proxyClient.cerrarConexion();
            		return;
            	}
            }
        }
};

class aceptadorClientes :  public Thread{ 
    private:
    	ProxyClient& proxyClient;
    	Paradas& paradas;
    
    public:
        aceptadorClientes(ProxyClient& proxyClient, Paradas& paradas) :
         	proxyClient(proxyClient), paradas(paradas) {}
        
		virtual void run() {            
			std::vector<Thread*> threads;
			ProxyClient proxyClientAceptado;
			while (proxyClient.hayConexion()){
				try {
					proxyClientAceptado = proxyClient.aceptarCliente();
				}
				catch	(const std::exception &e){
					break;
				}
				Thread* th = new manejadorCliente(std::move(proxyClientAceptado),paradas);
				th->start();
				threads.push_back(th);
			}

			for(unsigned int i=0; i < threads.size(); i++){
				threads[i]->join();
				delete threads[i];
			}
		}
};

int main(int argc, char *argv[]) {
	if (argc < 4){
		std::cout << "Faltan argumentos" << std::endl;
		return 0;
	}

	Paradas paradas(argv[2], argv[3]);

    std::vector<Thread*> threads;

	ProxyClient proxyClient(atoi(argv[1]));

    threads.push_back(new espearaQ(proxyClient));

	threads.push_back(new aceptadorClientes(proxyClient,paradas));

    for (size_t i=0; i < threads.size(); i++){
    	threads[i]->start();
	}

    for (size_t i=0; i < threads.size(); i++){
    	threads[i]->join();
		delete threads[i];
	}

	return 0;
}
