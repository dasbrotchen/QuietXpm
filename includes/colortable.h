#ifndef COLORTABLE_H
# define COLORTABLE_H

# include <stdint.h>
# include <stdlib.h>
# include <string.h>
# include "converter.h"
# include <math.h>

# define INITIAL_CAPACITY 16
/*
 * Reference: https://github.com/catb0t/fnv-hash/blob/master/hash_32.c
*/
# define FNV32_PRIME ((uint32_t)0x01000193)
/*
 * Reference: https://github.com/catb0t/fnv-hash/blob/master/fnv.h
*/
# define FNV32_OFFSET_BASIS ((uint32_t)0x811c9dc5)
# define MAX_NCHAR 92
# define MIN_CHAR_COLOR_ID 35

/*
	This hash table implementation is not really portable. I want to
	use it only to store colours. Thus, the value will always be 
	the hexadecimal version of the RGB(A) pixel colour.
*/
typedef struct s_colortableentry
{
	const char		*key;
	unsigned char	*value;
}	t_colortableentry;

/*
	The capacity of the color table *cannot* exceed 255^3 entries.
*/
typedef struct s_colortable
{
	t_colortableentry	*entries;
	uint32_t			capacity;
	uint32_t			used_slots;
}	t_colortable;

t_colortable	*init_color_table(void);
const char		*add_color(t_colortable *ct, const char *key, unsigned char *value);
const char		*generate_hex_color(t_rgba color);
unsigned char	*generate_color_identifier(uint32_t n_color, unsigned char chars_pp);
uint32_t		assign_color_identifier(t_colortable *ct);
unsigned char	*get_color_identifier(const char *key, t_colortable *ct);
unsigned char	get_chars_pp(uint32_t used_slots);
void			destroy_color_table(t_colortable *ct);

#endif
