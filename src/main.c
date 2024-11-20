#include "converter.h"

int	main(int ac, char **argv)
{
	FILE		*f;
	uint32_t	ret;
	uint32_t	ret_reader;

	if (ac != 2)
	{
		qx_error(QX_INVALID_ARG);
		return (1);
	}
	f = NULL;
	ret = open_file(argv[1], &f);
	if (ret)
	{
		qx_error(ret);
		return (1);
	}
	ret_reader = read_all_chunks(&f);
	if (ret_reader)
	{
		qx_error(ret_reader);
		fclose(f);
		return (1);
	}
	//printf("success!\n");
	fclose(f);
	return (0);
}
