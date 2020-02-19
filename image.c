#include "image.h"

int min(int a, int b) {
	return a < b ? a : b;
}

int max(int a, int b) {
	return a < b ? b : a;
}

int sgn(int a) {
	if (a < 0) return -1;
	if (a > 0) return 1;
	return 0;
}

image new_image(int width, int height) {
	image im = { width, height, calloc(sizeof(color), width * height) };
	return im;
}

void saveasppm(image im, FILE *f) {
	fprintf(f, "P6\n%d %d\n255\n", im.width, im.height);
	for (int y = 0; y < im.height; y++)
		for (int x = 0; x < im.width; x++)
			fwrite(&(im.data[y * im.width + x]), 1, 3 * sizeof(unsigned char), f);
}

const int headersize = 54;
unsigned char header[54] = { 'B', 'M', 0, 0, 0, 0, 0, 0, 0, 0, 0x36, 0, 0, 0, 0x28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x13, 0x0b, 0, 0, 0x13, 0x0b, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
void saveasbmp(image im, FILE *f) {
	int rowsize = (im.width * 3 + 3) / 4 * 4;
	int pixelsize = rowsize * im.height;
	int filesize = headersize + pixelsize;

	// set file size
	header[2] = (filesize & 0x000000ff) >> 0;
	header[3] = (filesize & 0x0000ff00) >> 8;
	header[4] = (filesize & 0x00ff0000) >> 16;
	header[5] = (filesize & 0xff000000) >> 24;

	// set width
	header[21] = (im.width & 0xff000000) >> 24;
	header[20] = (im.width & 0x00ff0000) >> 16;
	header[19] = (im.width & 0x0000ff00) >> 8;
	header[18] = (im.width & 0x000000ff) >> 0;

	// set width
	header[25] = (im.height & 0xff000000) >> 24;
	header[24] = (im.height & 0x00ff0000) >> 16;
	header[23] = (im.height & 0x0000ff00) >> 8;
	header[22] = (im.height & 0x000000ff) >> 0;

	// set width
	header[37] = (pixelsize & 0xff000000) >> 24;
	header[36] = (pixelsize & 0x00ff0000) >> 16;
	header[35] = (pixelsize & 0x0000ff00) >> 8;
	header[34] = (pixelsize & 0x000000ff) >> 0;

	fwrite(header, 54, 1, f);
	
	// write pixel data
	char *row = calloc(sizeof(char), rowsize);
	
	for (int y = im.height - 1; y >= 0; y--)
	{
		int imindex = y * im.width, rowindex = 0;
		for (int x = 0; x < im.width; x++)
		{
			row[rowindex++] = im.data[imindex].b;
			row[rowindex++] = im.data[imindex].g;
			row[rowindex++] = im.data[imindex++].r;
		}
		fwrite(row, sizeof(char), rowsize, f);
	}
	free(row);
}

void fill(image im, color col) {
	for (int i = 0; i < im.height * im.width; i++) im.data[i++] = col;
}

void line(image im, int x1, int y1, int x2, int y2, color col) {
	if (x1 > x2) { x1 ^= x2; x2 ^= x1; x1 ^= x2; y1 ^= y2; y2 ^= y1; y1 ^= y2; }
	int dx = x2 - x1, dy = abs(y2 - y1), incy = sgn(y2 - y1);
	int err = (dx > dy ? dx : -dy) / 2, olderr;

	do
	{
		if (0 <= x1 && x1 < im.width && 0 <= y1 && y1 < im.height) im.data[y1 * im.width + x1] = col;
		if (x1 == x2 && y1 == y2) break;
		olderr = err;
		if (olderr >-dx) { err -= dy; x1++; }
		if (olderr < dy) { err += dx; y1 += incy; }
	} while (!(x1 == x2 && y1 == y2));
}

void circle(image im, int xc, int yc, int r, color col) {
	int x = 0, y = r, d = 3 - 2 * r;
	while (x < y)
	{
		int x0 = xc + x, x1 = xc - x, y0 = yc + y, y1 = yc - y;
		if (0 <= x0 && x0 < im.width && 0 <= y0 && y0 < im.height) im.data[y0 * im.width + x0] = col;
		if (0 <= x1 && x1 < im.width && 0 <= y0 && y0 < im.height) im.data[y0 * im.width + x1] = col;
		if (0 <= x0 && x0 < im.width && 0 <= y1 && y1 < im.height) im.data[y1 * im.width + x0] = col;
		if (0 <= x1 && x1 < im.width && 0 <= y1 && y1 < im.height) im.data[y1 * im.width + x1] = col;

		x0 = xc + y; x1 = xc - y; y0 = yc + x; y1 = yc - x;
		if (0 <= x0 && x0 < im.width && 0 <= y0 && y0 < im.height) im.data[y0 * im.width + x0] = col;
		if (0 <= x1 && x1 < im.width && 0 <= y0 && y0 < im.height) im.data[y0 * im.width + x1] = col;
		if (0 <= x0 && x0 < im.width && 0 <= y1 && y1 < im.height) im.data[y1 * im.width + x0] = col;
		if (0 <= x1 && x1 < im.width && 0 <= y1 && y1 < im.height) im.data[y1 * im.width + x1] = col;
		
		if (d < 0) d += 4 * x++ + 6;
		else d += 4 * (x++ - y--) + 10;
	}
}

color fromHSV(double hue, double saturation, double value) {
	color col = { 0, 0, 0 };
	assert(0 <= hue <= 360);
	assert(0 <= saturation <= 1);
	assert(0 <= value <= 1);
	hue = hue / 60;
	double chroma = value * saturation;
	double x = chroma * (1 - fabs(fmod(hue, 2) - 1));
	double m = value - chroma;
	if (0 <= hue && hue < 1) col = fromRGB(chroma + m, x + m, m);
	if (1 <= hue && hue < 2) col = fromRGB(x + m, chroma + m, m);
	if (2 <= hue && hue < 3) col = fromRGB(m, chroma + m, x + m);
	if (3 <= hue && hue < 4) col = fromRGB(m, x + m, chroma + m);
	if (4 <= hue && hue < 5) col = fromRGB(x + m, m, chroma + m);
	if (5 <= hue && hue <= 6) col = fromRGB(chroma + m, m, x + m);
	return col;
}

color fromHSI(double hue, double saturation, double intensity) {
	color col = { 0, 0, 0 };
	assert(0 <= hue <= 360);
	assert(0 <= saturation <= 1);
	assert(0 <= intensity <= 1);
	hue = hue / 60;
	double z = 1 - fabs(fmod(hue, 2) - 1);
	double chroma = (3 * intensity * saturation) / (1 + z);
	double x = chroma * z;
	double m = intensity * (1 - saturation);
	if (0 <= hue && hue < 1) col = fromRGB(chroma + m, x + m, m);
	if (1 <= hue && hue < 2) col = fromRGB(x + m, chroma + m, m);
	if (2 <= hue && hue < 3) col = fromRGB(m, chroma + m, x + m);
	if (3 <= hue && hue < 4) col = fromRGB(m, x + m, chroma + m);
	if (4 <= hue && hue < 5) col = fromRGB(x + m, m, chroma + m);
	if (5 <= hue && hue <= 6) col = fromRGB(chroma + m, m, x + m);
	return col;
}

color fromHSL(double hue, double saturation, double lightness) {
	color col = { 0, 0, 0 };
	assert(0 <= hue <= 360);
	assert(0 <= saturation <= 1);
	assert(0 <= lightness <= 1);
	hue = hue / 60;
	double z = 1 - fabs(fmod(hue, 2) - 1);
	double chroma = (1 - fabs(2 * lightness - 1)) * saturation;
	double x = chroma * (1 - fabs(fmod(hue, 2) - 1));
	double m = lightness - .5 * chroma;
	if (0 <= hue && hue < 1) col = fromRGB(chroma + m, x + m, m);
	if (1 <= hue && hue < 2) col = fromRGB(x + m, chroma + m, m);
	if (2 <= hue && hue < 3) col = fromRGB(m, chroma + m, x + m);
	if (3 <= hue && hue < 4) col = fromRGB(m, x + m, chroma + m);
	if (4 <= hue && hue < 5) col = fromRGB(x + m, m, chroma + m);
	if (5 <= hue && hue <= 6) col = fromRGB(chroma + m, m, x + m);
	return col;
}

color fromRGB(double r, double g, double b) {
	int ri = (int)(r * 255 + .5), gi = (int)(g * 255 + .5), bi = (int)(b * 255 + .5);
	if (ri > 255) ri = 255;
	if (ri < 0) ri = 0;
	if (gi > 255) gi = 255;
	if (gi < 0) gi = 0;
	if (bi > 255) bi = 255;
	if (bi < 0) bi = 0;
	return (color) { ri, gi, bi };
}

color quantize(color col, int rbits, int gbits, int bbits) {
	assert(0 <= rbits <= 8);
	assert(0 <= gbits <= 8);
	assert(0 <= bbits <= 8);
	return fromRGB((col.r & (((1 << rbits) - 1) << (8 - rbits))) / (double)(((1 << rbits) - 1) << (8 - rbits)),
		(col.g & (((1 << gbits) - 1) << (8 - gbits))) / (double)(((1 << gbits) - 1) << (8 - gbits)),
		(col.b & (((1 << bbits) - 1) << (8 - bbits))) / (double)(((1 << bbits) - 1) << (8 - bbits)));
}

unsigned char clamp(int x) {
	if (x < 0) return 0;
	if (x > 255) return 255;
	return x;
}

void floydsteinberg(image im, color(*nearest_col)(color c)) {
	int size = im.width * im.height, index = 0;
	int *r = malloc(sizeof(float) * size), *g = malloc(sizeof(float) * size), *b = malloc(sizeof(float) * size);
	for (int y = 0; y < im.height; y++)
		for (int x = 0; x < im.width; x++)
		{
			r[index] = im.data[index].r;
			g[index] = im.data[index].g;
			b[index++] = im.data[index].b;
		}

	index = 0;
	for (int y = 0; y < im.height; y++)
		for (int x = 0; x < im.width; x++)
		{
			color near = nearest_col((color) { clamp((int)(r[index] + .5)), clamp((int)(g[index] + .5)), clamp((int)(b[index] + .5)) });
			im.data[index] = near;
			int rerr = r[index] - near.r, gerr = g[index] - near.g, berr = b[index] - near.b;
			
			if (x < im.width - 1) { r[index + 1] += rerr * 7 / 16; g[index + 1] += gerr * 7 / 16; b[index + 1] += berr * 7 / 16; }
			if (y < im.height - 1)
			{
				if (x > 0) { r[index + im.width - 1] += rerr * 3 / 16; g[index + im.width - 1] += gerr * 3 / 16; b[index + im.width - 1] += berr * 3 / 16; }
				r[index + im.width] += rerr * 5 / 16; g[index + im.width] += gerr * 5 / 16; b[index + im.width] += berr * 5 / 16;
				if (x < im.width - 1) r[index + im.width - 1] += rerr / 16; g[index + im.width - 1] += gerr / 16; b[index + im.width - 1] += berr / 16;
			}
			index++;
		}
	free(r);
	free(g);
	free(b);
}

void fillcircle(image im, int xc, int yc, int r, color col) {
	int r2 = r * r;
	int x0 = max(0, xc - r), x1 = min(im.width, xc + r);
	int y0 = max(0, yc - r), y1 = min(im.height, yc + r);
	for (int y = y0; y < y1; y++)
		for (int x = x0; x < x1; x++)
			if ((x - xc) * (x - xc) + (y - yc) * (y - yc) < r2) im.data[y * im.width + x] = col;
}

void rectangle(image im, int x1, int y1, int x2, int y2, color col) {
	line(im, x1, y1, x2, y1, col);
	line(im, x2, y1, x2, y2, col);
	line(im, x1, y2, x2, y2, col);
	line(im, x1, y1, x1, y2, col);
}

void fillrectangle(image im, int x1, int y1, int x2, int y2, color col) {
	x1 = max(0, x1); x2 = min(im.width, x2);
	y1 = max(0, y1); y2 = min(im.height, y2);
	for (int y = y1; y < y2; y++)
		for (int x = x1; x < x2; x++)
			im.data[y * im.width + x] = col;
}
