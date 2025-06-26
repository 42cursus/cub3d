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

		curr.x = curr.x + step.x;
		curr.y = curr.y + step.y;
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
	t_vect	pos[2];
	t_vect	step, curr;
	t_ivect	idxs[WIN_HEIGHT / 2][WIN_WIDTH - 1];

	t_ivect idx;
	t_cdata row;
	t_ivect iter;

	int y;
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
		depth = depths[iter.y * -1];

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
			idxs[iter.y][iter.x] = idx;

			curr.x += step.x;
			curr.y += step.y;

			iter.x += 2;
		}
	}
	iter.x = 0;
	while (iter.x < WIN_WIDTH - 1)
	{
		iter.y = -1;
		while (++iter.y < WIN_HEIGHT / 2)
		{
			y = iter.y + WIN_HEIGHT / 2;
			idx = idxs[iter.y][iter.x];

			int result = row.src[idx.y * tex.width + idx.x];
			int	*dst = row.dst + iter.x * WIN_HEIGHT + y;

			dst[0] = result;
			dst[WIN_HEIGHT] = result;
		}
		iter.x += 2;
	}
}

void	fill_floor_transposed_cols_avx2x4(t_info *app, t_player *player)
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
		depth = depths[iter.y * -1];

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
		while (iter.y < WIN_HEIGHT - 3)
		{

			__m128 p_val_x = _mm_loadu_ps(&pos_x[iter.y]);
			__m128 p_val_y = _mm_loadu_ps(&pos_y[iter.y]);

			__m128 tex_width = _mm_set1_ps(tex.width);
			__m128 tex_height = _mm_set1_ps(tex.height);

			__m128i tex_width_i = _mm_set1_epi32(tex.width - 1);
			__m128i tex_height_i = _mm_set1_epi32(tex.height - 1);


			__m128i i_x = _mm_cvtps_epi32(_mm_mul_ps(p_val_x, tex_width));
			__m128i i_y = _mm_cvtps_epi32(_mm_mul_ps(p_val_y, tex_height));

			__m128i idx_x = _mm_and_si128(i_x, tex_width_i);
			__m128i idx_y = _mm_and_si128(i_y, tex_height_i);

			__m128i final_idxs = _mm_add_epi32(_mm_mullo_epi32(idx_y, _mm_set1_epi32(tex.width)), idx_x);

			__m128i final_results = _mm_i32gather_epi32((const int *)row.src, final_idxs, sizeof(int));

			int	*dst = row.dst + iter.x * WIN_HEIGHT + iter.y;
			_mm_storeu_si128((__m128i *)dst, final_results);
			_mm_storeu_si128((__m128i *)(dst + WIN_HEIGHT), final_results);

			__m128 steps_xx = _mm_loadu_ps(&steps_x[iter.y]);
			__m128 steps_yy = _mm_loadu_ps(&steps_y[iter.y]);

			_mm_storeu_ps(&pos_x[iter.y], _mm_add_ps(p_val_x, steps_xx));
			_mm_storeu_ps(&pos_y[iter.y], _mm_add_ps(p_val_y, steps_yy));

			iter.y = iter.y + 4;
		}
		iter.x += 2;
	}
}

__attribute__((optnone))
void	fill_floor_transposed_cols_avx2x8(t_info *app, t_player *player)
{
	t_vect			dir[2];
	t_vect			pos[2];

	t_vect	step, curr;
	int		idxs[WIN_HEIGHT / 2][WIN_WIDTH - 1];

	float	pos_array_x[WIN_HEIGHT / 2];
	float	pos_array_y[WIN_HEIGHT / 2];

	float	steps_arr_x[WIN_HEIGHT / 2];
	float	steps_arr_y[WIN_HEIGHT / 2];

	t_cdata row;
	t_ivect iter;

	t_ivect idx;
	int		full_idx;
	t_img	tex;

	double *depths = &player->row_depths[WIN_HEIGHT / 2 - 1];

	dir[LEFT] = rotate_vect(player->dir, app->fov_rad_half);
	dir[RIGHT] = rotate_vect(player->dir, -app->fov_rad_half);

	tex = *app->lvl->planes[T_FLOOR];

	row.dst = (int *)app->canvas_r->data;
	row.src = (int *)tex.data;

	t_vect const	pl_pos = player->pos;
	iter.y = -1;
	while (++iter.y < WIN_HEIGHT / 2)
	{
		double	depth = depths[iter.y * -1];

		pos[LEFT] = add_vect(pl_pos, scale_vect(dir[LEFT], depth));
		pos[RIGHT] = add_vect(pl_pos, scale_vect(dir[RIGHT], depth));

		step.x = (pos[RIGHT].x - pos[LEFT].x) / WIN_WIDTH * 2;
		step.y = (pos[RIGHT].y - pos[LEFT].y) / WIN_WIDTH * 2;

		curr = pos[LEFT];

		steps_arr_x[iter.y] = step.x;
		steps_arr_y[iter.y] = step.y;

		pos_array_x[iter.y] = curr.x;
		pos_array_y[iter.y] = curr.y;

		iter.x = 0;
		while (iter.x < WIN_WIDTH - 1)
		{
			idx.x = ((int)(curr.x * tex.width)) & (tex.width - 1);
			idx.y = ((int)(curr.y * tex.height)) & (tex.height - 1);

			full_idx = idx.y * tex.width + idx.x;

			idxs[iter.y][iter.x] = full_idx;

			curr.x += step.x;
			curr.y += step.y;

			iter.x += 2;
		}
	}

//	__m256 tex_width = _mm256_set1_ps(tex.width);
//	__m256 tex_height = _mm256_set1_ps(tex.height);
//	__m256i tex_width_i = _mm256_set1_epi32(tex.width - 1);
//	__m256i tex_height_i = _mm256_set1_epi32(tex.height - 1);

//	iter.x = 0;
//	while (iter.x < WIN_WIDTH - 1)
//	{
//		iter.y = 0;
//		while (iter.y < WIN_HEIGHT / 2 - 7)
//		{
//			__m256 curr_x = _mm256_loadu_ps(&pos_array_x[iter.y]);
//			__m256 curr_y = _mm256_loadu_ps(&pos_array_y[iter.y]);
//
//			__m256i i_x = _mm256_cvtps_epi32(_mm256_mul_ps(curr_x, tex_width));
//			__m256i i_y = _mm256_cvtps_epi32(_mm256_mul_ps(curr_y, tex_height));
//
//			__m256i idx_x = _mm256_and_si256(i_x, tex_width_i);
//			__m256i idx_y = _mm256_and_si256(i_y, tex_height_i);
//
//			__m256i final_idxs = _mm256_add_epi32(_mm256_mullo_epi32(idx_y, _mm256_set1_epi32(tex.width)), idx_x);
//
//			int final_idxs_int[8];
//			_mm256_storeu_si256((__m256i_u *) final_idxs_int, final_idxs);
//
//			_mm256_storeu_si256((__m256i_u *) &idxs[iter.y][iter.x], final_idxs);
//
//			__m256 steps_xx = _mm256_loadu_ps(&steps_arr_x[iter.y]);
//			__m256 steps_yy = _mm256_loadu_ps(&steps_arr_y[iter.y]);
//
//			_mm256_storeu_ps(&pos_array_x[iter.y], _mm256_add_ps(curr_x, steps_xx));
//			_mm256_storeu_ps(&pos_array_y[iter.y], _mm256_add_ps(curr_y, steps_yy));
//
//			iter.y = iter.y + 8;
//		}
//		while (iter.y < WIN_HEIGHT / 2)
//		{
//			curr.x = pos_array_x[iter.y];
//			curr.y = pos_array_y[iter.y];
//
//			step.x = steps_arr_x[iter.y];
//			step.y = steps_arr_y[iter.y];
//
//			idx.x = ((int)(curr.x * tex.width)) & (tex.width - 1);
//			idx.y = ((int)(curr.y * tex.height)) & (tex.height - 1);
//
//			idxs[iter.y][iter.x] = idx.y * tex.width + idx.x;
//
//			curr.x += step.x;
//			curr.y += step.y;
//
//			pos_array_x[iter.y] = curr.x;
//			pos_array_y[iter.y] = curr.y;
//
//			iter.y++;
//		}
//		iter.x += 2;
//	}

	iter.x = 0;
	while (iter.x < WIN_WIDTH - 1)
	{
		iter.y = 0;
		while (iter.y < WIN_HEIGHT / 2 - 7)
		{

			int final_idxs_int[8];

			__m256i final_idxs = _mm256_loadu_si256((const __m256i_u *) &idxs[iter.y][iter.x]);

			_mm256_storeu_si256((__m256i_u *) final_idxs_int, final_idxs);

			final_idxs = _mm256_setr_epi32(
				idxs[iter.y + 0][iter.x],
				idxs[iter.y + 1][iter.x],
				idxs[iter.y + 2][iter.x],
				idxs[iter.y + 3][iter.x],
				idxs[iter.y + 4][iter.x],
				idxs[iter.y + 5][iter.x],
				idxs[iter.y + 6][iter.x],
				idxs[iter.y + 7][iter.x]
			);

			_mm256_storeu_si256((__m256i_u *) final_idxs_int, final_idxs);

			__m256i final_results = _mm256_i32gather_epi32((const int *)row.src, final_idxs, sizeof(int));

			int	*dst = row.dst + (iter.x * WIN_HEIGHT) + (iter.y + WIN_HEIGHT / 2);
			_mm256_storeu_si256((__m256i *)dst, final_results);
			_mm256_storeu_si256((__m256i *)(dst + WIN_HEIGHT), final_results);

			iter.y = iter.y + 8;
		}
		while (iter.y < WIN_HEIGHT / 2)
		{
			full_idx = idxs[iter.y][iter.x];
			int result = row.src[full_idx];
			int	*dst = row.dst + iter.x * WIN_HEIGHT + iter.y + WIN_HEIGHT / 2;
			dst[0] = result;
			dst[WIN_HEIGHT] = result;
			iter.y++;
		}
		iter.x += 2;
	}
}

void	fill_ceil_transposed_cols_avx2x8(t_info *app, t_player *player)
{
	t_vect	dir[2];
	float	pos_array_x[WIN_HEIGHT / 2];
	float	pos_array_y[WIN_HEIGHT / 2];

	float	steps_arr_x[WIN_HEIGHT / 2];
	float	steps_arr_y[WIN_HEIGHT / 2];

	t_vect	curr;

	t_cdata row;
	t_ivect iter;

	double	depth;
	t_img	tex;
	double *depths;

	dir[LEFT] = rotate_vect(player->dir, app->fov_rad_half);
	dir[RIGHT] = rotate_vect(player->dir, -app->fov_rad_half);

	tex = *app->lvl->planes[T_CEILING];

	depths = player->row_depths;
	row.dst = (int *)app->canvas_r->data;
	row.src = (int *)tex.data;

	iter.y = (WIN_HEIGHT / 2);
	while (iter.y--)
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
		iter.y = 0;
		while (iter.y < (WIN_HEIGHT / 2) - 7)
		{

			__m256 p_val_x = _mm256_loadu_ps(&pos_array_x[iter.y]);
			__m256 p_val_y = _mm256_loadu_ps(&pos_array_y[iter.y]);

			__m256 tex_width = _mm256_set1_ps(tex.width);
			__m256 tex_height = _mm256_set1_ps(tex.height);

			__m256i tex_width_i = _mm256_set1_epi32(tex.width - 1);
			__m256i tex_height_i = _mm256_set1_epi32(tex.height - 1);


			__m256i i_x = _mm256_cvtps_epi32(_mm256_mul_ps(p_val_x, tex_width));
			__m256i i_y = _mm256_cvtps_epi32(_mm256_mul_ps(p_val_y, tex_height));

			__m256i idx_x = _mm256_and_si256(i_x, tex_width_i);
			__m256i idx_y = _mm256_and_si256(i_y, tex_height_i);

			__m256i final_idxs = _mm256_add_epi32(_mm256_mullo_epi32(idx_y, _mm256_set1_epi32(tex.width)), idx_x);

			__m256i final_results = _mm256_i32gather_epi32((const int *)row.src, final_idxs, sizeof(int));

			int	*dst = row.dst + iter.x * WIN_HEIGHT + iter.y;
			_mm256_storeu_si256((__m256i *)dst, final_results);
			_mm256_storeu_si256((__m256i *)(dst + WIN_HEIGHT), final_results);

			__m256 steps_xx = _mm256_loadu_ps(&steps_arr_x[iter.y]);
			__m256 steps_yy = _mm256_loadu_ps(&steps_arr_y[iter.y]);

			_mm256_storeu_ps(&pos_array_x[iter.y], _mm256_add_ps(p_val_x, steps_xx));
			_mm256_storeu_ps(&pos_array_y[iter.y], _mm256_add_ps(p_val_y, steps_yy));

			iter.y = iter.y + 8;
		}
		while (iter.y < WIN_HEIGHT / 2)
		{
			t_ivect idx;

			idx.x = ((int)(pos_array_x[iter.y] * tex.width)) & (tex.width - 1);
			idx.y = ((int)(pos_array_y[iter.y] * tex.height)) & (tex.height - 1);

			int	*dst = row.dst + iter.x * WIN_HEIGHT + iter.y;

			dst[0] = row.src[idx.y * tex.width + idx.x];
			dst[WIN_HEIGHT] = row.src[idx.y * tex.width + idx.x];
			pos_array_x[iter.y] += steps_arr_x[iter.y];
			pos_array_y[iter.y] += steps_arr_y[iter.y];
			iter.y++;
		}
		iter.x += 2;
	}
}

void	fill_ceil_transposed(t_info *app, t_player *player)
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

	dir[LEFT] = rotate_vect(player->dir, app->fov_rad_half);
	dir[RIGHT] = rotate_vect(player->dir, -app->fov_rad_half);

	tex = *app->lvl->planes[T_CEILING];
	y_start = 0;
	depths_idx_start = 0;
	depths_step = 1;

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

void	fill_floor_transposed(t_info *app, t_player *player)
{
	t_vect	dir[2];
	t_vect	pos[2];
	t_vect	step, curr;
	t_ivect idx;
	t_cdata row;
	t_ivect iter;

	int y;
	double	depth;
	t_img	tex;

	dir[LEFT] = rotate_vect(player->dir, app->fov_rad_half);
	dir[RIGHT] = rotate_vect(player->dir, -app->fov_rad_half);

	tex = *app->lvl->planes[T_FLOOR];

	row.dst = (int *)app->canvas_r->data;
	row.src = (int *)tex.data;

	t_ivect	idxs[WIN_HEIGHT / 2][WIN_WIDTH - 1];

	iter.y = -1;
	while (++iter.y < WIN_HEIGHT / 2)
	{
		depth = player->row_depths[iter.y * -1 + (WIN_HEIGHT / 2 - 1)];
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
			idxs[iter.y][iter.x] = idx;
			iter.x += 2;
			curr.x += step.x;
			curr.y += step.y;
		}
	}
	iter.y = -1;
	while (++iter.y < WIN_HEIGHT / 2)
	{
		y = iter.y + WIN_HEIGHT / 2;

		iter.x = 0;
		while (iter.x < WIN_WIDTH - 1)
		{
			idx = idxs[iter.y][iter.x];

			row.dst[iter.x * WIN_HEIGHT + y] = row.src[idx.y * tex.width + idx.x];
			row.dst[(iter.x + 1) * WIN_HEIGHT + y] = row.src[idx.y * tex.width + idx.x];

			iter.x += 2;
		}
	}
}
