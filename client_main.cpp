#include <iostream>
#include <sstream>      
#include <fstream>
#include <string.h>
#include <arpa/inet.h>

#include <string>
#include "common_socket.h"
#include "common_paradas.h"
#include "common_proxyServer.h"

#define RESPUESTAERROR 0xFF

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
		std::cout << "Faltan argumentos" << std::endl;
		return 0;
	}

	ProxyServer proxyServer(argv[1],atoi(argv[2]));

	for (std::string line; std::getline(std::cin, line);){
	    std::string fecha;
	    char comando;
		
		std::istringstream iss(line);
		iss >> std::noskipws;
		iss >> fecha >> std::ws >> comando >> std::ws;

		proxyServer.enviar(comando, sizeof(char));

		uint32_t unixTime = obtenerFechaUnix(fecha);
		proxyServer.enviar(unixTime, sizeof(unixTime));
		
		switch (comando){
			case 'A':
			{
				uint32_t bondi;
				iss >> std::ws >> bondi;

				proxyServer.enviar(bondi, sizeof(bondi));
				
				if(proxyServer.recibir(sizeof(char)) == 0xFF){
					std::cout << "Error." << std::endl;
				} else {
					bondi = proxyServer.recibir(sizeof(bondi));
					std::cout << "Un colectivo de la línea "
							<< bondi << " ha sido agregado.\n";
				}
				break;
			}
			case 'F': 
			{ 
				uint32_t bondi, parada;	 
				iss >> std::ws >> bondi >> std::ws >>parada;

				proxyServer.enviar(bondi,sizeof(bondi));
				proxyServer.enviar(parada,sizeof(parada));

				if(proxyServer.recibir(sizeof(char)) == 0xFF){
					std::cout << "Error." << std::endl;
				} else {
					uint32_t segundos = proxyServer.recibir(sizeof(segundos));

					std::cout << "Faltan "
							<< segundos/60 << " minutos para que llegue"
							<<" el siguiente colectivo.\n";				
			    }
			    break;
			}
			case 'L':
			{
				uint32_t paradaUno, paradaDos; 
				iss >> std::ws >> paradaUno >> std::ws >>paradaDos;	

				proxyServer.enviar(paradaUno, sizeof(paradaUno));
				proxyServer.enviar(paradaDos, sizeof(paradaDos));

				proxyServer.recibir(sizeof(char));
	
				uint32_t bondi = proxyServer.recibir(sizeof(bondi));

				uint32_t segundos = proxyServer.recibir(sizeof(segundos));

				std::cout << "La línea con el recorrido más rápido es la " 
						<< bondi <<", que tarda "<< segundos/60 << " minutos y "
						<<  segundos % 60 <<" segundos en llegar a destino.\n";
				break;
			}
			case 'R':
			{
				uint32_t paradaUno, paradaDos; 
				iss >> std::ws >> paradaUno >> std::ws >>paradaDos;			
				proxyServer.enviar(paradaUno, sizeof(paradaUno));
				proxyServer.enviar(paradaDos, sizeof(paradaDos));

				proxyServer.recibir(sizeof(char));
	
				uint32_t bondi = proxyServer.recibir(sizeof(bondi));

				uint32_t segundos = proxyServer.recibir(sizeof(segundos));

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
	return 0;
}
