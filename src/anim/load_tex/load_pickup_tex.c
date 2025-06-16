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
	char	buf[BUFF_SIZE];
	t_tex	*tex;

	tex = app->shtex->health_pu;
	i = -1;
	while (++i < 4)
	{
		ft_snprintf(buf, BUFF_SIZE, TEX_DIR"/health_pu%c.xpm", i + '0');
		tex[i] = img_to_tex(app, buf);
	}
}

void	load_ammo_tex(t_info *app)
{
	int		i;
	char	buf[BUFF_SIZE];
	t_tex	*tex;

	i = -1;
	tex = app->shtex->missile_ammo;
	while (++i < 2)
	{
		ft_snprintf(buf, BUFF_SIZE, TEX_DIR"/missile_ammo%c.xpm", i + '0');
		tex[i] = img_to_tex(app, buf);
	}
	tex = app->shtex->super_ammo;
	i = -1;
	while (++i < 2)
	{
		ft_snprintf(buf, BUFF_SIZE, TEX_DIR"/super_ammo%c.xpm", i + '0');
		tex[i] = img_to_tex(app, buf);
	}
}

void	load_etank_tex(t_info *app)
{
	t_tex	*tex;

	tex = app->shtex->etank_tex;
	tex[0] = img_to_tex(app, TEX_DIR"/etank0.xpm");
	tex[1] = img_to_tex(app, TEX_DIR"/etank1.xpm");
}

void	load_trophy_tex(t_info *app)
{
	t_tex	*tex;

	tex = app->shtex->trophy_tex;
	tex[0] = img_to_tex(app, TEX_DIR"/trophy0.xpm");
	tex[1] = img_to_tex(app, TEX_DIR"/trophy1.xpm");
}
