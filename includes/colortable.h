#ifndef COLORTABLE_H
# define COLORTABLE_H

# include <stdint.h>
# include <stdlib.h>
# include <string.h>
# include "converter.h"

# define INITIAL_CAPACITY 16
/*
 * Reference: https://github.com/catb0t/fnv-hash/blob/master/hash_32.c
*/
# define FNV32_PRIME ((uint32_t)0x01000193)
/*
 * Reference: https://github.com/catb0t/fnv-hash/blob/master/fnv.h
*/
# define FNV32_OFFSET_BASIS ((uint32_t)0x811c9dc5)

/*
	This hash table inmplementation is not really portable. I want to
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

#endif
