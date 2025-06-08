/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   load_explode_tex.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/21 15:23:01 by fsmyth            #+#    #+#             */
/*   Updated: 2025/05/21 15:23:12 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	load_explode_tex(t_info *app)
{
	int			i;
	char		buf[50];
	t_texture	*tex;

	tex = app->shtex->explode_tex;
	i = -1;
	while (++i < 6)
	{
		ft_snprintf(buf, 50, "./resources/textures/explode%c.xpm", i + '0');
		tex[i].data = img_to_tex(app, buf, &tex[i].w, &tex[i].h);
	}
	i = -1;
	while (++i < 6)
	{
		ft_snprintf(buf, 50, "./resources/textures/explode_mid%c.xpm", i + '0');
		tex[i + 6].data = img_to_tex(app, buf, &tex[i + 6].w,
									 &tex[i + 6].h);
	}
	i = -1;
	while (++i < 5)
	{
		ft_snprintf(buf, 50, "./resources/textures/explode_miss_big%c.xpm", i + '0');
		tex[i + 12].data = img_to_tex(app, buf, &tex[i + 12].w,
									  &tex[i + 12].h);
	}
}
