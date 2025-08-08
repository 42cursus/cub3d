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

t_ivect3	fill_floor_scalar(int *idxs, t_cdata row, t_ivect3 it, t_cdata cd)
{
	while (it.y < WIN_HEIGHT / 2)
	{
		it.z = idxs[it.x * WIN_HEIGHT / 2 + it.y];
		cd.dst = row.dst + it.x * WIN_HEIGHT + it.y + WIN_HEIGHT / 2;
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

static inline
t_fvec256	load_depth_vec8(const float *d)
{
	t_fvec256	out;

	out.xx = _mm256_loadu_ps(d);
	out.yy = _mm256_loadu_ps(d);
	return (out);
}

static inline
t_fvec256	add_vec256(t_fvec256 a, t_fvec256 b)
{
	t_fvec256	r;

	r.xx = _mm256_add_ps(a.xx, b.xx);
	r.yy = _mm256_add_ps(a.yy, b.yy);
	return (r);
}

static inline
t_fvec256	sub_vec256(t_fvec256 a, t_fvec256 b)
{
	t_fvec256	r;

	r.xx = _mm256_sub_ps(a.xx, b.xx);
	r.yy = _mm256_sub_ps(a.yy, b.yy);
	return (r);
}

static inline
t_fvec256	scale_vec256(t_fvec256 a, t_fvec256 b)
{
	t_fvec256	r;

	r.xx = _mm256_mul_ps(a.xx, b.xx);
	r.yy = _mm256_mul_ps(a.yy, b.yy);
	return (r);
}

static inline
t_fvec256	div_vec256(t_fvec256 a, __m256 denom)
{
	t_fvec256	r;

	r.xx = _mm256_div_ps(a.xx, denom);
	r.yy = _mm256_div_ps(a.yy, denom);
	return (r);
}

static inline
void	calc_idxs_scalar(t_info *app, float *rd, int *idxs, t_img tex)
{
	t_ivect			it;
	t_ivect			ix;
	t_vect			pos[4];
	t_vect			dir[2];
	const t_player	player = *app->player;

	dir[LEFT] = rotate_vect(player.dir, app->fov_rad_half);
	dir[RIGHT] = rotate_vect(player.dir, -app->fov_rad_half);
	it.y = WIN_HEIGHT / 2 - 7;
	while (++it.y < WIN_HEIGHT / 2)
	{
		pos[CURR] = add_vect(player.pos, scale_vect(dir[LEFT], rd[it.y]));
		pos[RIGHT] = add_vect(player.pos, scale_vect(dir[RIGHT], rd[it.y]));
		pos[STEP].x = (pos[RIGHT].x - pos[CURR].x) / WIN_WIDTH * 2;
		pos[STEP].y = (pos[RIGHT].y - pos[CURR].y) / WIN_WIDTH * 2;
		it.x = 0;
		while (it.x < WIN_WIDTH - 1)
		{
			ix.x = ((int)(pos[CURR].x * tex.width)) & (tex.width - 1);
			ix.y = ((int)(pos[CURR].y * tex.height)) & (tex.height - 1);
			idxs[it.x * (WIN_HEIGHT / 2) + it.y] = ix.y * tex.width + ix.x;
			pos[CURR] = add_vect(pos[CURR], pos[STEP]);
			it.x += 2;
		}
	}
}

static
void	store_idx_row(int *ixs, t_fvec256 curr, t_img tex, int y_off, int x_off)
{
	__m256i			*ptr;
	__m256i			idx;
	const t_fvec256	scaled = {
		_mm256_mul_ps(curr.xx, _mm256_set1_ps(tex.width)),
		_mm256_mul_ps(curr.yy, _mm256_set1_ps(tex.height))
	};
	const t_ivec256	idxx = {
		_mm256_and_si256(_mm256_cvttps_epi32(scaled.xx),
			_mm256_set1_epi32(tex.width - 1)),
		_mm256_and_si256(_mm256_cvttps_epi32(scaled.yy),
			_mm256_set1_epi32(tex.height - 1))
	};

	idx = _mm256_add_epi32(_mm256_mullo_epi32(idxx.yy,
				_mm256_set1_epi32(tex.width)), idxx.xx);
	ptr = (__m256i_u *)(ixs + (x_off * (WIN_HEIGHT / 2) + y_off));
	_mm256_storeu_si256(ptr, idx);
}

static
t_ivect	calc_row_avx2(t_ivect it, float *depths, t_fvec256 *dir256,
						t_fvec256 pl, t_img tex, int *idxs)
{
	const t_fvec256	depth = load_depth_vec8(&depths[it.y]);
	t_fvec256		lr[2];
	t_fvec256		step;
	t_fvec256		curr;

	lr[LEFT] = scale_vec256(dir256[LEFT], depth);
	lr[RIGHT] = scale_vec256(dir256[RIGHT], depth);
	curr = add_vec256(pl, lr[LEFT]);
	step = div_vec256(sub_vec256(add_vec256(pl, lr[RIGHT]), curr),
			_mm256_set1_ps(WIN_WIDTH / 2.0f));
	it.x = 0;
	while (it.x < WIN_WIDTH - 1)
	{
		store_idx_row(idxs, curr, tex, it.y, it.x);
		curr = add_vec256(curr, step);
		it.x += 2;
	}
	return (it);
}

static
void	calc_idxs_avx2(t_info *app, float *row_depths, int *idxs, t_img tex)
{
	t_ivect			it;
	t_fvec256		dir256[2];
	t_fvec256		pl;
	t_player *const	player = app->player;

	dir256[LEFT] = vect_to_256f(rotate_vect(player->dir, app->fov_rad_half));
	dir256[RIGHT] = vect_to_256f(rotate_vect(player->dir, -app->fov_rad_half));
	pl = vect_to_256f(player->pos);
	it.y = 0;
	while (it.y < WIN_HEIGHT / 2 - 7)
	{
		it = calc_row_avx2(it, row_depths, dir256, pl, tex, idxs);
		it.y += 8;
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
