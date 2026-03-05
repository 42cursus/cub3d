/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   floor_transposed_avx2.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/30 17:35:07 by abelov            #+#    #+#             */
/*   Updated: 2025/08/11 18:39:47 by abelov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

t_fvec256	add_vec256(t_fvec256 a, t_fvec256 b);
t_fvec256	load_depth_vec8(const float *d);
t_fvec256	sub_vec256(t_fvec256 a, t_fvec256 b);
t_fvec256	scale_vec256(t_fvec256 a, t_fvec256 b);
t_fvec256	div_vec256(t_fvec256 a, __m256 denom);

inline __attribute__((always_inline, used))
t_ivect3	fill_floor_scalar(int *idxs, t_cdata row, t_ivect3 it, t_cdata cd)
{
	while (it.y < WIN_HEIGHT / 2)
	{
		it.z = idxs[it.x * WIN_HEIGHT / 2 + it.y];
		cd.dst = row.dst + (it.x * WIN_HEIGHT) + (it.y);
		cd.dst[0] = row.src[it.z];
		cd.dst[WIN_HEIGHT] = row.src[it.z];
		it.y++;
	}
	return (it);
}

inline __attribute__((always_inline, used))
t_fvec256	vect_to_256f(t_vect v)
{
	t_fvec256	r;

	r.xx = _mm256_set1_ps((float) v.x);
	r.yy = _mm256_set1_ps((float) v.y);
	return (r);
}

inline __attribute__((always_inline, used))
void	calc_idxs_scalar(t_info *app, float *depths, int *idxs, const t_img *tex)
{
	t_ivect			it;
	t_ivect			ix;
	t_vect			pos[4];
	t_vect			dir[2];
	const int		w = tex->width;
	const int		h = tex->height;
	t_vect player_pos = app->player->pos;
	t_vect player_dir = app->player->dir;
	
	dir[LEFT] = rotate_vect(player_dir, app->fov_rad_half);
	dir[RIGHT] = rotate_vect(player_dir, -app->fov_rad_half);
	it.y = WIN_HEIGHT / 2 - 7;
	while (++it.y < WIN_HEIGHT / 2)
	{

		pos[CURR] = add_vect(player_pos, scale_vect(dir[LEFT], depths[it.y]));
		pos[RIGHT] = add_vect(player_pos, scale_vect(dir[RIGHT], depths[it.y]));
		pos[STEP].x = (pos[RIGHT].x - pos[CURR].x) / WIN_WIDTH * 2;
		pos[STEP].y = (pos[RIGHT].y - pos[CURR].y) / WIN_WIDTH * 2;
		it.x = 0;
		while (it.x < WIN_WIDTH - 1)
		{
			ix.x = ((int)(pos[CURR].x * w)) & (w - 1);
			ix.y = ((int)(pos[CURR].y * h)) & (h - 1);
			idxs[it.x * (WIN_HEIGHT / 2) + it.y] = ix.y * w + ix.x;
			pos[CURR] = add_vect(pos[CURR], pos[STEP]);
			it.x += 2;
		}
	}
}

inline __attribute__((always_inline, used))
void	store_idx_row(int *ixs, t_fvec256 curr, const t_img *tex, t_ivect it)
{
	__m256i			*ptr;
	__m256i			idx;
	const int		w = tex->width;
	const int		h = tex->height;
	const t_fvec256	scaled = {
			_mm256_mul_ps(curr.xx, _mm256_set1_ps((float)w)),
			_mm256_mul_ps(curr.yy, _mm256_set1_ps((float)h))
	};
	const t_ivec256	idxx = {
			_mm256_and_si256(_mm256_cvttps_epi32(scaled.xx),
							 _mm256_set1_epi32(w - 1)),
			_mm256_and_si256(_mm256_cvttps_epi32(scaled.yy),
							 _mm256_set1_epi32(h - 1))
	};

	idx = _mm256_add_epi32(_mm256_mullo_epi32(idxx.yy,
											  _mm256_set1_epi32(w)), idxx.xx);
	ptr = (__m256i_u *)(ixs + (it.x * (WIN_HEIGHT / 2) + it.y));
	_mm256_storeu_si256(ptr, idx);
}

inline __attribute__((always_inline, used))
t_ivect	calc_row_avx2(t_ivect it, t_fvec256 *lr, const t_img *tex, int *idxs)
{
	it.x = 0;
	while (it.x < WIN_WIDTH - 1)
	{
		store_idx_row(idxs, lr[CURR], tex, it);
		lr[CURR] = add_vec256(lr[CURR], lr[STEP]);
		it.x += 2;
	}
	return (it);
}

t_fvec256	load_depth_vec8(const float *d);
t_fvec256	add_vec256(t_fvec256 a, t_fvec256 b);
t_fvec256	sub_vec256(t_fvec256 a, t_fvec256 b);
t_fvec256	scale_vec256(t_fvec256 a, t_fvec256 b);
t_fvec256	div_vec256(t_fvec256 a, __m256 denom);
t_fvec256	vect_to_256f(t_vect v);
t_ivect3	fill_floor_scalar(int *idxs, t_cdata row, t_ivect3 it, t_cdata cd);
t_ivect		calc_row_avx2(t_ivect it, t_fvec256 *lr, const t_img *tex, int *idxs);
void		calc_idxs_scalar(t_info *app, float *depths, int *idxs, const t_img *tex);

inline __attribute__((always_inline, used))
void	calc_idxs_avx2(t_info *app, float *depths, int *idxs, const t_img *tex)
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
	static int		idxs[WIN_WIDTH * WIN_HEIGHT / 2] = {0x00};
	t_ivect3		it;
	t_m256i2		mc;
	const t_img		*tex = app->lvl->planes[T_CEILING];
	const t_cdata	row = {(int *) tex->data, (int *) app->canvas_r->data};

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
	static int		idxs[WIN_WIDTH * WIN_HEIGHT / 2] = {0x00};
	t_ivect3		it;
	t_m256i2		mc;
	const t_img		*tx = app->lvl->planes[T_FLOOR];
	const t_cdata	row = {(int *)tx->data, (int *)app->canvas_r->data + WH / 2};

	calc_idxs_avx2(app, player->row_depths + WIN_HEIGHT / 2, idxs, tx);
	calc_idxs_scalar(app, player->row_depths + WIN_HEIGHT / 2, idxs, tx);
	it.x = 0;
	while (it.x < WIN_WIDTH - 1)
	{
		it.y = 0;
		while (it.y < WIN_HEIGHT / 2 - 7)
		{
			it.z = it.x * WIN_HEIGHT / 2 + it.y;
			mc.idxs = _mm256_loadu_si256((const __m256i_u *) &idxs[it.z]);
			mc.blend = _mm256_i32gather_epi32((int *) row.src, mc.idxs, 4);
			mc.cd.dst = row.dst + (it.x * WIN_HEIGHT) + (it.y);
			_mm256_storeu_si256((__m256i *)mc.cd.dst, mc.blend);
			_mm256_storeu_si256((__m256i *)(mc.cd.dst + WIN_HEIGHT), mc.blend);
			it.y += 8;
		}
		fill_floor_scalar(idxs, row, it, mc.cd);
		it.x += 2;
	}
}

t_fvec256	load_depth_vec8(const float *d);
t_fvec256	add_vec256(t_fvec256 a, t_fvec256 b);
t_fvec256	sub_vec256(t_fvec256 a, t_fvec256 b);
t_fvec256	scale_vec256(t_fvec256 a, t_fvec256 b);
t_fvec256	div_vec256(t_fvec256 a, __m256 denom);

inline __attribute__((always_inline, used))
t_fvec256	load_depth_vec8(const float *d)
{
	t_fvec256	out;

	out.xx = _mm256_loadu_ps(d);
	out.yy = _mm256_loadu_ps(d);
	return (out);
}

inline __attribute__((always_inline, used))
t_fvec256	add_vec256(t_fvec256 a, t_fvec256 b)
{
	t_fvec256	r;

	r.xx = _mm256_add_ps(a.xx, b.xx);
	r.yy = _mm256_add_ps(a.yy, b.yy);
	return (r);
}

inline __attribute__((always_inline, used))
t_fvec256	sub_vec256(t_fvec256 a, t_fvec256 b)
{
	t_fvec256	r;

	r.xx = _mm256_sub_ps(a.xx, b.xx);
	r.yy = _mm256_sub_ps(a.yy, b.yy);
	return (r);
}

inline __attribute__((always_inline, used))
t_fvec256	scale_vec256(t_fvec256 a, t_fvec256 b)
{
	t_fvec256	r;

	r.xx = _mm256_mul_ps(a.xx, b.xx);
	r.yy = _mm256_mul_ps(a.yy, b.yy);
	return (r);
}

inline __attribute__((always_inline, used))
t_fvec256	div_vec256(t_fvec256 a, __m256 denom)
{
	t_fvec256	r;

	r.xx = _mm256_div_ps(a.xx, denom);
	r.yy = _mm256_div_ps(a.yy, denom);
	return (r);
}
