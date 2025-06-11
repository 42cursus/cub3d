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
	char	buf[50];
	t_tex	*tex;

	tex = app->shtex->atomic_tex;
	i = -1;
	while (++i < 6)
	{
		ft_snprintf(buf, 50, TEX_DIR"/atomic%c.xpm", i + '0');
		tex[i].data = img_to_tex(app, buf, &tex[i].w, &tex[i].h);
	}
}

void	load_holtz_tex(t_info *app)
{
	int		i;
	char	buf[50];
	t_tex	*tex;

	tex = app->shtex->holtz_tex;
	i = -1;
	while (++i < 4)
	{
		ft_snprintf(buf, 50, TEX_DIR"/holtz%c.xpm", i + '0');
		tex[i].data = img_to_tex(app, buf, &tex[i].w, &tex[i].h);
	}
	tex[4].data = img_to_tex(app, (char *) TEX_DIR"/holtz2.xpm", &tex[4].w, &tex[4].h);
	tex[5].data = img_to_tex(app, (char *) TEX_DIR"/holtz1.xpm", &tex[5].w, &tex[5].h);
}

void	load_reo_tex(t_info *app)
{
	int		i;
	char	buf[50];
	t_tex	*tex;

	i = 0;
	tex = app->shtex->reo_tex;
	while (i < 2)
	{
		ft_snprintf(buf, 50, TEX_DIR"/reo_%c.xpm", i + '0');
		tex[i].data = img_to_tex(app, buf, &tex[i].w, &tex[i].h);
		i++;
	}
	while (i < 4)
	{
		ft_snprintf(buf, 50, TEX_DIR"/reo_attack_%c.xpm", i - 2 + '0');
		tex[i].data = img_to_tex(app, buf, &tex[i].w, &tex[i].h);
		i++;
	}
}

void	load_phantoon_tex(t_info *app)
{
	int		i;
	char	buf[50];
	t_tex	*tex;
	int		ids[10];

	ft_memset(ids, 0, 10 * sizeof(int));
	ids[1] = 1;
	ids[2] = 2;
	ids[3] = 1;
	ids[6] = 3;
	ids[7] = 4;
	ids[8] = 3;
	tex = app->shtex->phantoon;
	i = -1;
	while (++i < 10)
	{
		ft_snprintf(buf, 50, TEX_DIR"/phantoon%c.xpm", ids[i] + '0');
		tex[i].data = img_to_tex(app, buf, &tex[i].w, &tex[i].h);
	}
}

void	load_zoomer_tex(t_info *app)
{
	int		i;
	char	buf[50];
	t_tex	*tex;

	tex = app->shtex->crawler_tex;
	i = -1;
	while (++i < 6)
	{
		ft_snprintf(buf, 50, TEX_DIR"/crawler%c.xpm", i + '1');
		tex[i].data = img_to_tex(app, buf, &tex[i].w, &tex[i].h);
	}
}
