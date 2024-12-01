#include "colortable.h"

t_colortable	*init_color_table(void)
{
	t_colortable	*ct;
	
	ct = malloc(sizeof(t_colortable));
	if (!ct)
		return (NULL);
	ct->used_slots = 0;
	ct->capacity = INITIAL_CAPACITY;
	ct->entries = calloc(ct->capacity, sizeof(t_colortableentry));
	if (!ct->entries)
	{
		free(ct);
		return (NULL);
	}
	return (ct);
}

void	destroy_color_table(t_colortable *ct)
{
	uint32_t	i;

	i = 0;
	while (i < ct->capacity)
		free((void *)ct->entries[i++].value);
	free(ct->entries);
	free(ct);
}

/*
	The key will always be the unsigned int representation of the RGB channels of each color.
	Instead of iterating over a string with the hex representation, we instead iterate 
	over each byte of the key (ALWAYS 4 bytes).
*/
uint32_t	hash_key(uint32_t key)
{
	uint32_t	hash;
	uint32_t	i;

	hash = FNV32_OFFSET_BASIS;
	i = 0;
	while(i < 4)
	{
		hash ^= (uint32_t)(unsigned char)((key >> (8 * i) & 255));
		hash *= FNV32_PRIME;
		i++;
	}
	return (hash);
}

/*
	Assumption: there is at least 1 unused slot, otherwise infinite loop
*/
unsigned char	*get_color_identifier(uint32_t key, t_colortable *ct)
{
	uint32_t	hash;
	uint32_t	index;

	hash = hash_key(key);
	index = (uint32_t)(hash & (uint32_t)(ct->capacity - 1));
	while (ct->entries[index].used)
	{
		if (key == ct->entries[index].key)
			return (ct->entries[index].value);
		index++;
		if (index == ct->capacity)
			index = 0;
	}
	return (NULL);
}

/*
	The table cannot be full, otherwise there is an infinite loop.
*/
static void	new_colortable_entry(t_colortableentry *entries, uint32_t capacity,
						uint32_t key, unsigned char *value, uint32_t *used_slots)
{
	uint32_t	hash;
	uint32_t	index;

	hash = hash_key(key);
	index = (uint32_t)(hash & (uint32_t)(capacity - 1));
	while (entries[index].used) //find first available slot
	{
		//if the key already exists, we can just free the 'new' key.
		//we also free the current value, since the new value is also malloc'd.
		if (key == entries[index].key)
		{
			free((void *)entries[index].value);
			entries[index].value = value;
			return ;
		}
		index++;
		if (index == capacity)
			index = 0;
	}
	if (used_slots)
		*used_slots += 1;
	entries[index].key = key;
	entries[index].value = value;
	entries[index].used = 1;
}

/*
	Every time the hash table is expanded, we recompute the hash for each used key since
	its index depends on the capacity of the color table.
*/
static uint32_t	re_hash_ct_entries(t_colortable *ct,
					t_colortableentry *new_entries, uint32_t new_capacity)
{
	uint32_t			i;
	t_colortableentry	entry;

	i = 0;
	while (i < ct->capacity)
	{
		entry = ct->entries[i];
		if (!entry.used)
		{
			i++;
			continue ;
		}
		new_colortable_entry(new_entries, new_capacity, entry.key, entry.value, NULL);
		i++;
	}
	return (0);
}

/*
	We want to be able to store MAX_COLORS colors. If the new capacity
	is larger than MAX_COLORS, then we set the new capacity to MAX_COLORS.
	For now, a color table can actually only hold MAX_COLORS - 1 colors
	since the loops in get_color() and new_entry() won't exit otherwise.
*/
static uint32_t	expand_colortable(t_colortable *ct)
{
	uint32_t			new_capacity;
	t_colortableentry	*new_ct_entries;
	
	new_capacity = ct->capacity * 2;
	if (new_capacity > MAX_COLORS)
		new_capacity = MAX_COLORS - 1;
	new_ct_entries = calloc(new_capacity, sizeof(t_colortableentry));
	if (!new_ct_entries)
		return (QX_MALLOC_ERR);
	re_hash_ct_entries(ct, new_ct_entries, new_capacity);
	free(ct->entries);
	ct->entries = new_ct_entries;
	ct->capacity = new_capacity;
	return (0);
}

/*
	Allow NULL as values, but not as keys.
*/
uint32_t	add_color(t_colortable *ct, uint32_t key, unsigned char *value)
{
	if (ct->used_slots >= ct->capacity * 0.5f && ct->capacity != MAX_COLORS - 1)
	{
		if (expand_colortable(ct))
			return (QX_MALLOC_ERR);
	}
	new_colortable_entry(ct->entries, ct->capacity, key, value, &ct->used_slots);
	return (0);
}
