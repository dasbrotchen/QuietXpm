#include "colortable.h"

uint32_t	assign_color_identifier(t_colortable *ct)
{
	uint32_t	i;
	uint32_t	j;
	unsigned char	chars_pp;

	i = 0;
	j = 0;
	chars_pp = get_chars_pp(ct->used_slots);
	while (i < ct->capacity)
	{
		if (ct->entries[i].used)
		{
			ct->entries[i].value = generate_color_identifier(j++, chars_pp);
			if (!ct->entries[i].value)
				return (QX_MALLOC_ERR);
		}
		i++;
	}
	return (0);
}

uint32_t	rgba_to_uint(t_rgba colour)
{
	uint32_t	uint_colour;

	uint_colour = colour.r;
	uint_colour = (uint_colour << 8) | colour.g;
	uint_colour = (uint_colour << 8) | colour.b;
	return (uint_colour);
}

uint32_t	store_pixel_colors(unsigned char **pixel_data, t_pngmdata mdata,
				t_colortable *ct)
{
	uint32_t		y;
	uint32_t		x;
	unsigned char	*scanline;
	t_rgba			color;
	uint32_t		uint_key;

	y = 0;
	x = 0;
	while (y < mdata.height)
	{
		scanline = pixel_data[y];
		while (x < mdata.width)
		{
			color = assemble_color_channels(scanline, mdata, x * mdata.bytes_pp);
			uint_key = rgba_to_uint(color);
			if (add_color(ct, uint_key, NULL))
				return (QX_MALLOC_ERR);
			x++;
		}
		x = 0;
		y++;
	}
	return (0);
}

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
