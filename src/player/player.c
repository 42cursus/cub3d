/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   player.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/12 19:31:02 by fsmyth            #+#    #+#             */
/*   Updated: 2025/03/25 20:15:33 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	calculate_offsets(t_player *player)
{
	int		i;
	double	offset;
	double	angle;

	offset = 1.0 / (WIN_WIDTH / 2.0);
	i = 0;
	while (i < WIN_WIDTH / 2)
	{
		angle = atan(1.0 - (i * offset));
		player->angle_offsets[i] = angle;
		player->angle_offsets[WIN_WIDTH - i - 1] = -angle;
		i++;
	}
}

t_player	*init_player(t_data *map)
{
	t_player	*player;

	player = ft_calloc(1, sizeof(*player));
	player->pos = map->starting_pos;
	if (map->starting_dir == 'N' || map->starting_dir == 'S')
	{
		player->direction.x = 0;
		if (map->starting_dir == 'N')
			player->direction.y = 1;
		else if (map->starting_dir == 'N')
			player->direction.y = -1;
	}
	else if (map->starting_dir == 'W' || map->starting_dir == 'E')
	{
		player->direction.y = 0;
		if (map->starting_dir == 'W')
			player->direction.x = -1;
		else if (map->starting_dir == 'E')
			player->direction.x = 1;
	}
	calculate_offsets(player);
	return (player);
}

void	move_player(t_player *player, char **map, t_vect dir)
{
	t_vect	new_pos;
	t_cvect	tiles;
	char	both_tile;

	new_pos.x = player->pos.x + (dir.x * 0.1);
	new_pos.y = player->pos.y + (dir.y * 0.1);
	tiles.x = map[(int)player->pos.y][(int)new_pos.x];
	tiles.y = map[(int)new_pos.y][(int)player->pos.x];
	both_tile = map[(int)new_pos.y][(int)new_pos.x];
	if (both_tile == '0')
	{
		if (tiles.x == '0')
			player->pos.x = new_pos.x;
		if (tiles.y == '0')
			player->pos.y = new_pos.y;
	}
	else
	{
		if (tiles.x == '0' && tiles.y == '0')
			get_max_direction(dir, &player->pos, &new_pos);
		else if (tiles.x == '0')
			player->pos.x = new_pos.x;
		else if (tiles.y == '0')
			player->pos.y = new_pos.y;
	}
}

void	rotate_player(t_player *player, int direction)
{
	if (direction == 0)
		rotate_vect_inplace(&player->direction, M_PI_4 / 8);
	else
		rotate_vect_inplace(&player->direction, -M_PI_4 / 8);
}
