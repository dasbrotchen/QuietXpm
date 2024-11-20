#include "converter.h"

static inline uint32_t	is_valid_byte(unsigned char input,
							unsigned char target)
{
	return (input == target);
}

static uint32_t	validate_png(FILE *file)
{
	char		signature[8];
	size_t		elements_read;

	elements_read = fread(signature, 1, 8, file);
	if (elements_read != 8)
		return (QX_SIGN_ERR);
	if (!is_valid_byte(signature[0], 137)
		|| !is_valid_byte(signature[1], 80)
		|| !is_valid_byte(signature[2], 78)
		|| !is_valid_byte(signature[3], 71)
		|| !is_valid_byte(signature[4], 13)
		|| !is_valid_byte(signature[5], 10)
		|| !is_valid_byte(signature[6], 26)
		|| !is_valid_byte(signature[7], 10))
	{
		printf("signature %d: %d\n",0, signature[0]);
		printf("signature %d: %d\n",1, signature[1]);
		printf("signature %d: %d\n",2, signature[2]);
		printf("signature %d: %d\n",3, signature[3]);
		printf("signature %d: %d\n",4, signature[4]);
		printf("signature %d: %d\n",5, signature[5]);
		printf("signature %d: %d\n",6, signature[6]);
		printf("signature %d: %d\n",7, signature[7]);
		return (QX_SIGN_ERR);
	}
	return (0);
}

uint32_t	open_file(const char *filename, FILE **file)
{
	uint32_t	ret_validate;

	*file = fopen(filename, "r");
	if (!*file)
		return (QX_OPEN_ERR);
	ret_validate = validate_png(*file);
	if (ret_validate)
	{
		fclose(*file);
		return (ret_validate);
	}
	return (0);
}
