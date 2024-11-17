#include "converter.h"

int32_t	open_file(const char *filename, FILE *file)
{
	file = fopen(filename, "r");
	if (!file)
		return (QX_OPEN_ERR);
	return (0);
}
