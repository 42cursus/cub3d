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
static inline __attribute__((always_inline, unused))
int	get_tex_index_float(double pos, int dim)
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
void	draw_floor_row(t_vect pos[2], u_int (*const dst), t_img *tex)
{
	int				i;
	t_vect			step;
	t_vect			curr;
	t_ivect			idx;
	u_int			*src = (u_int *)tex->data;

	int width = tex->width;
	int height = tex->height;

	curr = (t_vect)(pos[LEFT]);
	step.x = (pos[RIGHT].x - pos[LEFT].x) / WIN_WIDTH * 2;
	step.y = (pos[RIGHT].y - pos[LEFT].y) / WIN_WIDTH * 2;

	i = 0;
	while (i < WIN_WIDTH - 1)
	{
		// if (!point_oob(curr, app->map))
		// {
			idx.x = ((int)(curr.x * width)) & (width - 1);
			idx.y = ((int)(curr.y * height)) & (height - 1);
			dst[i] = src[idx.y * width + idx.x];
			dst[i + 1] = src[idx.y * width + idx.x];
		// }
		// else
		// 	dst[i] = MLX_TRANSPARENT;
		curr = (t_vect){.x = curr.x + step.x, curr.y + step.y};
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
void	fill_floor(t_info *app, t_player *player, int is_floor)
{
	t_vect	dir[2];
	t_vect	pos[2];
	u_int	*dst;
	int		row;
	double	depth;
	t_img *const tex = (t_img *[]){app->map->texs[T_CEILING], app->map->texs[T_FLOOR]}[is_floor];

	row = -1;
	dir[LEFT] = rotate_vect(player->dir, app->fov_rad_half);
	dir[RIGHT] = rotate_vect(player->dir, -app->fov_rad_half);
	while (++row < WIN_HEIGHT / 2)
	{
		depth = (double[]){player->offsets[WIN_HEIGHT / 2 - row - 1], player->offsets[row]}[is_floor];
		pos[LEFT] = add_vect(player->pos, scale_vect(dir[LEFT], depth));
		pos[RIGHT] = add_vect(player->pos, scale_vect(dir[RIGHT], depth));
		dst = (u_int *)app->canvas->data + (row + (WIN_HEIGHT / 2) * is_floor) * app->canvas->width;
		draw_floor_row(pos, dst, tex);
	}
}
