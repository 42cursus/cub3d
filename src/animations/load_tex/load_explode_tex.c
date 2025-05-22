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
	t_texarr	*tex;

	i = -1;
	tex = app->shtex->explode_tex;
	while (++i < 6)
	{
		ft_snprintf(buf, 50, "./textures/explode%c.xpm", i + '0');
		tex[i].img = img_to_arr(buf, app, &tex[i].x, &tex[i].y);
	}
	i = -1;
	while (++i < 6)
	{
		ft_snprintf(buf, 50, "./textures/explode_mid%c.xpm", i + '0');
		tex[i + 6].img = img_to_arr(buf, app, &tex[i + 6].x, &tex[i + 6].y);
	}
	i = -1;
	while (++i < 5)
	{
		ft_snprintf(buf, 50, "./textures/explode_miss_big%c.xpm", i + '0');
		tex[i + 12].img = img_to_arr(buf, app, &tex[i + 12].x, &tex[i + 12].y);
	}
}
