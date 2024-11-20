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

size_t	ft_strlen(unsigned char *s)
{
	size_t	i;

	i = 0;
	while (s[i])
		i++;
	return (i);
}

unsigned char	*ft_strndup(unsigned char *s, int32_t size)
{
	unsigned char	*res;
	int		i;

	i = -1;
	res = malloc((size + 1) * sizeof(char));
	if (!res)
		return (NULL);
	while (++i, i < size)
		res[i] = s[i];
	res[i] = '\0';
	return (res);
}


/*
void	print_scanline(unsigned char *scanline, int32_t size)
{
	int32_t	i;

	i = 0;
	while (i < size)
		write(1, &scanline[i++], 1);
	write(1, "\n", 1);
}*/

/*
	written is CHUNK - strm.avail_out, i.e. the number of bytes written to the
	buffer 'out'.
*/
uint32_t	parse_data_chunk(uint32_t written, unsigned char *out, t_pngmdata mdata)
{
	(void)written;
	unsigned char	filter_type;
	unsigned char	bytes_pp;
	uint32_t		i;
	uint32_t		j;
	unsigned char	**pixel_data;

	pixel_data = (unsigned char **)malloc(sizeof(unsigned char *) * (mdata.height + 1));
	bytes_pp = (mdata.bit_depth * mdata.channels) / 8;/*8 bits per byte*/
	i = 0;
	j = 0;
	while (i < mdata.height)
	{
		filter_type = *out++;
		if (filter_type != 1)
			return (QX_INVALID_FILTER);
		j = bytes_pp;
		pixel_data[i] = malloc(sizeof(char) * (mdata.width * bytes_pp));
		pixel_data[i][0] = out[0];
		pixel_data[i][1] = out[1];
		pixel_data[i][2] = out[2];
		pixel_data[i][3] = out[3];
		while (j < mdata.width * (bytes_pp))
		{
			pixel_data[i][j] = ((unsigned char)(out[j] + pixel_data[i][j - bytes_pp]) % 256);
			j++;
		}
		out += mdata.width * bytes_pp;
		i++;
	}
	pixel_data[mdata.height] = NULL;
	convert_xpm(pixel_data, mdata, bytes_pp);
	i = 0;
	while (pixel_data[i])
		free(pixel_data[i++]);
	free(pixel_data);
	return (0);
}

