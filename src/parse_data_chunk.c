#include "colortable.h"
#include <math.h>

static uint32_t	apply_no_filter(unsigned char *scanline,
				t_pngmdata mdata, unsigned char *out)
{
	uint32_t		j;

	j = mdata.bytes_pp;
	scanline[0] = out[0];
	scanline[1] = out[1];
	scanline[2] = out[2];
	scanline[3] = out[3];
	while (j < mdata.width * mdata.bytes_pp)
	{
		scanline[j] = out[j];
		j++;
	}
	return (0);
}

static uint32_t	apply_up_filter(unsigned char **pixel_data, t_pngmdata mdata,
				unsigned char *out, uint32_t height)
{
	uint32_t		j;
	unsigned char	above;

	j = mdata.bytes_pp;
	pixel_data[height][0] = out[0];
	pixel_data[height][1] = out[1];
	pixel_data[height][2] = out[2];
	pixel_data[height][3] = out[3];
	if (!height)
		above = 0;
	while (j < mdata.width * mdata.bytes_pp)
	{
		if (height)
			above = pixel_data[height - 1][j];
		pixel_data[height][j] = ((unsigned char)(out[j] + above) % 256); 
		j++;
	}
	return (0);
}

static uint32_t	apply_average_filter(unsigned char **pixel_data,
				t_pngmdata mdata, unsigned char *out, uint32_t height)
{
	uint32_t		j;
	double			avg;
	unsigned char	*scanline_above;
	unsigned char	*scanline;

	j = mdata.bytes_pp;
	scanline = pixel_data[height];
	if (!height)
		scanline_above = NULL;
	else
		scanline_above = pixel_data[height - 1];
	scanline[0] = out[0];
	scanline[1] = out[1];
	scanline[2] = out[2];
	scanline[3] = out[3];
	while (j < mdata.width * mdata.bytes_pp)
	{
		if (scanline_above)
			avg = floor((scanline[j - mdata.bytes_pp] + scanline_above[j]) / 2);
		else
			avg = floor((scanline[j - mdata.bytes_pp]) / 2);
		scanline[j] = ((unsigned char)(out[j] + avg) % 256); 
		j++;
	}
	return (0);
}

static unsigned char	paeth_predictor(unsigned char left,
				unsigned char above, unsigned char upper_left)
{
	int32_t			distances[4];

	distances[0] = left + above - upper_left;
	distances[1] = abs(distances[0] - left);
	distances[2] = abs(distances[0] - above);
	distances[3] = abs(distances[0] - upper_left);
	if (distances[1] <= distances[2] && distances[1] <= distances[3])
		return (left);
	else if (distances[2] <= distances[3])
		  return (above);
	else
		return (upper_left);
}

/*
	From the official PNG documentation:
	"To reverse the effect of the Paeth filter after decompression, output the following value:

   Paeth(x) + PaethPredictor(Raw(x-bpp), Prior(x), Prior(x-bpp))
	(computed mod 256), where Raw and Prior refer to bytes already decoded.[...]

	Here, we need a bit more than just the scanline so we send the whole data array.
	pixel_data[height] is the actual scanline.
	assume all bytes are 0 for the first scanline.
 */
static uint32_t	apply_paeth_filter(unsigned char **pixel_data, t_pngmdata mdata,
				 unsigned char *out, uint32_t height)
{
	uint32_t		j;
	unsigned char	above;
	unsigned char	upper_left;

	j = mdata.bytes_pp;
	pixel_data[height][0] = out[0];
	pixel_data[height][1] = out[1];
	pixel_data[height][2] = out[2];
	pixel_data[height][3] = out[3];
	if (!height)
	{
		above = 0;
		upper_left = 0;
	}
	while (j < mdata.width * mdata.bytes_pp)
	{
		if (height)
		{
			above = pixel_data[height - 1][j];
			upper_left = pixel_data[height - 1][j - mdata.bytes_pp];
		}
		pixel_data[height][j] = ((unsigned char)(out[j]
			+ paeth_predictor(pixel_data[height][j - mdata.bytes_pp], above, upper_left)) % 256);
		j++;
	}
	return (0);
}

static uint32_t	apply_sub_filter(unsigned char *scanline,
				t_pngmdata mdata, unsigned char *out)
{
	uint32_t	j;

	j = mdata.bytes_pp;
	scanline[0] = out[0];
	scanline[1] = out[1];
	scanline[2] = out[2];
	scanline[3] = out[3];
	while (j < mdata.width * mdata.bytes_pp)
	{
		scanline[j] = ((unsigned char)(out[j] + scanline[j - mdata.bytes_pp]) % 256);
		j++;
	}
	return (0);
}

/*
	written is CHUNK - strm.avail_out, i.e. the number of bytes written to the
	buffer 'out'.
*/
uint32_t	parse_data_chunk(uint32_t written, unsigned char *out,
				t_pngmdata mdata, t_colortable *ct)
{
	(void)written;
	unsigned char	filter_type;
	uint32_t		i;
	uint32_t		j;
	unsigned char	**pixel_data;

	pixel_data = (unsigned char **)malloc(sizeof(unsigned char *) * (mdata.height + 1));
	i = 0;
	j = 0;
	(void)j;
	while (i < mdata.height)
	{
		filter_type = *out++;
		pixel_data[i] = malloc(sizeof(char) * (mdata.width * mdata.bytes_pp));
		if (!pixel_data[i])
			return (QX_MALLOC_ERR);
		if (!filter_type)
			apply_no_filter(pixel_data[i], mdata, out);
		else if (filter_type == 1)
			apply_sub_filter(pixel_data[i], mdata, out);
		else if (filter_type == 2)
			apply_up_filter(pixel_data, mdata, out, i);
		else if (filter_type == 3)
			apply_average_filter(pixel_data, mdata, out, i);
		else if (filter_type == 4)
			apply_paeth_filter(pixel_data, mdata, out, i);
		out += mdata.width * mdata.bytes_pp;
		i++;
	}
	pixel_data[mdata.height] = NULL;
	convert_xpm(pixel_data, mdata, ct);
	i = 0;
	while (pixel_data[i])
		free(pixel_data[i++]);
	free(pixel_data);
	return (0);
}

