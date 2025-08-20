/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   credits_avx2.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/04 17:52:50 by abelov            #+#    #+#             */
/*   Updated: 2025/08/10 17:39:38 by abelov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/param.h>
#include "cub3d.h"

void	calc_idxs(const t_info *app, const t_tex tex, t_params_ptr ptrs);
__m256i	get_dimmed(t_fvec256 weight, float d, t_cdata cd, t_vec2i_avx xx);

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
inline __attribute__((always_inline, used, externally_visible))
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

inline __attribute__((always_inline, used, externally_visible))
t_vec2i_avx	get_xx(t_ivect3 it, int *idx_xs)
{
	t_vec2i_avx		xx;

	xx.r0 = _mm256_loadu_si256(
			(__m256i_u *)&idx_xs[it.y * WIN_WIDTH + it.x]);
	xx.r1 = _mm256_loadu_si256(
			(__m256i_u *)&idx_xs[(it.y + WIN_HEIGHT) * WIN_WIDTH + it.x]);
	return (xx);
}

inline __attribute__((always_inline, used, externally_visible))
t_params_ptr	get_params_p(t_params *params)
{
	const t_params_ptr	ptr = (t_params_ptr)
	{
		.idx_xs = (*params).idx_xs,
		.y = (*params).y,
		.start_x = (*params).start_x,
		.stop_x = (*params).stop_x,
		.weight_y = (*params).weight_y,
		.weight_x = (*params).weight_x
	};

	return (ptr);
}

inline __attribute__((always_inline, used, externally_visible))
t_cdata	get_rows(const t_tex *tex, const int y0)
{
	const int		y1 = y0 + ((((tex->h - 1) - (y0 + 1)) >> 31) ^ 1);
	t_cdata const	cd = {
		(int *) tex->data + y0 * tex->w,
		(int *) tex->data + y1 * tex->w
	};

	return (cd);
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
void	draw_credits_avx2_unpacked(t_info *app, t_dummy *dummy,
									t_tex *tex, t_img overlay)
{
	static t_params	params;
	t_ivect3		it;
	t_fvec256		weight;
	int				y0;
	u_int			*dst;

	calc_idxs(app, *tex, get_params_p(&params));
	it.y = -1;
	while (++it.y < WIN_HEIGHT - 1)
	{
		y0 = params.y[it.y];
		if (y0 < 0 || y0 >= tex->h)
			continue ;
		it = (t_ivect3){.x = params.start_x[it.y], it.y, params.stop_x[it.y]};
		dst = (u_int *) overlay.data + overlay.width * (it.y + 1);
		weight.yy = _mm256_set1_ps(params.weight_y[it.y]);
		while (it.x < it.z - 7)
		{
			weight.xx = _mm256_loadu_ps(&params.weight_x[it.y * WW + it.x]);
			_mm256_storeu_si256((__m256i_u *)&dst[it.x], get_dimmed(
					weight, dummy->row_depths[it.y], get_rows(tex, y0),
					get_xx(it, params.idx_xs)));
			it.x += 8;
		}
	}
}
