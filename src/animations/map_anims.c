/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   map_anims.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/21 13:00:42 by fsmyth            #+#    #+#             */
/*   Updated: 2025/05/21 13:01:21 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	init_anims(t_info *app, t_data *map)
{
	int		i;
	int		j;
	char	tile;

	i = 0;
	while (i < map->height)
	{
		j = 0;
		while (j < map->width)
		{
			tile = map->map[i][j];
			if (tile == 'D')
				map->anims[i][j].tex_arr = app->shtex->door_tex;
			else if (tile == 'L')
				map->anims[i][j].tex_arr = app->shtex->door_super_tex;
			else if (tile == 'M')
				map->anims[i][j].tex_arr = app->shtex->door_missile_tex;
			else if (tile == 'B')
				map->anims[i][j].tex_arr = app->shtex->door_boss_tex;
			j++;
		}
		i++;
	}
}

void	reset_anims(t_info *app, t_data *map)
{
	int	i;

	i = -1;
	while (++i < map->height)
		ft_bzero(map->anims[i], map->width * sizeof(t_anim));
	init_anims(app, map);
}
