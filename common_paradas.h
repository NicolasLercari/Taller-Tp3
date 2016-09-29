/*
 * parada.h
 *
 *  Created on: 22/09/2016
 *      Author: nicolas
 */
#include <map>
#include <list>
#include <utility>
#include "grafo.h"
#include <mutex>
#ifndef PARADA_H_
#define PARADA_H_

class Paradas {
private:
	//Grafo<size_t> paradas;
	std::mutex m;
	std::map<size_t, std::map<size_t,size_t> > paradas;
	std::map<size_t, std::list<size_t> > paradasPorBondi;
	std::map<size_t, std::list<size_t> > bondisHorarios;
	std::map<size_t,std::list<size_t> > recorridosBondis;
public:
	Paradas();
	std::pair<size_t,size_t> bondiMasRapido(size_t paradaUno,size_t paradaDos);
	std::pair<size_t, size_t> bondiRecomendado(size_t unixTime,size_t paradaUno,size_t paradaDos);
	int agregarPartidaBondi(size_t bondi,size_t unixTime);
	int cuantoFaltaParaLlegar(size_t unixTimeAct,size_t bondi, size_t parada);
	~Paradas();
private:
	size_t cuantoTarda(size_t bondi,size_t paradaUno, size_t paradaDos);
};

#endif /* PARADA_H_ */
