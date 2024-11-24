#ifndef CONVERTER_H
# define CONVERTER_H

#include <stdio.h>
#include <stdint.h>
#include <zlib.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#ifndef QX
# define QX "QuietXpm"
#endif
#define CRC_OFFSET 4
#define CHUNK 126000
#define MAX_COLORS 16581375

typedef enum e_qxerrors
{
	QX_OPEN_ERR = 1,
	QX_SIGN_ERR = 2,
	QX_MALLOC_ERR = 3,
	QX_INVALID_ARG = 4,
	QX_INFLATEINIT_ERR = 5,
	QX_INVALID_MDATA = 6,
	QX_INVALID_FILTER = 7,
	QX_INVALID_COLORTYPE = 8,
}	t_qxerrors;

typedef struct s_pngmdata
{
	uint32_t		width;
	uint32_t		height;
	unsigned char	bit_depth;
	unsigned char	color_type;
	unsigned char	compression_method;
	unsigned char	filter_method;
	unsigned char	interlace_method;
	unsigned char	channels;
	unsigned char	bytes_pp;
}	t_pngmdata;

typedef struct s_rgba
{
	unsigned char	r;
	unsigned char	g;
	unsigned char	b;
	unsigned char	a;
}	t_rgba;

typedef struct s_rgb
{
	unsigned char	r;
	unsigned char	g;
	unsigned char	b;
}	t_rgb;

struct s_colortable;
typedef uint32_t	(*t_pixel_action)(unsigned char **pixel_data,
						t_pngmdata mdata, struct s_colortable *ct);

uint32_t	store_pixel_colors(unsigned char **pixel_data, t_pngmdata mdata,
				struct s_colortable *ct);
uint32_t	print_pixels(unsigned char **pixel_data, t_pngmdata mdata,
				struct s_colortable *ct);
uint32_t	parse_data_chunk(unsigned char *out, t_pngmdata mdata,
				struct s_colortable *ct, t_pixel_action pix_action);
uint32_t	read_all_chunks(FILE **file, struct s_colortable *ct,
				t_pixel_action pix_action);
uint32_t	open_file(const char *filename, FILE **file);
void		qx_error(uint32_t err_code);

#endif
