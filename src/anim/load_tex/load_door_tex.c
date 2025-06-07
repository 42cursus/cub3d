/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   load_door_tex.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/21 15:09:12 by fsmyth            #+#    #+#             */
/*   Updated: 2025/05/21 15:10:11 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	load_door_tex(t_info *app)
{
	int			i;
	char		buf[50];
	t_texture	*tex;

	tex = app->shtex->door_tex;
	i = 0;
	while (i < 5)
	{
		ft_snprintf(buf, 50,
			"./resources/textures/metroid_door_anim%c.xpm", i + '1');
		tex[i + 2].data = img_to_tex(app, buf, &tex[i + 2].x, &tex[i + 2].y);
		i++;
	}
	tex[0].data = img_to_tex(
		app, (char *) "./resources/textures/metroid_door3.xpm", &tex[0].x, &tex[0].y);
	tex[1].data = img_to_tex(
		app, (char *) "./resources/textures/metroid_door_open.xpm", &tex[1].x, &tex[1].y);
}

void	load_boss_door_tex(t_info *app)
{
	int			i;
	char		buf[50];
	t_texture	*tex;

	tex = app->shtex->door_boss_tex;
	i = 0;
	while (i < 5)
	{
		ft_snprintf(buf, 50,
			"./resources/textures/metroid_door_boss_anim%c.xpm", i + 1 + '0');
		tex[i + 2].data = img_to_tex(app, buf, &tex[i + 2].x, &tex[i + 2].y);
		i++;
	}
	tex[0].data = img_to_tex(
		app, (char *) "./resources/textures/metroid_door_boss.xpm", &tex[0].x, &tex[0].y);
	tex[1].data = img_to_tex(
		app, (char *) "./resources/textures/metroid_door_open.xpm", &tex[1].x, &tex[1].y);
}

void	load_super_door_tex(t_info *app)
{
	int			i;
	char		buf[50];
	t_texture	*tex;

	tex = app->shtex->door_super_tex;
	i = -1;
	while (++i < 5)
	{
		ft_snprintf(buf, 50,
			"./resources/textures/metroid_door_super_anim%c.xpm", i + '1');
		tex[i + 2].data = img_to_tex(app, buf, &tex[i + 2].x, &tex[i + 2].y);
	}
	tex[0].data = img_to_tex(
		app, (char *) "./resources/textures/metroid_door_super.xpm", &tex[0].x,
		&tex[0].y);
	tex[1].data = img_to_tex(
		app, (char *) "./resources/textures/metroid_door_open.xpm", &tex[1].x, &tex[1].y);
}

void	load_missile_door_tex(t_info *app)
{
	int			i;
	char		buf[50];
	t_texture	*tex;

	tex = app->shtex->door_missile_tex;
	i = -1;
	while (++i < 5)
	{
		ft_snprintf(buf, 50,
			"./resources/textures/metroid_door_missile_anim%c.xpm", i + 1 + '0');
		tex[i + 2].data = img_to_tex(app, buf, &tex[i + 2].x, &tex[i + 2].y);
	}
	tex[0].data = img_to_tex(
		app, (char *) "./resources/textures/metroid_door_missile.xpm", &tex[0].x,
		&tex[0].y);
	tex[1].data = img_to_tex(
		app, (char *) "./resources/textures/metroid_door_open.xpm", &tex[1].x, &tex[1].y);
}
