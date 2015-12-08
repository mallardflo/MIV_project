
//common code

#include "Display.h"
#include <iostream>


void (*Display::renderText)(void*, int, int, std::string, int) = NULL;
void* Display::object_instance = NULL;

bool loadTextureRAW(std::string filename, int width, int height, unsigned char** data);
bool loadTextureBmp(std::string filename, int& width, int& height, unsigned char** data);

Texture::Texture() {
	data = NULL;
}
Texture::~Texture() {
	delete[] data;
}
bool Texture::loadFromFile(std::string file_name) {

	//get the extension of the file
	int pos1 = file_name.find_last_of(".");
	std::string ext = file_name.substr(pos1 + 1);

	bool res;

	//RAW file
	if (ext.compare("raw") == 0) {
		width = 256;
		height = 256;
		res = loadTextureRAW(file_name, width, height, &data);
	}

	//BMP file
	if (ext.compare("bmp") == 0) {
		res =loadTextureBmp(file_name, width, height, &data);
	}

	return res;

}

int Display::createTexture(std::string file_name) {

	// !!!! WARNING : for convienience for the moment... even if not displayed, textured should be loaded !!!!
	if (!textures_enabled) return -1;

	//get a new texture
	Texture& t = textures[no_texture++];

	//create an opengl name for it
	glGenTextures(1, (GLuint*)&(t.open_gl_name));

	////bind to texture for loading
	glBindTexture(GL_TEXTURE_2D, t.open_gl_name);

	////load the texture
	t.loadFromFile(file_name);

	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, t.width, t.height, GL_RGB, GL_UNSIGNED_BYTE, t.data);

	//done
	return (no_texture - 1);

}
void Display::setTexture(int t_index) {

	if (!textures_enabled) return;

	//if the texture number is negative: disable texture
	if (t_index < 0) {
		glBindTexture(GL_TEXTURE_2D, 0);
		return;
	}

	//bind the correct texture
	glBindTexture(GL_TEXTURE_2D, textures[t_index].open_gl_name);

}

void Display::drawSky(float radius, float y_offset, rgbColor base_color, rgbColor top_color, float x_res, float y_res) {

	//set up rendering state
	setLighting(false);
	//setViewIdentity();

	//float x;
	float y = y_offset; //current height
	//float z;
	
	float dxa = 2.0f*PIf/x_res;
	float xa = 0.0f;
	float dya = PIf/(2.0f*y_res);
	float ya = 0.0f;

	rgbColor c1 = base_color;
	rgbColor dc = rgbColor((top_color.r - c1.r)/y_res, (top_color.g - c1.g)/y_res, (top_color.b - c1.b)/y_res);
	rgbColor c2;

	//draw the base
	for (int i = 0 ; i < x_res ; i++) {
		drawTriangle(0.0f, y, 0.0f, c1, cos(xa + dxa)*radius, y, sin(xa + dxa)*radius, c1, cos(xa)*radius, y, sin(xa)*radius, c1);
		xa += dxa;
	}

	//draw inner surface
	for (int n = 0 ; n < y_res - 1 ; n++) {
		float s = sqrt((ya + dya)/(PIf/2.0f));
		c2 = rgbColor((top_color.r - base_color.r)*s + base_color.r, (top_color.g - base_color.g)*s + base_color.g, (top_color.b - base_color.b)*s + base_color.b);
		xa = 0.0f;
		for (int i = 0 ; i < x_res ; i++) {
			drawTriangle(	cos(xa)*cos(ya)*radius, sin(ya)*radius + y, sin(xa)*cos(ya)*radius, c1,
							cos(xa + dxa)*cos(ya + dya)*radius, sin(ya + dya)*radius + y, sin(xa + dxa)*cos(ya + dya)*radius, c2,
							cos(xa)*cos(ya + dya)*radius, sin(ya + dya)*radius + y, sin(xa)*cos(ya + dya)*radius, c2);

			drawTriangle(	cos(xa)*cos(ya)*radius, sin(ya)*radius + y, sin(xa)*cos(ya)*radius, c1,
							cos(xa + dxa)*cos(ya)*radius, sin(ya)*radius + y, sin(xa + dxa)*cos(ya)*radius, c1,
							cos(xa + dxa)*cos(ya + dya)*radius, sin(ya + dya)*radius + y, sin(xa + dxa)*cos(ya + dya)*radius, c2);
			xa += dxa;
		}
		ya += dya;
		c1 = c2;
	}

	//draw the top!
	for (int i = 0 ; i < x_res ; i++) {
			drawTriangle(	cos(xa)*cos(ya)*radius, sin(ya)*radius + y, sin(xa)*cos(ya)*radius, c1,
							cos(xa + dxa)*cos(ya)*radius, sin(ya)*radius + y, sin(xa + dxa)*cos(ya)*radius, c1,
							0.0f, radius + y, 0.0f, top_color);
		xa += dxa;
	}

}

void Display::enableTexturing(bool set_enable) {
	if (textures_enabled && !set_enable) setTexture(-1);
	textures_enabled = set_enable;
}

// load a 256x256 RGB .RAW file as a texture
bool loadTextureRAW(std::string filename, int width, int height, unsigned char** data) {

	FILE * file;

	// open texture data
	file = fopen( filename.c_str(), "rb" );
	if ( file == NULL ) return false;

	// allocate buffer
	*data = new unsigned char[width*height*3];

	// read texture data
	fread( *data, width * height * 3, 1, file );
	fclose( file );

	return true;

}

#ifdef WIN32

#include <windows.h>

bool loadTextureBmp(std::string filename, int& width, int& height, unsigned char** data) {

	FILE *filePtr; //our file pointer
	BITMAPFILEHEADER bitmapFileHeader; //our bitmap file header
	unsigned char *bitmapImage;  //store image data
	int imageIdx=0;  //image index counter
//	unsigned char tempRGB;  //our swap variable

	//open filename in read binary mode
	filePtr = fopen(filename.c_str(),"rb");
	if (filePtr == NULL) return false;

	//read the bitmap file header
	fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER),1,filePtr);

	//verify that this is a bmp file by check bitmap id
	if (bitmapFileHeader.bfType !=0x4D42) {
		fclose(filePtr);
		return false;
	}

	//read the bitmap info header
	BITMAPINFOHEADER bitmapInfoHeader;
	fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);

	//the image size may be forgotten...
	if (bitmapInfoHeader.biSizeImage == 0) bitmapInfoHeader.biSizeImage = bitmapInfoHeader.biWidth*bitmapInfoHeader.biHeight*3;

	//move file point to the begging of bitmap data
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	//allocate enough memory for the bitmap image data
	bitmapImage = (unsigned char*)malloc(bitmapInfoHeader.biSizeImage);

	//verify memory allocation
	if (!bitmapImage) {
		free(bitmapImage);
		fclose(filePtr);
		return false;
	}

	//read in the bitmap image data
	fread(bitmapImage, bitmapInfoHeader.biSizeImage, 1, filePtr);

	//make sure bitmap image data was read
	if (bitmapImage == NULL) {
		fclose(filePtr);
		return false;
	}

	//swap the r and b values to get RGB (bitmap is BGR) and copy image into data
	width = bitmapInfoHeader.biWidth;
	height = bitmapInfoHeader.biHeight;
	*data = new unsigned char[width*height*3];
	int li_width = (bitmapInfoHeader.biSizeImage/height)/3;
	for (int i = 0 ; i < width ; i++) {
		for (int j = 0 ; j < height ; j++) {
			unsigned char* tab = &bitmapImage[((j*li_width) + i)*3];
			unsigned char* dtab = &((*(data))[((j*li_width) + i)*3]);
			dtab[0] = tab[2]; dtab[1] = tab[1];	dtab[2] = tab[0];
		}
	}

	//close file and return bitmap iamge data
	free(bitmapImage);
	fclose(filePtr);
	return true;

}

#else
bool loadTextureBmp(std::string filename, int& width, int& height, unsigned char** data) {
	std::cout << "no bmp file loading support for this OS :( !" << std::endl;
	return false;
}
#endif

Texture Display::textures[MAX_TEXTURES];
int Display::no_texture = 0;
bool Display::textures_enabled = true;
int Display::render_width = 0;
int Display::render_height = 0;