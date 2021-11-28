#pragma once

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
public:
	double x_pos;
	double y_pos;

	Player(double x, double y) : x_pos(x), y_pos(y), view_angle(0.), fov(M_PI/3) {};
	Player(double x, double y, double a) : x_pos(x), y_pos(y), fov(M_PI/3) {
		view_angle = amod(a);
	}
	Player(double x, double y, double a, double f) : x_pos(x), y_pos(y) {
		fov = amod(f);
		view_angle = amod(a);
	}

	double 	get_angle(){ return view_angle;}
	double 	get_fov  (){ return fov;}
	void 	set_angle(double a){ view_angle = amod(a);}
	void 	set_fov (double f){ fov = amod(f);}

private:
	double view_angle;
	double fov;

};

class Enemy { 
public:
	double x_pos;
	double y_pos;
	unsigned int texture_id; // ID of the texture in the creature texture file.
	
	Enemy(double x, double y, unsigned int n) : x_pos(x), y_pos(y), texture_id(n) {};
};

