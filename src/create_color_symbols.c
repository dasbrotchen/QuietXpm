#include "colortable.h"

uint32_t	assign_color_identifier(t_colortable *ct)
{
	uint32_t		i;
	unsigned char	chars_pp;

	i = 0;
	chars_pp = get_chars_pp(ct->used_slots);
	while (i < ct->capacity)
	{
		if (ct->entries[i].key)
		{
			ct->entries[i].value = generate_color_identifier(i, chars_pp);
			printf("new entry => <%s>, <%s>\n", ct->entries[i].key, ct->entries[i].value);
		}
		i++;
	}
	return (0);
}

uint32_t	store_pixel_colors(unsigned char **pixel_data, t_pngmdata mdata, t_colortable *ct)
{
	uint32_t		y;
	uint32_t		x;
	unsigned char	*scanline;
	t_rgba			color;
	const char		*hex_color;

	y = 0;
	x = 0;
	while (y < mdata.height)
	{
		scanline = pixel_data[y];
		while (x < mdata.width)
		{
			color = (t_rgba){scanline[x * mdata.bytes_pp],
						scanline[(x * mdata.bytes_pp) + 1],
						scanline[(x * mdata.bytes_pp) + 2],
						scanline[(x * mdata.bytes_pp) + 3]};
			hex_color = generate_hex_color(color);
			if (!color.a)
			{
				free((void *)hex_color);
				hex_color = strdup("None");
			}
			add_color(ct, hex_color, NULL);
			x++;
		}
		x = 0;
		y++;
	}
	return (0);
}

uint32_t	print_pixels(unsigned char **pixel_data, t_pngmdata mdata, t_colortable *ct)
{
	uint32_t		y;
	uint32_t		x;
	unsigned char	*scanline;
	t_rgba			color;
	const char		*hex_color;

	y = 0;
	x = 0;
	while (y < mdata.height)
	{
		scanline = pixel_data[y];
		printf("\"");
		while (x < mdata.width)
		{
			color = (t_rgba){scanline[x * mdata.bytes_pp],
						scanline[(x * mdata.bytes_pp) + 1],
						scanline[(x * mdata.bytes_pp) + 2],
						scanline[(x * mdata.bytes_pp) + 3]};
			if (color.a)
				hex_color = generate_hex_color(color);
			else
				hex_color = strdup("None");
			printf("%s", get_color_identifier(hex_color, ct));
			free((void *)hex_color);
			x++;
		}
		x = 0;
		y++;
		if (y != mdata.height)
			printf("\",\n");
		else
			printf("\"\n");
	}
	printf("};");
	return (0);
}
