#pragma once
#include <cmath>
#include <iostream>

struct Pixel{
	public:
		unsigned char r;
		unsigned char g;
		unsigned char b;
		unsigned char a;

		Pixel(unsigned char R, unsigned char G, unsigned char B, unsigned char A) : r(R), g(G), b(B), a(A) {};

		Pixel(unsigned char R, unsigned char G, unsigned char B) :  r(R), g(G), b(B), a(255) {};

		Pixel() :  r(0), g(0), b(0), a(0) {};

		friend std::ostream& operator<<(std::ostream& os, const Pixel& pixel);
		friend std::ostream& dump_ppmout(const Pixel& pixel);

};

std::ostream& operator<<(std::ostream& os, const Pixel& pixel){
	os << (int)pixel.r << " " <<  (int)pixel.g << " " << (int)pixel.b;
	return os; // human-formatted output
}

std::ostream& dump_ppmout(std::ostream& os, const Pixel& pixel){
	os << pixel.r << pixel.g << pixel.b; 
	return os; // ppm-formatted output
}

double amod(double theta){
	while(std::abs(theta) > M_PI){
		if(theta>M_PI) {
			theta-=2*M_PI;
		} else if(theta<M_PI){
			theta+=2*M_PI;
		}
	}

	return theta;
}
