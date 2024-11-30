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
		if (ct->entries[i].key)
		{
			ct->entries[i].value = generate_color_identifier(j++, chars_pp);
			if (!ct->entries[i].value)
				return (QX_MALLOC_ERR);
		}
		i++;
	}
	return (0);
}

static t_rgba	assemble_color_channels(unsigned char *scanline, t_pngmdata mdata,
			uint32_t curr_pixel)
{
	t_rgba		color;

	color.r = scanline[curr_pixel];
	color.g = scanline[curr_pixel + 1];
	color.b = scanline[curr_pixel + 2];
	if (mdata.bytes_pp == 4)
		color.a = scanline[curr_pixel + 3];
	return (color);
}

uint32_t	store_pixel_colors(unsigned char **pixel_data, t_pngmdata mdata,
				t_colortable *ct)
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
			color = assemble_color_channels(scanline, mdata, x * mdata.bytes_pp);
			if (!color.a && mdata.bytes_pp == 4)
				hex_color = strdup("None");
			else
				hex_color = generate_hex_color(color);
			if (!hex_color)
				return (QX_MALLOC_ERR);
			add_color(ct, hex_color, NULL);
			x++;
		}
		x = 0;
		y++;
	}
	return (0);
}

uint32_t	print_pixels(unsigned char **pixel_data, t_pngmdata mdata,
				t_colortable *ct)
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
			color = assemble_color_channels(scanline, mdata, x * mdata.bytes_pp);
			if (!color.a)
				hex_color = strdup("None");
			else
				hex_color = generate_hex_color(color);
			if (!hex_color)
				return (QX_MALLOC_ERR);
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
	printf("};\n");
	return (0);
}

void	print_color_mapping(t_colortable *ct)
{
	uint32_t	j;
	
	j = 0;
	while (j < ct->capacity)
	{
		if (ct->entries[j].key)
		{
			if (!strcmp(ct->entries[j].key, "None"))
				printf("\"%s c %s\",\n", ct->entries[j].value, ct->entries[j].key);
			else
				printf("\"%s c #%s\",\n", ct->entries[j].value, ct->entries[j].key);
		}
		j++;
	}
}
