#include "colortable.h"
#include <unistd.h>

int	main(int ac, char **argv)
{
	FILE			*f;
	FILE			*outf;
	unsigned char	**pixel_data;
	uint32_t		ret;
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
	ret = open_file(argv[1], &f);
	if (ret)
	{
		qx_error(ret);
		destroy_color_table(ct);
		return (1);
	}
	ret = read_all_chunks(&f, &mdata, &pixel_data);
	if (ret)
	{
		qx_error(ret);
		free_pixel_data(pixel_data);
		destroy_color_table(ct);
		fclose(f);
		return (1);
	}
	if (store_pixel_colors(pixel_data, mdata, ct))
	{
		qx_error(QX_MALLOC_ERR);
		free_pixel_data(pixel_data);
		destroy_color_table(ct);
		fclose(f);
		return (1);
	}
	if (assign_color_identifier(ct))
	{
		qx_error(QX_MALLOC_ERR);
		free_pixel_data(pixel_data);
		destroy_color_table(ct);
		fclose(f);
		return (1);
	}
	ret = open_outfile(argv[1], &outf);
	if (ret)
	{
		qx_error(ret);
		free_pixel_data(pixel_data);
		destroy_color_table(ct);
		fclose(f);
		return (1);
	}
	fprintf(outf, "/* XPM */\nstatic char *image[] = {\n/* columns rows colors chars-per-pixel */\n");
	fprintf(outf, "\"%u %u %u %d\",\n", mdata.width, mdata.height, ct->used_slots, get_chars_pp(ct->used_slots));
	print_color_mapping(ct, mdata, outf);
	print_pixels(pixel_data, mdata, ct, outf);
	free_pixel_data(pixel_data);
	destroy_color_table(ct);
	fclose(f);
	fclose(outf);
	return (0);
}
