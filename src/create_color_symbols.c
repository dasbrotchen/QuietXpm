#include "converter.h"

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

uint32_t	count_rgba_colors(unsigned char **pixel_data, t_pngmdata mdata, unsigned char bytes_pp)
{
	uint32_t		y;
	uint32_t		x;
	unsigned char	*scanline;
	t_rgba			color;
	t_rgba			color_table[MAX_COLORS] = {0};
	unsigned char	color_count;
	int32_t			color_index;

	y = 0;
	x = 0;
	color_count = 0;
	while (y < mdata.height)
	{
		scanline = pixel_data[y];
		while (x < mdata.width)
		{
			color = (t_rgba){scanline[x * bytes_pp],
						scanline[(x * bytes_pp) + 1],
						scanline[(x * bytes_pp) + 2],
						scanline[(x * bytes_pp) + 3]};
			color_index = rgba_color_index(color_table, color, color_count);
			if (color_index == -1)
			{
				color_index = color_count++;
				color_table[color_index] = color;
			}
			x++;
		}
		x = 0;
		y++;
	}
	printf("/* XPM */\n");
    printf("static char *xpm_image[] = {\n");
	printf("/* columns rows colors chars-per-pixel */\n");
    printf("\"%d %d %d 1\",\n", mdata.width, mdata.height, color_count);
	int i = 0;
	while (i < color_count)
	{        printf("\"%c c #%.2X%.2X%.2X\",\n", '#' + i, color_table[i].r, color_table[i].g, color_table[i].b);
		i++;
	}
	printf("/* pixels */\n");
	print_xpm_data(pixel_data, mdata, bytes_pp, color_table, color_count);
    printf("};\n");
	return (0);
}

void	convert_xpm(unsigned char **pixel_data, t_pngmdata mdata, unsigned char bytes_pp)
{
	uint32_t	rgba_colorcount;

	rgba_colorcount = display_rgba_colors(pixel_data, mdata, bytes_pp);
	(void)rgba_colorcount;
}
