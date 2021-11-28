#pragma once

#include <string>
#include <vector>

struct Map{
	public:
		unsigned int width;
		unsigned int height;
		const std::string map;
		const std::vector<Pixel> colours {Pixel(100, 100, 100),Pixel(200,50,50),Pixel(0,100,100)};

		Map(unsigned int w, unsigned int h, std::string s) : width(w), height(h), map(s) {assert(s.size() == w*h);}
};
