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
void	calc_idxs_scalar(t_info *app, float *depths, int *idxs, t_img tex)
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
		pos[CURR] = add_vect(player.pos, scale_vect(dir[LEFT], depths[it.y]));
		pos[RIGHT] = add_vect(player.pos, scale_vect(dir[RIGHT], depths[it.y]));
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

inline __attribute__((always_inline, used))
void	store_idx_row(int *ixs, t_fvec256 curr, t_img tex, t_ivect it)
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
	ptr = (__m256i_u *)(ixs + (it.x * (WIN_HEIGHT / 2) + it.y));
	_mm256_storeu_si256(ptr, idx);
}

inline __attribute__((always_inline, used))
t_ivect	calc_row_avx2(t_ivect it, t_fvec256 *lr, t_img tex, int *idxs)
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
