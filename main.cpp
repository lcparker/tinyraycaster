#include "main.h"

/* TODO
	- make it fast
	- write image processing code (can use library to start with)
	- inheritance for e.g. types of image, type of agent (player/npc)
	- make i,j use consistent somehow
	- proper classes code, make sure memory managed, parallelise raycaster, make variables private	
*/

// CURRENTLY: Version 1, the dumb, slow version

using namespace std;

constexpr unsigned int image_map_width  	{512};
constexpr unsigned int image_height 		{512};
constexpr unsigned int image_view_width  	{512};


void draw_rectangle(Image &image, int x, int y, int rect_width, int rect_height, Pixel colour){
// x&y are horizontal and vertical pixel position of (top-left corner of) rect, respectively.
	for(int i=0;i<rect_height;i++){
		for(int j=0;j<rect_width;j++){
			if(y+i<image.height && x+j<image_map_width+image.view_width) image.set_pixel(y+i,x+j,colour);
		}
	}
}

void draw_view_texture_rectangle(Image &image, int x, int y, int rect_width, int rect_height,Texture &texture, int texture_num, int texture_pos){
// x&y are horizontal and vertical pixel position of (top-left corner of) rect, respectively.
	for(unsigned int i=0;i<rect_height;i++){
		for(unsigned int j=0;j<rect_width;j++){
			if(y+i<image.height && x+j<image_map_width+image.view_width) image.set_pixel(y+i,x+j,texture.get_pixel(texture_num, texture_pos, texture.texture_height*i/rect_height));
		}
	}
}

void draw_view_texture_rectangle_full(Image &image, int x, int y, int rect_width, int rect_height,Texture &texture, int texture_num){ // TODO ugly hack, merge this and prev func
// x&y are horizontal and vertical pixel position of (top-left corner of) rect, respectively.
	for(unsigned int i=0;i<rect_height;i++){
		for(unsigned int j=0;j<rect_width;j++){
			if(y+i<image.height && x+j<image_map_width+image.view_width){
				image.set_pixel(y+i,x+j,texture.get_pixel(texture_num, texture.texture_width*j/rect_width, texture.texture_height*i/rect_height));
			}
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

void draw_character_on_map(Image &image, double x_pos, double y_pos, Pixel colour){
	double height_ratio = image.height/double(image.image_map->height); // Make it so that this fails gracefully if map not defined
	double width_ratio =  image.map_width/double(image.image_map->width); // Maybe use game_image class inherit from image, that requires map
	for(int i=-2;i<=2;i++){
		for(int j=-2;j<=2;j++){
			image.set_pixel(height_ratio*y_pos+i, width_ratio*x_pos+j, colour);	
		}
	}
}

void draw_enemy_on_screen(Image &image, Player &player, Enemy &enemy, Texture &enemy_textures, double fov){
	double height_ratio = image.height/double(image.image_map->height); // Make it so that this fails gracefully if map not defined
	double width_ratio =  image.map_width/double(image.image_map->width); // Maybe use game_image class inherit from image, that requires map

	double rel_x = enemy.x_pos - player.x_pos;
	double rel_y = enemy.y_pos - player.y_pos;

	double rel_angle = atan2(rel_y, rel_x);
	
	if (abs(player.view_angle - rel_angle) <= fov/2) { // Make it so this shows partial enemies without encroaching on map
		double distance = sqrt(pow(rel_x,2)+pow(rel_y,2));
		int size = min(image.height, (unsigned int) (image.height/distance)); // rectangular dimension of enemy on screen
		draw_view_texture_rectangle_full(image,image.map_width+((rel_angle-player.view_angle)/fov+0.5)*image.view_width - size/2, image.height/2 - size/2,size,size, enemy_textures, enemy.texture_id);
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
				int column_height = image.height/(c*cos(angle-player.view_angle)); // TODO really figure out where the cos comes from here
				double frac_x = rx-floor(rx);
				double frac_y = ry-floor(ry);
				double frac = min(abs(frac_x),abs(1-frac_x)) < min(abs(frac_y),abs(1-frac_y)) ? frac_y : frac_x;
				draw_view_texture_rectangle(image, image.map_width+sweep, int(image.height/2. - column_height/2.),
					1, column_height, texture, d-'0', frac*texture.texture_width);
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

	Texture enemy_textures("monsters.png");

	// Define enemies

	vector<Enemy> enemies {{1.834, 8.765, 0}, {5.323, 5.365, 1}, {4.123, 10.26, 2}};


	// simple dumb rotation stream - later this will be done in a loop controlled by the player in a gui
	double a=M_PI/3;
	double fov = M_PI/2.5;
	for(int x=0;x<360;x++){
		a+=2*M_PI/360.;
		Image image(image_map_width, image_view_width, image_height, map);
		for(unsigned int i=0;i<image_height;i++){
			for(unsigned int j=0;j<image_map_width+image_view_width;j++){
					image.set_pixel(i,j,Pixel(255,255,255));
			}
		}
		Player player(2.3,2.3,a);
		draw_character_on_map(image, player.x_pos, player.y_pos, Pixel(0,0,255)); // TODO make it so this takes any abstract Character object
		player_rangefinder(player, image, map, texture, fov);
		for(auto &enemy : enemies) {
			draw_character_on_map(image, enemy.x_pos, enemy.y_pos, Pixel(255,0,0));
			draw_enemy_on_screen(image, player, enemy, enemy_textures, M_PI/3);
		}
		draw_map(image, map,texture);
		string outputf = "output_" + to_string(x) + ".ppm";;
		drop_ppm_image(outputf,image);
		cout << x << " done" << endl;
	}
	
	return 0;
}
