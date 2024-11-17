#include "converter.h"

int	main(int ac, char **argv)
{
	FILE	f;
	int32_t	ret;

	(void)ac;
	ret = open_file(argv[1], &f);
	if (ret)
	{
		qx_error(ret);
		return (1);
	}
	return (0);
}
