#include "colortable.h"

static int32_t	rgba_color_index(t_rgba *color_table, t_rgba color, unsigned char color_count)
{
	int32_t	i;

	i = 0;
	while ((unsigned char)i < color_count)
	{
		if (color_table[i].r == color.r && color_table[i].g == color.g
			&& color_table[i].b == color.b && color_table[i].a == color.a)
			return (i);
		i++;
	}
	return (-1);
}

void	print_xpm_data(unsigned char **pixel_data, t_pngmdata mdata, unsigned char bytes_pp,
			t_rgba *color_table, unsigned char color_count)
{
	uint32_t		y = 0;
	uint32_t		x = 0;
	int32_t			color_index = 0;
	t_rgba			color;
	unsigned char	*scanline;

	while (y < mdata.height)
	{
		scanline = pixel_data[y];
		printf("\"");
		while (x < mdata.width * bytes_pp)
		{
			color = (t_rgba){scanline[x],
						scanline[(x) + 1],
						scanline[(x) + 2],
						scanline[(x) + 3]};
			color_index = rgba_color_index(color_table, color, color_count);
			printf("%c", '#' + color_index);
			x += bytes_pp;
		}
		x = 0;
		y++;
		if (y != mdata.height)
			printf("\",");
		else
			printf("\"");
		printf("\n");
	}
}

uint32_t	store_pixel_colors(unsigned char **pixel_data, t_colortable *ct, t_pngmdata mdata)
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
			add_color(ct, hex_color, NULL);
			x++;
		}
		x = 0;
		y++;
	}
	return (0);
}

void	convert_xpm(unsigned char **pixel_data, t_pngmdata mdata, t_colortable *ct)
{
	uint32_t	rgba_colorcount;

	rgba_colorcount = store_pixel_colors(pixel_data, ct, mdata);
	destroy_color_table(ct);
	(void)rgba_colorcount;
}
