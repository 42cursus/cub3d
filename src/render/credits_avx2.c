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
	const __m256i shuffle = _mm256_setr_epi8(
		0,  4,  8, 12,
		1,  5,  9, 13,
		2,  6, 10, 14,
		3,  7, 11, 15,

		16, 20, 24, 28,
		17, 21, 25, 29,
		18, 22, 26, 30,
		19, 23, 27, 31
	);
	const __m256i	zero = _mm256_setzero_si256();
	const __m256i	grouped = _mm256_shuffle_epi8(pixels, shuffle);

	const __m256i	ar_16 = _mm256_unpackhi_epi8(grouped, zero);
	const __m256i	gb_16 = _mm256_unpacklo_epi8(grouped, zero);

	out.b = _mm256_cvtepi32_ps(_mm256_unpacklo_epi16(gb_16, zero));
	out.g = _mm256_cvtepi32_ps(_mm256_unpackhi_epi16(gb_16, zero));
	out.r = _mm256_cvtepi32_ps(_mm256_unpacklo_epi16(ar_16, zero));
	out.a = _mm256_cvtepi32_ps(_mm256_unpackhi_epi16(ar_16, zero));

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
__m256i repack_rgba_floats_to_bytes_avx2(t_rgba_ps256 blended)
{
	__m256i			out;
	t_rgba_si256	rgba;

	const __m256i shuffle = _mm256_setr_epi8(
		0,  4,  8, 12,
		1,  5,  9, 13,
		2,  6, 10, 14,
		3,  7, 11, 15,

		16, 20, 24, 28,
		17, 21, 25, 29,
		18, 22, 26, 30,
		19, 23, 27, 31
	);

	rgba.b = _mm256_cvtps_epi32(blended.b);
	rgba.g = _mm256_cvtps_epi32(blended.g);
	rgba.r = _mm256_cvtps_epi32(blended.r);
	rgba.a = _mm256_cvtps_epi32(blended.a);

	__m256i ra_b = _mm256_packs_epi32(rgba.r, rgba.a);
	__m256i bg_b = _mm256_packs_epi32(rgba.b, rgba.g);

	__m256i rgba_a = _mm256_packus_epi16(bg_b, ra_b);

	out = _mm256_shuffle_epi8(rgba_a, shuffle);
	return (out);
}

static inline __attribute((always_inline))
t_rgba_ps256 lerp_biased_unpvec(t_rgba_ps256 argb_a, t_rgba_ps256 argb_b, __m256 tt)
{
	t_rgba_ps256	out;

	/* == START BLENDING === */
	__m256 diff = _mm256_sub_ps(argb_b.a, argb_a.a);
	__m256 res_a = _mm256_add_ps(_mm256_mul_ps(diff, tt), argb_a.a);
	__m256 mask = _mm256_cmp_ps(argb_a.a, argb_b.a, _CMP_LT_OQ);

	out.a = res_a;
	out.r = _mm256_blendv_ps(argb_b.r, argb_a.r, mask);
	out.g = _mm256_blendv_ps(argb_b.g, argb_a.g, mask);
	out.b = _mm256_blendv_ps(argb_b.b, argb_a.b, mask);
	/* == END BLENDING === */

	return (out);
}

/**
 * https://github.com/vpinball/pinmame/blob/master/ext/basicbitmap/BasicBitmap_SSE2.cpp
 * https://github.com/pjincz/qt-kso-integration/blob/wpsenv/src/gui/painting/qpathgradient_p_sse4.cpp#L110
 * @param color_vec
 * @param dim
 * @return
 */
static inline __attribute__((always_inline))
t_rgba_ps256	dim_colour2_unpvec_avx2(t_rgba_ps256 in, float dim)
{
	t_rgba_ps256	out;
	__m256			dim_vec;
	__m256			alpha_vec;

	__m256 alphas = _mm256_set1_ps(255.0f);

	__m256 mask = _mm256_cmp_ps(in.a, alphas, _CMP_EQ_OQ);

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
void draw_credits_avx2_unpacked(t_info *app, t_dummy *dummy)
{
	t_vect dir[2];
	t_vect pos[2];
	int row;

	int i;

	const t_tex *tex = &app->shtex->credits;

	__m256 half_ps = _mm256_set1_ps(0.5f);
	__m256 one_ps = _mm256_set1_ps(1.0f);
	__m256i one_epi32 = _mm256_set1_epi32(1);
	__m256i max_val = _mm256_set1_epi32(tex->w - 1);

	dir[LEFT] = rotate_vect(dummy->dir, app->fov_rad_half);
	dir[RIGHT] = rotate_vect(dummy->dir, -app->fov_rad_half);
	update_rocks(app, dummy);

	row = 0;
	while (++row < WIN_HEIGHT)
	{
		float depth = app->dummy->row_depths[row - 1];

		pos[LEFT] = add_vect(dummy->pos, scale_vect(dir[LEFT], depth));
		pos[RIGHT] = add_vect(dummy->pos, scale_vect(dir[RIGHT], depth));

		float idx_y = (-pos[LEFT].y) * tex->w;
		const __m256 weight_yy = _mm256_set1_ps(fmodf(idx_y, 1.0f));

		if (pos[LEFT].y > 0)
			continue;
		int y = (int) idx_y;
		if (y >= tex->h)
			break;

		__m256 pos_LEFT_x = _mm256_set1_ps(pos[LEFT].x);

		float step_x = (pos[RIGHT].x - pos[LEFT].x) / WIN_WIDTH;

		const float falloff = (depth - 1.5f) * 6.0f;
		float inv = 1.0 / (falloff + DBL_EPSILON);
		float dim = 1.0 + (inv - 1.0) * (falloff >= 1.0);

		t_vect lim = {-0.48, 0.48}; // Relative to 1 block on the map

		u_int *const p_row = (u_int *) app->overlay.data + app->overlay.width * row;

		int y1 = y + ((((tex->h - 1) - (y + 1)) >> 31) ^ 1); // y1 = MIN(y + 1, tex->h - 1);

		int start = MAX(0, (lim.x - pos[LEFT].x) / step_x);
		int stop = MIN(WIN_WIDTH, (lim.y - pos[LEFT].x) / step_x);

		i = start;
		while (i < stop - 7)
		{

			__m256i initial = _mm256_setr_epi32(0, 1, 2, 3, 4, 5, 6, 7);
			__m256i ii = _mm256_set1_epi32(i);

			__m256 step_xx = _mm256_set1_ps(step_x);
			__m256 scaled_xx = _mm256_mul_ps(step_xx, _mm256_cvtepi32_ps( _mm256_add_epi32(ii, initial)));

			__m256 currs_xx = _mm256_add_ps(half_ps, _mm256_add_ps(pos_LEFT_x, scaled_xx));
			__m256 idx_xx = _mm256_mul_ps(currs_xx, _mm256_cvtepi32_ps(_mm256_set1_epi32(tex->w)));

			/* ===============bilinear_filter=============== */

			// fmodf(x, y) == (x - y * trunc(x / y))
			// https://hugeonotation.github.io/pblog/2024/06/07/fmod.html
			__m256 div = _mm256_div_ps(idx_xx, one_ps);
			__m256 trunc_div = _mm256_round_ps(div, _MM_FROUND_TO_ZERO | _MM_FROUND_NO_EXC);
			__m256 prod = _mm256_mul_ps(one_ps, trunc_div);
			__m256 weight_xx = _mm256_sub_ps(idx_xx, prod);

			__m256i xx = _mm256_cvttps_epi32(idx_xx);
			__m256i xx1 = _mm256_min_epi32(_mm256_add_epi32(xx, one_epi32), max_val);

			// Load the 2x2 texels
			const int *row1 = (int *) tex->data + y * tex->w;
			const int *row2 = (int *) tex->data + y1 * tex->w;

			t_vec4i_avx source;

			source.r0 = _mm256_i32gather_epi32(row1, xx, sizeof(int));
			source.r1 = _mm256_i32gather_epi32(row1, xx1, sizeof(int));
			source.r2 = _mm256_i32gather_epi32(row2, xx, sizeof(int));
			source.r3 = _mm256_i32gather_epi32(row2, xx1, sizeof(int));

			t_rgba_ps256 src_a = unpack_rgba_bytes_to_floats_avx(source.r0);
			t_rgba_ps256 src_b = unpack_rgba_bytes_to_floats_avx(source.r1);
			t_rgba_ps256 src_c = unpack_rgba_bytes_to_floats_avx(source.r2);
			t_rgba_ps256 src_d = unpack_rgba_bytes_to_floats_avx(source.r3);

			t_rgba_ps256 dimmed_a = dim_colour2_unpvec_avx2(src_a, dim);
			t_rgba_ps256 dimmed_b = dim_colour2_unpvec_avx2(src_b, dim);
			t_rgba_ps256 dimmed_c = dim_colour2_unpvec_avx2(src_c, dim);
			t_rgba_ps256 dimmed_d = dim_colour2_unpvec_avx2(src_d, dim);

			t_rgba_ps256 top = lerp_biased_unpvec(dimmed_a, dimmed_b, weight_xx);
			t_rgba_ps256 bottom = lerp_biased_unpvec(dimmed_c, dimmed_d, weight_xx);
			t_rgba_ps256 outf = lerp_biased_unpvec(top, bottom, weight_yy);

			__m256i rgba = repack_rgba_floats_to_bytes_avx2(outf);

			_mm256_storeu_si256((__m256i_u *) &p_row[i], rgba);

			/* ============================================== */
			i += 8;
		}

//		while (i < stop)
//		{
//			curr_x = pos[LEFT].x + (step_x * i);
//
//			idx.x = (0.5 + curr_x) * tex->w;
//			idx.y = idx_y;
//
//			/* ===============bilinear_filter=============== */
//
//			const int x = (int) idx.x;
//			const int y = (int) idx.y;
//
//			const double weight_x = idx.x - x;
//			const double weight_y = idx.y - y;
//
//			int x1 = x + ((((tex->w - 1) - (x + 1)) >> 31) ^ 1); // x1 = MIN(x + 1, tex->w - 1);
//			int y1 = y + ((((tex->h - 1) - (y + 1)) >> 31) ^ 1); // y1 = MIN(y + 1, tex->h - 1);
//
//			// Load the 2x2 texels
//			u_int *row1 = tex->data + y * tex->w;
//			u_int *row2 = tex->data + y1 * tex->w;
//
////			double dim = falloff >= 1.0 ? 1.0 / falloff : 1.0;
//
//			double inv = 1.0 / (falloff + DBL_EPSILON);
//			double dim = 1.0 + (inv - 1.0) * (falloff >= 1.0);
//
//			u_int a = dim_colour2(row1[x], dim);
//			u_int b = dim_colour2(row1[x1], dim);
//			u_int c = dim_colour2(row2[x], dim);
//			u_int d = dim_colour2(row2[x1], dim);
//
//			u_int top = lerp_biased(a, b, weight_x);
//			u_int bottom = lerp_biased(c, d, weight_x);
//			u_int out = lerp_biased(top, bottom, weight_y);
//
//			/* ================dim_colour_alpha============== */
////					p_row[i] = dim_colour_alpha(src, falloff).raw;
//			p_row[i] = out;
//			/* ============================================== */
//
////			curr_y += step_y;
//			i++;
//		}
	}
	t_point p = (t_point) {0, 0};
	place_img_alpha_avx2_soa(app->canvas, &app->overlay, p);
}
