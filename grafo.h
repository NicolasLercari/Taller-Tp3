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

#ifndef GRAFO_H_
#define GRAFO_H_

template<class T>
class Grafo {

private:
	std::map<T, std::map<T,T> > grafo;
	
public:
	Grafo(){	
		std::ifstream archgrafo("paradas.txt");

	    for (std::string line; std::getline(archgrafo, line);) {
			size_t verticeInicial,verticeFinal,tiempo;
			std::istringstream iss(line);
			iss >> std::noskipws;
			iss >> verticeInicial >> std::ws >> verticeFinal >> std::ws >> tiempo;
					
			if (grafo.find( verticeInicial ) == grafo.end()){
				std::map<size_t,size_t> ady;
		   		ady.insert(std::pair<size_t,size_t>(verticeFinal,tiempo));
			   	grafo.insert(std::pair<size_t,std::map<size_t,size_t> >(verticeInicial,ady));		
			} else {
				grafo[verticeInicial].insert(std::pair<size_t,size_t>(verticeFinal,tiempo));
			} 
	    }
	}
	
	T obtenerPesoArista(T vertice, T verticeVecino){
		return grafo[vertice][verticeVecino];
	}
	
	~Grafo(){}
};

#endif /* GRAFO_H_ */
