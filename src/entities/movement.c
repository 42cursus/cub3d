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

static inline __attribute__((always_inline))
int	point_oob(t_vect pos, t_lvl *lvl)
{
	return ((pos.x < 0 || pos.x > lvl->width)
		|| (pos.y < 0 || pos.y > lvl->height));
}

static inline __attribute__((always_inline, unused))
void	update_x_y(t_lvl *lvl, t_chvec3 tiles, t_vect new_pos, t_vect *pos)
{
	if (check_tile_open(tiles.x, lvl))
		pos->x = new_pos.x;
	if (check_tile_open(tiles.y, lvl))
		pos->y = new_pos.y;
}

int	check_tile_open(char tile, t_lvl *lvl)
{
	if (tile == 'O' || tile == '0')
		return (1);
	else if (tile == 'B' && lvl->boss_active == 0)
		return (1);
	return (0);
}

void	move_entity(t_vect *pos, t_lvl *lvl, t_vect dir)
{
	t_vect		new_pos;
	t_chvec3	tiles;
	char		**map;

	map = lvl->map;
	new_pos = add_vect(*pos, dir);
	if (point_oob(new_pos, lvl))
		return ;
	tiles.x = map[(int)pos->y][(int)new_pos.x];
	tiles.y = map[(int)new_pos.y][(int)pos->x];
	tiles.z = map[(int)new_pos.y][(int)new_pos.x];
	if (check_tile_open(tiles.z, lvl))
		update_x_y(lvl, tiles, new_pos, pos);
	else
	{
		if (check_tile_open(tiles.x, lvl)
			&& check_tile_open(tiles.y, lvl))
		{
			if (get_max_direction(dir) == 'x')
				pos->x = new_pos.x;
			else
				pos->y = new_pos.y;
		}
		else
			update_x_y(lvl, tiles, new_pos, pos);
	}
}

void	move_obj_bounce(t_info *app, t_obj *obj, t_lvl *data)
{
	t_vect	new_pos;
	char	**map;
	char	tile;

	new_pos = add_vect(obj->pos,
			scale_vect(obj->dir, obj->speed / app->fr_scale));
	map = data->map;
	tile = map[(int)new_pos.y][(int)new_pos.x];
	if (point_oob(new_pos, app->lvl) || !check_tile_open(tile, app->lvl))
		rotate_vect_inplace(&obj->dir, rand_range(-M_PI, M_PI));
	else
		obj->pos = new_pos;
}
