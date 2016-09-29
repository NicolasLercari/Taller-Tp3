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
#include <algorithm>  

#include "common_paradas.h"

Paradas::Paradas(){
	std::ifstream archParadas("paradas.txt");

    for (std::string line; std::getline(archParadas, line);) {
		size_t verticeInicial,verticeFinal,tiempo;
		std::istringstream iss(line);
		iss >> std::noskipws;
		iss >> verticeInicial >> std::ws >> verticeFinal >> std::ws >> tiempo;
				
		if (paradas.find( verticeInicial ) == paradas.end()){
			std::map<size_t,size_t> ady;
	   		ady.insert(std::pair<size_t,size_t>(verticeFinal,tiempo));
		   	paradas.insert(std::pair<size_t,std::map<size_t,size_t> >(verticeInicial,ady));		
		} else {
			paradas[verticeInicial].insert(std::pair<size_t,size_t>(verticeFinal,tiempo));
		} 
    }

	std::ifstream archBondis("bondis.txt");

    for (std::string line; std::getline(archBondis, line);) {
		size_t bondi;
		std::istringstream iss(line);
		iss >> std::noskipws;
		iss >> bondi;

		std::list<size_t> listParadas;
		for(int parada ; iss >> std::ws >> parada;){
			listParadas.push_back(parada);
			if (paradasPorBondi.find( parada ) == paradasPorBondi.end()){
				std::list<size_t> lista;
				lista.push_back(bondi);	
				paradasPorBondi.insert(std::pair<size_t, std::list<size_t> >(parada,lista));
			} else {
				paradasPorBondi[parada].push_back(bondi);
			}
		}
		recorridosBondis.insert(std::pair<size_t, std::list<size_t> >(bondi,listParadas));
		std::list<size_t> lista;
		bondisHorarios.insert(std::pair<size_t, std::list<size_t> >(bondi,lista));
    }
}

// HARDCODEADO la parada inicial
int Paradas::cuantoFaltaParaLlegar(size_t unixTimeAct, size_t bondi, size_t parada){

	if (bondisHorarios.find( bondi ) != bondisHorarios.end()){
		// si la parada destino no es valida, no pertence a las paradas del bondi
		std::list<size_t> lParadas = recorridosBondis[bondi];
		if ( std::find(lParadas.begin(), lParadas.end(), parada) == lParadas.end()){
			return -1;
		}
		size_t paradaInicial = recorridosBondis[bondi].front();
		size_t bondiTarda = cuantoTarda(bondi,paradaInicial,parada);
		std::list<size_t> partidas = bondisHorarios[bondi];

		for (auto const &partida : partidas){
			int cuantoFalta = partida + bondiTarda - unixTimeAct;
			if (cuantoFalta >= 0){
				return cuantoFalta;
			}
    	}
	}
	return -1;
}

int Paradas::agregarPartidaBondi(size_t bondi,size_t unixTime){
	m.lock();
	if (bondisHorarios.find( bondi ) != bondisHorarios.end()){
		bondisHorarios[bondi].push_back(unixTime);
		m.unlock();
		return 0;
	}
	m.unlock();
	return -1;
}

// modificar con la agregacion de recorridosBondis
size_t Paradas::cuantoTarda(size_t bondi,size_t paradaUno, size_t paradaDos){
	/*std::map<size_t,size_t> adyacentes = paradas[paradaUno];
	std::map<size_t,size_t>::iterator it=adyacentes.begin();
	size_t tarda = 0;

	while ( it!=adyacentes.end() ){
		std::list<size_t> bondis;
		bondis = paradasPorBondi[it->first];
		if (std::find(bondis.begin(), bondis.end(), bondi) != bondis.end()){
			tarda += it->second;
			if (paradas.find(it->first) == paradas.end()){
				break;
			}
			adyacentes = paradas[it->first];
			it = adyacentes.begin();
			continue;
		}
		++it;
	}*/
	//std::list<size_t>::iterator parada=recorridosBondis[bondi].begin();
	std::list<size_t> listaParadas = recorridosBondis[bondi];
	std::list<size_t>::iterator parada = std::find(listaParadas.begin(),listaParadas.end(),paradaUno);
	size_t tarda=0;
	while (parada != listaParadas.end() && (*parada != paradaDos)){
		// obtengo el peso de la arista
		std::map<size_t,size_t> ady = paradas[*parada];
		parada++;
		tarda += ady[*parada];
	}
	return tarda;
}

std::pair<size_t, size_t> Paradas::bondiRecomendado(size_t unixTime, size_t paradaUno, size_t paradaDos){
	std::list<size_t> lParadaUno = paradasPorBondi[paradaUno];
	std::list<size_t> lParadaDos = paradasPorBondi[paradaDos];

	// BUSCO LOS ELEMENTOS EN COMUN ENTRE lparadauno y lparados
	std::map<size_t,size_t> intersepcion;
	std::list<size_t> lIntersepcion;
	for (std::list<size_t>::iterator it=lParadaUno.begin(); it != lParadaUno.end(); ++it){
		intersepcion.insert(std::pair<size_t,size_t>(*it,1));
	}
	for (std::list<size_t>::iterator it=lParadaDos.begin(); it != lParadaDos.end(); ++it){
		if (intersepcion.find(*it) != intersepcion.end()){
			lIntersepcion.push_back(*it);
		}
	}
	size_t bondi = lIntersepcion.front();
  	size_t cuantoFalta = cuantoFaltaParaLlegar(unixTime, bondi, paradaUno);
  	size_t bondiCuantoTarda = cuantoTarda(bondi, paradaUno, paradaDos);
	size_t bondiRecomendado = bondi;
	size_t menorTiempo = cuantoFalta + bondiCuantoTarda;

	for (auto const &bondi : lIntersepcion){			
	  	int falta = cuantoFaltaParaLlegar(unixTime, bondi, paradaUno);
	  	if (falta < 0) continue;
	  	
	  	size_t tarda = cuantoTarda(bondi, paradaUno, paradaDos);
	  	//if (tarda < 0) return std::pair<size_t,size_t>(0,0);

	  	size_t tiempo = falta + tarda;

	  	if (tiempo < menorTiempo){
	  		menorTiempo = tiempo;
	  		bondiRecomendado = bondi;
	  	}
	}
	return std::pair<size_t,size_t>(bondiRecomendado,menorTiempo);  	
}



std::pair<size_t,size_t> Paradas::bondiMasRapido(size_t paradaUno,size_t paradaDos){
	std::list<size_t> lParadaUno = paradasPorBondi[paradaUno];
	std::list<size_t> lParadaDos = paradasPorBondi[paradaDos];

	// BUSCO LOS ELEMENTOS EN COMUN ENTRE lparadauno y lparados
	std::map<size_t,size_t> intersepcion;
	std::list<size_t> lIntersepcion;
	for (std::list<size_t>::iterator it=lParadaUno.begin(); it != lParadaUno.end(); ++it){
		intersepcion.insert(std::pair<size_t,size_t>(*it,1));
	}
	for (std::list<size_t>::iterator it=lParadaDos.begin(); it != lParadaDos.end(); ++it){
		if (intersepcion.find(*it) != intersepcion.end()){
			lIntersepcion.push_back(*it);
		}
	}

  	std::list<size_t>::iterator it=lIntersepcion.begin();
  	size_t bondiMenorTiempo = *it;
  	size_t menorTiempo = cuantoTarda(*it,paradaUno,paradaDos);

  	for (std::list<size_t>::iterator it=lIntersepcion.begin(); it != lIntersepcion.end(); ++it){
		size_t tarda = cuantoTarda(*it,paradaUno,paradaDos);
		if (tarda < menorTiempo){
			menorTiempo = tarda;
			bondiMenorTiempo = *it;
		}
	}
	return std::pair<size_t,size_t>(bondiMenorTiempo,menorTiempo);
}

Paradas::~Paradas(){

}

