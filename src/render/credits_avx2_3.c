/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   credits_avx2_3.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 15:57:10 by abelov            #+#    #+#             */
/*   Updated: 2025/08/11 15:57:11 by abelov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

inline __attribute__((always_inline, used))
__m256	get_fma(float step_x, int x)
{
	t_fma_avx2	fma_var;

	fma_var.step = _mm256_set1_ps(step_x);
	fma_var.indices = _mm256_add_epi32(_mm256_set1_epi32(x),
			_mm256_setr_epi32(0, 1, 2, 3, 4, 5, 6, 7));
	fma_var.offsets = _mm256_mul_ps(fma_var.step,
			_mm256_cvtepi32_ps(fma_var.indices));
	return (fma_var.offsets);
}

inline __attribute__((always_inline, used))
__m256	calc_weights(__m256 idxx)
{
	t_fmodf_avx2	weight_xm;
	const __m256	one_ps = _mm256_set1_ps(1.0f);

	weight_xm.div = _mm256_div_ps(idxx, one_ps);
	weight_xm.tdiv = _mm256_round_ps(weight_xm.div, ROUND_ZERO);
	weight_xm.prod = _mm256_mul_ps(one_ps, weight_xm.tdiv);
	return (_mm256_sub_ps(idxx, weight_xm.prod));
}

inline __attribute__((always_inline, used))
__m256	get_idxx(const int tex_w, t_vect v, __m256 offsets)
{
	__m256			id_xx;
	const __m256	half_ps = _mm256_set1_ps(0.5f);

	id_xx = _mm256_mul_ps(_mm256_add_ps(half_ps,
				_mm256_add_ps(_mm256_set1_ps((float) v.x), offsets)),
			_mm256_cvtepi32_ps(_mm256_set1_epi32(tex_w)));
	return (id_xx);
}

inline __attribute__((always_inline, used))
void	calc_pos(const t_info *app, t_dummy *dummy, t_vect *pos, float d)
{
	t_vect	scaled[2];

	scaled[LEFT] = scale_vect(rotate_vect(dummy->dir, app->fov_rad_half), d);
	scaled[RIGHT] = scale_vect(rotate_vect(dummy->dir, -app->fov_rad_half), d);
	pos[LEFT] = add_vect(dummy->pos, scaled[LEFT]);
	pos[RIGHT] = add_vect(dummy->pos, scaled[RIGHT]);
}

inline __attribute__((always_inline, used))
t_vec2i_avx	calc_xx(__m256 id_xx, const int tex_w)
{
	t_vec2i_avx		xx;
	const __m256i	max_val = _mm256_set1_epi32(tex_w - 1);
	const __m256i	one_epi32 = _mm256_set1_epi32(1);

	xx.r0 = _mm256_cvttps_epi32(id_xx);
	xx.r1 = _mm256_min_epi32(_mm256_add_epi32(xx.r0, one_epi32), max_val);
	return (xx);
}
