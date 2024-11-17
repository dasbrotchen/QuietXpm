#ifndef CONVERTER_H
# define CONVERTER_H

#include <stdio.h>
#include <stdint.h>
#include <zlib.h>
#include <stdlib.h>

#ifndef QX
# define QX "QuietXpm"
#endif

typedef enum e_qxerrors
{
	QX_OPEN_ERR = 1,
	QX_SIGN_ERR = 2,
}	t_qxerrors;

//int32_t	inflate_chunk(FILE *file);
void	qx_error(int err_code);
int32_t	open_file(const char *filename, FILE *file);

#endif
