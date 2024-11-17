#include "converter.h"

void	qx_error(uint32_t err_code)
{
	if (err_code == QX_OPEN_ERR)
		fprintf(stderr, QX": cannot open input file\n");
	if (err_code == QX_SIGN_ERR)
		fprintf(stderr, QX": not a PNG file\n");
	if (err_code == QX_MALLOC_ERR)
		fprintf(stderr, QX": malloc failure\n");
	if (err_code == QX_INVALID_ARG)
		fprintf(stderr, QX": please provide one PNG file\n");
}