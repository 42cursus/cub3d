/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   movement.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/19 15:28:58 by fsmyth            #+#    #+#             */
/*   Updated: 2025/05/19 16:05:04 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

static inline __attribute__((always_inline, unused))
int	point_oob(t_vect pos, t_data *map)
{
	return ((pos.x < 0 || pos.x > map->width) || (pos.y < 0 || pos.y > map->height));
}

void	move_entity(t_vect *pos, t_data *data, t_vect dir)
{
	t_vect	new_pos;
	t_cvect	tile;
	char	**map;
	char	both_tile;

	map = data->map;
	new_pos.x = pos->x + dir.x;
	new_pos.y = pos->y + dir.y;
	if (point_oob(new_pos, data))
		return ;
	tile = (t_cvect){map[(int)pos->y][(int)new_pos.x], map[(int)new_pos.y][(int)pos->x]};
	both_tile = map[(int)new_pos.y][(int)new_pos.x];
	if (check_tile_open(both_tile, data))
	{
		if (check_tile_open(tile.x, data))
			pos->x = new_pos.x;
		if (check_tile_open(tile.y, data))
			pos->y = new_pos.y;
	}
	else
	{
		if (check_tile_open(tile.x, data) && check_tile_open(tile.y, data))
		{
			if (get_max_direction(dir) == 'x')
				pos->x = new_pos.x;
			else
				pos->y = new_pos.y;
		}
		else if (check_tile_open(tile.x, data))
			pos->x = new_pos.x;
		else if (check_tile_open(tile.y, data))
			pos->y = new_pos.y;
	}
}

void	move_obj_bounce(t_info *app, t_object *obj, t_data *data)
{
	t_vect	new_pos;
	char	**map;
	char	tile;

	new_pos = add_vect(obj->pos, scale_vect(obj->dir, obj->speed / app->fr_scale));
	map = data->map;
	tile = map[(int)new_pos.y][(int)new_pos.x];
	if (point_oob(new_pos, app->map) || !check_tile_open(tile, app->map))
		rotate_vect_inplace(&obj->dir, rand_range(-M_PI, M_PI));
	else
		obj->pos = new_pos;
}
