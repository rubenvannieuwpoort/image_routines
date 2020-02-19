#ifndef IMAGE_H
#define IMAGE_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

typedef struct {
	unsigned char r, g, b;
} color;

typedef struct {
	int width, height;
	color *data;
} image;

image new_image(int width, int height);
void delete_image(image im);

void saveasbmp(image im, FILE *f);
void saveasppm(image im, FILE *f);

void fill(image im, color col);
void line(image im, int x1, int y1, int x2, int y2, color col);
void circle(image im, int xc, int yc, int r, color col);

color fromHSV(double hue, double saturation, double value);
color fromHSI(double hue, double saturation, double intensity);
color fromHSL(double hue, double saturation, double lightness);
color fromRGB(double r, double g, double b);

color quantize(color col, int rbits, int gbits, int bbits);

void floydsteinberg(image im, color(*nearest_col)(color c));

unsigned char clamp(int x);

void fillcircle(image im, int xc, int yc, int r, color col);
void rectangle(image im, int x1, int y1, int x2, int y2, color col);
void fillrectangle(image im, int x1, int y1, int x2, int y2, color col);

#endif
