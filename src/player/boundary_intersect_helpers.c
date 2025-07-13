/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   boundary_intersect_helpers.c                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/13 18:57:23 by fsmyth            #+#    #+#             */
/*   Updated: 2025/07/13 18:57:57 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

t_ray	get_horiz_intersect_south(t_player *player,
			t_data *map, double gradient, double c)
{
	int		i;
	t_ray	out;

	i = ceil(player->pos.y);
	out.face = SOUTH;
	while (i < map->height)
	{
		out.intcpt = get_horizontal_int(i, gradient, c);
		if (out.intcpt.x < map->width && out.intcpt.x >= 0
			&& map->map[(int)out.intcpt.y][(int)out.intcpt.x] == '1')
			break ;
		i += 1.0f;
	}
	return (out);
}

t_ray	get_horiz_intersect_north(t_player *player,
			t_data *map, double gradient, double c)
{
	int		i;
	t_ray	out;

	i = floor(player->pos.y);
	out.face = NORTH;
	while (i > 0)
	{
		out.intcpt = get_horizontal_int(i, gradient, c);
		if (out.intcpt.x < map->width && out.intcpt.x >= 0
			&& map->map[(int)out.intcpt.y - 1][(int)out.intcpt.x] == '1')
			break ;
		i -= 1.0f;
	}
	return (out);
}

t_ray	get_vert_intersect_west(t_player *player,
			t_data *map, double gradient, double c)
{
	int		i;
	t_ray	out;

	i = ceil(player->pos.x);
	out.face = WEST;
	while (i < map->width)
	{
		out.intcpt = get_vertical_int(i, gradient, c);
		if (out.intcpt.y < map->height && out.intcpt.y >= 0
			&& map->map[(int)out.intcpt.y][(int)out.intcpt.x] == '1')
			break ;
		i += 1.0f;
	}
	return (out);
}

t_ray	get_vert_intersect_east(t_player *player,
			t_data *map, double gradient, double c)
{
	int		i;
	t_ray	out;

	i = floor(player->pos.x);
	out.face = EAST;
	while (i > 0)
	{
		out.intcpt = get_vertical_int(i, gradient, c);
		if (out.intcpt.y < map->height && out.intcpt.y >= 0
			&& map->map[(int)out.intcpt.y][(int)out.intcpt.x - 1] == '1')
			break ;
		i -= 1.0f;
	}
	return (out);
}
