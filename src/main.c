#include "colortable.h"

int	main(int ac, char **argv)
{
	FILE			*f;
	uint32_t		ret;
	uint32_t		ret_reader;
	t_colortable	*ct;

	if (ac != 2)
	{
		qx_error(QX_INVALID_ARG);
		return (1);
	}
	f = NULL;
	ct = init_color_table();
	if (!ct)
	{
		qx_error(QX_MALLOC_ERR);
		return (1);
	}
	ret = open_file(argv[1], &f);
	if (ret)
	{
		qx_error(ret);
		return (1);
	}
	ret_reader = read_all_chunks(&f, ct, store_pixel_colors);
	if (ret_reader)
	{
		destroy_color_table(ct);
		qx_error(ret_reader);
		fclose(f);
		return (1);
	}
	fclose(f);
	assign_color_identifier(ct);
	ret = open_file(argv[1], &f);
	if (ret)
	{
		qx_error(ret);
		return (1);
	}
	ret_reader = read_all_chunks(&f, ct, print_pixels);
	if (ret_reader)
	{
		destroy_color_table(ct);
		qx_error(ret_reader);
		fclose(f);
		return (1);
	}
	destroy_color_table(ct);
	fclose(f);
	return (0);
}
