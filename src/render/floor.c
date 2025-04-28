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

#include "cub3d.h"

int	get_tex_index(double pos, int dim)
{
	int	whole;

	whole = pos * dim;
	return ((whole % dim + dim) % dim);
}

void	draw_floor_row(t_info *app, t_vect l_pos, t_vect r_pos, int row)
{
	int				i;
	t_vect			step;
	t_vect			curr;
	t_ivect			idx;
	const t_texarr	*tex = &app->map->floor_tex;

	step.x = (r_pos.x - l_pos.x) / WIN_WIDTH;
	step.y = (r_pos.y - l_pos.y) / WIN_WIDTH;
	curr.x = l_pos.x;
	curr.y = l_pos.y;
	i = -1;
	while (++i < WIN_WIDTH)
	{
		idx.y = get_tex_index(curr.y, tex->y);
		idx.x = get_tex_index(curr.x, tex->x);
		my_put_pixel_32(app->canvas, i, row, tex->img[idx.y][idx.x]);
		curr.x += step.x;
		curr.y += step.y;
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

	row = 0;
	l_dir = rotate_vect(player->dir, M_PI_4);
	r_dir = rotate_vect(player->dir, -M_PI_4);
	while (++row < WIN_HEIGHT / 2)
	{
		distance = WIN_WIDTH / (4.0 * row);
		// distance = WIN_HEIGHT / (2.0 * row);
		l_pos = add_vect(player->pos, scale_vect(l_dir, distance * M_SQRT2));
		r_pos = add_vect(player->pos, scale_vect(r_dir, distance * M_SQRT2));
		// printf("row: %d l_pos: (%f, %f) r_pos: (%f, %f)\n", row, l_pos.x, l_pos.y, r_pos.x, r_pos.y);
		draw_floor_row(app, l_pos, r_pos, row + (WIN_HEIGHT / 2));
	}
	(void)map;
}
