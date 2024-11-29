#include "colortable.h"
#include <math.h>

static uint32_t	apply_no_filter(unsigned char *scanline,
				t_pngmdata mdata, unsigned char *out, t_bytepositions *bytepos)
{
	uint32_t		j;

	if (!bytepos->left_overs)
		j = 0;
	else
		j = (mdata.width * mdata.bytes_pp) - bytepos->left_overs;
	while (j < mdata.width * mdata.bytes_pp && bytepos->decoded < bytepos->written)
	{
		scanline[j] = out[j];
		bytepos->decoded += 1;
		j++;
	}
	return ((mdata.width * mdata.bytes_pp) - j);
}

static uint32_t	apply_up_filter(unsigned char **pixel_data, t_pngmdata mdata,
				unsigned char *out, uint32_t height, t_bytepositions *bytepos)
{
	uint32_t		j;
	unsigned char	above;

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
	unsigned char	left;
	unsigned char	*scanline_above;
	unsigned char	*scanline;

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
		scanline[j] = ((unsigned char)(out[j] + avg) % 256); 
		bytepos->decoded += 1;
		j++;
	}
	return ((mdata.width * mdata.bytes_pp) - j);
}

static unsigned char paeth_predictor(unsigned char left,
                                     unsigned char above, 
                                     unsigned char upper_left)
{
    int32_t predictor = left + above - upper_left;
    int32_t pa = abs(predictor - left);
    int32_t pb = abs(predictor - above);
    int32_t pc = abs(predictor - upper_left);

    if (pa <= pb && pa <= pc)
        return left;
    else if (pb <= pc)
        return above;
    else
        return upper_left;
}
/*
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
}*/

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
	uint32_t		outj;
	unsigned char	above;
	unsigned char	upper_left;
	unsigned char	left;

	outj = 0;
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
		pixel_data[height][j] = ((unsigned char)(out[outj]
			+ paeth_predictor(left, above, upper_left)) % 256);
		/*
		if (height == 35)
			printf("Scanline 35 byte at index %u: %d, with raw byte: %d, LAU: (%d,%d,%d), manual AU: (%d, %d)\n", j, pixel_data[height][j], out[outj], left, above, upper_left, pixel_data[height - 1][j], pixel_data[height - 1][j - mdata.bytes_pp]);
		*/
		bytepos->decoded += 1;
		j++;
		outj++;
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
		j = (mdata.width * mdata.bytes_pp) - bytepos->left_overs;
	while (j < mdata.width * mdata.bytes_pp && bytepos->decoded < bytepos->written)
	{
		scanline[j] = ((unsigned char)(out[j] + scanline[j - mdata.bytes_pp]) % 256);
		bytepos->decoded += 1;
		j++;
	}
	return ((mdata.width * mdata.bytes_pp) - j);
}

void hex_dump_inflated(unsigned char *inflated_data, uint32_t data_length, int dumpcount) 
{
    FILE *dump_file = fopen("inflated_dump.hex", !dumpcount ? "w" : "a");
    if (!dump_file) return;

    fprintf(dump_file, "\n--- NEW CHUNK ---\n");  // Separator between chunks

    for (size_t i = 0; i < data_length; i++) {
        fprintf(dump_file, "%02X ", inflated_data[i]);
        
        if ((i + 1) % 16 == 0) 
            fprintf(dump_file, "\n");
    }
    
    fclose(dump_file);
}

uint32_t	parse_data_chunk(uint32_t written, unsigned char *out,
				t_pngmdata mdata, unsigned char **pixel_data, t_chunk_state *chunk_state)
{
	static unsigned char	last_filter_type = 0;
	static uint32_t			i = 0;
	static int				inflate_count = 0;
	unsigned char			filter_type;
	t_bytepositions			bytepos;

	bytepos.decoded = 0;
	bytepos.written = written;
	bytepos.left_overs = chunk_state->left_in_scanline;
	//hex_dump_inflated(out, written, inflate_count);
	inflate_count++;
	(void)inflate_count;
	while (bytepos.decoded < bytepos.written)
	{
		bytepos.left_overs = chunk_state->left_in_scanline;
		if (bytepos.left_overs)
		{
			filter_type = last_filter_type;
			//printf("filter type from last scanline: %d, %u left in scanline\n", filter_type, bytepos.left_overs);
		}
		else
		{
			filter_type = *out++;
			//printf("filter type: %d, nothing left in scanline\n", filter_type);
			bytepos.decoded++;
			last_filter_type = filter_type;
		}
		if (bytepos.decoded == bytepos.written)
			break ;
		if (!filter_type)
			chunk_state->left_in_scanline = apply_no_filter(pixel_data[i], mdata, out, &bytepos);
		else if (filter_type == 1)
			chunk_state->left_in_scanline = apply_sub_filter(pixel_data[i], mdata, out, &bytepos);
		else if (filter_type == 2)
			chunk_state->left_in_scanline = apply_up_filter(pixel_data, mdata, out, i, &bytepos);
		else if (filter_type == 3)
			chunk_state->left_in_scanline = apply_average_filter(pixel_data, mdata, out, i, &bytepos);
		else if (filter_type == 4)
			chunk_state->left_in_scanline = apply_paeth_filter(pixel_data, mdata, out, i, &bytepos);
		if (bytepos.left_overs)
			out += bytepos.left_overs;
		else
			out += (mdata.width * mdata.bytes_pp) - chunk_state->left_in_scanline;
		if (!chunk_state->left_in_scanline)
			i++; //only increase the scanline count if we're done with it 
	}
	return (0);
}
