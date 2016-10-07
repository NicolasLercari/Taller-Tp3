
#include <iostream>

#ifndef COMMON_BUSCADOR_H_
#define COMMON_BUSCADOR_H_


class Buscador{
public:
	Buscador(){}
	
	template<class InputIterator, class T>
	static InputIterator find(InputIterator first, 
					InputIterator last, const T& val){
		while (first!=last) {
			if (*first==val) return first;
			++first;
		}
		return last;
	}
	
	~Buscador(){}
};

#endif
