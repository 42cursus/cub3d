/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   credits_avx2.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/04 17:52:50 by abelov            #+#    #+#             */
/*   Updated: 2025/08/08 20:21:18 by abelov           ###   ########.fr       */
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

inline __attribute__((always_inline, used))
t_rgba4_ps256	get_and_unpack(t_vec2i_avx xx, const int **row)
{
	t_vec4i_avx		source;
	t_rgba4_ps256	out;

	source.r0 = _mm256_i32gather_epi32(row[0], xx.r0, sizeof(int));
	source.r1 = _mm256_i32gather_epi32(row[0], xx.r1, sizeof(int));
	source.r2 = _mm256_i32gather_epi32(row[1], xx.r0, sizeof(int));
	source.r3 = _mm256_i32gather_epi32(row[1], xx.r1, sizeof(int));
	out.r0 = unpack_rgba_bytes_to_floats_avx(source.r0);
	out.r1 = unpack_rgba_bytes_to_floats_avx(source.r1);
	out.r2 = unpack_rgba_bytes_to_floats_avx(source.r2);
	out.r3 = unpack_rgba_bytes_to_floats_avx(source.r3);
	return (out);
}

inline __attribute__((always_inline, used))
t_rgba4_ps256	dim_avx2_unp4(float dim, t_rgba4_ps256 s)
{
	const t_rgba4_ps256	d = {
		dim_colour2_unpvec_avx2(s.r0, dim),
		dim_colour2_unpvec_avx2(s.r1, dim),
		dim_colour2_unpvec_avx2(s.r2, dim),
		dim_colour2_unpvec_avx2(s.r3, dim),
	};

	return (d);
}

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
float	get_dim(float depth)
{
	const float	falloff = (depth - 1.5f) * 6.0f;
	const float	inv = 1.0f / (falloff + FLT_EPSILON);
	const float	multiplier = (float)(falloff >= 1.0f);
	const float	dim = 1.0f + (inv - 1.0f) * multiplier;

	return (dim);
}

inline __attribute__((always_inline, used))
__m256i	get_dimmed(t_fvec256 weight, float d, const int **rows, t_vec2i_avx xx)
{
	__m256i			ret;
	t_rgba4_ps256	dim;

	dim = dim_avx2_unp4(get_dim(d), get_and_unpack(xx, rows));
	ret = repack_rgba_floats_to_bytes_avx2(lerp_biased_unpvec(
				lerp_biased_unpvec(dim.r0, dim.r1, weight.xx),
				lerp_biased_unpvec(dim.r2, dim.r3, weight.xx), weight.yy));
	return (ret);
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
t_vec2i_avx	get_xx(__m256 id_xx, const int tex_w)
{
	t_vec2i_avx		xx;
	const __m256i	max_val = _mm256_set1_epi32(tex_w - 1);
	const __m256i	one_epi32 = _mm256_set1_epi32(1);

	xx.r0 = _mm256_cvttps_epi32(id_xx);
	xx.r1 = _mm256_min_epi32(_mm256_add_epi32(xx.r0, one_epi32), max_val);
	return (xx);
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
	const t_img	overlay = app->overlay;
	t_ivect3	it;
	t_fvec256	weight;
	t_vect		pos[2];
	float		depth;
	float		idx_y;
	float		step_x;
	int			y[2];
	u_int		*dst;
	__m256		id_xx;

	update_rocks(app, dummy);
	it.y = 0;
	while (++it.y < WIN_HEIGHT)
	{
		depth = app->dummy->row_depths[it.y - 1];
		calc_pos(app, dummy, pos, depth);
		idx_y = (-pos[LEFT].y) * tex->w;
		weight.yy = _mm256_set1_ps(fmodf(idx_y, 1.0f));
		if (pos[LEFT].y <= 0)
		{
			y[0] = (int) idx_y;
			y[1] = y[0] + ((((tex->h - 1) - (y[0] + 1)) >> 31) ^ 1);
			if (y[0] < tex->h)
			{
				step_x = (pos[RIGHT].x - pos[LEFT].x) / WIN_WIDTH;
				dst = (u_int *) overlay.data + overlay.width * it.y;
				it.z = MIN(WIN_WIDTH, (0.48 - pos[LEFT].x) / step_x);
				it.x = MAX(0, (-0.48 - pos[LEFT].x) / step_x);
				while (it.x < it.z - 7)
				{
					id_xx = get_idxx(tex->w, pos[LEFT], get_fma(step_x, it.x));
					weight.xx = calc_weights(id_xx);
					_mm256_storeu_si256((__m256i_u *) & dst[it.x],
						get_dimmed(weight, depth, (const int *[2]){
							(int *)tex->data + y[0] * tex->w,
							(int *)tex->data + y[1] * tex->w
						}, get_xx(id_xx, tex->w)));
					it.x += 8;
				}
			}
		}
	}
	place_img_alpha_avx2_soa(app->canvas, &overlay, (t_point){0, 0});
}
