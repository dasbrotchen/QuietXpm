#include "converter.h"

void	qx_error(uint32_t err_code)
{
	if (err_code == QX_OPEN_ERR)
		fprintf(stderr, QX": cannot open input or output file\n");
	if (err_code == QX_INVALID_FILE)
		fprintf(stderr, QX": file has to be PNG\n");
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
	if (err_code == QX_IDAT_TOO_LARGE)
		fprintf(stderr, QX": IDAT chunk too large. Aborting.\n");
	if (err_code == QX_INVALID_FILTER_TYPE)
		fprintf(stderr, QX": Invalid filter type in IDAT chunk. File is probably corrupted. Aborting.\n");
}
