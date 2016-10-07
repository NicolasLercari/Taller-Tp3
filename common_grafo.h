/*
 * grafo.h
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
#include "common_buscador.h"

#ifndef COMMON_GRAFO_H_
#define COMMON_GRAFO_H_

template <typename T>
class Grafo {
private:
	std::map<T, std::map<T,T> > grafo;
	//T grafo;
public:
	Grafo(){}

	void agregarArista(T vertice, T vecino, T peso){
		if (grafo.find(vertice) == grafo.end()){
			std::map<size_t,size_t> ady;
	   		ady.insert(std::pair<size_t,size_t>(vecino,peso));
		   	grafo.insert(std::pair<size_t,std::map<size_t,size_t> >(vertice,ady));		
		} else {
			grafo[vertice].insert(std::pair<size_t,size_t>(vecino,peso));
		}
	}
	
	T obtenerPesoArista(T vertice, T verticeVecino){
		// tira exception si no hay arista y la atrapa Paradas.
		//return grafo[vertice][verticeVecino];
		return grafo.at(vertice).at(verticeVecino);
	}
	
	T obtenerPesoCaminoDesdeHasta(std::list<T> camino, 
									T verticeInicial, T verticeFinal){	
		std::list<size_t>::iterator parada;
		parada = Buscador::find(camino.begin(),camino.end(),verticeInicial);
		T peso=0;
		while (parada != camino.end() && (*parada != verticeFinal)){
			size_t vertice = *parada;
			parada++;
			peso += this -> obtenerPesoArista(vertice, *parada);
		}
		return peso;
	}
	
	~Grafo(){}
};

#endif /* GRAFO_H_ */
