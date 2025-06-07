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
	t_texture *const	tex = app->shtex->energy_tex;

	tex[0].data = img_to_tex_row_major(
		app, (char *) "./resources/textures/energy.xpm", &tex[0].x, &tex[0].y);
	tex[1].data = img_to_tex_row_major(
		app, (char *) "./resources/textures/energy_bu_full.xpm", &tex[1].x, &tex[1].y);
	tex[2].data = img_to_tex_row_major(
		app, (char *) "./resources/textures/energy_bu_empty.xpm", &tex[2].x, &tex[2].y);
}

void	load_logo_tex(t_info *app)
{
	t_texture *const	tex = app->shtex->logo_tex;

	tex[0].data = img_to_tex(
		app, (char *) "./resources/textures/logo_4-1.xpm", &tex[0].x, &tex[0].y);
	tex[1].data = img_to_tex(
		app, (char *) "./resources/textures/logo_4-2.xpm", &tex[1].x, &tex[1].y);
	tex[2].data = img_to_tex(
		app, (char *) "./resources/textures/logo_4-3.xpm", &tex[2].x, &tex[2].y);
	tex[3].data = img_to_tex(
		app, (char *) "./resources/textures/logo_2a-1.xpm", &tex[3].x, &tex[3].y);
	tex[4].data = img_to_tex(
		app, (char *) "./resources/textures/logo_2a-2.xpm", &tex[4].x, &tex[4].y);
	tex[5].data = img_to_tex(
		app, (char *) "./resources/textures/logo_2b-1.xpm", &tex[5].x, &tex[5].y);
	tex[6].data = img_to_tex(
		app, (char *) "./resources/textures/logo_2b-2.xpm", &tex[6].x, &tex[6].y);
	tex[7].data = img_to_tex(
		app, (char *) "./resources/textures/logo_pipe.xpm", &tex[7].x, &tex[7].y);
	tex[8].data = img_to_tex(
		app, (char *) "./resources/textures/logo_l.xpm", &tex[8].x, &tex[8].y);
	tex[9].data = img_to_tex(
		app, (char *) "./resources/textures/logo_o1.xpm", &tex[9].x, &tex[9].y);
	tex[10].data = img_to_tex(
		app, (char *) "./resources/textures/logo_n1.xpm", &tex[10].x, &tex[10].y);
	tex[11].data = img_to_tex(
		app, (char *) "./resources/textures/logo_d.xpm", &tex[11].x, &tex[11].y);
	tex[12].data = img_to_tex(
		app, (char *) "./resources/textures/logo_o2.xpm", &tex[12].x, &tex[12].y);
	tex[13].data = img_to_tex(
		app, (char *) "./resources/textures/logo_n2.xpm", &tex[13].x, &tex[13].y);
}

void	load_dmg_tex(t_info *app)
{
	int			i;
	char		buf[50];
	t_texture	*tex;

	tex = app->shtex->dmg_tex;
	i = -1;
	while (++i < 8)
	{
		ft_snprintf(buf, 50, "./resources/textures/dmg%c.xpm", i + '0');
		tex[i].data = img_to_tex_row_major(app, buf, &tex[i].x, &tex[i].y);
	}
}

void	load_cannon_tex(t_info *app)
{
	t_texture	*tex;

	tex = app->shtex->cannon_tex;
	tex[0].data = img_to_tex_row_major(
		app, (char *) "./resources/textures/arm_cannon_big.xpm", &tex[0].x, &tex[0].y);
	tex[1].data = img_to_tex_row_major(
		app, (char *) "./resources/textures/arm_cannon_big_firing.xpm", &tex[1].x,
		&tex[1].y);
}

void	load_boss_bar_tex(t_info *app)
{
	t_texture	*tex;

	tex = app->shtex->boss_bar;
	tex[0].data = img_to_tex_row_major(
		app, (char *) "./resources/textures/boss_bar_left.xpm", &tex[0].x, &tex[0].y);
	tex[1].data = img_to_tex_row_major(
		app, (char *) "./resources/textures/boss_bar_right.xpm", &tex[1].x, &tex[1].y);
}

void	load_misc_graphics(t_info *app)
{
	t_texture			*tex;
	t_img				dummy;
	extern const char	*title_card_xpm[];
	extern const char	*empty_xpm[];
	extern const char	*teleporter_xpm[];
	extern const char	*scope_xpm[];
	extern const char	*credits_xpm[];
	extern const char	*MAPPLAYER_xpm[];
	extern const char	*MAP1111_xpm[];

	tex = &app->shtex->title;
	tex->data = img_to_tex_static_row_major(app, title_card_xpm, &tex->x,
											&tex->y);
	tex = &app->shtex->empty;
	tex->data = img_to_tex_static_row_major(app, empty_xpm, &tex->x, &tex->y);
	tex = &app->shtex->tele;
	tex->data = img_to_tex_static_col_major(app, teleporter_xpm, &tex->x, &tex->y);
	tex = &app->shtex->scope;
	tex->data = img_to_tex_static_row_major(app, scope_xpm, &tex->x, &tex->y);
	tex = &app->shtex->credits;
	tex->data = img_to_tex_static_row_major(app, credits_xpm, &tex->x, &tex->y);
	app->shtex->alphabet = mlx_xpm_file_to_image(app->mlx,
			(char *)"./resources/textures/small_font.xpm", &dummy.width, &dummy.height);
	tex = &app->shtex->playertile;
	tex->data = img_to_tex_static_row_major(app, MAPPLAYER_xpm, &tex->x, &tex->y);
	tex = &app->shtex->square;
	tex->data = img_to_tex_static_row_major(app, MAP1111_xpm, &tex->x, &tex->y);
}
