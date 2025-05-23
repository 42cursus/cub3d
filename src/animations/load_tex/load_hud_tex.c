/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   load_hud_tex.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/21 15:19:04 by fsmyth            #+#    #+#             */
/*   Updated: 2025/05/21 22:40:03 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	load_energy_tex(t_info *app)
{
	t_texarr	*tex;

	tex = app->shtex->energy_tex;
	tex[0].img = img_to_arr((char *)"./textures/energy.xpm",
			app, &tex[0].x, &tex[0].y);
	tex[1].img = img_to_arr((char *)"./textures/energy_bu_full.xpm",
			app, &tex[1].x, &tex[1].y);
	tex[2].img = img_to_arr((char *)"./textures/energy_bu_empty.xpm",
			app, &tex[2].x, &tex[2].y);
}

void	load_dmg_tex(t_info *app)
{
	int			i;
	char		buf[50];
	t_texarr	*tex;

	tex = app->shtex->dmg_tex;
	i = 0;
	while (i < 8)
	{
		ft_snprintf(buf, 50, "./textures/dmg%c.xpm", i + '0');
		tex[i].img = img_to_arr(buf, app, &tex[i].x, &tex[i].y);
		i++;
	}
}

void	load_cannon_tex(t_info *app)
{
	t_texarr	*tex;

	tex = app->shtex->cannon_tex;
	tex[0].img = img_to_arr((char *)"./textures/arm_cannon_big.xpm",
			app, &tex[0].x, &tex[0].y);
	tex[1].img = img_to_arr((char *)"./textures/arm_cannon_big_firing.xpm",
			app, &tex[1].x, &tex[1].y);
}

void	load_boss_bar_tex(t_info *app)
{
	t_texarr	*tex;

	tex = app->shtex->boss_bar;
	tex[0].img = img_to_arr((char *)"./textures/boss_bar_left.xpm",
			app, &tex[0].x, &tex[0].y);
	tex[1].img = img_to_arr((char *)"./textures/boss_bar_right.xpm",
			app, &tex[1].x, &tex[1].y);
}

void	load_misc_graphics(t_info *app)
{
	t_texarr	*tex;
	t_img		dummy;

	tex = &app->shtex->title;
	tex->img = img_to_arr((char *)"./textures/title_card.xpm",
			app, &tex->x, &tex->y);
	tex = &app->shtex->empty;
	tex->img = img_to_arr((char *)"./textures/empty.xpm",
			app, &tex->x, &tex->y);
	tex = &app->shtex->tele;
	tex->img = img_to_arr((char *)"./textures/teleporter.xpm",
			app, &tex->x, &tex->y);
	tex = &app->shtex->scope;
	tex->img = img_to_arr((char *)"./textures/scope.xpm",
			app, &tex->x, &tex->y);
	tex = &app->shtex->credits;
	tex->img = img_to_arr((char *)"./textures/credits_lorem.xpm",
			app, &tex->x, &tex->y);
	app->shtex->alphabet = mlx_xpm_file_to_image(app->mlx,
			(char *)"./textures/small_font.xpm", &dummy.width, &dummy.height);
	app->shtex->playertile = mlx_xpm_file_to_image(app->mlx,
			(char *)"./textures/mmap/MAPPLAYER.xpm",
			&dummy.width, &dummy.height);
}
