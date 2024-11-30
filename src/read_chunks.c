#include "colortable.h"

/*
	If an IDAT chunk has been identified, we put the compressed
	chunk out. The next step is to de-compress
	data (with zlib functions) and put it in a buffer before further
	processing.
*/
static int32_t	process_data_chunk(FILE **file, uint32_t len,
					t_pngmdata mdata, z_stream *strm,
					unsigned char **pixel_data, t_chunk_state *chunk_state)
{
	unsigned char	in[CHUNK];
	unsigned char	out[CHUNK];
	int32_t			ret;

	strm->avail_in = fread(in, 1, len, *file);
	strm->next_in = in;
	ret = Z_OK;
	if (ferror(*file))
		return (Z_ERRNO);
	if (!strm->avail_in) /* fread() fail, or IDAT chunk with len 0 */
	{
		fseek(*file, CRC_OFFSET, SEEK_CUR);
		return (Z_ERRNO); /* for now, treat empty IDAT chunk as an error */
	}
	do
	{
		strm->avail_out = CHUNK;
		strm->next_out = out;
		ret = inflate(strm, Z_NO_FLUSH);
		assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
		switch (ret)
		{
			case Z_NEED_DICT:
				ret = Z_DATA_ERROR;     /* and fall through */
			case Z_DATA_ERROR:
			case Z_MEM_ERROR:
				return (ret);
		}
		/* Return value is always 0, this function cannot fail. */
		parse_data_chunk(CHUNK - strm->avail_out, out, mdata, pixel_data, chunk_state);
	} while (!strm->avail_out);
	fseek(*file, CRC_OFFSET, SEEK_CUR);
	if (ret == Z_STREAM_END) /* This should never be true */
		return (Z_OK);
    return (ret == Z_OK ? Z_OK : Z_DATA_ERROR);
}

uint32_t	process_metadata_chunk(FILE *file, uint32_t len, t_pngmdata *mdata)
{
	if (len != 13)
		return (QX_INVALID_MDATA);
	if (fread(&mdata->width, 1, 4, file) != 4)
		return (QX_INVALID_MDATA);
	if (fread(&mdata->height, 1, 4, file) != 4)
		return (QX_INVALID_MDATA);
	if (fread(&mdata->bit_depth, 1, 1, file) != 1)
		return (QX_INVALID_MDATA);
	if (fread(&mdata->color_type, 1, 1, file) != 1)
		return (QX_INVALID_MDATA);
	if (fread(&mdata->compression_method, 1, 1, file) != 1)
		return (QX_INVALID_MDATA);
	if (fread(&mdata->filter_method, 1, 1, file) != 1)
		return (QX_INVALID_MDATA);
	if (fread(&mdata->interlace_method, 1, 1, file) != 1)
		return (QX_INVALID_MDATA);
	fseek(file, CRC_OFFSET, SEEK_CUR);
	mdata->width = __builtin_bswap32(mdata->width);
	mdata->height = __builtin_bswap32(mdata->height);
	if (mdata->filter_method)
		return (QX_INVALID_FILTER);
	if (mdata->color_type != 2 && mdata->color_type != 6)
		return (QX_INVALID_COLORTYPE);
	if (mdata->color_type == 2)
		mdata->channels = 3;
	else if (mdata->color_type == 6)
		mdata->channels = 4;
	mdata->bytes_pp = (mdata->bit_depth * mdata->channels) / 8;
	return (0);
}

static unsigned char **allocate_pixel_data(t_pngmdata mdata)
{
	unsigned char	**pixel_data;
	uint32_t		i;

	i = 0;
	pixel_data = calloc(mdata.height + 1, sizeof(unsigned char *));
	if (!pixel_data)
		return (NULL);
	while (i < mdata.height)
	{
		pixel_data[i] = calloc(mdata.width * mdata.bytes_pp + 1, sizeof(unsigned char));
		if (!pixel_data[i])
		{
			free_pixel_data(pixel_data);
			return (NULL);
		}
		i++;
	}
	return (pixel_data);
}

uint32_t	read_all_chunks(FILE **file, t_pngmdata *mdata, unsigned char ***pixel_data)
{
	uint32_t		len;
	uint32_t		ret;
	uint32_t		ret_mdata;
	char			type[5];
	z_stream		strm = {0};
	t_chunk_state	chunk_state = {0};

	strm.total_in = 0;
	strm.avail_in = 0;
	strm.next_in = Z_NULL;
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
	ret = inflateInit(&strm);
	if (ret != Z_OK)
		return (QX_INFLATEINIT_ERR);
	while (!feof(*file))
	{
		fread(&len, 1, 4, *file);
		len = __builtin_bswap32(len);
		fread(type, 1, 4, *file);
		type[4] = 0;
		if (!strcmp(type, "IHDR"))
		{
			ret_mdata = process_metadata_chunk(*file, len, mdata);
			if (ret_mdata)
			{
				(void)inflateEnd(&strm);
				return (ret_mdata);
			}
			*pixel_data = allocate_pixel_data(*mdata);
			if (!*pixel_data)
			{
				(void)inflateEnd(&strm);
				return (QX_MALLOC_ERR);
			}
		}
		else if (!strcmp(type, "IDAT"))
		{
			ret = process_data_chunk(file, len, *mdata, &strm, *pixel_data, &chunk_state);
			if (ret)
			{
				(void)inflateEnd(&strm);
				return (ret);
			}
		}
		else if (!strcmp(type, "IEND"))
			break ;
		else  
			fseek(*file, len + CRC_OFFSET, SEEK_CUR);
	}
	(void)inflateEnd(&strm);
	return (0);
}
