/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   floor_transposed_avx2_3.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 14:32:36 by abelov            #+#    #+#             */
/*   Updated: 2025/08/11 14:32:36 by abelov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

t_fvec256	load_depth_vec8(const float *d);
t_fvec256	add_vec256(t_fvec256 a, t_fvec256 b);
t_fvec256	sub_vec256(t_fvec256 a, t_fvec256 b);
t_fvec256	scale_vec256(t_fvec256 a, t_fvec256 b);
t_fvec256	div_vec256(t_fvec256 a, __m256 denom);

inline __attribute__((always_inline, used, externally_visible))
t_fvec256	load_depth_vec8(const float *d)
{
	t_fvec256	out;

	out.xx = _mm256_loadu_ps(d);
	out.yy = _mm256_loadu_ps(d);
	return (out);
}

inline __attribute__((always_inline, used, externally_visible))
t_fvec256	add_vec256(t_fvec256 a, t_fvec256 b)
{
	t_fvec256	r;

	r.xx = _mm256_add_ps(a.xx, b.xx);
	r.yy = _mm256_add_ps(a.yy, b.yy);
	return (r);
}

inline __attribute__((always_inline, used, externally_visible))
t_fvec256	sub_vec256(t_fvec256 a, t_fvec256 b)
{
	t_fvec256	r;

	r.xx = _mm256_sub_ps(a.xx, b.xx);
	r.yy = _mm256_sub_ps(a.yy, b.yy);
	return (r);
}

inline __attribute__((always_inline, used, externally_visible))
t_fvec256	scale_vec256(t_fvec256 a, t_fvec256 b)
{
	t_fvec256	r;

	r.xx = _mm256_mul_ps(a.xx, b.xx);
	r.yy = _mm256_mul_ps(a.yy, b.yy);
	return (r);
}

inline __attribute__((always_inline, used, externally_visible))
t_fvec256	div_vec256(t_fvec256 a, __m256 denom)
{
	t_fvec256	r;

	r.xx = _mm256_div_ps(a.xx, denom);
	r.yy = _mm256_div_ps(a.yy, denom);
	return (r);
}
