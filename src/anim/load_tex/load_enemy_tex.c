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
	int		i;
	char	buf[BUFF_SIZE];
	t_tex	*tex;

	tex = app->shtex->atomic_tex;
	i = -1;
	while (++i < 6)
	{
		ft_snprintf(buf, BUFF_SIZE, TEX_DIR"/atomic/atomic%c.xpm", i + '0');
		tex[i] = img_to_tex(app, buf);
	}
}

void	load_holtz_tex(t_info *app)
{
	int		i;
	char	buf[BUFF_SIZE];
	t_tex	*tex;

	tex = app->shtex->holtz_tex;
	i = -1;
	while (++i < 4)
	{
		ft_snprintf(buf, BUFF_SIZE, TEX_DIR"/holtz/holtz%c.xpm", i + '0');
		tex[i] = img_to_tex(app, buf);
	}
	tex[4] = img_to_tex(app, TEX_DIR"/holtz/holtz2.xpm");
	tex[5] = img_to_tex(app, TEX_DIR"/holtz/holtz1.xpm");
}

void	load_reo_tex(t_info *app)
{
	int		i;
	char	buf[BUFF_SIZE];
	t_tex	*tex;

	tex = app->shtex->reo_tex;
	i = -1;
	while (++i < 2)
	{
		ft_snprintf(buf, BUFF_SIZE, TEX_DIR"/reo/reo_%c.xpm", i + '0');
		tex[i] = img_to_tex(app, buf);
	}
	while (i < 4)
	{
		ft_snprintf(buf, BUFF_SIZE, TEX_DIR"/reo/reo_attack_%c.xpm", i - 2 + '0');
		tex[i] = img_to_tex(app, buf);
		i++;
	}
}

void	load_phantoon_tex(t_info *app)
{
	int			i;
	char		buf[BUFF_SIZE];
	t_tex		*tex;
	const int	ids[10] = {1, 2, 1, 3, 4, 3};

	tex = app->shtex->phantoon;
	i = -1;
	while (++i < 10)
	{
		ft_snprintf(buf, BUFF_SIZE, TEX_DIR"/phantoon/phantoon%c.xpm", ids[i] + '0');
		tex[i] = img_to_tex(app, buf);
	}
}

void	load_zoomer_tex(t_info *app)
{
	int		i;
	char	buf[BUFF_SIZE];
	t_tex	*tex;

	tex = app->shtex->crawler_tex;
	i = -1;
	while (++i < 6)
	{
		ft_snprintf(buf, BUFF_SIZE, TEX_DIR"/crawler/crawler%c.xpm", i + '1');
		tex[i] = img_to_tex(app, buf);
	}
}
