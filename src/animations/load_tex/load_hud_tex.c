/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   load_hud_tex.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/21 15:19:04 by fsmyth            #+#    #+#             */
/*   Updated: 2025/05/21 15:20:20 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	load_energy_textures(t_info *app)
{
	t_texture *const	tex = app->shtex->energy_tex;

	tex[0].data = img_to_tex(
		app, (char *) "./textures/energy.xpm", &tex[0].x, &tex[0].y);
	tex[1].data = img_to_tex(
		app, (char *) "./textures/energy_bu_full.xpm", &tex[1].x, &tex[1].y);
	tex[2].data = img_to_tex(
		app, (char *) "./textures/energy_bu_empty.xpm", &tex[2].x, &tex[2].y);
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
		ft_snprintf(buf, 50, "./textures/dmg%c.xpm", i + '0');
		tex[i].data = img_to_tex(app, buf, &tex[i].x, &tex[i].y);
	}
}

void	load_cannon_tex(t_info *app)
{
	t_texture	*tex;

	tex = app->shtex->cannon_tex;
	tex[0].data = img_to_tex(
		app, (char *) "./textures/arm_cannon_big.xpm", &tex[0].x, &tex[0].y);
	tex[1].data = img_to_tex(
		app, (char *) "./textures/arm_cannon_big_firing.xpm", &tex[1].x,
		&tex[1].y);
}

void	load_boss_bar_tex(t_info *app)
{
	t_texture	*tex;

	tex = app->shtex->boss_bar;
	tex[0].data = img_to_tex(
		app, (char *) "./textures/boss_bar_left.xpm", &tex[0].x, &tex[0].y);
	tex[1].data = img_to_tex(
		app, (char *) "./textures/boss_bar_right.xpm", &tex[1].x, &tex[1].y);
}

void	load_misc_graphics(t_info *app)
{
	t_texture	*tex;
	t_img		dummy;

	tex = &app->shtex->title;
	tex->data = img_to_tex(app, (char *) "./textures/title_card.xpm", &tex->x, &tex->y);
	tex = &app->shtex->empty;
	tex->data = img_to_tex(app, (char *) "./textures/empty.xpm", &tex->x, &tex->y);
	tex = &app->shtex->tele;
	tex->data = img_to_tex(app, (char *) "./textures/teleporter.xpm", &tex->x, &tex->y);
	tex = &app->shtex->scope;
	tex->data = img_to_tex(app, (char *) "./textures/scope.xpm", &tex->x, &tex->y);
	tex = &app->shtex->credits;
	tex->data = img_to_tex(app, (char *) "./textures/credits_lorem.xpm", &tex->x, &tex->y);
	app->shtex->alphabet = mlx_xpm_file_to_image(app->mlx,
			(char *)"./textures/small_font.xpm", &dummy.width, &dummy.height);
	app->shtex->playertile = mlx_xpm_file_to_image(app->mlx,
			(char *)"./textures/mmap/MAPPLAYER.xpm",
			&dummy.width, &dummy.height);
}
