#include "colortable.h"

int	main(int ac, char **argv)
{
	FILE			*f;
	unsigned char	**pixel_data;
	uint32_t		ret;
	uint32_t		ret_reader;
	uint32_t		i;
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
	ret_reader = read_all_chunks(&f, ct, store_pixel_colors, &mdata, &pixel_data);
	if (ret_reader)
	{
		destroy_color_table(ct);
		qx_error(ret_reader);
		fclose(f);
		return (1);
	}
	printf("\"%u %u %u %d\",\n", mdata.width, mdata.height, ct->used_slots, get_chars_pp(ct->used_slots));
	fclose(f);
	assign_color_identifier(ct);
	print_pixels(pixel_data, mdata, ct);
	/*
	ret = open_file(argv[1], &f);
	if (ret)
	{
		qx_error(ret);
		return (1);
	}
	print_color_mapping(ct);
	ret_reader = read_all_chunks(&f, ct, print_pixels, &mdata);
	if (ret_reader)
	{
		destroy_color_table(ct);
		qx_error(ret_reader);
		fclose(f);
		return (1);
	}
	*/
	i = 0;
	while (pixel_data[i])
		free(pixel_data[i++]);
	free(pixel_data);
	destroy_color_table(ct);
	fclose(f);
	return (0);
}
