#include "colortable.h"

int	main(int ac, char **argv)
{
	FILE			*f;
	unsigned char	**pixel_data;
	uint32_t		ret;
	uint32_t		ret_reader;
	t_colortable	*ct;
	t_pngmdata		mdata;

	if (ac != 2)
	{
		qx_error(QX_INVALID_ARG);
		return (1);
	}
	pixel_data = NULL;
	f = NULL;
	ct = init_color_table();
	if (!ct)
	{
		qx_error(QX_MALLOC_ERR);
		return (1);
	}
	printf("/* XPM */\nstatic char *image[] = {\n/* columns rows colors chars-per-pixel */\n");
	ret = open_file(argv[1], &f);
	if (ret)
	{
		qx_error(ret);
		return (1);
	}
	ret_reader = read_all_chunks(&f, &mdata, &pixel_data);
	if (ret_reader)
	{
		destroy_color_table(ct);
		qx_error(ret_reader);
		fclose(f);
		return (1);
	}
	store_pixel_colors(pixel_data, mdata, ct);
	printf("\"%u %u %u %d\",\n", mdata.width, mdata.height, ct->used_slots, get_chars_pp(ct->used_slots));
	assign_color_identifier(ct);
	print_color_mapping(ct);
	print_pixels(pixel_data, mdata, ct);
	free_pixel_data(pixel_data);
	destroy_color_table(ct);
	fclose(f);
	return (0);
}
