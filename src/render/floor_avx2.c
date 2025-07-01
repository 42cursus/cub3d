/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   floor_avx2.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/30 17:44:29 by abelov            #+#    #+#             */
/*   Updated: 2025/06/30 19:35:39 by abelov           ###   ########.fr       */
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
#if defined(__clang__)
//inline __attribute__((optnone))
inline __attribute__((always_inline))
#elif defined(__GNUC__)
inline __attribute__((always_inline))
#endif
void	fill_floor_sse4x4(t_info *app, t_player *player)
{
	t_vect	dir[2];
	t_vect	pos[2];

	t_ivect it;

	t_cdata row;
	double	depth;
	t_img	tex;

	dir[LEFT] = rotate_vect(player->dir, app->fov_rad_half);
	dir[RIGHT] = rotate_vect(player->dir, -app->fov_rad_half);

	t_vect	step;
	t_vect	curr;
	int	idxs_x[4];
	int	idxs_y[4];

	tex = *app->lvl->planes[T_FLOOR];

	__m128 tex_width = _mm_set1_ps(tex.width);
	__m128 tex_height = _mm_set1_ps(tex.height);
	__m128i tex_width_i = _mm_set1_epi32(tex.width);
	__m128i tex_width_mask = _mm_set1_epi32(tex.width - 1);
	__m128i tex_height_mask = _mm_set1_epi32(tex.height - 1);

	it.y = -1;
	while (++it.y < WIN_HEIGHT / 2)
	{
		depth = player->row_depths[it.y + (WIN_HEIGHT / 2 - 1)];

		pos[LEFT] = add_vect(player->pos, scale_vect(dir[LEFT], depth));
		pos[RIGHT] = add_vect(player->pos, scale_vect(dir[RIGHT], depth));

		__m128  pos_left_xx = _mm_set1_ps(pos[LEFT].x);
		__m128  pos_left_yy = _mm_set1_ps(pos[LEFT].y);

		row.dst = (int *)app->canvas->data + (it.y + WIN_HEIGHT / 2) * app->canvas->width;
		row.src = (int *)tex.data;

		step.x = ((pos[RIGHT].x - pos[LEFT].x) / WIN_WIDTH);
		step.y = ((pos[RIGHT].y - pos[LEFT].y) / WIN_WIDTH);

		__m128  step_xx = _mm_set1_ps(step.x);
		__m128  step_yy = _mm_set1_ps(step.y);
		__m128  initial = _mm_setr_epi32(0, 2, 4, 6);

		curr = pos[LEFT];

		it.x = 0;
		while (it.x < WIN_WIDTH - 7)
		{
			__m128i it_xx = _mm_set1_epi32(it.x);
			__m128  scaled_xx = _mm_mul_ps(step_xx, _mm_cvtepi32_ps(_mm_add_epi32( it_xx, initial)));
			__m128  scaled_yy = _mm_mul_ps(step_yy, _mm_cvtepi32_ps(_mm_add_epi32( it_xx, initial)));

			__m128  currs_xx = _mm_add_ps(pos_left_xx, scaled_xx);
			__m128  currs_yy = _mm_add_ps(pos_left_yy, scaled_yy);

			__m128i idxs_xx = _mm_cvttps_epi32(_mm_mul_ps(currs_xx, tex_width));
			__m128i idxs_yy = _mm_cvttps_epi32(_mm_mul_ps(currs_yy, tex_height));

			idxs_xx = _mm_and_si128(idxs_xx, tex_width_mask);
			idxs_yy = _mm_and_si128(idxs_yy, tex_height_mask);

			_mm_storeu_si128((__m128i_u *) idxs_x, idxs_xx);
			_mm_storeu_si128((__m128i_u *) idxs_y, idxs_yy);

			__m128i final_idxss = _mm_add_epi32(_mm_mullo_epi32(idxs_yy, tex_width_i), idxs_xx);

			__m128i final_results = _mm_i32gather_epi32((const int *)row.src, final_idxss, sizeof(int));

			__m128i low = _mm_shuffle_epi32(final_results, _MM_SHUFFLE(1, 1, 0, 0));
			__m128i high = _mm_shuffle_epi32(final_results, _MM_SHUFFLE(3, 3, 2, 2));

			int *dst = &row.dst[it.x];
			_mm_storeu_si128((__m128i*)&dst[0], low);
			_mm_storeu_si128((__m128i*)&dst[4], high);

			it.x += 8;
		}

		t_ivect	idx;
		while (it.x < WIN_WIDTH - 1)
		{

			idx.x = ((int) (curr.x * tex.width)) & (tex.width - 1);
			idx.y = ((int) (curr.y * tex.height)) & (tex.height - 1);

			row.dst[it.x] = row.src[idx.y * tex.width + idx.x];
			row.dst[it.x + 1] = row.src[idx.y * tex.width + idx.x];

			curr.x = curr.x + step.x;
			curr.y = curr.y + step.y;
			it.x += 2;
		}

	}
}

void	fill_floor_avx2x8(t_info *app, t_player *player)
{
	t_vect	dir[2];
	t_vect	pos[2];

	t_ivect it;

	t_cdata row;
	double	depth;
	t_img	tex;

	dir[LEFT] = rotate_vect(player->dir, app->fov_rad_half);
	dir[RIGHT] = rotate_vect(player->dir, -app->fov_rad_half);

	t_vect	step;
	t_vect	curr;
	int	idxs_x[4];
	int	idxs_y[4];

	tex = *app->lvl->planes[T_FLOOR];

	__m256 tex_width = _mm256_set1_ps(tex.width);
	__m256 tex_height = _mm256_set1_ps(tex.height);
	__m256i tex_width_i = _mm256_set1_epi32(tex.width);
	__m256i tex_width_mask = _mm256_set1_epi32(tex.width - 1);
	__m256i tex_height_mask = _mm256_set1_epi32(tex.height - 1);

	it.y = -1;
	while (++it.y < WIN_HEIGHT / 2)
	{
		depth = player->row_depths[it.y + (WIN_HEIGHT / 2 - 1)];

		pos[LEFT] = add_vect(player->pos, scale_vect(dir[LEFT], depth));
		pos[RIGHT] = add_vect(player->pos, scale_vect(dir[RIGHT], depth));

		__m256  pos_left_xx = _mm256_set1_ps(pos[LEFT].x);
		__m256  pos_left_yy = _mm256_set1_ps(pos[LEFT].y);

		row.dst = (int *)app->canvas->data + (it.y + WIN_HEIGHT / 2) * app->canvas->width;
		row.src = (int *)tex.data;

		step.x = ((pos[RIGHT].x - pos[LEFT].x) / WIN_WIDTH);
		step.y = ((pos[RIGHT].y - pos[LEFT].y) / WIN_WIDTH);

		__m256  step_xx = _mm256_set1_ps(step.x);
		__m256  step_yy = _mm256_set1_ps(step.y);
		__m256  initial = _mm256_setr_epi32(0, 2, 4, 6, 8, 10, 12, 14);

		curr = pos[LEFT];

		it.x = 0;
		while (it.x < WIN_WIDTH - 15)
		{
			__m256i it_xx = _mm256_set1_epi32(it.x);
			__m256  scaled_xx = _mm256_mul_ps(step_xx, _mm256_cvtepi32_ps(_mm256_add_epi32( it_xx, initial)));
			__m256  scaled_yy = _mm256_mul_ps(step_yy, _mm256_cvtepi32_ps(_mm256_add_epi32( it_xx, initial)));

			__m256  currs_xx = _mm256_add_ps(pos_left_xx, scaled_xx);
			__m256  currs_yy = _mm256_add_ps(pos_left_yy, scaled_yy);

			__m256i idxs_xx = _mm256_cvttps_epi32(_mm256_mul_ps(currs_xx, tex_width));
			__m256i idxs_yy = _mm256_cvttps_epi32(_mm256_mul_ps(currs_yy, tex_height));

			idxs_xx = _mm256_and_si256(idxs_xx, tex_width_mask);
			idxs_yy = _mm256_and_si256(idxs_yy, tex_height_mask);

			_mm256_storeu_si256((__m256i_u *) idxs_x, idxs_xx);
			_mm256_storeu_si256((__m256i_u *) idxs_y, idxs_yy);

			__m256i final_idxss = _mm256_add_epi32(_mm256_mullo_epi32(idxs_yy, tex_width_i), idxs_xx);

			__m256i final_results = _mm256_i32gather_epi32((const int *)row.src, final_idxss, sizeof(int));

			__m128i lo = _mm256_castsi256_si128(final_results);
			__m128i hi = _mm256_extracti128_si256(final_results, 1);

			__m256i low_result = _mm256_set_m128i(
				_mm_shuffle_epi32(lo, _MM_SHUFFLE(3, 3, 2, 2)),
				_mm_shuffle_epi32(lo, _MM_SHUFFLE(1, 1, 0, 0))
			);

			__m256i high_result = _mm256_set_m128i(
				_mm_shuffle_epi32(hi, _MM_SHUFFLE(3, 3, 2, 2)),
				_mm_shuffle_epi32(hi, _MM_SHUFFLE(1, 1, 0, 0))
			);

			int *dst = &row.dst[it.x];
			_mm256_storeu_si256((__m256i*)&dst[0],  low_result);
			_mm256_storeu_si256((__m256i*)&dst[8], high_result);

			it.x += 16;
		}

		t_ivect	idx;
		curr.x = curr.x + step.x * it.x; // FIXME: not quite
		curr.y = curr.y + step.y * it.x;
		while (it.x < WIN_WIDTH - 1)
		{

			idx.x = ((int) (curr.x * tex.width)) & (tex.width - 1);
			idx.y = ((int) (curr.y * tex.height)) & (tex.height - 1);

			row.dst[it.x] = row.src[idx.y * tex.width + idx.x];
			row.dst[it.x + 1] = row.src[idx.y * tex.width + idx.x];

			curr.x = curr.x + step.x;
			curr.y = curr.y + step.y;
			it.x += 2;
		}

	}
}