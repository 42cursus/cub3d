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
	int	whole;

	whole = pos * dim;
	return ((whole % dim + dim) % dim);
}

static inline __attribute__((always_inline))
int	point_oob(t_vect pos, t_data *map)
{
	return ((pos.x < 0 || pos.x > map->width) || (pos.y < 0 || pos.y > map->height));
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
		if (!point_oob(curr, app->map))
		{
			idx.y = get_tex_index(curr.y, tex->y);
			idx.x = get_tex_index(curr.x, tex->x);
			my_put_pixel_32(app->canvas, i, row, tex->img[idx.y][idx.x]);
			// my_put_pixel_32(app->canvas, i, row, dim_colour(tex->img[idx.y][idx.x], app->player->floor_offsets[row - (WIN_HEIGHT / 2) - 1] / 4));
		}
		curr.x += step.x;
		curr.y += step.y;
	}
}

double	get_hyp_len(double len1, double len2)
{
	return (sqrt(len1 * len1 + len2 * len2));
}

void	fill_floor(t_info *app, t_data *map, t_player *player)
{
	t_vect	l_dir;
	t_vect	r_dir;
	t_vect	l_pos;
	t_vect	r_pos;
	// double	distance;
	// double	scalar;
	int		row;

	row = 0;
	l_dir = rotate_vect(player->dir, app->fov_rad_half);
	r_dir = rotate_vect(player->dir, -app->fov_rad_half);
	// scalar = get_hyp_len(app->fov_opp_len, 1);
	while (++row < WIN_HEIGHT / 2)
	{
		// distance = WIN_WIDTH / (4.0 * row * app->fov_opp_len);
		// l_pos = add_vect(player->pos, scale_vect(l_dir, scalar * distance));
		// r_pos = add_vect(player->pos, scale_vect(r_dir, scalar * distance));
		l_pos = add_vect(player->pos, scale_vect(l_dir, player->floor_offsets[row - 1]));
		r_pos = add_vect(player->pos, scale_vect(r_dir, player->floor_offsets[row - 1]));
		// if (!point_oob(l_pos, map) && !point_oob(r_pos, map))
			draw_floor_row(app, l_pos, r_pos, row + (WIN_HEIGHT / 2));
	}
	(void)map;
}
