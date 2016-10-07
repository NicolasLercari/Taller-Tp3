/*
 * parada.cpp
 *
 *  Created on: 22/09/2016
 *      Author: nicolas
 */
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <list>

#include <utility>  
#include <string>

#include "common_grafo.h"
#include "common_paradas.h"
#include "common_buscador.h"

Paradas::Paradas(char* nombreArchParadas, char* nombreArchBondis){
	std::ifstream archParadas(nombreArchParadas);

    for (std::string line; std::getline(archParadas, line);) {
		size_t verticeInicial,verticeFinal,tiempo;
		std::istringstream iss(line);
		iss >> std::noskipws;
		iss >> verticeInicial >> std::ws >> verticeFinal >> std::ws >> tiempo;
				
		grafoParadas.agregarArista(verticeInicial, verticeFinal, tiempo);		
    }

	std::ifstream archBondis(nombreArchBondis);

    for (std::string line; std::getline(archBondis, line);) {
		size_t bondi;
		std::istringstream iss(line);
		iss >> std::noskipws;
		iss >> bondi;

		std::list<size_t> listParadas;
		for(int parada ; iss >> std::ws >> parada;){
			listParadas.push_back(parada);
			if (paradasPorBondi.find(parada) == paradasPorBondi.end()){
				std::list<size_t> lista;
				lista.push_back(bondi);	
				paradasPorBondi.insert(std::pair<size_t, std::list<size_t> >(parada,lista));
			} else {
				paradasPorBondi[parada].push_back(bondi);
			}
		}
		recorridosBondis.insert(std::pair<size_t, std::list<size_t> >
												(bondi,listParadas));
		std::list<size_t> lista;
		bondisHorarios.insert(std::pair<size_t, std::list<size_t> >(bondi,lista));
    }
}

int Paradas::cuantoFaltaParaLlegar(size_t unixTimeAct, 
											size_t bondi, size_t parada){
	if (bondisHorarios.find(bondi) != bondisHorarios.end()){
		std::list<size_t> lParadas = recorridosBondis[bondi];
		// si la parada destino no es valida, no pertence a las paradas del bondi
		// es un error contemplado.
		if (Buscador::find(lParadas.begin(), lParadas.end(), parada) == 
															lParadas.end()){
			return -1;
		}
		size_t paradaInicial = recorridosBondis[bondi].front();
		int bondiTarda = cuantoTarda(bondi,paradaInicial,parada);
		if (bondiTarda < 0) return -1;
		m.lock();
		std::list<size_t> partidas = bondisHorarios[bondi];

		for (auto const &partida : partidas){
			int cuantoFalta = partida + bondiTarda - unixTimeAct;
			if (cuantoFalta >= 0){
				m.unlock();
				return cuantoFalta;
			}
    	}
    	m.unlock();
	}
	return -1;
}

int Paradas::agregarPartidaBondi(size_t bondi,size_t unixTime){
	if (bondisHorarios.find(bondi) != bondisHorarios.end()){
		m.lock();
		bondisHorarios[bondi].push_back(unixTime);
		m.unlock();
		return 0;
	}
	return -1;
}

size_t Paradas::cuantoTarda(size_t bondi,size_t paradaUno, size_t paradaDos){
	std::list<size_t> listaParadas = recorridosBondis[bondi];
	return grafoParadas.obtenerPesoCaminoDesdeHasta
									(listaParadas, paradaUno, paradaDos);
}

std::list<size_t> intersepcion(std::list<size_t> listaUno, 
												std::list<size_t> listaDos){
	std::map<size_t,size_t> intersepcion;
	std::list<size_t> lIntersepcion;
	for (std::list<size_t>::iterator it=listaUno.begin(); 
												it != listaUno.end(); ++it){
		intersepcion.insert(std::pair<size_t,size_t>(*it,1));
	}
	for (std::list<size_t>::iterator it=listaDos.begin(); 
												it != listaDos.end(); ++it){
		if (intersepcion.find(*it) != intersepcion.end()){
			lIntersepcion.push_back(*it);
		}
	}
	return lIntersepcion;
}

std::pair<size_t, size_t> Paradas::bondiRecomendado(size_t unixTime, 
											size_t paradaUno, size_t paradaDos){
	std::list<size_t> lParadaUno = paradasPorBondi[paradaUno];
	std::list<size_t> lParadaDos = paradasPorBondi[paradaDos];

	std::list<size_t> lIntersepcion = intersepcion(lParadaUno,lParadaDos);

	size_t bondi = lIntersepcion.front();
  	int cuantoFalta = cuantoFaltaParaLlegar(unixTime, bondi, paradaUno);
  	int bondiCuantoTarda = cuantoTarda(bondi, paradaUno, paradaDos);
	size_t bondiRecomendado = bondi;
	size_t menorTiempo = cuantoFalta + bondiCuantoTarda;

	for (auto const &bondi : lIntersepcion){			
	  	int falta = cuantoFaltaParaLlegar(unixTime, bondi, paradaUno);
	  	if (falta < 0) continue;
	  	
	  	int tarda = cuantoTarda(bondi, paradaUno, paradaDos);
	  	// si fallo el es un error devuelvo un valor 
	  	// invalido en bondi
	  	if (tarda < 0) return std::pair<size_t,size_t>(0,0);

	  	size_t tiempo = falta + tarda;

	  	if (tiempo < menorTiempo){
	  		menorTiempo = tiempo;
	  		bondiRecomendado = bondi;
	  	}
	}
	return std::pair<size_t,size_t>(bondiRecomendado,menorTiempo);  	
}


std::pair<size_t,size_t> Paradas::bondiMasRapido
											(size_t paradaUno,size_t paradaDos){
	std::list<size_t> lParadaUno = paradasPorBondi[paradaUno];
	std::list<size_t> lParadaDos = paradasPorBondi[paradaDos];

	std::list<size_t> lIntersepcion = intersepcion(lParadaUno,lParadaDos);

  	std::list<size_t>::iterator it=lIntersepcion.begin();
  	size_t bondiMenorTiempo = *it;
  	int menorTiempo = cuantoTarda(*it,paradaUno,paradaDos);
  	for (std::list<size_t>::iterator it=lIntersepcion.begin(); 
  										it != lIntersepcion.end(); ++it){
		int tarda = cuantoTarda(*it,paradaUno,paradaDos);
	  	// si fallo el es un error devuelvo un valor 
	  	// invalido en bondi (0 en este caso)
	  	if (tarda < 0) return std::pair<size_t,size_t>(0,0);
		if (tarda < menorTiempo){
			menorTiempo = tarda;
			bondiMenorTiempo = *it;
		}
	}
	return std::pair<size_t,size_t>(bondiMenorTiempo,menorTiempo);
}



Paradas::~Paradas(){}
