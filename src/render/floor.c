/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   floor.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 18:17:23 by fsmyth            #+#    #+#             */
/*   Updated: 2025/06/23 16:22:16 by abelov           ###   ########.fr       */
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

// __attribute__((optnone))
void	fill_ceil_transposed_cols(t_info *app, t_player *player)
{

	return ;
	(void) app;
	(void) player;
}

__attribute__((optnone))
void	fill_floor_transposed_cols(t_info *app, t_player *player)
{
	t_vect	dir[2];
	t_vect	pos[WIN_HEIGHT / 2];
	t_vect	steps[WIN_HEIGHT / 2];
	t_vect	curr;
	t_ivect idx;
	t_cdata row;
	t_ivect iter;

	int y;
	int y_start;
	double	depth;
	t_img	tex;
	double *depths;

	dir[LEFT] = rotate_vect(player->dir, app->fov_rad_half);
	dir[RIGHT] = rotate_vect(player->dir, -app->fov_rad_half);

	tex = *app->lvl->planes[T_FLOOR];
	y_start = WIN_HEIGHT / 2;

	depths = &player->row_depths[WIN_HEIGHT / 2 - 1];
	row.dst = (int *)app->canvas_r->data;
	row.src = (int *)tex.data;

	iter.y = -1;
	while (++iter.y < WIN_HEIGHT / 2)
	{
		depth = depths[iter.y];
		pos[iter.y] = add_vect(player->pos, scale_vect(dir[LEFT], depth));
		curr = add_vect(player->pos, scale_vect(dir[RIGHT], depth));
		steps[iter.y].x = (curr.x - pos[iter.y].x) / WIN_WIDTH * 2;
		steps[iter.y].y = (curr.y - pos[iter.y].y) / WIN_WIDTH * 2;
	}
	iter.x = 0;
	while (iter.x < WIN_WIDTH - 1)
	{
		iter.y = -1;
		while (++iter.y < WIN_HEIGHT / 2)
		{
			y = y_start + iter.y;
			idx.x = ((int)(pos[iter.y].x * tex.width)) & (tex.width - 1);
			idx.y = ((int)(pos[iter.y].y * tex.height)) & (tex.height - 1);

			int i = iter.x * WIN_HEIGHT + y;
			row.dst[i] = row.src[idx.y * tex.width + idx.x];
			row.dst[i + WIN_HEIGHT] = row.src[idx.y * tex.width + idx.x];
			pos[iter.y].x += steps[iter.y].x;
			pos[iter.y].y += steps[iter.y].y;
		}
		iter.x += 2;
	}
}

void	fill_floor_transposed_cols_avx2(t_info *app, t_player *player)
{
	t_vect	dir[2];
	float	pos_array_x[WIN_HEIGHT / 2];
	float	pos_array_y[WIN_HEIGHT / 2];

	float	*pos_x = pos_array_x - WIN_HEIGHT / 2;
	float	*pos_y = pos_array_y - WIN_HEIGHT / 2;

	float	steps_arr_x[WIN_HEIGHT / 2];
	float	steps_arr_y[WIN_HEIGHT / 2];

	float	*steps_x = steps_arr_x - WIN_HEIGHT / 2;
	float	*steps_y = steps_arr_y - WIN_HEIGHT / 2;

	t_vect	curr;

	t_cdata row;
	t_ivect iter;

	double	depth;
	t_img	tex;
	double *depths;

	dir[LEFT] = rotate_vect(player->dir, app->fov_rad_half);
	dir[RIGHT] = rotate_vect(player->dir, -app->fov_rad_half);

	tex = *app->lvl->planes[T_FLOOR];

	depths = &player->row_depths[WIN_HEIGHT / 2 - 1];
	row.dst = (int *)app->canvas_r->data;
	row.src = (int *)tex.data;

	iter.y = -1;
	while (++iter.y < WIN_HEIGHT / 2)
	{
		depth = depths[iter.y];

		t_vect temp = add_vect(player->pos, scale_vect(dir[LEFT], depth));

		pos_array_x[iter.y] = temp.x;
		pos_array_y[iter.y] = temp.y;

		curr = add_vect(player->pos, scale_vect(dir[RIGHT], depth));

		steps_arr_x[iter.y] = (curr.x - pos_array_x[iter.y]) / WIN_WIDTH * 2;
		steps_arr_y[iter.y] = (curr.y - pos_array_y[iter.y]) / WIN_WIDTH * 2;
	}
	iter.x = 0;
	while (iter.x < WIN_WIDTH - 1)
	{
		iter.y = WIN_HEIGHT / 2;
		while (iter.y < WIN_HEIGHT)
		{

//			int y[4] = {
//			       iter.y + 0,
//				   iter.y + 1,
//				   iter.y + 2,
//				   iter.y + 3,
//			};

			int y_0 = iter.y + 0;
			int y_1 = iter.y + 1;
			int y_2 = iter.y + 2;
			int y_3 = iter.y + 3;

			float p_val0_x = pos_x[y_0];
			float p_val1_x = pos_x[y_1];
			float p_val2_x = pos_x[y_2];
			float p_val3_x = pos_x[y_3];

			float p_val0_y = pos_y[y_0];
			float p_val1_y = pos_y[y_1];
			float p_val2_y = pos_y[y_2];
			float p_val3_y = pos_y[y_3];

			int i0_x = (int) (p_val0_x * tex.width);
			int i1_x = (int) (p_val1_x * tex.width);
			int i2_x = (int) (p_val2_x * tex.width);
			int i3_x = (int) (p_val3_x * tex.width);

			int i0_y = (int) (p_val0_y * tex.height);
			int i1_y = (int) (p_val1_y * tex.height);
			int i2_y = (int) (p_val2_y * tex.height);
			int i3_y = (int) (p_val3_y * tex.height);

			t_ivect idx0;
			t_ivect idx1;
			t_ivect idx2;
			t_ivect idx3;

			idx0.x = i0_x & (tex.width - 1);
			idx1.x = i1_x & (tex.width - 1);
			idx2.x = i2_x & (tex.width - 1);
			idx3.x = i3_x & (tex.width - 1);

			idx0.y = i0_y & (tex.height - 1);
			idx1.y = i1_y & (tex.height - 1);
			idx2.y = i2_y & (tex.height - 1);
			idx3.y = i3_y & (tex.height - 1);

			int rowrow = iter.x * WIN_HEIGHT;

			int i0 = rowrow + y_0;
			int i1 = rowrow + y_1;
			int i2 = rowrow + y_2;
			int i3 = rowrow + y_3;

			row.dst[i0] = row.src[idx0.y * tex.width + idx0.x];
			row.dst[i1] = row.src[idx1.y * tex.width + idx1.x];
			row.dst[i2] = row.src[idx2.y * tex.width + idx2.x];
			row.dst[i3] = row.src[idx3.y * tex.width + idx3.x];

			i0 += WIN_HEIGHT;
			i1 += WIN_HEIGHT;
			i2 += WIN_HEIGHT;
			i3 += WIN_HEIGHT;


			row.dst[i0] = row.src[idx0.y * tex.width + idx0.x];
			row.dst[i1] = row.src[idx1.y * tex.width + idx1.x];
			row.dst[i2] = row.src[idx2.y * tex.width + idx2.x];
			row.dst[i3] = row.src[idx3.y * tex.width + idx3.x];

			pos_x[y_0] += steps_x[y_0];
			pos_x[y_1] += steps_x[y_1];
			pos_x[y_2] += steps_x[y_2];
			pos_x[y_3] += steps_x[y_3];

			pos_y[y_0] += steps_y[y_0];
			pos_y[y_1] += steps_y[y_1];
			pos_y[y_2] += steps_y[y_2];
			pos_y[y_3] += steps_y[y_3];

			iter.y = iter.y + 4;
		}
		iter.x += 2;
	}
}

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
