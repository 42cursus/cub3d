/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   floor_transposed_avx2_2.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 13:57:30 by abelov            #+#    #+#             */
/*   Updated: 2025/08/11 13:57:30 by abelov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

t_fvec256	load_depth_vec8(const float *d);
t_fvec256	add_vec256(t_fvec256 a, t_fvec256 b);
t_fvec256	sub_vec256(t_fvec256 a, t_fvec256 b);
t_fvec256	scale_vec256(t_fvec256 a, t_fvec256 b);
t_fvec256	div_vec256(t_fvec256 a, __m256 denom);
t_fvec256	vect_to_256f(t_vect v);
t_ivect3	fill_floor_scalar(int *idxs, t_cdata row, t_ivect3 it, t_cdata cd);
t_ivect		calc_row_avx2(t_ivect it, t_fvec256 *lr, t_img tex, int *idxs);
void		calc_idxs_scalar(t_info *app, float *depths, int *idxs, t_img tex);

inline __attribute__((always_inline, used))
void	calc_idxs_avx2(t_info *app, float *depths, int *idxs, t_img tex)
{
	t_ivect			it;
	t_fvec256		dir256[2];
	t_fvec256		depth;
	t_fvec256		lr[4];
	t_player *const	player = app->player;

	dir256[LEFT] = vect_to_256f(rotate_vect(player->dir, app->fov_rad_half));
	dir256[RIGHT] = vect_to_256f(rotate_vect(player->dir, -app->fov_rad_half));
	lr[LEFT] = vect_to_256f(player->pos);
	it.y = 0;
	while (it.y < WIN_HEIGHT / 2 - 7)
	{
		depth = load_depth_vec8(&depths[it.y]);
		lr[RIGHT] = scale_vec256(dir256[RIGHT], depth);
		lr[CURR] = add_vec256(lr[LEFT], scale_vec256(dir256[LEFT], depth));
		lr[STEP] = div_vec256(sub_vec256(add_vec256(lr[LEFT], lr[RIGHT]),
					lr[CURR]), _mm256_set1_ps(WIN_WIDTH / 2.0f));
		it = calc_row_avx2(it, lr, tex, idxs);
		it.y += 8;
	}
}

void	fill_ceil_transposed_cols_avx2x8(t_info *app, t_player *player)
{
	int				idxs[WIN_WIDTH * WIN_HEIGHT / 2];
	t_ivect3		it;
	t_m256i2		mc;
	const t_img		tex = *app->lvl->planes[T_CEILING];
	const t_cdata	row = {(int *) tex.data, (int *) app->canvas_r->data};

	calc_idxs_avx2(app, player->row_depths, idxs, tex);
	calc_idxs_scalar(app, player->row_depths, idxs, tex);
	it.x = 0;
	while (it.x < WIN_WIDTH - 1)
	{
		it.y = 0;
		while (it.y < WIN_HEIGHT / 2 - 7)
		{
			it.z = it.x * WIN_HEIGHT / 2 + it.y;
			mc.idxs = _mm256_loadu_si256((const __m256i_u *) &idxs[it.z]);
			mc.blend = _mm256_i32gather_epi32((int *)row.src, mc.idxs, 4);
			mc.cd.dst = row.dst + it.x * WIN_HEIGHT + it.y;
			_mm256_storeu_si256((__m256i *)mc.cd.dst, mc.blend);
			_mm256_storeu_si256((__m256i *)(mc.cd.dst + WIN_HEIGHT), mc.blend);
			it.y += 8;
		}
		fill_floor_scalar(idxs, row, it, mc.cd);
		it.x += 2;
	}
}

/**
 * int	idxs[WIN_WIDTH * WIN_HEIGHT / 2] __attribute__((aligned(32)));
 * @param app
 * @param player
 */
inline __attribute__((always_inline))
void	fill_floor_transposed_cols_avx2x8(t_info *app, t_player *player)
{
	int				idxs[WIN_WIDTH * WIN_HEIGHT / 2];
	t_ivect3		it;
	t_m256i2		mc;
	const t_img		tex = *app->lvl->planes[T_FLOOR];
	const t_cdata	row = {(int *) tex.data, (int *) app->canvas_r->data};

	calc_idxs_avx2(app, player->row_depths + WIN_HEIGHT / 2, idxs, tex);
	calc_idxs_scalar(app, player->row_depths + WIN_HEIGHT / 2, idxs, tex);
	it.x = 0;
	while (it.x < WIN_WIDTH - 1)
	{
		it.y = 0;
		while (it.y < WIN_HEIGHT / 2 - 7)
		{
			it.z = it.x * WIN_HEIGHT / 2 + it.y;
			mc.idxs = _mm256_loadu_si256((const __m256i_u *) &idxs[it.z]);
			mc.blend = _mm256_i32gather_epi32((int *) row.src, mc.idxs, 4);
			mc.cd.dst = row.dst + (it.x * WIN_HEIGHT) + (it.y + WIN_HEIGHT / 2);
			_mm256_storeu_si256((__m256i *)mc.cd.dst, mc.blend);
			_mm256_storeu_si256((__m256i *)(mc.cd.dst + WIN_HEIGHT), mc.blend);
			it.y += 8;
		}
		fill_floor_scalar(idxs, row, it, mc.cd);
		it.x += 2;
	}
}
