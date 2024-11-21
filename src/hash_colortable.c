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
		free((void *)ct->entries[i++].key);
	free(ct->entries);
	free(ct);
}

uint32_t	hash_key(const char *key)
{
	uint32_t	hash;
	uint32_t	i;

	hash = FNV32_OFFSET_BASIS;
	i = 0;
	while(key[i])
	{
		hash ^= (uint32_t)(unsigned char)(key[i]);
		hash *= FNV32_PRIME;
		i++;
	}
	return (hash);
}

/*
	Assumption: there is at least 1 NULL key, otherwise infinite loop
*/
unsigned char	*get_color(const char *key, t_colortable *ct)
{
	uint32_t	hash;
	uint32_t	index;

	hash = hash_key(key);
	index = (uint32_t)(hash & (uint32_t)(ct->capacity - 1));
	while (ct->entries[index].key)
	{
		if (!strcmp(key, ct->entries[index].key))
			return (ct->entries[index].value);
		index++;
		if (index == ct->capacity)
			index = 0;
	}
	return (NULL);
}

/*
	We will never have to update the value: once a key (pixel identifier) has a
	value, it remains like so until destruction of the table.
	Likewise, the table cannot be full here otherwise we are in an infinite loop.
*/
static const char	*new_colortable_entry(t_colortable *ct, const char *key,
						unsigned char *value)
{
	uint32_t	hash;
	uint32_t	index;

	hash = hash_key(key);
	index = (uint32_t)(hash & (uint32_t)(ct->capacity - 1));
	while (ct->entries[index].key) //find first available slot
	{
		if (!strcmp(key, ct->entries[index].key)) //key already exists, return and exit
			return (ct->entries[index].key);
		index++;
		if (index == ct->capacity)
			index = 0;
	}
	key = strdup(key);
	if (!key)
		return (NULL);
	ct->used_slots++;
	ct->entries[index].key = key;
	ct->entries[index].value = value;
	return (key);
}

static void	free_colortable_entries(t_colortableentry *entries, uint32_t capacity)
{
	uint32_t	i;

	i = 0;
	while (i < capacity)
	{
		if (entries[i].key)
			free((void *)entries[i].key);
		i++;
	}
	free(entries);
}

static uint32_t	copy_colortable_entries(t_colortable *ct,
					t_colortableentry *new_entries)
{
	uint32_t	i;

	i = 0;
	while (i < ct->capacity)
	{
		if (!ct->entries[i].key)
		{
			i++;
			continue ;
		}
		new_entries[i].key = strdup(ct->entries[i].key);
		if (!new_entries[i].key)
			return (QX_MALLOC_ERR); //free all current and new entries here
		new_entries[i].value = ct->entries[i].value;
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
	if (copy_colortable_entries(ct, new_ct_entries))
	{
		//free everything here
		return (QX_MALLOC_ERR);
	}
	free_colortable_entries(ct->entries, ct->capacity);
	ct->entries = new_ct_entries;
	ct->capacity = new_capacity;
	return (0);
}

/*
	Otherwise return NULL if the value is NULL, or if a malloc broke either
	while expanding the table or adding a new color to the table.
*/
const char	*add_color(t_colortable *ct, const char *key, unsigned char *value)
{
	assert(value);
	if (!value)
		return (NULL);
	if (ct->used_slots >= ct->capacity * 0.5f && ct->capacity != MAX_COLORS - 1)
	{
		if (expand_colortable(ct))
			return (NULL);
	}
	return (new_colortable_entry(ct, key, value));
}
