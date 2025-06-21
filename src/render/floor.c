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
 * @param lvl
 * @return
 */
static inline __attribute__((always_inline, unused))
int	point_oob(t_vect pos, t_lvl *lvl)
{
	return ((pos.x < 0 || pos.x > lvl->width)
		|| (pos.y < 0 || pos.y > lvl->height));
}

static inline __attribute__((always_inline, unused))
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
		idx.x = ((int)(curr.x * width)) & (width - 1);
		idx.y = ((int)(curr.y * height)) & (height - 1);

		dst[i] = src[idx.y * width + idx.x];
		dst[i + 1] = src[idx.y * width + idx.x];
		curr = (t_vect){.x = curr.x + step.x, curr.y + step.y};
		i += 2;
	}
}

/**
 * floor-drawing logic using planar projection and nearest-neighbor sampling
 *
 * can be improved with bi-linear interpolation
 *
 * `& (tex.width - 1)` is faster then `% tex.width`
 * @param app
 * @param map
 * @param player
 */
void	fill_floor(t_info *app, t_player *player, int is_floor)
{
	t_vect	dir[2];
	t_vect	pos[2];

	t_ivect iter;

	t_cdata row;
	double	depth;
	t_img	tex;

	bool is_ceil = !is_floor;

	dir[LEFT] = rotate_vect(player->dir, app->fov_rad_half);
	dir[RIGHT] = rotate_vect(player->dir, -app->fov_rad_half);

	int	y;
	int y_start;
	int depths_idx_start;
	int depths_step;
	t_vect	step;
	t_vect	curr;
	t_ivect	idx;

	if (is_ceil)
	{
		tex = *app->lvl->planes[T_CEILING];
		y_start = 0;
		depths_idx_start = 0;
		depths_step = 1;
	}
	else
	{
		tex = *app->lvl->planes[T_FLOOR];
		y_start = WIN_HEIGHT / 2;
		depths_idx_start = WIN_HEIGHT / 2 - 1;
		depths_step = -1;
	}

	iter.y = -1;
	while (++iter.y < WIN_HEIGHT / 2)
	{
		y = y_start + iter.y;
		depth = player->row_depths[iter.y * depths_step + depths_idx_start];

		pos[LEFT] = add_vect(player->pos, scale_vect(dir[LEFT], depth));
		pos[RIGHT] = add_vect(player->pos, scale_vect(dir[RIGHT], depth));

		row.dst = (int *)app->canvas->data + y * app->canvas->width;
		row.src = (int *)tex.data;

		curr = pos[LEFT];

		step.x = (pos[RIGHT].x - pos[LEFT].x) / WIN_WIDTH * 2;
		step.y = (pos[RIGHT].y - pos[LEFT].y) / WIN_WIDTH * 2;

		iter.x = 0;
		while (iter.x < WIN_WIDTH - 1)
		{
			idx.x = ((int) (curr.x * tex.width)) & (tex.width - 1);
			idx.y = ((int) (curr.y * tex.height)) & (tex.height - 1);

			row.dst[iter.x] = row.src[idx.y * tex.width + idx.x];
			row.dst[iter.x + 1] = row.src[idx.y * tex.width + idx.x];

			curr.x = curr.x + step.x;
			curr.y = curr.y + step.y;
			iter.x += 2;
		}
	}
}

inline __attribute__((used))
void	fill_floor_transposed(t_info *app, t_player *player, int is_floor)
{
	t_vect	dir[2];
	t_vect	pos[2];
	t_vect	step, curr;
	t_ivect idx;
	t_cdata row;
	t_ivect iter;

	int depths_step;
	int y;
	int y_start;
	int depths_idx_start;
	double	depth;
	t_img	tex;

	bool is_ceil = !is_floor;

	dir[LEFT] = rotate_vect(player->dir, app->fov_rad_half);
	dir[RIGHT] = rotate_vect(player->dir, -app->fov_rad_half);

	if (is_ceil)
	{
		tex = *app->lvl->planes[T_CEILING];
		y_start = 0;
		depths_idx_start = 0;
		depths_step = 1;
	}
	else
	{
		tex = *app->lvl->planes[T_FLOOR];
		y_start = WIN_HEIGHT / 2;
		depths_idx_start = WIN_HEIGHT / 2 - 1;
		depths_step = -1;
	}

	row.dst = (int *)app->canvas_r->data;
	row.src = (int *)tex.data;

	iter.y = -1;
	while (++iter.y < WIN_HEIGHT / 2)
	{
		y = y_start + iter.y;
		depth = player->row_depths[iter.y * depths_step + depths_idx_start];

		pos[LEFT] = add_vect(player->pos, scale_vect(dir[LEFT], depth));
		pos[RIGHT] = add_vect(player->pos, scale_vect(dir[RIGHT], depth));

		step.x = (pos[RIGHT].x - pos[LEFT].x) / WIN_WIDTH * 2;
		step.y = (pos[RIGHT].y - pos[LEFT].y) / WIN_WIDTH * 2;

		curr = pos[LEFT];
		iter.x = 0;
		while (iter.x < WIN_WIDTH - 1)
		{
			idx.x = ((int)(curr.x * tex.width)) & (tex.width - 1);
			idx.y = ((int)(curr.y * tex.height)) & (tex.height - 1);

			row.dst[iter.x * WIN_HEIGHT + y] = row.src[idx.y * tex.width + idx.x];
			row.dst[(iter.x + 1) * WIN_HEIGHT + y] = row.src[idx.y * tex.width + idx.x];

			curr.x += step.x;
			curr.y += step.y;

			iter.x += 2;
		}
	}
}
