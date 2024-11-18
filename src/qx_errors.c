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
	if (err_code == QX_INFLATEINIT_ERR)
		fprintf(stderr, QX": could not inflate data chunk\n");
	if (err_code == QX_INVALID_MDATA)
		fprintf(stderr, QX": PNG file metadata could not be read\n");
	if (err_code == QX_INVALID_FILTER)
		fprintf(stderr, QX": PNG file filter method not supported (yet!)\n");
	if (err_code == QX_INVALID_COLORTYPE)
		fprintf(stderr, QX": PNG file color type not supported (yet!)\n");
}
