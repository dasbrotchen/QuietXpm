#include "colortable.h"
#include <math.h>

static uint32_t	apply_no_filter(unsigned char *scanline,
				t_pngmdata mdata, unsigned char *out, t_bytepositions *bytepos)
{
	uint32_t	j;
	uint32_t	out_j;

	out_j = 0;
	if (!bytepos->left_overs)
		j = 0;
	else
		j = (mdata.width * mdata.bytes_pp) - bytepos->left_overs;
	while (j < mdata.width * mdata.bytes_pp && bytepos->decoded < bytepos->written)
	{
		scanline[j] = out[out_j];
		bytepos->decoded += 1;
		j++;
		out_j++;
	}
	return ((mdata.width * mdata.bytes_pp) - j);
}

static uint32_t	apply_up_filter(unsigned char **pixel_data, t_pngmdata mdata,
				unsigned char *out, uint32_t height, t_bytepositions *bytepos)
{
	uint32_t		j;
	uint32_t		out_j;
	unsigned char	above;

	out_j = 0;
	if (!bytepos->left_overs)
		j = 0;
	else
		j = (mdata.width * mdata.bytes_pp) - bytepos->left_overs;
	if (!height)
		above = 0;
	while (j < mdata.width * mdata.bytes_pp && bytepos->decoded < bytepos->written)
	{
		if (height)
			above = pixel_data[height - 1][j];
		pixel_data[height][j] = ((unsigned char)(out[out_j] + above) % 256); 
		bytepos->decoded += 1;
		j++;
		out_j++;
	}
	return ((mdata.width * mdata.bytes_pp) - j);
}

static uint32_t	apply_average_filter(unsigned char **pixel_data,
				t_pngmdata mdata, unsigned char *out, uint32_t height, t_bytepositions *bytepos)
{
	uint32_t		j;
	uint32_t		out_j;
	double			avg;
	unsigned char	left;
	unsigned char	*scanline_above;
	unsigned char	*scanline;

	out_j = 0;
	scanline = pixel_data[height];
	if (!height)
		scanline_above = NULL;
	else
		scanline_above = pixel_data[height - 1];
	if (!bytepos->left_overs)
		j = 0;
	else
		j = (mdata.width * mdata.bytes_pp) - bytepos->left_overs;
	while (j < mdata.width * mdata.bytes_pp && bytepos->decoded < bytepos->written)
	{
		if (j >= mdata.bytes_pp)
			left = scanline[j - mdata.bytes_pp];
		else
			left = 0;
		if (scanline_above)
			avg = floor((left + scanline_above[j]) / 2);
		else
			avg = floor(left / 2);
		scanline[j] = ((unsigned char)(out[out_j] + avg) % 256); 
		bytepos->decoded += 1;
		j++;
		out_j++;
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
	uint32_t		out_j;
	unsigned char	above;
	unsigned char	upper_left;
	unsigned char	left;

	out_j = 0;
	if (!bytepos->left_overs)
		j = 0;
	else
		j = (mdata.width * mdata.bytes_pp) - bytepos->left_overs;
	while (j < mdata.width * mdata.bytes_pp && bytepos->decoded < bytepos->written)
	{
		if (!height && j>= mdata.bytes_pp)
		{
			above = 0;
			upper_left = 0;
			left = pixel_data[height][j - mdata.bytes_pp];
		}
		else if (!height)
		{
			above = 0;
			upper_left = 0;
			left = 0;
		}
		else if (height && j >= mdata.bytes_pp)
		{
			above = pixel_data[height - 1][j];
			upper_left = pixel_data[height - 1][j - mdata.bytes_pp];
			left = pixel_data[height][j - mdata.bytes_pp];
		}
		else if (height)
		{
			above = pixel_data[height - 1][j];
			upper_left = 0;
			left = 0;
		}
		pixel_data[height][j] = ((unsigned char)(out[out_j]
			+ paeth_predictor(left, above, upper_left)) % 256);
		bytepos->decoded += 1;
		j++;
		out_j++;
	}
	return ((mdata.width * mdata.bytes_pp) - j);
}

static uint32_t	apply_sub_filter(unsigned char *scanline,
				t_pngmdata mdata, unsigned char *out,
				t_bytepositions *bytepos)
{
	uint32_t		j;
	uint32_t		out_j;
	unsigned char	left;

	out_j = 0;
	if (!bytepos->left_overs)
		j = 0;
	else
		j = (mdata.width * mdata.bytes_pp) - bytepos->left_overs;
	while (j < mdata.width * mdata.bytes_pp && bytepos->decoded < bytepos->written)
	{
		if (j >= mdata.bytes_pp)
			left = scanline[j - mdata.bytes_pp];
		else
			left = 0;
		scanline[j] = ((unsigned char)(out[out_j] + left) % 256);
		bytepos->decoded += 1;
		j++;
		out_j++;
	}
	return ((mdata.width * mdata.bytes_pp) - j);
}

uint32_t	parse_data_chunk(uint32_t written, unsigned char *out,
				t_pngmdata mdata, unsigned char **pixel_data, t_chunk_state *chunk_state)
{
	unsigned char			filter_type;
	t_bytepositions			bytepos;

	bytepos.decoded = 0;
	bytepos.written = written;
	bytepos.left_overs = chunk_state->left_in_scanline;
	while (bytepos.decoded < bytepos.written)
	{
		bytepos.left_overs = chunk_state->left_in_scanline;
		if (bytepos.left_overs)
			filter_type = chunk_state->last_filter_type;
		else
		{
			filter_type = *out;
			out++;
			bytepos.decoded++;
			chunk_state->last_filter_type = filter_type;
		}
		if (bytepos.decoded == bytepos.written)
		{
			chunk_state->left_in_scanline = mdata.width * mdata.bytes_pp;
			return (0);
		}
		if (!filter_type)
			chunk_state->left_in_scanline = apply_no_filter(pixel_data[chunk_state->current_scanline], mdata, out, &bytepos);
		else if (filter_type == 1)
			chunk_state->left_in_scanline = apply_sub_filter(pixel_data[chunk_state->current_scanline], mdata, out, &bytepos);
		else if (filter_type == 2)
			chunk_state->left_in_scanline = apply_up_filter(pixel_data, mdata, out, chunk_state->current_scanline, &bytepos);
		else if (filter_type == 3)
			chunk_state->left_in_scanline = apply_average_filter(pixel_data, mdata, out, chunk_state->current_scanline, &bytepos);
		else if (filter_type == 4)
			chunk_state->left_in_scanline = apply_paeth_filter(pixel_data, mdata, out, chunk_state->current_scanline, &bytepos);
		else
			return (QX_INVALID_FILTER_TYPE);
		if (bytepos.left_overs)
			out += bytepos.left_overs;
		else
			out += (mdata.width * mdata.bytes_pp) - chunk_state->left_in_scanline;
		if (!chunk_state->left_in_scanline)
			chunk_state->current_scanline += 1; //only increase the scanline count if we're done with it 
	}
	return (0);
}
