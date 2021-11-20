#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <string>
#include <cmath>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

/* TODO
	- refactor into multiple files
	- make it fast
	- write image processing code (can use library to start with)
	- inheritance for e.g. types of image, type of agent (player/npc)
	- make i,j use consistent somehow
	- proper classes code, make sure memory managed, parallelise raycaster, make variables private	
*/

using namespace std;

constexpr unsigned int image_map_width  	{512};
constexpr unsigned int image_height 		{512};
constexpr unsigned int image_view_width  	{512};

struct Pixel{
	public:
		unsigned char a;
		unsigned char r;
		unsigned char g;
		unsigned char b;

		Pixel(unsigned char A, unsigned char R, unsigned char G, unsigned char B) : a(A), r(R), g(G), b(B) {};

		Pixel(unsigned char R, unsigned char G, unsigned char B) : a(0), r(R), g(G), b(B) {};

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
	os << (int)pixel.r << " " <<  (int)pixel.g << " " << (int)pixel.b << " ";
	return os;
}

std::ostream& dump_ppmout(std::ostream& os, const Pixel& pixel){
	os << pixel.r << pixel.g << pixel.b;	
	return os;
}

class Player{
		public:

		double x_pos;
		double  y_pos;
		double view_angle;

		Player(double x, double y) : x_pos(x), y_pos(y), view_angle(0.) {};

		Player(double x, double y, double a) : x_pos(x), y_pos(y), view_angle(a) {};

		void set_position(double x, double y){
			x_pos = x;
			y_pos = y;
		}

};

class Image{
	public:
		// See about whether it's fine to make these public
		const unsigned int map_width; // Width of the 2D (map) part of the screen
		const unsigned int view_width; // Width of the 3D (view) part of the screen
		const unsigned int height;
		std::vector<Pixel> image_data;
		Map* image_map;

		// How to know when we need to define the other constructors?
		Image(unsigned int mw, unsigned int vw, unsigned int h) : map_width(mw), view_width(vw),  height(h), image_data((mw+vw)*h) {};
		Image(unsigned int mw, unsigned int vw, unsigned int h, Map &map) : map_width(mw), view_width(vw),  height(h), image_data((mw+vw)*h), image_map(&map) {};

		void set_pixel(int i, int j,const Pixel p){
			image_data[i*(map_width+view_width)+j] = p;
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
					textures[i*width+j] = Pixel(pixmap[4*(i*width+j)+3],
												pixmap[4*(i*width+j)+0],
												pixmap[4*(i*width+j)+1], 
												pixmap[4*(i*width+j)+2]);
				}
			}
			stbi_image_free(pixmap); // free the data. TODO write some code that obviates this using proper C++
		}

		Pixel get_pixel(int n, int i, int j){
			return textures[j*num_textures*texture_width+n*texture_width+i];
		}
};

void draw_rectangle(Image &image, int x, int y, int rect_width, int rect_height, Pixel colour){
// x&y are horizontal and vertical pixel position of (top-left corner of) rect, respectively.
	for(int i=0;i<rect_height;i++){
		for(int j=0;j<rect_width;j++){
			if(y+i<image.height && x+j<image_map_width+image.view_width) image.set_pixel(y+i,x+j,colour);
		}
	}
}

void draw_map(Image &image, Map &map, Texture &texture){
	int rect_width = image.map_width/map.width;
	int rect_height = image.height/map.height;
	for(unsigned int i=0; i<map.height; i++){
		for(unsigned int j=0; j<map.width; j++){
			char c = map.map[i*map.width + j];
			if (c != ' ') draw_rectangle(image, j*rect_width, i*rect_height, rect_width, rect_height, texture.get_pixel(c-'0', 0, 0));
		}
	}
}

void draw_player(Image &image, Player &player){
	double height_ratio = image.height/double(image.image_map->height); // Make it so that this fails gracefully if map not defined
	double width_ratio =  image.map_width/double(image.image_map->width); // Maybe use game_image class inherit from image, that requires map
	for(int i=-2;i<=2;i++){
		for(int j=-2;j<=2;j++){
			image.set_pixel(height_ratio*player.y_pos+i, width_ratio*player.x_pos+j, Pixel(255,255,255));	
		}
	}
}

void player_rangefinder(Player &player, Image &image, Map &map, Texture &texture, double fov){ 

	double c=0.;
	double max_range = 20;
	double width_ratio = image.map_width/double(map.width); 
	double height_ratio = image.height/double(map.height); 

	double angle = player.view_angle - fov/2.;
	for(unsigned int sweep = 0;sweep < image.map_width; sweep++){
		angle = player.view_angle - fov/2. + fov*sweep/image.map_width;	
		c=0.;
		for(;c<max_range;c+=0.01){
			double ry = player.y_pos+c*sin(angle);
			double rx = player.x_pos+c*cos(angle);
			image.set_pixel(ry*height_ratio,rx*width_ratio,Pixel(100,100,100));
			char d = map.map[int(ry)*map.width + int(rx)];
			if(d != ' '){
				int column_height = image.height/(c*cos(angle-player.view_angle));
				draw_rectangle(image, image.map_width+sweep, int(image.height/2. - column_height/2.),
					1, column_height, texture.get_pixel(d-'0',0,0));
				break;
			}
		}
	}
}

void drop_ppm_image(std::string fname, Image image ){
// Saves image to file fname
	std::ofstream ofs {fname, std::ios::binary};
	
    ofs << "P6\n" << (image.map_width+image.view_width) << " " << image.height << "\n255\n";
	for_each(image.image_data.begin(),image.image_data.end(),[&ofs](Pixel p){dump_ppmout(ofs, p);});

	ofs.close();
}

int main(){

	// should be: y/height/j, x/width/i ???????
	

	Map map(16,16, 		"0000222222220000"\
                       	"1              0"\
                       	"1      11111   0"\
                       	"1     0        0"\
                        "0     0  1110000"\
                        "0     3        0"\
                        "0   10000      0"\
                        "0   3   11100  0"\
                        "5   4   0      0"\
                        "5   4   1  00000"\
                        "0       1      0"\
                        "2       1      0"\
                        "0       0      0"\
						"0 0000000      0"\
                       	"0              0"\
                       	"0002222222200000"); // our game map [ssloy]

	// load the textures using the library, into a vector of images
	// image index corresponds to number in mapstring

	Texture texture("walltext.png");



	// simple dumb rotation stream - later this will be done in a loop controlled by the player in a gui
	double a=0;
	for(int x=0;x<360;x++){
		a+=2*M_PI/360.;
		Image image(image_map_width, image_view_width, image_height, map);
		for(unsigned int i=0;i<image_height;i++){
			for(unsigned int j=0;j<image_map_width+image_view_width;j++){
					image.set_pixel(i,j,Pixel(255,255,255));
			}
		}
		draw_map(image, map,texture);
		Player player(2.3,2.3,a);
		draw_player(image, player);
		player_rangefinder(player, image, map, texture, M_PI/3);
		string outputf = "output_" + to_string(x) + ".ppm";;
		// draw texture 1 in tl corner of screen
		for(int i=0;i<texture.texture_height;i++){
			for(int j=0;j<texture.texture_width;j++){
				image.set_pixel(i,j,texture.get_pixel(0,j,i));
			}
		}
		drop_ppm_image(outputf,image);
		cout << x << " done" << endl;
	}
	
	return 0;
}
