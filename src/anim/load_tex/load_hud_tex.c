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

	tex[0].data = img_to_tex_row_major(app, (char *) TEX_DIR"/energy.xpm", &tex[0].w, &tex[0].h);
	tex[1].data = img_to_tex_row_major(app, (char *) TEX_DIR"/energy_bu_full.xpm", &tex[1].w, &tex[1].h);
	tex[2].data = img_to_tex_row_major(app, (char *) TEX_DIR"/energy_bu_empty.xpm", &tex[2].w, &tex[2].h);
}

void	load_logo_tex(t_info *app)
{
	t_tex *const	tex = app->shtex->logo_tex;

	tex[0].data = img_to_tex(app, (char *) TEX_DIR"/logo_4-1.xpm", &tex[0].w, &tex[0].h);
	tex[1].data = img_to_tex(app, (char *) TEX_DIR"/logo_4-2.xpm", &tex[1].w, &tex[1].h);
	tex[2].data = img_to_tex(app, (char *) TEX_DIR"/logo_4-3.xpm", &tex[2].w, &tex[2].h);
	tex[3].data = img_to_tex(app, (char *) TEX_DIR"/logo_2a-1.xpm", &tex[3].w, &tex[3].h);
	tex[4].data = img_to_tex(app, (char *) TEX_DIR"/logo_2a-2.xpm", &tex[4].w, &tex[4].h);
	tex[5].data = img_to_tex(app, (char *) TEX_DIR"/logo_2b-1.xpm", &tex[5].w, &tex[5].h);
	tex[6].data = img_to_tex(app, (char *) TEX_DIR"/logo_2b-2.xpm", &tex[6].w, &tex[6].h);
	tex[7].data = img_to_tex(app, (char *) TEX_DIR"/logo_pipe.xpm", &tex[7].w, &tex[7].h);
	tex[8].data = img_to_tex(app, (char *) TEX_DIR"/logo_l.xpm", &tex[8].w, &tex[8].h);
	tex[9].data = img_to_tex(app, (char *) TEX_DIR"/logo_o1.xpm", &tex[9].w, &tex[9].h);
	tex[10].data = img_to_tex(app, (char *) TEX_DIR"/logo_n1.xpm", &tex[10].w, &tex[10].h);
	tex[11].data = img_to_tex(app, (char *) TEX_DIR"/logo_d.xpm", &tex[11].w, &tex[11].h);
	tex[12].data = img_to_tex(app, (char *) TEX_DIR"/logo_o2.xpm", &tex[12].w, &tex[12].h);
	tex[13].data = img_to_tex(app, (char *) TEX_DIR"/logo_n2.xpm", &tex[13].w, &tex[13].h);
}

void	load_dmg_tex(t_info *app)
{
	int		i;
	char	buf[50];
	t_tex	*tex;

	tex = app->shtex->dmg_tex;
	i = -1;
	while (++i < 8)
	{
		ft_snprintf(buf, 50, TEX_DIR"/dmg%c.xpm", i + '0');
		tex[i].data = img_to_tex_row_major(app, buf, &tex[i].w, &tex[i].h);
	}
}

void	load_cannon_tex(t_info *app)
{
	t_tex	*tex;

	tex = app->shtex->cannon_tex;
	tex[0].data = img_to_tex_row_major(app, (char *) TEX_DIR"/arm_cannon_big.xpm", &tex[0].w, &tex[0].h);
	tex[1].data = img_to_tex_row_major(app, (char *) TEX_DIR"/arm_cannon_big_firing.xpm", &tex[1].w, &tex[1].h);
}

void	load_boss_bar_tex(t_info *app)
{
	t_tex	*tex;

	tex = app->shtex->boss_bar;
	tex[0].data = img_to_tex_row_major(app, (char *) TEX_DIR"/boss_bar_left.xpm", &tex[0].w, &tex[0].h);
	tex[1].data = img_to_tex_row_major(app, (char *) TEX_DIR"/boss_bar_right.xpm", &tex[1].w, &tex[1].h);
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
