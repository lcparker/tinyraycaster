#pragma once

#include <vector>
#include <iostream>

#include "main.h"
#include "map.h"

class Image{
	public:
		// See about whether it's fine to make these public
		const unsigned int map_width; // Width of the 2D (map) part of the screen
		const unsigned int view_width; // Width of the 3D (view) part of the screen
		const unsigned int height;
		std::vector<Pixel> image_data;
		std::vector<double> depth;
		Map* image_map = nullptr;

		Image(unsigned int mw, unsigned int vw, unsigned int h) : map_width(mw), view_width(vw),  height(h), image_data((mw+vw)*h) {depth.reserve(vw);}
		Image(unsigned int mw, unsigned int vw, unsigned int h, Map &map) : map_width(mw), view_width(vw),  height(h), image_data((mw+vw)*h), image_map(&map) {depth.reserve(vw);}

		void set_pixel(int i, int j,const Pixel p){
			if (p.a > 128) image_data[i*(map_width+view_width)+j] = p; // PPM image format not alpha-friendly
		}

		void set_map(Map &map){
			image_map = &map;
		}

		Pixel get_pixel(int i, int j){
			return Pixel(image_data[i*(map_width+view_width)+j]);
		}
};

