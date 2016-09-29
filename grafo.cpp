t/*
 * grafo.cpp
 *
 *  Created on: 22/09/2016
 *      Author: nicolas
 */

#include "grafo.h"


Grafo::Grafo(){
	std::ifstream archgrafo("grafo.txt");

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


