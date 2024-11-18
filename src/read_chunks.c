#include "converter.h"

/*
	If an IDAT chunk has been identified, we put the compressed
	chunk out. The next step is to de-compress
	data (with zlib functions) and put it in a buffer before further
	processing.
*/
static uint32_t	process_data_chunk(FILE **file, uint32_t len)
{
	unsigned char	in[CHUNK];
	unsigned char	out[CHUNK];
	int32_t			ret;
	uint64_t			have;
	z_stream		strm = {0};

	strm.total_in = 0;
	strm.avail_in = 0;
	strm.next_in = Z_NULL;
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
	ret = inflateInit(&strm);
	if (ret != Z_OK)
		return (QX_INFLATEINIT_ERR);
	while (ret != Z_STREAM_END)
	{
		strm.avail_in = fread(in, 1, len, *file);
        if (ferror(*file))
		{
            (void)inflateEnd(&strm);
            return (Z_ERRNO);
        }
        if (!strm.avail_in)
            break;
        strm.next_in = in;
		do
		{
			strm.avail_out = CHUNK;
            strm.next_out = out;
			ret = inflate(&strm, Z_NO_FLUSH);
            assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
            switch (ret)
			{
				case Z_NEED_DICT:
					ret = Z_DATA_ERROR;     /* and fall through */
				case Z_DATA_ERROR:
				case Z_MEM_ERROR:
					(void)inflateEnd(&strm);
					return (ret);
			}
			have = CHUNK - strm.avail_out;
			(void)have;
		} while (!strm.avail_out);
	}
	fseek(*file, CRC_OFFSET, SEEK_CUR);
	(void)inflateEnd(&strm);
    return (ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR);
}

//BSWAP ALL READ VALUES
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
	printf("PNG file:\n\tWIDTH = %u\n\tHEIGHT = %u\n", mdata->width, mdata->height);
	printf("\tCOLOR TYPE = %d\n", mdata->color_type);
	return (0);
}

uint32_t	read_all_chunks(FILE **file)
{
	uint32_t	len[1];
	uint32_t	ret;
	uint32_t	ret_mdata;
	char		type[5];
	t_pngmdata	mdata;

	while (!feof(*file))
	{
		fread(len, 1, 4, *file);
		*len = __builtin_bswap32(*len);
		fread(type, 1, 4, *file);
		type[4] = 0;
		if (!strcmp(type, "IHDR"))
		{
			ret_mdata = process_metadata_chunk(*file, *len, &mdata);
			if (ret_mdata)
				return (ret_mdata);
		}
		else if (!strcmp(type, "IDAT"))
		{
			ret = process_data_chunk(file, *len);
			if (ret)
				return (ret);
		}
		else if (!strcmp(type, "IEND"))
			break ;
		else  
			fseek(*file, *len + CRC_OFFSET, SEEK_CUR);
	}
	return (0);
}
