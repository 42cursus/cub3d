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

#define BUFF_SIZE 50

void	load_door_tex(t_info *app)
{
	int		i;
	char	buf[BUFF_SIZE];
	t_tex	*tex;

	tex = app->shtex->door_tex;


	tex[0].data = img_to_tex(app, (char *) TEX_DIR"/metroid_door3.xpm", &tex[0].w, &tex[0].h);
	tex[1].data = img_to_tex(app, (char *) TEX_DIR"/metroid_door_open.xpm", &tex[1].w, &tex[1].h);
	i = 1;
	while (++i < 7)
	{
		ft_snprintf(buf, BUFF_SIZE, TEX_DIR"/metroid_door_anim%c.xpm", i - 2 + '1');
		tex[i].data = img_to_tex(app, buf, &tex[i].w, &tex[i].h);
	}

	tex = &app->shtex->textures[tex_DOOR];
	load_tex(app, tex, (char *)TEX_DIR"/metroid_door3.xpm");
	load_tex(app, tex, (char *)TEX_DIR"/metroid_door_open.xpm");
	i = 1;
	while (++i < 7)
	{
		ft_snprintf(buf, BUFF_SIZE, TEX_DIR"/metroid_door_anim%c.xpm", i - 2 + '1');
		load_tex(app, tex, (char *)TEX_DIR"/metroid_door3.xpm");
	}
}

void	load_boss_door_tex(t_info *app)
{
	int		i;
	char	buf[BUFF_SIZE];
	t_tex	*tex;

	tex = app->shtex->door_boss_tex;
	i = 0;
	while (i < 5)
	{
		ft_snprintf(buf, BUFF_SIZE, TEX_DIR"/metroid_door_boss_anim%c.xpm", i + 1 + '0');
		tex[i + 2].data = img_to_tex(app, buf, &tex[i + 2].w, &tex[i + 2].h);
		i++;
	}
	tex[0].data = img_to_tex(app, (char *) TEX_DIR"/metroid_door_boss.xpm", &tex[0].w, &tex[0].h);
	tex[1].data = img_to_tex(app, (char *) TEX_DIR"/metroid_door_open.xpm", &tex[1].w, &tex[1].h);
}

void	load_super_door_tex(t_info *app)
{
	int		i;
	char	buf[BUFF_SIZE];
	t_tex	*tex;

	tex = app->shtex->door_super_tex;
	i = -1;
	while (++i < 5)
	{
		ft_snprintf(buf, BUFF_SIZE, TEX_DIR"/metroid_door_super_anim%c.xpm", i + '1');
		tex[i + 2].data = img_to_tex(app, buf, &tex[i + 2].w, &tex[i + 2].h);
	}
	tex[0].data = img_to_tex(app, (char *) TEX_DIR"/metroid_door_super.xpm", &tex[0].w, &tex[0].h);
	tex[1].data = img_to_tex(app, (char *) TEX_DIR"/metroid_door_open.xpm", &tex[1].w, &tex[1].h);
}

void	load_missile_door_tex(t_info *app)
{
	int		i;
	char	buf[BUFF_SIZE];
	t_tex	*tex;

	tex = app->shtex->door_missile_tex;
	i = -1;
	while (++i < 5)
	{
		ft_snprintf(buf, BUFF_SIZE, TEX_DIR"/metroid_door_missile_anim%c.xpm", i + 1 + '0');
		tex[i + 2].data = img_to_tex(app, buf, &tex[i + 2].w, &tex[i + 2].h);
	}
	tex[0].data = img_to_tex(
		app, (char *) TEX_DIR"/metroid_door_missile.xpm", &tex[0].w,
		&tex[0].h);
	tex[1].data = img_to_tex(
		app, (char *) TEX_DIR"/metroid_door_open.xpm", &tex[1].w, &tex[1].h);
}
