#pragma once

#include <vector>
#include <fstream>
#include <iostream>
#include <string>

#include "main.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

class Texture {
	private:
		std::vector<Pixel> textures;
	public:
		unsigned int num_textures; // TODO make private
		unsigned int texture_width;
		unsigned int texture_height;

		Texture(const std::string filename){
			// For now, stb_image library code mostly taken from sslow/tinyraycaster
			// TODO write code to interface the textures natively with Pixel structs
			int width, height, numchannels;
			unsigned char *pixmap = stbi_load(filename.c_str(), &width, &height, &numchannels, 0); // see library file for description of this
			// TODO implement error checking
			if(!pixmap) std::cerr << "error: failed to load" << std::endl;
			if(numchannels != 4) std::cerr << "error: values not in RGBA form" << std::endl;

			textures.reserve(width*height);
			num_textures 	= width/height;
			texture_width	= width/num_textures; // assumes square textures
			texture_height	= height;

			for(int i=0;i<height;i++){
				for(int j=0;j<width;j++){
					textures[i*width+j] = Pixel(pixmap[4*(i*width+j)+0],
												pixmap[4*(i*width+j)+1],
												pixmap[4*(i*width+j)+2], 
												pixmap[4*(i*width+j)+3]);
				}
			}

			stbi_image_free(pixmap); 
		}

		Pixel get_pixel(int n, int i, int j){
			return textures[j*num_textures*texture_width+n*texture_width+i];
		}
};
