#include "colortable.h"

void	free_pixel_data(unsigned char **pixel_data)
{
	uint32_t	i;

	i = 0;
	if (!pixel_data)
		return ;
	while (pixel_data[i])
		free(pixel_data[i++]);
	free(pixel_data);
}
