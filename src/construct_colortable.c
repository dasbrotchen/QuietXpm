#include "colortable.h"

unsigned char	get_chars_pp(uint32_t used_slots)
{
	uint32_t		ratio;
	unsigned char	chars_pp;

	chars_pp = 1;
	ratio = 1;
	while (ratio >= 1)
	{
		ratio = used_slots / powf(MAX_NCHAR, chars_pp);
		if (ratio < 1)
			break ;
		chars_pp++;
	}
	return (chars_pp);
}

/*
	The colortable must first be populated with keys, setting the values to NULL.
	chars_pp can be found once we filled the hash table with keys, and sent to this function.
	Then, call this function in a loop iterating over the numbers of different colors.
*/
unsigned char	*generate_color_identifier(uint32_t n_color, unsigned char chars_pp)
{
	unsigned char	*color_id;
	uint32_t		ratio;
	uint32_t		i;

	ratio = 1;
	color_id = malloc(sizeof(char) * (chars_pp + 1));
	if (!color_id)
		return (NULL);
	i = 0;
	while (i < chars_pp)
	{
		ratio = n_color / pow(MAX_NCHAR, chars_pp - i - 1);
		color_id[i++] = (unsigned char)(MIN_CHAR_COLOR_ID + (ratio % MAX_NCHAR));
	}
	color_id[chars_pp] = 0;
	return (color_id);
}

/*
	This generates our hash table keys. It takes the RGB values
	of a pixel, and returns a malloc'd char pointer with the 
	format expected in .xpm files, without the leading #.
*/
const char	*generate_hex_color(t_rgba color)
{
	const char		*hex_set = "0123456789ABCDEF";
	char			*hex_color;
	unsigned char	i;
	unsigned char	hex_char[6];
	
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
