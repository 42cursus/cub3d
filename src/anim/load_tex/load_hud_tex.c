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
		tex[i] = img_to_tex_row_major(app,  buf);
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

void	load_misc_graphics(t_info *app)
{
	t_tex				*tex;
	extern const char	*title_card_xpm[];
	extern const char	*empty_xpm[];
	extern const char	*teleporter_xpm[];
	extern const char	*scope_xpm[];
	extern const char	*credits_xpm[];
	static u_int		player[] = (u_int[]) {[0 ... 4] = MLX_PALE_GRAY};
	extern const char	*small_font_xpm[];

	tex = &app->shtex->title;
	tex->data = img_to_tex_static_row_major(app, title_card_xpm, &tex->w, &tex->h);
	tex = &app->shtex->empty;
	tex->data = img_to_tex_static_row_major(app, empty_xpm, &tex->w, &tex->h);
	tex = &app->shtex->tele;
	tex->data = img_to_tex_static_col_major(app, teleporter_xpm, &tex->w, &tex->h);
	tex = &app->shtex->scope;
	tex->data = img_to_tex_static_row_major(app, scope_xpm, &tex->w, &tex->h);
	tex = &app->shtex->credits;
	tex->data = img_to_tex_static_row_major(app, credits_xpm, &tex->w, &tex->h);
	tex = &app->shtex->alphabet;
	tex->data = img_to_tex_static_row_major(app, small_font_xpm, &tex->w, &tex->h);
	app->shtex->playertile = (t_tex){.data = player, .w = 2, .h = 2};
	app->shtex->square = get_tile(15);
}
