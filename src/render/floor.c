/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   floor.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 18:17:23 by fsmyth            #+#    #+#             */
/*   Updated: 2025/04/11 18:17:49 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/cub3d.h"

int	get_tex_index(double pos, int dim)
{
	int	whole;

	whole = pos * dim;
	return ((whole % dim + dim) % dim);
}

void	draw_floor_row(t_info *app, t_vect l_pos, t_vect r_pos, int row)
{
	double	step_x;
	double	step_y;
	double	x;
	double	y;
	int		i;

	step_x = (r_pos.x - l_pos.x) / WIN_WIDTH;
	step_y = (r_pos.y - l_pos.y) / WIN_WIDTH;
	i = 0;
	x = l_pos.x;
	y = l_pos.y;
	while (i < WIN_WIDTH)
	{
		my_put_pixel(&app->canvas, i, row, app->map->floor_tex.img
			   [get_tex_index(y, app->map->floor_tex.y)][get_tex_index(x, app->map->floor_tex.x)]);
		x += step_x;
		y += step_y;
		i++;
	}
}

void	fill_floor(t_info *app, t_data *map, t_player *player)
{
	t_vect	l_dir;
	t_vect	r_dir;
	t_vect	l_pos;
	t_vect	r_pos;
	double	distance;
	int		row;

	row = 1;
	l_dir = rotate_vect(player->dir, M_PI_4);
	r_dir = rotate_vect(player->dir, -M_PI_4);
	while (row < WIN_HEIGHT / 2)
	{
		distance = WIN_HEIGHT / (3.2 * row);
		l_pos = add_vect(player->pos, scale_vect(l_dir, distance * M_SQRT2));
		r_pos = add_vect(player->pos, scale_vect(r_dir, distance * M_SQRT2));
		// printf("row: %d l_pos: (%f, %f) r_pos: (%f, %f)\n", row, l_pos.x, l_pos.y, r_pos.x, r_pos.y);
		draw_floor_row(app, l_pos, r_pos, row + (WIN_HEIGHT / 2));
		row++;
	}
	(void)map;
}
