#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <string>
#include <cmath>

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

		Map(unsigned int w, unsigned int h, std::string s) : width(w), height(h), map(s) {assert(s.size() == w*h);};
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

void draw_rectangle(Image &image, int x, int y, int rect_width, int rect_height, Pixel colour){
// x&y are horizontal and vertical pixel position of (top-left corner of) rect, respectively.
	for(int i=0;i<rect_height;i++){
		for(int j=0;j<rect_width;j++){
			image.set_pixel(y+i,x+j,colour);
		}
	}
}

void draw_map(Image &image, Map &map){
	int rect_width = image.map_width/map.width;
	int rect_height = image.height/map.height;
	for(unsigned int i=0; i<map.height; i++){
		for(unsigned int j=0; j<map.width; j++){
			if (map.map[i*map.width + j] != ' ') draw_rectangle(image, j*rect_width, i*rect_height, rect_width, rect_height, Pixel(0,255,255));
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

void player_rangefinder(Player &player, Image &image, Map &map, double fov){ 

	double c=0.;
	double max_range = 20;
	double width_ratio = image.map_width/double(map.width); 
	double height_ratio = image.height/double(map.height); 

	double angle = player.view_angle - fov/2.;
	for(unsigned int sweep = 0;sweep < image.map_width; sweep++){
		angle+=fov/image.map_width;	
		c=0.;
		for(;c<max_range;c+=0.05){
			double ry = player.y_pos+c*sin(angle);
			double rx = player.x_pos+c*cos(angle);
			image.set_pixel(ry*height_ratio,rx*width_ratio,Pixel(100,100,100));
			if(map.map[int(ry)*map.width + int(rx)] != ' '){
				draw_rectangle(image, image.map_width+sweep, image.height/2 - image.height/(2*c),
					1, int(image.height/c), Pixel(255, 0,255));
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
	

	Map map(16,16,		"0000222222220000"\
                       	"1              0"\
                       	"1      11111   0"\
                       	"1     0        0"\
                       	"0     0  1110000"\
                       	"0     3        0"\
                       	"0   10000      0"\
                       	"0   0   11100  0"\
                       	"0   0   0      0"\
                       	"0   0   1  00000"\
                       	"0       1      0"\
                       	"2       1      0"\
                       	"0       0      0"\
                       	"0 0000000      0"\
                       	"0              0"\
                       	"0002222222200000"); // our game map [ssloy]

	// simple dumb rotation stream - later this will be done in a loop controlled by the player in a gui
	double a=0;
	for(int x=0;x<360;x++){
		a+=2*M_PI/260.;
		Image image(image_map_width, image_view_width, image_height, map);
		for(unsigned int i=0;i<image_height;i++){
			for(unsigned int j=0;j<image_map_width;j++){
				image.set_pixel(i,j,Pixel(256*i/image.height, 256*j/image.map_width,0));
			}
		}
		draw_map(image, map);
		Player player(2.3,2.3,a);
		cout << player.view_angle << endl;
		cout << a << endl;
		draw_player(image, player);
		player_rangefinder(player, image, map, M_PI/2);
		string outputf = "output_" + to_string(x) + ".ppm";;
		drop_ppm_image(outputf,image);
		cout << x << " done" << endl;
	}
	
	return 0;
}
