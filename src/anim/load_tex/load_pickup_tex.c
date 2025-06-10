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
	int		i;
	char	buf[50];
	t_tex	*tex;

	tex = app->shtex->health_pu;
	i = -1;
	while (++i < 4)
	{
		ft_snprintf(buf, 50, "./resources/textures/health_pu%c.xpm", i + '0');
		tex[i].data = img_to_tex(app, buf, &tex[i].w, &tex[i].h);
	}
}

void	load_ammo_tex(t_info *app)
{
	int		i;
	char	buf[50];
	t_tex	*tex;

	i = -1;
	tex = app->shtex->missile_ammo;
	while (++i < 2)
	{
		ft_snprintf(buf, 50, "./resources/textures/missile_ammo%c.xpm", i + '0');
		tex[i].data = img_to_tex(app, buf, &tex[i].w, &tex[i].h);
	}
	tex = app->shtex->super_ammo;
	i = -1;
	while (++i < 2)
	{
		ft_snprintf(buf, 50, "./resources/textures/super_ammo%c.xpm", i + '0');
		tex[i].data = img_to_tex(app, buf, &tex[i].w, &tex[i].h);
	}
}

void	load_etank_tex(t_info *app)
{
	t_tex	*tex;

	tex = app->shtex->etank_tex;
	tex[0].data = img_to_tex(app, (char *) "./resources/textures/etank0.xpm", &tex[0].w, &tex[0].h);
	tex[1].data = img_to_tex(app, (char *) "./resources/textures/etank1.xpm", &tex[1].w, &tex[1].h);
}

void	load_trophy_tex(t_info *app)
{
	t_tex	*tex;

	tex = app->shtex->trophy_tex;
	tex[0].data = img_to_tex(app, (char *) "./resources/textures/trophy0.xpm", &tex[0].w, &tex[0].h);
	tex[1].data = img_to_tex(app, (char *) "./resources/textures/trophy1.xpm", &tex[1].w, &tex[1].h);
}
