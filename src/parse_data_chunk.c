#include "colortable.h"
#include <math.h>

static uint32_t	apply_no_filter(unsigned char *scanline,
				t_pngmdata mdata, unsigned char *out, t_bytepositions *bytepos)
{
	uint32_t		j;

	if (!bytepos->left_overs)
	{
		j = mdata.bytes_pp;
		scanline[0] = out[0];
		scanline[1] = out[1];
		scanline[2] = out[2];
		scanline[3] = out[3];
		bytepos->decoded += 4;
	}
	else
		j = (mdata.width * mdata.bytes_pp) - bytepos->left_overs; //pick up from where we stopped decoding in the last chunk
	while (j < mdata.width * mdata.bytes_pp && bytepos->decoded < bytepos->written)
	{
		scanline[j] = out[j];
		bytepos->decoded += 1;
		j++;
	}
	if (bytepos->decoded == bytepos->written)
		printf("remaining in scanline: %u\n", (mdata.width * mdata.bytes_pp) - j);
	return ((mdata.width * mdata.bytes_pp) - j);
}

static uint32_t	apply_up_filter(unsigned char **pixel_data, t_pngmdata mdata,
				unsigned char *out, uint32_t height, t_bytepositions *bytepos)
{
	uint32_t		j;
	unsigned char	above;

	if (!bytepos->left_overs)
	{
		j = mdata.bytes_pp;
		pixel_data[height][0] = out[0];
		pixel_data[height][1] = out[1];
		pixel_data[height][2] = out[2];
		pixel_data[height][3] = out[3];
		bytepos->decoded += 4;
	}
	else
		j = (mdata.width * mdata.bytes_pp) - bytepos->left_overs; //pick up from where we stopped decoding in the last chunk
	if (!height)
		above = 0;
	while (j < mdata.width * mdata.bytes_pp && bytepos->decoded < bytepos->written)
	{
		if (height)
			above = pixel_data[height - 1][j];
		pixel_data[height][j] = ((unsigned char)(out[j] + above) % 256); 
		bytepos->decoded += 1;
		j++;
	}
	return ((mdata.width * mdata.bytes_pp) - j);
}

static uint32_t	apply_average_filter(unsigned char **pixel_data,
				t_pngmdata mdata, unsigned char *out, uint32_t height, t_bytepositions *bytepos)
{
	uint32_t		j;
	double			avg;
	unsigned char	*scanline_above;
	unsigned char	*scanline;

	scanline = pixel_data[height];
	if (!height)
		scanline_above = NULL;
	else
		scanline_above = pixel_data[height - 1];
	if (!bytepos->left_overs)
	{
		j = mdata.bytes_pp;
		pixel_data[height][0] = out[0];
		pixel_data[height][1] = out[1];
		pixel_data[height][2] = out[2];
		pixel_data[height][3] = out[3];
		bytepos->decoded += 4;
	}
	else
		j = (mdata.width * mdata.bytes_pp) - bytepos->left_overs; //pick up from where we stopped decoding in the last chunk
	while (j < mdata.width * mdata.bytes_pp && bytepos->decoded < bytepos->written)
	{
		if (scanline_above)
			avg = floor((scanline[j - mdata.bytes_pp] + scanline_above[j]) / 2);
		else
			avg = floor((scanline[j - mdata.bytes_pp]) / 2);
		scanline[j] = ((unsigned char)(out[j] + avg) % 256); 
		bytepos->decoded += 1;
		j++;
	}
	return ((mdata.width * mdata.bytes_pp) - j);
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
				 unsigned char *out, uint32_t height, t_bytepositions *bytepos)
{
	uint32_t		j;
	unsigned char	above;
	unsigned char	upper_left;

	if (!bytepos->left_overs)
	{
		j = mdata.bytes_pp;
		pixel_data[height][0] = out[0];
		pixel_data[height][1] = out[1];
		pixel_data[height][2] = out[2];
		pixel_data[height][3] = out[3];
		bytepos->decoded += 4;
	}
	else
		j = (mdata.width * mdata.bytes_pp) - bytepos->left_overs; //pick up from where we stopped decoding in the last chunk
	if (!height)
	{
		above = 0;
		upper_left = 0;
	}
	while (j < mdata.width * mdata.bytes_pp && bytepos->decoded < bytepos->written)
	{
		if (height)
		{
			above = pixel_data[height - 1][j];
			upper_left = pixel_data[height - 1][j - mdata.bytes_pp];
		}
		pixel_data[height][j] = ((unsigned char)(out[j]
			+ paeth_predictor(pixel_data[height][j - mdata.bytes_pp], above, upper_left)) % 256);
		bytepos->decoded += 1;
		j++;
	}
	return ((mdata.width * mdata.bytes_pp) - j);
}

static uint32_t	apply_sub_filter(unsigned char *scanline,
				t_pngmdata mdata, unsigned char *out,
				t_bytepositions *bytepos)
{
	uint32_t	j;

	if (!bytepos->left_overs)
	{
		j = mdata.bytes_pp;
		scanline[0] = out[0];
		scanline[1] = out[1];
		scanline[2] = out[2];
		scanline[3] = out[3];
		bytepos->decoded += 4;
	}
	else
		j = (mdata.width * mdata.bytes_pp) - bytepos->left_overs; //pick up from where we stopped decoding in the last chunk
	while (j < mdata.width * mdata.bytes_pp && bytepos->decoded < bytepos->written)
	{
		scanline[j] = ((unsigned char)(out[j] + scanline[j - mdata.bytes_pp]) % 256);
		bytepos->decoded += 1;
		j++;
	}
	return ((mdata.width * mdata.bytes_pp) - j);
}

uint32_t	parse_data_chunk(uint32_t written, unsigned char *out,
				t_pngmdata mdata, unsigned char **pixel_data)
{
	static uint32_t			left_in_scanline = 0;
	static unsigned char	last_filter_type = 0;
	static uint32_t			i = 0;
	unsigned char			filter_type;
	t_bytepositions			bytepos;

	bytepos.decoded = 0;
	bytepos.written = written;
	bytepos.left_overs = left_in_scanline;
	printf("PARSING NEW DATA CHUNK. Written to that chunk: %u\n", bytepos.written);
	while (bytepos.decoded < bytepos.written)
	{
		bytepos.left_overs = left_in_scanline;
		if (bytepos.left_overs)
		{
			filter_type = last_filter_type;
			printf("filter type from last scanline: %d, %u left in scanline\n", filter_type, bytepos.left_overs);
		}
		else
		{
			filter_type = *out++;
			printf("filter type: %d, nothing left in scanline\n", filter_type);
			bytepos.decoded++;
			last_filter_type = filter_type;
		}
		if (bytepos.decoded == bytepos.written)
			break ;
		if (!filter_type)
			left_in_scanline = apply_no_filter(pixel_data[i], mdata, out, &bytepos);
		else if (filter_type == 1)
			left_in_scanline = apply_sub_filter(pixel_data[i], mdata, out, &bytepos);
		else if (filter_type == 2)
			left_in_scanline = apply_up_filter(pixel_data, mdata, out, i, &bytepos);
		else if (filter_type == 3)
			left_in_scanline = apply_average_filter(pixel_data, mdata, out, i, &bytepos);
		else if (filter_type == 4)
			left_in_scanline = apply_paeth_filter(pixel_data, mdata, out, i, &bytepos);
		if (bytepos.left_overs)
			out += bytepos.left_overs;
		else
			out += (mdata.width * mdata.bytes_pp);
		if (!left_in_scanline)
			i++; //only increase the scanline count if we're done with it 
	}
	return (0);
}
