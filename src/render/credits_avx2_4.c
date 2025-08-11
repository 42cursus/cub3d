/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   credits_avx2_4.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 15:58:12 by abelov            #+#    #+#             */
/*   Updated: 2025/08/11 15:58:13 by abelov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

t_rgba4_ps256	dim_avx2_unp4(float dim, t_rgba4_ps256 s);
void			calc_pos(const t_info *app, t_dummy *dummy,
					t_vect *pos, float d);
t_vec2i_avx		calc_xx(__m256 id_xx, const int tex_w);
__m256			calc_weights(__m256 idxx);
__m256			get_idxx(const int tex_w, t_vect v, __m256 offsets);
__m256			get_fma(float step_x, int x);
t_rgba4_ps256	get_and_unpack(t_vec2i_avx xx, const int **row);
__m256i			repack_rgba_floats_to_bytes_avx2(t_rgba_ps256 blended);
t_rgba_ps256	lerp_biased_unpvec(t_rgba_ps256 argb_a, t_rgba_ps256 argb_b,
					__m256 tt);

inline __attribute__((always_inline, used))
void	calc_fvars(const t_info *app, t_ivect3 it, t_vect *pos, float *fvars)
{
	t_dummy *const	dummy = app->dummy;
	const t_tex		*tex = &app->shtex->credits;

	fvars[DEPTH] = dummy->row_depths[it.y];
	calc_pos(app, dummy, pos, fvars[DEPTH]);
	fvars[IDX_Y] = (-pos[LEFT].y) * tex->w;
	fvars[STEP_X] = (pos[RIGHT].x - pos[LEFT].x) / WIN_WIDTH;
}

inline __attribute__((always_inline, used))
void	store_weights_idxs(const t_tex tex, t_params_ptr ptrs, t_ivect3 it,
							__m256 id)
{
	t_vec2i_avx	xx;
	__m256i_u	*row_one;
	__m256i_u	*row_two;

	xx = calc_xx(id, tex.w);
	row_one = (__m256i_u *)&ptrs.idx_xs[it.y * WIN_WIDTH + it.x];
	row_two = (__m256i_u *)&ptrs.idx_xs[(it.y + WIN_HEIGHT) * WIN_WIDTH + it.x];
	_mm256_storeu_ps(&ptrs.weight_x[it.y * WIN_WIDTH + it.x], calc_weights(id));
	_mm256_storeu_si256(row_one, xx.r0);
	_mm256_storeu_si256(row_two, xx.r1);
}

inline __attribute__((always_inline, used))
void	calc_idxs(const t_info *app, const t_tex tex, t_params_ptr ptrs)
{
	t_ivect3		it;
	t_vect			pos[2];
	__m256			id_xx;
	float			fvars[CALC_IDXS_MAX];

	it.y = -1;
	while (++it.y < WIN_HEIGHT)
	{
		calc_fvars(app, it, pos, fvars);
		ptrs.weight_y[it.y] = fmodf(fvars[IDX_Y], 1.0f);
		ptrs.y[it.y] = (int)fvars[IDX_Y];
		if (pos[LEFT].y > 0)
			continue ;
		it.x = MAX(0, (-0.48 - pos[LEFT].x) / fvars[STEP_X]);
		it.z = MIN(WIN_WIDTH, (0.48 - pos[LEFT].x) / fvars[STEP_X]);
		ptrs.start_x[it.y] = it.x;
		ptrs.stop_x[it.y] = it.z;
		while (it.x < it.z - 7)
		{
			id_xx = get_idxx(tex.w, pos[LEFT], get_fma(fvars[STEP_X], it.x));
			store_weights_idxs(tex, ptrs, it, id_xx);
			it.x += 8;
		}
	}
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
__m256i	get_dimmed(t_fvec256 weight, float d, t_cdata cd, t_vec2i_avx xx)
{
	__m256i			ret;
	t_rgba4_ps256	dim;
	int const		*rows[2] = {cd.src, cd.dst};

	dim = dim_avx2_unp4(get_dim(d), get_and_unpack(xx, rows));
	ret = repack_rgba_floats_to_bytes_avx2(lerp_biased_unpvec(
				lerp_biased_unpvec(dim.r0, dim.r1, weight.xx),
				lerp_biased_unpvec(dim.r2, dim.r3, weight.xx), weight.yy));
	return (ret);
}
