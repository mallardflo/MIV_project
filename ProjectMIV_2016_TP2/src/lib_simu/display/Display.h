
#ifndef DISPLAY_H
#define DISPLAY_H

#include "AllMath.h"

#include <string>

enum fogType {
	FOG_LINEAR,
	FOG_EXP
};

struct rgbColor {
	float r;
	float g;
	float b;
	rgbColor(float r, float g, float b) : r(r), g(g), b(b) {}
	rgbColor() : r(0.0f),  g(0.0f),  b(0.0f) {}
};

struct Texture {

	int width;
	int height;

	bool loadFromFile(std::string);

	int open_gl_name;

	unsigned char* data;

	Texture();
	~Texture();

private:
	std::string file_src;


};

#include "DisplayOpenGL.h"

#endif