#include "colortable.h"

t_rgba	assemble_color_channels(unsigned char *scanline, t_pngmdata mdata,
			uint32_t curr_pixel)
{
	t_rgba		color;

	color.r = scanline[curr_pixel];
	color.g = scanline[curr_pixel + 1];
	color.b = scanline[curr_pixel + 2];
	if (mdata.bytes_pp == 4)
		color.a = scanline[curr_pixel + 3];
	else
		color.a = 0;
	return (color);
}

uint32_t	print_pixels(unsigned char **pixel_data, t_pngmdata mdata,
				t_colortable *ct, FILE *outf)
{
	uint32_t		y;
	uint32_t		x;
	unsigned char	*scanline;
	t_rgba			color;
	uint32_t		uint_key;
	uint32_t		transparent_key;
	unsigned char	transparent_found;

	y = 0;
	x = 0;
	transparent_found = 0;
	while (y < mdata.height)
	{
		scanline = pixel_data[y];
		fprintf(outf, "\"");
		while (x < mdata.width)
		{
			color = assemble_color_channels(scanline, mdata, x * mdata.bytes_pp);
			uint_key = rgba_to_uint(color);
			if (!color.a && mdata.channels == 4 && !transparent_found)
			{
				transparent_key = uint_key;
				transparent_found = 1;
			}
			else if (!color.a && mdata.channels == 4 && transparent_found)
				uint_key = transparent_key;
			fprintf(outf, "%s", get_color_identifier(uint_key, ct));
			x++;
		}
		x = 0;
		y++;
		if (y != mdata.height)
			fprintf(outf, "\",\n");
		else
			fprintf(outf, "\"\n");
	}
	fprintf(outf, "};\n");
	return (0);
}

void	print_color_mapping(t_colortable *ct, t_pngmdata mdata, FILE *outf)
{
	uint32_t	j;
	
	j = 0;
	while (j < ct->capacity)
	{
		if (ct->entries[j].used)
		{
			if (!(ct->entries[j].key & 0xFF) && mdata.channels == 4)
				fprintf(outf, "\"%s c None\",\n", ct->entries[j].value);
			else
				fprintf(outf, "\"%s c #%06X\",\n", ct->entries[j].value,
					(ct->entries[j].key >> 8) & 0xFFFFFF);
		}
		j++;
	}
}
