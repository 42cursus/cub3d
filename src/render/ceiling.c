/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ceiling.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/02 18:06:21 by abelov            #+#    #+#             */
/*   Updated: 2025/05/02 18:06:25 by abelov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

static inline __attribute__((always_inline))
void	my_put_pixel_32(t_img *img, int x, int y, unsigned int colour)
{
	// if (colour == MLX_TRANSPARENT)
	// 	return ;
	(*(unsigned int (*)[img->height][img->width])img->data)[y][x] = colour;
}

static inline __attribute__((always_inline))
int	get_tex_index(double pos, int dim)
{
	// int	whole;
	//
	// whole = pos * dim;
	return ((int)(pos * dim) % dim);
}

static inline __attribute__((always_inline))
int	point_oob(t_vect pos, t_data *map)
{
	return ((pos.x < 0 || pos.x > map->width) || (pos.y < 0 || pos.y > map->height));
}

void	draw_ceil_row(t_info *app, t_vect l_pos, t_vect r_pos, int row)
{
	int				i;
	t_vect			step;
	t_vect			curr;
	t_ivect			idx;
	const t_texarr	*tex = &app->map->ceil_tex;

	step.x = (r_pos.x - l_pos.x) / WIN_WIDTH * 2;
	step.y = (r_pos.y - l_pos.y) / WIN_WIDTH * 2;
	curr.x = l_pos.x;
	curr.y = l_pos.y;
	i = 0;
	while (i < WIN_WIDTH - 1)
	{
		if (!point_oob(curr, app->map))
		{
			idx.y = get_tex_index(curr.y, tex->y);
			idx.x = get_tex_index(curr.x, tex->x);
			my_put_pixel_32(app->canvas, i, row, tex->img[idx.y][idx.x]);
			my_put_pixel_32(app->canvas, i + 1, row, tex->img[idx.y][idx.x]);
			// my_put_pixel_32(app->canvas, i, row, dim_colour(tex->img[idx.y][idx.x], app->player->floor_offsets[row - (WIN_HEIGHT / 2) - 1] / 4));
		}
		curr.x += step.x;
		curr.y += step.y;
		i += 2;
	}
}

void	fill_ceiling(t_info *app, t_data *map, t_player *player)
{
	t_vect	l_dir;
	t_vect	r_dir;
	t_vect	l_pos;
	t_vect	r_pos;
	// double	distance;
	// double	scalar;
	int		row;

	l_dir = rotate_vect(player->dir, app->fov_rad_half);
	r_dir = rotate_vect(player->dir, -app->fov_rad_half);
	row = 0;
	// scalar = get_hyp_len(app->fov_opp_len, 1);
	while (++row < WIN_HEIGHT / 2)
	{
		// distance = WIN_WIDTH / (4.0 * row * app->fov_opp_len);
		// l_pos = add_vect(player->pos, scale_vect(l_dir, scalar * distance));
		// r_pos = add_vect(player->pos, scale_vect(r_dir, scalar * distance));
		l_pos = add_vect(player->pos, scale_vect(l_dir, player->ceil_offsets[row - 1]));
		r_pos = add_vect(player->pos, scale_vect(r_dir, player->ceil_offsets[row - 1]));
		// if (!point_oob(l_pos, map) && !point_oob(r_pos, map))
		draw_ceil_row(app, l_pos, r_pos, row);
	}
	(void)map;
}
