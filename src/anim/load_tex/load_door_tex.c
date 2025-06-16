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
	int		i;
	char	buf[BUFF_SIZE];
	t_tex	*tex;

	tex = app->shtex->door_tex;

	tex[0] = img_to_tex(app, TEX_DIR"/metroid_door/metroid_door3.xpm");
	tex[1] = img_to_tex(app, TEX_DIR"/metroid_door/metroid_door_open.xpm");
	i = 1;
	while (++i < 7)
	{
		ft_snprintf(buf, BUFF_SIZE, TEX_DIR"/metroid_door/metroid_door_anim%c.xpm", i + '/');
		tex[i] = img_to_tex(app, buf);
	}
}

void	load_boss_door_tex(t_info *app)
{
	int		i;
	char	buf[BUFF_SIZE];
	t_tex	*tex;

	tex = app->shtex->door_boss_tex;
	tex[0] = img_to_tex(app, TEX_DIR"/metroid_door_boss/metroid_door_boss.xpm");
	tex[1] = img_to_tex(app, TEX_DIR"/metroid_door_boss/metroid_door_open.xpm");
	i = 1;
	while (++i < 7)
	{
		ft_snprintf(buf, BUFF_SIZE, TEX_DIR"/metroid_door_boss/metroid_door_boss_anim%c.xpm", i + '/');
		tex[i] = img_to_tex(app, buf);
	}
}

void	load_super_door_tex(t_info *app)
{
	int		i;
	char	buf[BUFF_SIZE];
	t_tex	*tex;

	tex = app->shtex->door_super_tex;
	tex[0] = img_to_tex(app, TEX_DIR"/metroid_door_super/metroid_door_super.xpm");
	tex[1] = img_to_tex(app, TEX_DIR"/metroid_door_super/metroid_door_open.xpm");
	i = 1;
	while (++i < 7)
	{
		ft_snprintf(buf, BUFF_SIZE, TEX_DIR"/metroid_door_super/metroid_door_super_anim%c.xpm", i + '/');
		tex[i] = img_to_tex(app, buf);
	}
}

void	load_missile_door_tex(t_info *app)
{
	int		i;
	char	buf[BUFF_SIZE];
	t_tex	*tex;

	tex = app->shtex->door_missile_tex;
	tex[0] = img_to_tex(app, TEX_DIR"/metroid_door_missile/metroid_door_missile.xpm");
	tex[1] = img_to_tex(app, TEX_DIR"/metroid_door_missile/metroid_door_open.xpm");
	i = 1;
	while (++i < 7)
	{
		ft_snprintf(buf, BUFF_SIZE, TEX_DIR"/metroid_door_missile/metroid_door_missile_anim%c.xpm", i + '/');
		tex[i] = img_to_tex(app, buf);
	}
}
