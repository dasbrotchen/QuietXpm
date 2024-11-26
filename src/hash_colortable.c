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
	{
		free((void *)ct->entries[i].key);
		free((void *)ct->entries[i++].value);
	}
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
unsigned char	*get_color_identifier(const char *key, t_colortable *ct)
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
	printf("did not find color <%s>\n", key);
	return (NULL);
}

/*
	The table cannot be full, otherwise there is an infinite loop.
*/
static const char	*new_colortable_entry(t_colortableentry *entries, uint32_t capacity,
						const char *key, unsigned char *value, uint32_t *used_slots)
{
	uint32_t	hash;
	uint32_t	index;

	hash = hash_key(key);
	index = (uint32_t)(hash & (uint32_t)(capacity - 1));
	while (entries[index].key) //find first available slot
	{
		//if the key already exists, we can just free the 'new' key.
		//we also free the current value, since the new value is also malloc'd.
		if (!strcmp(key, entries[index].key))
		{
			free((void *)key);
			free((void *)entries[index].value);
			entries[index].value = value;
			return (entries[index].key);
		}
		index++;
		if (index == capacity)
			index = 0;
	}
	if (used_slots)
		*used_slots += 1;
	entries[index].key = key;
	entries[index].value = value;
	return (key);
}

/*
static void	free_colortable_entries(t_colortableentry *entries, uint32_t capacity)
{
	uint32_t	i;

	i = 0;
	while (i < capacity)
	{
		if (entries[i].key)
		{
			free((void *)entries[i].key);
			free((void *)entries[i].value);
		}
		i++;
	}
	free(entries);
}*/

static uint32_t	copy_colortable_entries(t_colortable *ct,
					t_colortableentry *new_entries, uint32_t new_capacity)
{
	uint32_t			i;
	t_colortableentry	entry;

	i = 0;
	while (i < ct->capacity)
	{
		entry = ct->entries[i];
		if (!entry.key)
		{
			i++;
			continue ;
		}
		new_colortable_entry(new_entries, new_capacity, entry.key, entry.value, NULL);
		i++;
		/*
		new_entries[i].key = strdup(ct->entries[i].key);
		if (!new_entries[i].key)
			return (QX_MALLOC_ERR); //free all current and new entries here
		if (ct->entries[i].value)
			new_entries[i].value = (unsigned char *)strdup((const char *)ct->entries[i].value);
		else
			new_entries[i].value = NULL;
		*/
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
	if (copy_colortable_entries(ct, new_ct_entries, new_capacity))
	{
		//free everything here
		return (QX_MALLOC_ERR);
	}
	free(ct->entries);
	//free_colortable_entries(ct->entries, ct->capacity);
	ct->entries = new_ct_entries;
	ct->capacity = new_capacity;
	return (0);
}

/*
	Allow NULL as values, but not as keys.
*/
const char	*add_color(t_colortable *ct, const char *key, unsigned char *value)
{
	if (ct->used_slots >= ct->capacity * 0.5f && ct->capacity != MAX_COLORS - 1)
	{
		if (expand_colortable(ct))
			return (NULL);
	}
	return (new_colortable_entry(ct->entries, ct->capacity, key, value, &ct->used_slots));
}
