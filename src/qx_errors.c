#include "converter.h"

void	qx_error(int err_code)
{
	if (err_code == QX_OPEN_ERR)
		fprintf(stderr, QX": cannot open input file\n");
}
