#include "image.h"
#include <stdlib.h>

int main()
{
	image im = new_image(1920, 1080);
	
	for (int i = 0, y = 0; y < im.height; y++)
		for (int x = 0; x < im.width; x++)
			im.data[i++] = fromHSV((1.0 * x) / im.width * 360, (1.0 * y) / im.height, 1 - (1.0 * y) / im.height);
	
	FILE *f = fopen("test.bmp", "wb");
	saveasbmp(im, f);
	fclose(f);
	
	return 0;
}
