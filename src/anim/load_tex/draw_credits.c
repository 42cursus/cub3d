/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   draw_credits.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 16:14:37 by fsmyth            #+#    #+#             */
/*   Updated: 2025/08/11 16:15:20 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	apply(char *str, void *ref)
{
	t_str_arr *const	strings = ref;

	strings->arr[strings->current++] = str;
}

void	draw_credits_setup_font(t_info *app, t_fnt *fnt, int size)
{
	fnt->face = app->typ.faces[fnt_snes];
	FT_Set_Pixel_Sizes(fnt->face, 0, 40);
	fnt->metrics = fnt->face->size->metrics;
	fnt->line_height = fnt->metrics.height >> 6;
	fnt->spacing = fnt->metrics.ascender >> 6;
	fnt->total_height = size * (fnt->line_height + fnt->spacing);
}

t_tex	draw_credits(t_info *app)
{
	int			fd;
	t_tex		tex;
	t_str_arr	str_arr;
	t_list		*lines;
	t_fnt		fnt;

	ft_memset(&tex, 0, sizeof(tex));
	fd = open("resources/credits.txt", O_RDONLY);
	if (fd == -1)
		return (tex);
	lines = read_file_stripped(fd);
	str_arr.size = ft_list_size(lines);
	str_arr.arr = ft_calloc(str_arr.size + 1, sizeof(char *));
	str_arr.current = 0;
	ft_list_foreach_ref(lines, (void *)apply, &str_arr);
	ft_list_destroy(&lines, NULL);
	draw_credits_setup_font(app, &fnt, str_arr.size);
	tex = (t_tex){.w = 1000, .h = (int)fnt.total_height + 50};
	tex.sl = tex.w * sizeof(int);
	if (posix_memalign((void **) &tex.data, 64, tex.h * tex.sl))
		return (tex);
	fill_with_colour_tex(tex, XPM_TRANSPARENT);
	draw_multiline_text_centered(fnt.face, tex, str_arr.arr, str_arr.size);
	ft_tab_str_free(str_arr.arr);
	return (tex);
}
