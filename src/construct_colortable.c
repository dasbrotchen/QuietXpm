#include "colortable.h"

/*
	This generates our hash table keys. It takes the RGB values
	of a pixel, and returns a malloc'd char pointer with the 
	format expected in .xpm files, without the leading #.
*/
unsigned char	*generate_hex_color(t_rgba color, t_colortable *ct)
{
	const char		*hex_set = "0123456789ABCDEF";
	unsigned char	*hex_color;
	unsigned char	i;
	unsigned char	hex_char[6];
	
	(void)ct;
	hex_char[0] = (color.r >> 4) % 16;
	hex_char[1] = color.r % 16;
	hex_char[2] = (color.g >> 4) % 16;
	hex_char[3] = color.g % 16;
	hex_char[4] = (color.b >> 4) % 16;
	hex_char[5] = color.b % 16;
	//6 bytes for the colors, 1 byte for the \0.
	hex_color = malloc(sizeof(char) * 7);
	if (!hex_color)
		return (NULL);
	i = 0;
	while (i < 6)
	{
		hex_color[i] = hex_set[hex_char[i]];
		i++;
	}
	hex_color[i] = 0;
	return (hex_color);
}
