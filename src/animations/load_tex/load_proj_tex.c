/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   load_proj_tex.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/21 15:13:51 by fsmyth            #+#    #+#             */
/*   Updated: 2025/05/21 15:15:28 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	load_proj_green_tex(t_info *app)
{
	int			i;
	char		buf[50];
	t_texarr	*tex;

	tex = app->shtex->proj_green_tex;
	i = 0;
	while (i < 4)
	{
		ft_snprintf(buf, 50, "./textures/proj_green%c.xpm", i + '0');
		tex[i].img = img_to_arr(buf, app, &tex[i].x, &tex[i].y);
		i++;
	}
}

void	load_proj_tex(t_info *app)
{
	int			i;
	char		buf[50];
	t_texarr	*tex;

	tex = app->shtex->proj_tex;
	i = 1;
	while (i < 5)
	{
		ft_snprintf(buf, 50, "./textures/proj%c.xpm", i - 1 + '0');
		tex[i].img = img_to_arr(buf, app, &tex[i].x, &tex[i].y);
		i++;
	}
	tex[0].img = img_to_arr((char *)"./textures/projectile2.xpm",
			app, &tex[0].x, &tex[0].y);
	tex = app->shtex->proj_tex;
	i = 5;
	while (i < 10)
	{
		ft_snprintf(buf, 50, "./textures/proj_super%c.xpm", i - 5 + '0');
		tex[i].img = img_to_arr(buf, app, &tex[i].x, &tex[i].y);
		i++;
	}
}

void	load_phantoon_proj_tex(t_info *app)
{
	int			i;
	char		buf[50];
	t_texarr	*tex;

	tex = app->shtex->phantoon_proj;
	i = 0;
	while (i < 6)
	{
		ft_snprintf(buf, 50, "./textures/phantoon_eye%c.xpm", i + '0');
		tex[i].img = img_to_arr(buf, app, &tex[i].x, &tex[i].y);
		i++;
	}
}

void	load_missile_textures(t_info *app)
{
	int			i;
	char		buf[50];
	t_texarr	*tex;

	tex = app->shtex->missile_tex;
	i = 0;
	while (i < 8)
	{
		ft_snprintf(buf, 50, "./textures/missile%c.xpm", i + '0');
		tex[i + 4].img = img_to_arr(buf, app, &tex[i + 4].x, &tex[i + 4].y);
		i++;
	}
	tex[0].img = img_to_arr((char *)"./textures/missile_pu0.xpm",
			app, &tex[0].x, &tex[0].y);
	tex[1].img = img_to_arr((char *)"./textures/missile_pu1.xpm",
			app, &tex[1].x, &tex[1].y);
	tex[2].img = img_to_arr((char *)"./textures/missile_off.xpm",
			app, &tex[2].x, &tex[2].y);
	tex[3].img = img_to_arr((char *)"./textures/missile_on.xpm",
			app, &tex[3].x, &tex[3].y);
}

void	load_super_textures(t_info *app)
{
	int			i;
	char		buf[50];
	t_texarr	*tex;

	i = 0;
	tex = app->shtex->super_tex;
	while (i < 8)
	{
		ft_snprintf(buf, 50, "./textures/super%c.xpm", i + '0');
		tex[i + 4].img = img_to_arr(buf, app, &tex[i + 4].x, &tex[i + 4].y);
		i++;
	}
	tex[0].img = img_to_arr((char *)"./textures/super_missile_pu1.xpm",
			app, &tex[0].x, &tex[0].y);
	tex[1].img = img_to_arr((char *)"./textures/super_missile_pu0.xpm",
			app, &tex[1].x, &tex[1].y);
	tex[2].img = img_to_arr((char *)"./textures/super_missile_off.xpm",
			app, &tex[2].x, &tex[2].y);
	tex[3].img = img_to_arr((char *)"./textures/super_missile_on.xpm",
			app, &tex[3].x, &tex[3].y);
}
