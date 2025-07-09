/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   floor_transposed_avx2.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/30 17:35:07 by abelov            #+#    #+#             */
/*   Updated: 2025/06/30 17:35:07 by abelov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void calc_idxs_avx2(const t_info *app, t_player *player, int *idxs, t_img	tex);

/**
 * Clamping
 * 	`__m256i tex_width_mask = _mm256_set1_epi32(tex.width - 1);`
 * 	`i_xx = _mm256_and_si256(i_xx, tex_width_mask);`
 *
 * only works if tex.width is a power of 2, aka
 * 	`x % tex.width == x & (tex.width - 1)`
 *
 * better use:
 * 	`i_xx = _mm256_min_epi32(i_xx, _mm256_set1_epi32(tex.width - 1));`
 * 	`i_xx = _mm256_max_epi32(i_xx, _mm256_setzero_si256());`
 *
 * or full modulo:
 * 	`__m256i divisor = _mm256_set1_epi32(tex.width);`
 *
 * 	1. Integer divide (no native AVX2 integer division, convert to float)
 * 	`__m256 xf      = _mm256_cvtepi32_ps(x);`
 * 	`__m256 df      = _mm256_cvtepi32_ps(divisor);`
 * 	`__m256 quotf   = _mm256_div_ps(xf, df);`
 *
 * 	2. Floor the result
 * 	`__m256 quotf_trunc = _mm256_round_ps(quotf, _MM_FROUND_TO_ZERO | _MM_FROUND_NO_EXC);`
 * 	`__m256i quot = _mm256_cvttps_epi32(quotf_trunc);`
 *
 * 	3. Multiply and subtract to get modulo
 * 	`__m256i prod = _mm256_mullo_epi32(quot, divisor);`
 * 	`__m256i mod  = _mm256_sub_epi32(x, prod);`
 *
 *	...
 * or compute the magic number with:
 * 	https://github.com/hcs0/Hackers-Delight/blob/master/magicu.c.txt
 * @param app
 * @param player
 */
inline __attribute__((always_inline))
void	fill_floor_transposed_cols_avx2x8(t_info *app, t_player *player)
{

	int		idxs[WIN_WIDTH * WIN_HEIGHT / 2];

	t_ivect iter;
	int full_idx;

	t_img	tex;
	t_cdata row;

	tex = *app->lvl->planes[T_FLOOR];

	row.dst = (int *)app->canvas_r->data;
	row.src = (int *)tex.data;

	calc_idxs_avx2(app, player, idxs, tex);

	iter.x = 0;
	while (iter.x < WIN_WIDTH - 1)
	{
		iter.y = 0;
		while (iter.y < WIN_HEIGHT / 2 - 7)
		{

			__m256i final_idxs = _mm256_loadu_si256((const __m256i_u *) &idxs[iter.x *  WIN_HEIGHT / 2 + iter.y]);
			__m256i final_results = _mm256_i32gather_epi32((const int *)row.src, final_idxs, sizeof(int));

			int	*dst = row.dst + (iter.x * WIN_HEIGHT) + (iter.y + WIN_HEIGHT / 2);
			_mm256_storeu_si256((__m256i *)dst, final_results);
			_mm256_storeu_si256((__m256i *)(dst + WIN_HEIGHT), final_results);

			iter.y = iter.y + 8;
		}
		while (iter.y < WIN_HEIGHT / 2)
		{
			full_idx = idxs[iter.x *  WIN_HEIGHT / 2 + iter.y];
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

	dir[LEFT] = rotate_vect(player->dir, app->fov_rad_half);
	dir[RIGHT] = rotate_vect(player->dir, -app->fov_rad_half);

	tex = *app->lvl->planes[T_CEILING];

	float *depths = player->row_depths;
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


			__m256i i_x = _mm256_cvttps_epi32(_mm256_mul_ps(p_val_x, tex_width));
			__m256i i_y = _mm256_cvttps_epi32(_mm256_mul_ps(p_val_y, tex_height));

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

inline __attribute__((always_inline))
void calc_idxs_avx2(const t_info *app, t_player *player, int *idxs, t_img tex)
{
	t_ivect iter;

	float *depths = &player->row_depths[WIN_HEIGHT / 2 - 1];

	t_vect	dir[2];
	t_fvec256	dir256[2];
	t_vect	pos[2];
	t_ivect idx;
	t_vect	step, curr;

	dir[LEFT] = rotate_vect(player->dir, app->fov_rad_half);
	dir[RIGHT] = rotate_vect(player->dir, -app->fov_rad_half);

	dir256[LEFT].x = _mm256_set1_ps(dir[LEFT].x);
	dir256[LEFT].y = _mm256_set1_ps(dir[LEFT].y);

	dir256[RIGHT].x = _mm256_set1_ps(dir[RIGHT].x);
	dir256[RIGHT].y = _mm256_set1_ps(dir[RIGHT].y);

	t_vect const	pl_pos = player->pos;

	t_fvec256 pl_pos256;

	pl_pos256.x = _mm256_set1_ps(pl_pos.x);
	pl_pos256.y = _mm256_set1_ps(pl_pos.y);

	__m256 half_width = _mm256_set1_ps(WIN_WIDTH / 2);
	__m256 tex_width = _mm256_set1_ps(tex.width);
	__m256 tex_height = _mm256_set1_ps(tex.height);

	__m256i tex_width_i = _mm256_set1_epi32(tex.width);
	__m256i tex_width_mask = _mm256_set1_epi32(tex.width - 1);
	__m256i tex_height_mask = _mm256_set1_epi32(tex.height - 1);


	iter.y = 0;
	while (iter.y < WIN_HEIGHT / 2 - 7)
	{
		__m256 depthxx = _mm256_loadu_ps(&depths[iter.y]);

		t_fvec256 scaled_left;
		t_fvec256 scaled_right;
		t_fvec256 pos_left;
		t_fvec256 pos_right;
		t_fvec256 stepx;
		t_fvec256 currv;

		scaled_left.x = _mm256_mul_ps(dir256[LEFT].x, depthxx);
		scaled_right.x = _mm256_mul_ps(dir256[RIGHT].x, depthxx);
		pos_left.x = _mm256_add_ps(pl_pos256.x, scaled_left.x);
		pos_right.x = _mm256_add_ps(pl_pos256.x, scaled_right.x);
		stepx.x = _mm256_sub_ps(pos_right.x, pos_left.x);
		stepx.x = _mm256_div_ps(stepx.x, half_width);

		scaled_left.y = _mm256_mul_ps(dir256[LEFT].y, depthxx);
		scaled_right.y = _mm256_mul_ps(dir256[RIGHT].y, depthxx);
		pos_left.y = _mm256_add_ps(pl_pos256.y, scaled_left.y);
		pos_right.y = _mm256_add_ps(pl_pos256.y, scaled_right.y);
		stepx.y = _mm256_sub_ps(pos_right.y, pos_left.y);
		stepx.y = _mm256_div_ps(stepx.y, half_width);

		currv.x = pos_left.x;
		currv.y = pos_left.y;

		iter.x = 0;
		while (iter.x < WIN_WIDTH - 1)
		{
			t_fvec256 i;

			i.x = _mm256_cvttps_epi32(_mm256_mul_ps(currv.x, tex_width));
			i.x = _mm256_and_si256(i.x, tex_width_mask);

			i.y = _mm256_cvttps_epi32(_mm256_mul_ps(currv.y, tex_height));
			i.y = _mm256_and_si256(i.y, tex_height_mask);

			__m256i final_idxs = _mm256_add_epi32(_mm256_mullo_epi32(i.y, tex_width_i), i.x);

			_mm256_storeu_si256((__m256i_u *) &idxs[iter.x *  WIN_HEIGHT / 2 + iter.y], final_idxs);

			currv.x = _mm256_add_ps(currv.x, stepx.x);
			currv.y = _mm256_add_ps(currv.y, stepx.y);

			iter.x += 2;
		}
		iter.y += 8;
	}

	while (iter.y < WIN_HEIGHT / 2)
	{
		double	depth = depths[iter.y * -1];

		pos[LEFT] = add_vect(pl_pos, scale_vect(dir[LEFT], depth));
		pos[RIGHT] = add_vect(pl_pos, scale_vect(dir[RIGHT], depth));

		step.x = (pos[RIGHT].x - pos[LEFT].x) / WIN_WIDTH * 2;
		step.y = (pos[RIGHT].y - pos[LEFT].y) / WIN_WIDTH * 2;

		curr = pos[LEFT];

		iter.x = 0;
		while (iter.x < WIN_WIDTH - 1)
		{
			idx.x = ((int)(curr.x * tex.width)) & (tex.width - 1);
			idx.y = ((int)(curr.y * tex.height)) & (tex.height - 1);

			idxs[iter.x * WIN_HEIGHT / 2 + iter.y] = (idx.y * tex.width) + idx.x;

			curr.x += step.x;
			curr.y += step.y;

			iter.x += 2;
		}
		iter.y++;
	}
}
