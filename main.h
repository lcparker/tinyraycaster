#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;

struct Pixel{
	public:
		unsigned char r;
		unsigned char g;
		unsigned char b;
		unsigned char a;

		Pixel( unsigned char R, unsigned char G, unsigned char B, unsigned char A) : r(R), g(G), b(B), a(A) {};

		Pixel(unsigned char R, unsigned char G, unsigned char B) : a(255), r(R), g(G), b(B) {};

		Pixel() : a(0), r(0), g(0), b(0) {};

		friend std::ostream& operator<<(std::ostream& os, const Pixel& pixel);
		friend std::ostream& dump_ppmout(const Pixel& pixel);

};

struct Map{
	public:
		unsigned int width;
		unsigned int height;
		const std::string map;
		const std::vector<Pixel> colours {Pixel(100, 100, 100),Pixel(200,50,50),Pixel(0,100,100)};

		Map(unsigned int w, unsigned int h, std::string s) : width(w), height(h), map(s) {assert(s.size() == w*h);}
};

std::ostream& operator<<(std::ostream& os, const Pixel& pixel){
	os << (int)pixel.r << " " <<  (int)pixel.g << " " << (int)pixel.b;
	return os;
}

std::ostream& dump_ppmout(std::ostream& os, const Pixel& pixel){
	os << pixel.r << pixel.g << pixel.b;
	return os;
}

class Character{
public:
	double x_pos;
	double y_pos;
	
	Character(double x, double y) : x_pos(x), y_pos(y) {};

	void set_position(double x, double y){
		x_pos = x;
		y_pos = y;
	}
};


class Player {
private:
	double view_angle;
public:
	double x_pos;
	double y_pos;

	Player(double x, double y) : x_pos(x), y_pos(y), view_angle(0.) {};
	Player(double x, double y, double a) : x_pos(x), y_pos(y), view_angle(a) {
		while(abs(view_angle)>M_PI){
			if(view_angle>M_PI){
				view_angle-=2*M_PI;
			} else if(view_angle<M_PI){
				view_angle+=2*M_PI;
			}
		}
	}

	double get_angle(){ return view_angle;}

};

class Enemy { // TODO Make this and Player come from the same base class somehow?
public:
	double x_pos;
	double y_pos;
	unsigned int texture_id; // ID of the texture in the creature texture file.
	
	Enemy(double x, double y, unsigned int n) : x_pos(x), y_pos(y), texture_id(n) {};
};

class Image{
	public:
		// See about whether it's fine to make these public
		const unsigned int map_width; // Width of the 2D (map) part of the screen
		const unsigned int view_width; // Width of the 3D (view) part of the screen
		const unsigned int height;
		std::vector<Pixel> image_data;
		std::vector<double> depth;
		Map* image_map;

		// How to know when we need to define the other constructors?
		Image(unsigned int mw, unsigned int vw, unsigned int h) : map_width(mw), view_width(vw),  height(h), image_data((mw+vw)*h) {depth.reserve(vw);}
		Image(unsigned int mw, unsigned int vw, unsigned int h, Map &map) : map_width(mw), view_width(vw),  height(h), image_data((mw+vw)*h), image_map(&map) {depth.reserve(vw);}

		void set_pixel(int i, int j,const Pixel p){
			if (p.a > 128) image_data[i*(map_width+view_width)+j] = p;
		}

		void set_map(Map &map){
			image_map = &map;
		}

		std::unique_ptr<Pixel> get_pixel(int i, int j){
			std::unique_ptr<Pixel> pixel(new Pixel(image_data[i*(map_width+view_width)+j]));
			return pixel; // Returns a copy of the pixel at image_data[i,j] w/o having to worry about alteration? Is this the right way to do it?
		}

};

class Texture {
	private:
		std::vector<Pixel> textures;
	public:
		unsigned int num_textures; // TODO make private
		unsigned int texture_width;
		unsigned int texture_height;

		Texture(const string filename){
			// For now, stb_image library code mostly taken from sslow/tinyraycaster
			// TODO write code to interface the textures natively with Pixel structs
			int width,height,numchannels;
			unsigned char *pixmap = stbi_load(filename.c_str(), &width, &height, &numchannels, 0); // see library file for description of this
			// TODO implement error checking
			if(!pixmap) cerr << "error: failed to load" << endl;
			if(numchannels != 4) cerr << "error: values not in RGBA form" << endl;

			textures.reserve(width*height);
			num_textures = width/height;
			texture_width=width/num_textures; // assumes square textures
			texture_height=height;

			for(int i=0;i<height;i++){
				for(int j=0;j<width;j++){
					textures[i*width+j] = Pixel(pixmap[4*(i*width+j)+0],
												pixmap[4*(i*width+j)+1],
												pixmap[4*(i*width+j)+2], 
												pixmap[4*(i*width+j)+3]);
				}
			}
			stbi_image_free(pixmap); // free the data. TODO write some code that obviates this using proper C++
		}

		Pixel get_pixel(int n, int i, int j){
			return textures[j*num_textures*texture_width+n*texture_width+i];
		}
};
