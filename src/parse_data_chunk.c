#include "converter.h"


/*
	For now, we only handle sub filter type.
	Start at i = bytes_pp since the first byte is *not* subbed;
	the previous byte corresponds to the filter type.
*/
/*
static void	reconstruct_scanline_sub(unsigned char *scanline,
				t_pngmdata mdata, unsigned char bytes_pp)
{
	uint32_t	i;

	i = bytes_pp;
	while (i < mdata.width * bytes_pp)
	{
		scanline[i] = (unsigned char)((scanline[i] - scanline[i - bytes_pp]) % 256);
		i++;
	}
}*/

/*
	written is CHUNK - strm.avail_out, i.e. the number of bytes written to the
	buffer 'out'.
*/
uint32_t	parse_data_chunk(uint32_t written, unsigned char *out, t_pngmdata mdata)
{
	(void)written;
	unsigned char	*scanline;
	unsigned char	filter_type;
	unsigned char	bytes_pp;
	uint32_t		i;
	uint32_t		j;

	bytes_pp = (mdata.bit_depth * mdata.channels) / 8;/*8 bits per byte*/
	printf("bytes per pixel: %d\n", bytes_pp);
	scanline = out;
	i = 0;
	j = 0;
	while (i < mdata.height)
	{
		filter_type = *scanline++;
		if (filter_type != 1)
			return (QX_INVALID_FILTER);
		//reconstruct_scanline_sub(scanline, mdata, bytes_pp);
		j = bytes_pp;
		while (j < mdata.width * bytes_pp)
		{
			scanline[j] = (unsigned char)((scanline[j] - scanline[j - bytes_pp]) % 256);
			if (scanline[i + j])
				printf("scanline %d at index %d: %d\n", i, j, scanline[i + j]);
			j++;
		}
		scanline += mdata.width * bytes_pp;
		i++;
	}
	*scanline = 0;
	convert_xpm(scanline, mdata, bytes_pp);
	return (0);
}

