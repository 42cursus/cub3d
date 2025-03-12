/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   player.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/12 19:31:02 by fsmyth            #+#    #+#             */
/*   Updated: 2025/03/12 19:57:42 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/cub3d.h"

t_player	*init_player(t_map *map)
{
	t_player	*player;

	player = ft_calloc(1, sizeof(*player));
	player->pos = map->starting_pos;
	if (map->starting_dir == 'N')
	{
		player->direction.x = 0;
		player->direction.y = 1;
	}
	else if (map->starting_dir == 'S')
	{
		player->direction.x = 0;
		player->direction.y = -1;
	}
	else if (map->starting_dir == 'E')
	{
		player->direction.x = 1;
		player->direction.y = 0;
	}
	else if (map->starting_dir == 'W')
	{
		player->direction.x = -1;
		player->direction.y = 0;
	}
	return (player);
}
