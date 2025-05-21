/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   load_pickup_tex.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/21 15:20:42 by fsmyth            #+#    #+#             */
/*   Updated: 2025/05/21 15:21:20 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	load_health_pu_tex(t_info *app)
{
	int			i;
	char		buf[50];
	t_texarr	*tex;

	tex = app->shtex->health_pu;
	i = 0;
	while (i < 4)
	{
		ft_snprintf(buf, 50, "./textures/health_pu%c.xpm", i + '0');
		tex[i].img = img_to_arr(buf, app, &tex[i].x, &tex[i].y);
		i++;
	}
}

void	load_ammo_tex(t_info *app)
{
	int			i;
	char		buf[50];
	t_texarr	*tex;

	i = 0;
	tex = app->shtex->missile_ammo;
	while (i < 2)
	{
		ft_snprintf(buf, 50, "./textures/missile_ammo%c.xpm", i + '0');
		tex[i].img = img_to_arr(buf, app, &tex[i].x, &tex[i].y);
		i++;
	}
	tex = app->shtex->super_ammo;
	i = 0;
	while (i < 2)
	{
		ft_snprintf(buf, 50, "./textures/super_ammo%c.xpm", i + '0');
		tex[i].img = img_to_arr(buf, app, &tex[i].x, &tex[i].y);
		i++;
	}
}

void	load_etank_tex(t_info *app)
{
	t_texarr	*tex;

	tex = app->shtex->etank_tex;
	tex[0].img = img_to_arr((char *)"./textures/etank0.xpm",
			app, &tex[0].x, &tex[0].y);
	tex[1].img = img_to_arr((char *)"./textures/etank1.xpm",
			app, &tex[1].x, &tex[1].y);
}

void	load_trophy_tex(t_info *app)
{
	t_texarr	*tex;

	tex = app->shtex->trophy_tex;
	tex[0].img = img_to_arr((char *)"./textures/trophy0.xpm",
			app, &tex[0].x, &tex[0].y);
	tex[1].img = img_to_arr((char *)"./textures/trophy1.xpm",
			app, &tex[1].x, &tex[1].y);
}
