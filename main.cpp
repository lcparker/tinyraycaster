#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <string>


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

		unsigned int x_pos;
		unsigned int y_pos;

		Player(unsigned int horizontal, unsigned int vertical) : x_pos(horizontal), y_pos(vertical) {};

		void set_position(unsigned int horizontal, unsigned int vertical){
			x_pos = horizontal;
			y_pos = vertical;
		}
};

class Image{
		unsigned int width;
		unsigned int height;
	public:
		std::vector<Pixel> image_data;

		Image(unsigned int w, unsigned int h) : width(w), height(h), image_data(w*h) {};

		constexpr unsigned int get_width(){ // is this the right use of constexpr? 
			return width;
		}
		constexpr unsigned int get_height(){
			return height;
		}	
		
		void set_pixel(int i, int j,const Pixel p){
			image_data[i*width+j] = p;
		}

		std::unique_ptr<Pixel> get_pixel(int i, int j){
			std::unique_ptr<Pixel> pixel(new Pixel(image_data[i*width+j]));
			return pixel; // Returns a copy of the pixel at image_data[i,j] w/o having to worry about alteration
		}

};

void draw_rectangle(Image &image, int x, int y, int rect_width, int rect_height){
	Pixel blank_pixel(255,255,255);
	for(int i=0;i<rect_height;i++){
		for(int j=0;j<rect_width;j++){
			image.set_pixel(y*rect_height+i,x*rect_width+j,blank_pixel);
			//std::cout << "pixel set to\t" << rect_x+j << "\t" << rect_y+i << "\t" << "to: " << image.image_data[(rect_y+i)*image.get_width() + rect_x+j] << std::endl;
		}
	}
}

void draw_map(Image &image, Map &map){
	int rect_width = image.get_width()/map.width;
	int rect_height = image.get_height()/map.height;
	for(unsigned int i=0; i<map.height; i++){
		for(unsigned int j=0; j<map.width; j++){
			if (map.map[i*map.width + j] != ' ') draw_rectangle(image, j, i, rect_width, rect_height);
		}
	}
}

void draw_player(Image &image, Player &player){
	for(int i=-2;i<=2;i++){
		for(int j=-2;j<=2;j++){
			image.set_pixel(player.y_pos+i, player.x_pos+j, Pixel(255,255,255));	
		}
	}
}

void drop_ppm_image(std::string fname, Image image ){
// Saves image to file fname
	std::ofstream ofs {fname, std::ios::binary};
	
    ofs << "P6\n" << image.get_width() << " " << image.get_height() << "\n255\n";
	for_each(image.image_data.begin(),image.image_data.end(),[&ofs](Pixel p){dump_ppmout(ofs, p);});

	ofs.close();
}

int main(){

	// should be: y/height/j, x/width/i ???????
	
	constexpr unsigned int image_width  	{512};
	constexpr unsigned int image_height 	{512};

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

	Image image(image_width, image_height);
	for(unsigned int i=0;i<image_height;i++){
		for(unsigned int j=0;j<image_width;j++){
			image.set_pixel(i,j,Pixel(256*i/image.get_height(), 256*j/image.get_width(),0));
		}
	}

	draw_map(image, map);
	Player player(80,80);
	draw_player(image, player);
	drop_ppm_image("output.ppm",image);
	
	return 0;
}
