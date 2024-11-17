#include "converter.h"

/*
	If an IDAT chunk has been identified, we put the compressed
	chunk data in a malloc'd buffer. The next step is to de-compress
	data (with zlib functions) and put it in a buffer before further
	processing.
*/
static uint32_t	process_data_chunk(FILE **file, uint32_t len)
{
	unsigned char	*data;
	z_stream		strm;
	size_t			elements_read;

	(void)strm;
	data = malloc(len);
	if (!data)
		return (QX_MALLOC_ERR);
	elements_read = fread(data, 1, len, *file);
	printf("read %zu chars in data chunk\n", elements_read);
	fseek(*file, CRC_OFFSET, SEEK_CUR);
	free(data);
	return (0);
}

uint32_t	read_all_chunks(FILE **file)
{
	uint32_t	len[1];
	uint32_t	ret;
	char		type[5];

	while (!feof(*file))
	{
		fread(len, 1, 4, *file);
		*len = __builtin_bswap32(*len);
		fread(type, 1, 4, *file);
		printf("type: %s, len: %u\n", type, *len);
		if (!strcmp(type, "IDAT"))
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
