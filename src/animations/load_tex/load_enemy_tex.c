/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   load_enemy_tex.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/21 15:11:40 by fsmyth            #+#    #+#             */
/*   Updated: 2025/05/21 15:13:08 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	load_atomic_tex(t_info *app)
{
	int			i;
	char		buf[50];
	t_texarr	*tex;

	i = 0;
	tex = app->shtex->atomic_tex;
	while (i < 6)
	{
		ft_snprintf(buf, 50, "./textures/atomic%c.xpm", i + '0');
		tex[i].img = img_to_arr(buf, app, &tex[i].x, &tex[i].y);
		i++;
	}
}

void	load_holtz_tex(t_info *app)
{
	int			i;
	char		buf[50];
	t_texarr	*tex;

	i = 0;
	tex = app->shtex->holtz_tex;
	while (i < 4)
	{
		ft_snprintf(buf, 50, "./textures/holtz%c.xpm", i + '0');
		tex[i].img = img_to_arr(buf, app, &tex[i].x, &tex[i].y);
		i++;
	}
	tex[4].img = img_to_arr((char *)"./textures/holtz2.xpm",
			app, &tex[4].x, &tex[4].y);
	tex[5].img = img_to_arr((char *)"./textures/holtz1.xpm",
			app, &tex[5].x, &tex[5].y);
}

void	load_reo_tex(t_info *app)
{
	int			i;
	char		buf[50];
	t_texarr	*tex;

	i = 0;
	tex = app->shtex->reo_tex;
	while (i < 2)
	{
		ft_snprintf(buf, 50, "./textures/reo_%c.xpm", i + '0');
		tex[i].img = img_to_arr(buf, app, &tex[i].x, &tex[i].y);
		i++;
	}
	while (i < 4)
	{
		ft_snprintf(buf, 50, "./textures/reo_attack_%c.xpm", i - 2 + '0');
		tex[i].img = img_to_arr(buf, app, &tex[i].x, &tex[i].y);
		i++;
	}
}

void	load_phantoon_tex(t_info *app)
{
	int			i;
	char		buf[50];
	t_texarr	*tex;
	int			ids[10];

	ft_memset(ids, 0, 10 * sizeof(int));
	ids[1] = 1;
	ids[2] = 2;
	ids[3] = 1;
	ids[6] = 3;
	ids[7] = 4;
	ids[8] = 3;
	i = 0;
	tex = app->shtex->phantoon;
	while (i < 10)
	{
		ft_snprintf(buf, 50, "./textures/phantoon%c.xpm", ids[i] + '0');
		tex[i].img = img_to_arr(buf, app, &tex[i].x, &tex[i].y);
		i++;
	}
}

void	load_zoomer_tex(t_info *app)
{
	int			i;
	char		buf[50];
	t_texarr	*tex;

	tex = app->shtex->crawler_tex;
	i = 0;
	while (i < 6)
	{
		ft_snprintf(buf, 50, "./textures/crawler%c.xpm", i + '1');
		tex[i].img = img_to_arr(buf, app, &tex[i].x, &tex[i].y);
		i++;
	}
}
