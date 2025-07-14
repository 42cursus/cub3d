/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   credits_avx2.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/04 17:52:50 by abelov            #+#    #+#             */
/*   Updated: 2025/07/04 17:52:51 by abelov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <math.h>
#include <sys/param.h>
#include "cub3d.h"

static inline __attribute__((always_inline))
t_rgba_ps256	unpack_rgba_bytes_to_floats_avx(__m256i pixels)
{
	t_rgba_ps256	out;
	t_vec4i_avx		vec;
	const __m256i	shuffle
		= _mm256_setr_epi8(0, 4, 8, 12, 1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15,
			16, 20, 24, 28, 17, 21, 25, 29, 18, 22, 26, 30, 19, 23, 27, 31);

	vec.r0 = _mm256_setzero_si256();
	vec.r1 = _mm256_shuffle_epi8(pixels, shuffle);
	vec.r2 = _mm256_unpackhi_epi8(vec.r1, vec.r0);
	vec.r3 = _mm256_unpacklo_epi8(vec.r1, vec.r0);
	out.b = _mm256_cvtepi32_ps(_mm256_unpacklo_epi16(vec.r3, vec.r0));
	out.g = _mm256_cvtepi32_ps(_mm256_unpackhi_epi16(vec.r3, vec.r0));
	out.r = _mm256_cvtepi32_ps(_mm256_unpacklo_epi16(vec.r2, vec.r0));
	out.a = _mm256_cvtepi32_ps(_mm256_unpackhi_epi16(vec.r2, vec.r0));
	return (out);
}

/**
 * One can clamp channels to [0, 255] if needed:
 * 	const __m256i	zero = _mm256_setzero_si256();
 * 	const __m256i	max255 = _mm256_set1_epi32(255);
 * 	rgba.b = _mm256_min_epi32(_mm256_max_epi32(rgba.b, zero), max255);
 * 	rgba.g = _mm256_min_epi32(_mm256_max_epi32(rgba.g, zero), max255);
 * 	rgba.r = _mm256_min_epi32(_mm256_max_epi32(rgba.r, zero), max255);
 * 	rgba.a = _mm256_min_epi32(_mm256_max_epi32(rgba.a, zero), max255);
 * @param blended
 * @return
 */
static inline __attribute__((always_inline))
__m256i	repack_rgba_floats_to_bytes_avx2(t_rgba_ps256 blended)
{
	__m256i			out;
	t_vec4i_avx		vec;
	t_rgba_si256	rgba;
	const __m256i	shuffle
		= _mm256_setr_epi8(0, 4, 8, 12, 1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15,
			16, 20, 24, 28, 17, 21, 25, 29, 18, 22, 26, 30, 19, 23, 27, 31);

	rgba.b = _mm256_cvtps_epi32(blended.b);
	rgba.g = _mm256_cvtps_epi32(blended.g);
	rgba.r = _mm256_cvtps_epi32(blended.r);
	rgba.a = _mm256_cvtps_epi32(blended.a);
	vec.r0 = _mm256_packs_epi32(rgba.r, rgba.a);
	vec.r1 = _mm256_packs_epi32(rgba.b, rgba.g);
	vec.r2 = _mm256_packus_epi16(vec.r1, vec.r0);
	out = _mm256_shuffle_epi8(vec.r2, shuffle);
	return (out);
}

static inline __attribute__((always_inline))
t_rgba_ps256	lerp_biased_unpvec(t_rgba_ps256 argb_a,
									t_rgba_ps256 argb_b, __m256 tt)
{
	t_rgba_ps256	out;
	const __m256	diff = _mm256_sub_ps(argb_b.a, argb_a.a);
	const __m256	mask = _mm256_cmp_ps(argb_a.a, argb_b.a, _CMP_LT_OQ);

	out.a = _mm256_add_ps(_mm256_mul_ps(diff, tt), argb_a.a);
	out.r = _mm256_blendv_ps(argb_b.r, argb_a.r, mask);
	out.g = _mm256_blendv_ps(argb_b.g, argb_a.g, mask);
	out.b = _mm256_blendv_ps(argb_b.b, argb_a.b, mask);
	return (out);
}

static inline __attribute__((always_inline))
t_rgba_ps256	dim_colour2_unpvec_avx2(t_rgba_ps256 in, float dim)
{
	t_rgba_ps256	out;
	__m256			dim_vec;
	__m256			alpha_vec;
	const __m256	alphas = _mm256_set1_ps(255.0f);
	const __m256	mask = _mm256_cmp_ps(in.a, alphas, _CMP_EQ_OQ);

	dim = fmaxf(0.0f, fminf(1.0f, dim));
	dim_vec = _mm256_set1_ps(dim);
	alpha_vec = _mm256_set1_ps((1.0f - dim) * 255.0f);
	out.a = alpha_vec;
	out.r = _mm256_mul_ps(in.r, dim_vec);
	out.g = _mm256_mul_ps(in.g, dim_vec);
	out.b = _mm256_mul_ps(in.b, dim_vec);
	out.a = _mm256_blendv_ps(out.a, in.a, mask);
	out.r = _mm256_blendv_ps(out.r, in.r, mask);
	out.g = _mm256_blendv_ps(out.g, in.g, mask);
	out.b = _mm256_blendv_ps(out.b, in.b, mask);
	return (out);
}

t_rgba4_ps256	get_and_unpack(__m256i xx, __m256i xx1, const int *row1,
								const int *row2)
{
	t_vec4i_avx		source;
	t_rgba4_ps256	out;

	source.r0 = _mm256_i32gather_epi32(row1, xx, sizeof(int));
	source.r1 = _mm256_i32gather_epi32(row1, xx1, sizeof(int));
	source.r2 = _mm256_i32gather_epi32(row2, xx, sizeof(int));
	source.r3 = _mm256_i32gather_epi32(row2, xx1, sizeof(int));
	out.r0 = unpack_rgba_bytes_to_floats_avx(source.r0);
	out.r1 = unpack_rgba_bytes_to_floats_avx(source.r1);
	out.r2 = unpack_rgba_bytes_to_floats_avx(source.r2);
	out.r3 = unpack_rgba_bytes_to_floats_avx(source.r3);
	return (out);
}

t_rgba4_ps256	dim_avx2_unp4(float dim, t_rgba4_ps256 s)
{
	const t_rgba4_ps256	d = {
		dim_colour2_unpvec_avx2(s.r0, dim),
		dim_colour2_unpvec_avx2(s.r1, dim),
		dim_colour2_unpvec_avx2(s.r2, dim),
		dim_colour2_unpvec_avx2(s.r3, dim),
	};
	return d;
}

/**
 * for branchless min and max we can use
 * #define MIN_BRANCHLESS(a, b) ((b) ^ (((a) ^ (b)) & -((a) < (b))))
 * #define MAX_BRANCHLESS(a, b) ((a) ^ (((a) ^ (b)) & -((a) < (b))))
 *
 * #include <sys/param.h> defines MIN() and MAX() macros like ternaries
 * they do expand to ternary expressions, which means:
 * 	They can generate branches
 * 	not safe in all circumstances, e.g.: MIN(i++, j++)
 *
 * @param idx
 * @param tex
 * @return
 */
void	draw_credits_avx2_unpacked(t_info *app, t_dummy *dummy)
{
	const t_tex	*tex = &app->shtex->credits;
	int			row;
	t_vect		dir[2];
	t_vect		pos[2];
	t_fvec256	weight;
	t_fvec256	currss;
	t_fvec256	idxx;
	float		depth;
	float		idx_y;
	t_fvec256	poss[2];
	float		step_x;
	float		falloff;
	float		inv;
	float		dim;
	t_ivect		lim;
	int			i;
	int			y1;
	u_int		*p_row;
	t_fma_avx2	fma;
	t_vec2i_avx	xx;
	t_fmodf_avx2	weight_xm;
	const __m256	half_ps = _mm256_set1_ps(0.5f);
	const __m256	one_ps = _mm256_set1_ps(1.0f);
	const __m256i	one_epi32 = _mm256_set1_epi32(1);
	const __m256i	max_val = _mm256_set1_epi32(tex->w - 1);

	update_rocks(app, dummy);
	dir[LEFT] = rotate_vect(dummy->dir, app->fov_rad_half);
	dir[RIGHT] = rotate_vect(dummy->dir, -app->fov_rad_half);
	row = 0;
	while (++row < WIN_HEIGHT)
	{
		depth = app->dummy->row_depths[row - 1];
		pos[LEFT] = add_vect(dummy->pos, scale_vect(dir[LEFT], depth));
		pos[RIGHT] = add_vect(dummy->pos, scale_vect(dir[RIGHT], depth));
		idx_y = (-pos[LEFT].y) * tex->w;
		weight.yy = _mm256_set1_ps(fmodf(idx_y, 1.0f));
		if (pos[LEFT].y > 0)
			continue;
		int y = (int) idx_y;
		if (y >= tex->h)
			break;
		poss[LEFT].xx = _mm256_set1_ps(pos[LEFT].x);
		step_x = (pos[RIGHT].x - pos[LEFT].x) / WIN_WIDTH;
		falloff = (depth - 1.5f) * 6.0f;
		inv = 1.0 / (falloff + DBL_EPSILON);
		dim = 1.0 + (inv - 1.0) * (falloff >= 1.0);
		p_row = (u_int *) app->overlay.data + app->overlay.width * row;
		y1 = y + ((((tex->h - 1) - (y + 1)) >> 31) ^ 1); // y1 = MIN(y + 1, tex->h - 1);
		lim.x = MAX(0, (-0.48 - pos[LEFT].x) / step_x);
		lim.y = MIN(WIN_WIDTH, (0.48 - pos[LEFT].x) / step_x);
		i = lim.x;
		while (i < lim.y - 7)
		{
			fma.step = _mm256_set1_ps(step_x);
			fma.indices = _mm256_add_epi32(_mm256_set1_epi32(i), _mm256_setr_epi32(0, 1, 2, 3, 4, 5, 6, 7));
			fma.offsets = _mm256_mul_ps(fma.step, _mm256_cvtepi32_ps(fma.indices));
			currss.xx = _mm256_add_ps(half_ps, _mm256_add_ps(poss[LEFT].xx, fma.offsets));
			idxx.xx = _mm256_mul_ps(currss.xx, _mm256_cvtepi32_ps(_mm256_set1_epi32(tex->w)));
			weight_xm.div = _mm256_div_ps(idxx.xx, one_ps);
			weight_xm.tdiv = _mm256_round_ps(weight_xm.div, _MM_FROUND_TO_ZERO | _MM_FROUND_NO_EXC);
			weight_xm.prod = _mm256_mul_ps(one_ps, weight_xm.tdiv);
			weight.xx = _mm256_sub_ps(idxx.xx, weight_xm.prod);
			xx.r0 = _mm256_cvttps_epi32(idxx.xx);
			xx.r1 = _mm256_min_epi32(_mm256_add_epi32(xx.r0, one_epi32), max_val);
			const int *row1 = (int *) tex->data + y * tex->w;
			const int *row2 = (int *) tex->data + y1 * tex->w;
			t_rgba4_ps256 d = dim_avx2_unp4(dim, get_and_unpack(xx.r0, xx.r1, row1, row2));
			t_rgba_ps256 top = lerp_biased_unpvec(d.r0, d.r1, weight.xx);
			t_rgba_ps256 bottom = lerp_biased_unpvec(d.r2, d.r3, weight.xx);
			_mm256_storeu_si256((__m256i_u *) &p_row[i], repack_rgba_floats_to_bytes_avx2(lerp_biased_unpvec(top, bottom, weight.yy)));
			i += 8;
		}
	}
	place_img_alpha_avx2_soa(app->canvas, &app->overlay, (t_point) {0, 0});
}
