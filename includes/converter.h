#ifndef CONVERTER_H
# define CONVERTER_H

#include <stdio.h>
#include <stdint.h>
#include <zlib.h>
#include <stdlib.h>
#include <string.h>

#ifndef QX
# define QX "QuietXpm"
#endif
#define CRC_OFFSET 4

typedef enum e_qxerrors
{
	QX_OPEN_ERR = 1,
	QX_SIGN_ERR = 2,
	QX_MALLOC_ERR = 3,
	QX_INVALID_ARG = 4,
}	t_qxerrors;

uint32_t	read_all_chunks(FILE **file);
uint32_t	open_file(const char *filename, FILE **file);
void		qx_error(uint32_t err_code);

#endif
