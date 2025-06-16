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
	int		i;
	char	buf[BUFF_SIZE];
	t_tex	*tex;

	tex = app->shtex->proj_green_tex;
	i = -1;
	while (++i < 4)
	{
		ft_snprintf(buf, BUFF_SIZE, TEX_DIR"/proj_green%c.xpm", i + '0');
		tex[i] = img_to_tex(app, buf);
	}
}

void	load_proj_tex(t_info *app)
{
	int		i;
	char	buf[BUFF_SIZE];
	t_tex	*tex;

	tex = app->shtex->proj_tex;
	i = 0;
	while (++i < 5)
	{
		ft_snprintf(buf, BUFF_SIZE, TEX_DIR"/proj%c.xpm", i - 1 + '0');
		tex[i] = img_to_tex(app, buf);
	}
	tex[0] = img_to_tex(app, TEX_DIR"/projectile2.xpm");
	tex = app->shtex->proj_tex;
	i = 4;
	while (++i < 10)
	{
		ft_snprintf(buf, BUFF_SIZE, TEX_DIR"/proj_super%c.xpm", i - 5 + '0');
		tex[i] = img_to_tex(app, buf);
	}
}

void	load_phantoon_proj_tex(t_info *app)
{
	int		i;
	char	buf[BUFF_SIZE];
	t_tex	*tex;

	tex = app->shtex->phantoon_proj;
	i = -1;
	while (++i < 6)
	{
		ft_snprintf(buf, BUFF_SIZE, TEX_DIR"/phantoon_eye%c.xpm", i + '0');
		tex[i] = img_to_tex(app, buf);
	}
}

void	load_missile_textures(t_info *app)
{
	int		i;
	char	buf[BUFF_SIZE];
	t_tex	*tex;

	tex = app->shtex->missile_tex;
	i = -1;
	while (++i < 8)
	{
		ft_snprintf(buf, BUFF_SIZE, TEX_DIR"/missile%c.xpm", i + '0');
		tex[i + 4] = img_to_tex(app, buf);
	}
	tex[0] = img_to_tex(app, TEX_DIR"/missile_pu0.xpm");
	tex[1] = img_to_tex(app, TEX_DIR"/missile_pu1.xpm");
	tex[2] = img_to_tex_row_major(app, TEX_DIR"/missile_off.xpm");
	tex[3] = img_to_tex_row_major(app, TEX_DIR"/missile_on.xpm");
}

void	load_super_textures(t_info *app)
{
	int		i;
	char	buf[BUFF_SIZE];
	t_tex	*tex;

	tex = app->shtex->super_tex;
	i = -1;
	while (++i < 8)
	{
		ft_snprintf(buf, BUFF_SIZE, TEX_DIR"/super%c.xpm", i + '0');
		tex[i + 4] = img_to_tex(app, buf);
	}
	tex[0] = img_to_tex(app, TEX_DIR"/super_missile_pu1.xpm");
	tex[1] = img_to_tex(app, TEX_DIR"/super_missile_pu0.xpm");
	tex[2] = img_to_tex_row_major(app, TEX_DIR"/super_missile_off.xpm");
	tex[3] = img_to_tex_row_major(app, TEX_DIR"/super_missile_on.xpm");
}
