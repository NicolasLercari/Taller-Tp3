#include <iostream>
#include <sstream>      
#include <fstream>
#include <string.h>
#include <arpa/inet.h>

#include <string>
#include "common_socket.h"
#include "common_paradas.h"
#include "proxyServer.h"

#define RESPUESTAERROR 0xFF

using namespace std;

uint32_t recibirUint32_t(Socket& socket){
	char buff[5];
	bzero(buff,5);

	socket.receive(buff,sizeof(buff)-1);

	uint32_t cs = 0;

	memcpy ( &cs, buff, sizeof(cs) );

	return ntohl(cs);
}

void enviarUint32_t(Socket& socket, uint32_t entero){

	uint32_t ui = htonl(entero);

	char buff[5];

	memcpy ( buff, &ui, sizeof(buff)-1 );

	socket.send(buff,sizeof(buff)-1);
}

uint32_t recibirByte(Socket& skt){
	char respuestaComando;
	skt.receive(&respuestaComando,sizeof(respuestaComando));

	uint32_t ui;

	memcpy(&ui, &respuestaComando,1);

	return ui;
}


size_t obtenerFechaUnix(const std::string &fecha){
	int anio, mes, dia,hora, mins, segs;
	char dash;

	std::istringstream iso_date(fecha);   
	iso_date >> dash >> dia >> dash >> mes >> dash >> anio >> dash >> hora 
			>> dash >>mins >> dash >> segs >> dash;

    struct tm tm;
    time_t ts;

    memset(&tm, 0, sizeof(struct tm));
	tm.tm_year = anio - 1900;
	tm.tm_mon = mes - 1;
	tm.tm_mday = dia;
	tm.tm_hour = hora;
	tm.tm_min = mins;
	tm.tm_sec = segs;
    ts = mktime(&tm);

    return (uint32_t)ts;
}

int main(int argc, char *argv[]) {

	if (argc < 3){
		cout << "Faltan argumentos" << endl;
		return 0;
	}

	ProxyServer proxyServer(argv[1],atoi(argv[2]));

	//Socket skt; 

	//skt.connect("127.0.0.1",8080);

	for (std::string line; std::getline(std::cin, line);){
    
	    std::string fecha;
	    char comando;
		
		std::istringstream iss(line);
		iss >> std::noskipws;
		iss >> fecha >> std::ws >> comando >> std::ws;

		skt.send(&comando,sizeof(comando));
		size_t unixTime = obtenerFechaUnix(fecha);
		enviarUint32_t(skt,unixTime);
		
		switch (comando){
			case 'A':
			{
				uint32_t bondi;
				iss >> std::ws >> bondi;
				enviarUint32_t(skt, bondi);

				if (recibirByte(skt) == 0xFF){
					cout << "Error." << endl;
				} else {
					bondi = recibirUint32_t(skt);

					std::cout << "Un colectivo de la línea "
						<< bondi << " ha sido agregado.\n";
				}
				break;
			}
			case 'F': 
			{ 
				size_t bondi, parada;	 
				iss >> std::ws >> bondi >> std::ws >>parada;
				enviarUint32_t(skt,bondi);
				enviarUint32_t(skt,parada);

				if(recibirByte(skt) == 0xFF){
					cout << "Error." << endl;
				} else {
					uint32_t segundos = recibirUint32_t(skt);

					std::cout << "Faltan "
							<< segundos/60 << " minutos para que llegue"
							<<" el siguiente colectivo.\n";				
			    }
			    break;
			}
			case 'L':
			{
				size_t paradaUno, paradaDos; 
				iss >> std::ws >> paradaUno >> std::ws >>paradaDos;	

				enviarUint32_t(skt,paradaUno);
				enviarUint32_t(skt,paradaDos);

				recibirByte(skt);
				
				uint32_t bondi = recibirUint32_t(skt);
				uint32_t segundos = recibirUint32_t(skt);
				std::cout << "La línea con el recorrido más rápido es la " 
						<< bondi <<", que tarda "<< segundos/60 << " minutos y "
						<<  segundos % 60 <<" segundos en llegar a destino.\n";
				break;
			}
			case 'R':
			{
				size_t paradaUno, paradaDos; 
				iss >> std::ws >> paradaUno >> std::ws >>paradaDos;			
				enviarUint32_t(skt,paradaUno);
				enviarUint32_t(skt,paradaDos);

				recibirByte(skt);

				uint32_t bondi = recibirUint32_t(skt);
				uint32_t segundos =recibirUint32_t(skt);

				std::cout << "El colectivo de la línea "
						<< bondi <<  " tardará " << segundos/60
						<< " minutos y " << segundos % 60 
						<< " segundos en llegar a destino.\n";
				break; 
			}	 

			default: 
			    break;
		}
	}

	skt.shutdown();

	return 0;
}
