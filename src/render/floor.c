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

/**
 * This function maps a continuous coordinate (like pos = 0.5)
 * to an integer pixel index in a texture of size dim.
 *
 * The modulo trick ensures the result is always between 0 and dim - 1,
 * even if whole is negative
 *
 * @param pos
 * @param dim
 * @return
 */
static inline __attribute__((always_inline))
int	get_tex_index(double pos, int dim)
{
	// int	whole;
	//
	// whole = pos * dim;
	// return ((whole % dim + dim) % dim);
	return ((int)(pos * dim) % dim);
}

/**
 * Checks if the position pos is outside the boundaries of the map
 * oob stands for "out of bounds"
 * @param pos
 * @param map
 * @return
 */
static inline __attribute__((always_inline, unused))
int	point_oob(t_vect pos, t_data *map)
{
	return ((pos.x < 0 || pos.x > map->width) || (pos.y < 0 || pos.y > map->height));
}

static inline __attribute__((always_inline))
void	draw_floor_row(t_info *app, t_vect l_pos, t_vect r_pos, u_int (*const dst))
{
	int				i;
	t_vect			step;
	t_vect			curr;
	t_ivect			idx;
	const t_img		*tex = app->map->texs[T_FLOOR];

	step.x = (r_pos.x - l_pos.x) / WIN_WIDTH * 2;
	step.y = (r_pos.y - l_pos.y) / WIN_WIDTH * 2;
	curr.x = l_pos.x;
	curr.y = l_pos.y;

	u_int *src = (u_int *)tex->data;
	i = 0;
	while (i < WIN_WIDTH - 1)
	{
		if (!point_oob(curr, app->map))
		{
			idx.x = get_tex_index(curr.x, tex->width);
			idx.y = get_tex_index(curr.y, tex->height);
			dst[i] = src[idx.y * tex->width + idx.x];
			dst[i + 1] = src[idx.y * tex->width + idx.x];
		}
		else
			dst[i] = MLX_TRANSPARENT;
		curr.x += step.x;
		curr.y += step.y;
		i += 2;
	}
}

/**
 * floor-drawing logic using nearest-neighbor sampling
 *
 * can be improved with bilinear interpolation
 * @param app
 * @param map
 * @param player
 */
void	fill_floor(t_info *app, t_data *map, t_player *player)
{
	t_vect	l_dir;
	t_vect	r_dir;
	t_vect	l_pos;
	t_vect	r_pos;
	u_int	*dst;
	int		row;

	row = 0;
	l_dir = rotate_vect(player->dir, app->fov_rad_half);
	r_dir = rotate_vect(player->dir, -app->fov_rad_half);
	while (++row < WIN_HEIGHT / 2)
	{
		double depth = player->floor_offsets[row - 1];
		l_pos = add_vect(player->pos, scale_vect(l_dir, depth));
		r_pos = add_vect(player->pos, scale_vect(r_dir, depth));
		dst = (u_int *)app->canvas->data + (row + (WIN_HEIGHT / 2)) * app->canvas->width;
		draw_floor_row(app, l_pos, r_pos, dst);
	}
	(void)map;
}
