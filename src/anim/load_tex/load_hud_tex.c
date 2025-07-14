/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   load_hud_tex.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/21 15:19:04 by fsmyth            #+#    #+#             */
/*   Updated: 2025/06/04 20:06:39 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"
#include "libft.h"

void	load_energy_tex(t_info *app)
{
	t_tex *const	tex = app->shtex->energy_tex;

	tex[0] = img_to_tex_row_major(app, TEX_DIR"/energy.xpm");
	tex[1] = img_to_tex_row_major(app, TEX_DIR"/energy_bu_full.xpm");
	tex[2] = img_to_tex_row_major(app, TEX_DIR"/energy_bu_empty.xpm");
}

void	load_logo_tex(t_info *app)
{
	t_tex *const	tex = app->shtex->logo_tex;

	tex[0] = img_to_tex(app, TEX_DIR"/logo_4-1.xpm");
	tex[1] = img_to_tex(app, TEX_DIR"/logo_4-2.xpm");
	tex[2] = img_to_tex(app, TEX_DIR"/logo_4-3.xpm");
	tex[3] = img_to_tex(app, TEX_DIR"/logo_2a-1.xpm");
	tex[4] = img_to_tex(app, TEX_DIR"/logo_2a-2.xpm");
	tex[5] = img_to_tex(app, TEX_DIR"/logo_2b-1.xpm");
	tex[6] = img_to_tex(app, TEX_DIR"/logo_2b-2.xpm");
	tex[7] = img_to_tex(app, TEX_DIR"/logo_pipe.xpm");
	tex[8] = img_to_tex(app, TEX_DIR"/logo_l.xpm");
	tex[9] = img_to_tex(app, TEX_DIR"/logo_o1.xpm");
	tex[10] = img_to_tex(app, TEX_DIR"/logo_n1.xpm");
	tex[11] = img_to_tex(app, TEX_DIR"/logo_d.xpm");
	tex[12] = img_to_tex(app, TEX_DIR"/logo_o2.xpm");
	tex[13] = img_to_tex(app, TEX_DIR"/logo_n2.xpm");
}

void	load_dmg_tex(t_info *app)
{
	int		i;
	char	buf[BUFF_SIZE];
	t_tex	*tex;

	tex = app->shtex->dmg_tex;
	i = -1;
	while (++i < 8)
	{
		ft_snprintf(buf, BUFF_SIZE, TEX_DIR"/dmg%c.xpm", i + '0');
		tex[i] = img_to_tex_row_major(app, buf);
	}
}

void	load_cannon_tex(t_info *app)
{
	t_tex	*tex;

	tex = app->shtex->cannon_tex;
	tex[0] = img_to_tex_row_major(app, TEX_DIR"/arm_cannon_big.xpm");
	tex[1] = img_to_tex_row_major(app, TEX_DIR"/arm_cannon_big_firing.xpm");
}

void	load_boss_bar_tex(t_info *app)
{
	t_tex	*tex;

	tex = app->shtex->boss_bar;
	tex[0] = img_to_tex_row_major(app, TEX_DIR"/boss_bar_left.xpm");
	tex[1] = img_to_tex_row_major(app, TEX_DIR"/boss_bar_right.xpm");
}

void	apply(char *str, void *ref)
{
	t_str_arr *const	strings = ref;

	strings->arr[strings->current++] = str;
}

t_tex	create_ft_string(FT_Face face, t_str_arr str_arr)
{
	t_tex	tex;
	t_fnt	fnt;

	fnt.metrics = face->size->metrics;
	fnt.line_height = fnt.metrics.height >> 6;
	fnt.spacing = fnt.metrics.ascender >> 6;
	fnt.total_height = str_arr.size * (fnt.line_height + fnt.spacing);
	fnt.width = compute_text_width(face, str_arr.arr[str_arr.longest_index]);
	tex = (t_tex){.w = (int)fnt.width, .h = (int)fnt.total_height * 2};
	tex.sl = tex.w * sizeof(int);
	if (posix_memalign((void **) &tex.data, 64, tex.h * tex.sl))
		return (tex);
	fill_with_colour_tex(tex, XPM_TRANSPARENT);
	draw_multiline_text_centered(face, tex, str_arr.arr, str_arr.size);
	return (tex);
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
	fnt.face = app->typ.faces[fnt_snes];
	FT_Set_Pixel_Sizes(fnt.face, 0, 40);
	fnt.metrics = fnt.face->size->metrics;
	fnt.line_height = fnt.metrics.height >> 6;
	fnt.spacing = fnt.metrics.ascender >> 6;
	fnt.total_height = str_arr.size * (fnt.line_height + fnt.spacing);
	tex = (t_tex){.w = 1000, .h = (int)fnt.total_height + 50};
	tex.sl = tex.w * sizeof(int);
	if (posix_memalign((void **) &tex.data, 64, tex.h * tex.sl))
		return (tex);
	fill_with_colour_tex(tex, XPM_TRANSPARENT);
	draw_multiline_text_centered(fnt.face, tex, str_arr.arr, str_arr.size);
	ft_tab_str_free(str_arr.arr);
	return (tex);
}

t_tex	draw_playertile(void)
{
	static u_int	data[4];
	t_tex			out;

	data[0] = MLX_PALE_GRAY;
	data[1] = MLX_PALE_GRAY;
	data[2] = MLX_PALE_GRAY;
	data[3] = MLX_PALE_GRAY;
	out.data = data;
	out.w = 2;
	out.h = 2;
	return (out);
}

void	generate_msg_text(t_info *app)
{
	int					i;
	t_tex *const		msgs = app->shtex->messages;
	FT_Face const		face = app->typ.faces[fnt_snes];
	t_str_arr			str_arrs[MSG_MAX];
	static const char	*str_arr[6] = {
		"Teleporter ONE inactive. Find the key.",
		"Teleporter TWO inactive. Find the key.",
		"Teleporter THREE inactive. Find the key.",
		"You found key for teleporter ONE.",
		"You found key for teleporter TWO.",
		"You found key for teleporter THREE."
	};

	FT_Set_Pixel_Sizes(face, 0, 40);
	i = -1;
	while (++i < MSG_MAX)
	{
		str_arrs[i].arr = (char **)&str_arr[i];
		str_arrs[i].longest_index = 0;
		str_arrs[i].current = 0;
		str_arrs[i].size = 1;
	}
	i = -1;
	while (++i < MSG_MAX)
		msgs[i] = create_ft_string(face, str_arrs[i]);
}

void	load_misc_graphics(t_info *app)
{
	extern const char	*title_card_xpm[];
	extern const char	*empty_xpm[];
	extern const char	*teleporter_xpm[];
	extern const char	*scope_xpm[];
	extern const char	*small_font_xpm[];

	app->shtex->title = img_to_tex_static_rm(app, title_card_xpm);
	app->shtex->empty = img_to_tex_static_rm(app, empty_xpm);
	app->shtex->tele = img_to_tex_static_cm(app, teleporter_xpm);
	app->shtex->scope = img_to_tex_static_rm(app, scope_xpm);
	app->shtex->credits = draw_credits(app);
	app->shtex->alphabet = img_to_tex_static_rm(app, small_font_xpm);
	app->shtex->playertile = draw_playertile();
	app->shtex->square = get_tile(15);
	generate_msg_text(app);
}
