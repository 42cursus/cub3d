/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   credits.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 16:14:27 by fsmyth            #+#    #+#             */
/*   Updated: 2025/06/04 16:58:16 by abelov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <math.h>
#include <sys/param.h>
#include "cub3d.h"

static inline __attribute__((always_inline, unused))
t_rgba_ps128	unpack_rgba_bytes_to_floats(__m128i pixels)
{
	t_rgba_ps128	out;
	const __m128i	shuffle = _mm_set_epi8(15, 11, 7, 3, 14, 10, 6, 2, 13, 9, 5, 1, 12, 8, 4, 0);
	const __m128i	zero = _mm_setzero_si128();
	const __m128i	grouped = _mm_shuffle_epi8(pixels, shuffle);

	const __m128i	ar_16 = _mm_unpackhi_epi8(grouped, zero);
	const __m128i	gb_16 = _mm_unpacklo_epi8(grouped, zero);

	out.b = _mm_cvtepi32_ps(_mm_unpacklo_epi16(gb_16, zero));
	out.g = _mm_cvtepi32_ps(_mm_unpackhi_epi16(gb_16, zero));
	out.r = _mm_cvtepi32_ps(_mm_unpacklo_epi16(ar_16, zero));
	out.a = _mm_cvtepi32_ps(_mm_unpackhi_epi16(ar_16, zero));

	return (out);
}

static inline __attribute__((always_inline, unused))
__m128i repack_rgba_floats_to_bytes_sse(t_rgba_ps128 blended)
{
	__m128i			out;
	t_rgba_si128	rgba;

	const __m128i	shuffle = _mm_set_epi8(15, 11, 7, 3, 14, 10, 6, 2, 13, 9, 5, 1, 12, 8, 4, 0);

	rgba.b = _mm_cvtps_epi32(blended.b);
	rgba.g = _mm_cvtps_epi32(blended.g);
	rgba.r = _mm_cvtps_epi32(blended.r);
	rgba.a = _mm_cvtps_epi32(blended.a);

	// Clamp channels to [0, 255]
//	const __m128i	zero = _mm_setzero_si128();
//	const __m128i	max255 = _mm_set1_epi32(255);
//	rgba.b = _mm_min_epi32(_mm_max_epi32(rgba.b, zero), max255);
//	rgba.g = _mm_min_epi32(_mm_max_epi32(rgba.g, zero), max255);
//	rgba.r = _mm_min_epi32(_mm_max_epi32(rgba.r, zero), max255);
//	rgba.a = _mm_min_epi32(_mm_max_epi32(rgba.a, zero), max255);

	__m128i ra_b = _mm_packs_epi32(rgba.r, rgba.a);
	__m128i bg_b = _mm_packs_epi32(rgba.b, rgba.g);

	__m128i rgba_a = _mm_packus_epi16(bg_b, ra_b);

	out = _mm_shuffle_epi8(rgba_a, shuffle);
	return (out);
}

/**
 * https://en.wikipedia.org/wiki/Linear_interpolation
 * @param a
 * @param b
 * @param t
 * @return
 */
static inline __attribute((always_inline))
t_colour lerp_biased_c(t_colour a, t_colour b, double t)
{
	t_colour result;
	uint32_t mask;

	mask = -(a.a < b.a);
	// Choose RGB from more opaque color (lower alpha)
	result.raw = ((a.raw & mask) | (b.raw & ~mask));
	result.a = (unsigned char) ((b.a - a.a) * t + a.a);
	return result;
}

static inline __attribute((always_inline, unused))
u_int lerp_biased(u_int aa, u_int bb, float t)
{
	t_colour result;
	uint32_t mask;

	t_colour a = {.raw = aa};
	t_colour b = {.raw = bb};

	mask = -(a.a < b.a);
	// Choose RGB from more opaque color (lower alpha)
	result.raw = ((a.raw & mask) | (b.raw & ~mask));
	result.a = (u_char) ((b.a - a.a) * t + a.a);
	return result.raw;
}

static inline __attribute((always_inline, unused))
t_rgba_ps128 lerp_biased_unpvec(t_rgba_ps128 argb_a, t_rgba_ps128 argb_b, __m128 tt)
{
	t_rgba_ps128	out;

	/* == START BLENDING === */
	__m128 diff = _mm_sub_ps(argb_b.a, argb_a.a);
	__m128 res_a = _mm_add_ps(_mm_mul_ps(diff, tt), argb_a.a);
	__m128 mask = _mm_cmplt_ps(argb_a.a, argb_b.a);

	out.a = res_a;
	out.r = _mm_blendv_ps(argb_b.r, argb_a.r, mask);
	out.g = _mm_blendv_ps(argb_b.g, argb_a.g, mask);
	out.b = _mm_blendv_ps(argb_b.b, argb_a.b, mask);
	/* == END BLENDING === */

	return (out);
}

static inline __attribute((always_inline, unused))
__m128i lerp_biased_vec(__m128i aa, __m128i bb, __m128 tt)
{

	t_m128i			mc;

	t_rgba_ps128	argb_a = unpack_rgba_bytes_to_floats(aa);
	t_rgba_ps128	argb_b = unpack_rgba_bytes_to_floats(bb);

	/* == START BLENDING === */
	__m128 diff = _mm_sub_ps(argb_b.a, argb_a.a);
	__m128 res_a = _mm_add_ps(_mm_mul_ps(diff, tt), argb_a.a);

	/* == END BLENDING === */

	mc.mask = _mm_castps_si128(_mm_cmplt_ps(argb_a.a, argb_b.a));

	argb_a.a = res_a;
	argb_b.a = res_a;

	mc.dst = repack_rgba_floats_to_bytes_sse(argb_b);
	mc.src = repack_rgba_floats_to_bytes_sse(argb_a);
	mc.blend = _mm_blendv_epi8(mc.dst, mc.src, mc.mask);

//	__m128 mask = _mm_castsi128_ps(_mm_cmplt_epi32(aa_a, aa_b));
//	__m128 result = _mm_blendv_ps(_mm_castsi128_ps(mc.dst), _mm_castsi128_ps(mc.src), mask);
//	mc.blend = _mm_castps_si128(result);

//	mc.blend = _mm_or_si128(
//		_mm_andnot_si128(mc.mask, rgba_b),
//		_mm_and_si128(mc.mask, rgba_a)
//	);

	return mc.blend;
}

static inline __attribute__((always_inline, unused))
t_colour linear_filter(t_vect idx, const t_tex *tex)
{
	const double	frac = fmod(idx.x, 1.0);
	int				x = (int) idx.y * tex->w + (int) (idx.x);

	t_colour		left = *(t_colour *) &tex->data[x];
	t_colour		right = *(t_colour *) &tex->data[x + 1];
	t_colour		out = lerp_biased_c(left, right, frac);
	return (out);
}

static inline __attribute__((always_inline, unused))
int interpolate_colour_inline(int col1, int col2, double frac)
{
	int r;
	int g;
	int b;

	if (col1 != col2 && col1 != (int) XPM_TRANSPARENT)
	{
		r = ((col2 & MLX_RED) - (col1 & MLX_RED)) * frac + (col1 & MLX_RED);
		g = ((col2 & MLX_GREEN) - (col1 & MLX_GREEN)) * frac + (col1 & MLX_GREEN);
		b = ((col2 & MLX_BLUE) - (col1 & MLX_BLUE)) * frac + (col1 & MLX_BLUE);
		col1 = (r & MLX_RED) + (g & MLX_GREEN) + b;
	}
	return (col1);
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
//inline __attribute__((always_inline, unused, visibility("hidden")))
static inline __attribute__((optnone, used))
t_colour bilinear_filter(t_vect idx, const t_tex *tex)
{
	const int		x = (int) idx.x;
	const int		y = (int) idx.y;

	const double	frac_x = idx.x - x;
	const double	frac_y = idx.y - y;

	int				x1;
	int				y1;

	x1 = x + (((tex->w - 2 - x) >> 31) ^ 1); // x1 = MIN(x + 1, tex->w - 1);
	y1 = y + (((tex->h - 2 - y) >> 31) ^ 1); // y1 = MIN(y + 1, tex->h - 1);

	// Load the 2x2 texels
	u_int *row1 = tex->data + y * tex->w;
	u_int *row2 = tex->data + y1 * tex->w;

	u_int top = lerp_biased(row1[x], row1[x1], frac_x);
	u_int bottom = lerp_biased(row2[x], row2[x1], frac_x);
	t_colour out = {.raw = lerp_biased(top, bottom, frac_y)};

	return (out);
}

static inline __attribute__((always_inline, unused))
int bilinear_filter_old(double x, double y, const t_tex *tex)
{
	int x_lower;
	int x_upper;
	int y_lower;
	int y_upper;
	int interp;

	x_lower = (int) x;
	y_lower = (int) y;
	x_upper = x_lower + 1;
	y_upper = y_lower + 1;

	if (x_upper == tex->w)
		x_upper = 0;

	int col1 = (int) tex->data[y_lower * tex->w + x_lower];
	int col2 = (int) tex->data[y_lower * tex->w + x_upper];
	if (y_upper != tex->h)
	{
		int col3 = (int) tex->data[y_upper * tex->w + x_lower];
		int col4 = (int) tex->data[y_upper * tex->w + x_upper];

		col1 = interpolate_colour_inline(col1, col2, fmod(x, 1));
		col2 = interpolate_colour_inline(col3, col4, fmod(x, 1));
	}
	interp = interpolate_colour_inline(col1, col2, fmod(y, 1));
	return interp;
}

static inline __attribute__((always_inline, unused))
t_colour dim_colour_alpha(t_colour src, double falloff)
{
	if (falloff < 1 || src.raw == XPM_TRANSPARENT)
		return (src);

	const double	dim = 1.0 / falloff;
	double			alpha = (1 - dim);

	alpha = alpha > 228 ? 255 : alpha;

	src.a = (u_char)alpha;
	src.r = (u_char)(src.r * dim);
	src.g = (u_char)(src.g * dim);
	src.b = (u_char)(src.b * dim);
	return (src);
}

/**
 * dim is actually opacity and % of colour
 * if dim is 1 => colour stays the same
 * if dim is 0 => colour is black and pixel is fully transparent
 * @param col
 * @param dim
 * @return
 */
static inline __attribute__((unused))
u_int dim_colour2_scal(u_int col, double dim)
{
	u_int result;
	t_colour src = {.raw = col};

	dim = fmax(0.0, fmin(1.0, dim));

	double alpha = (1.0 - dim) * 255;

	src.a = (u_char) (alpha);
	src.r = (u_char) (src.r * dim);
	src.g = (u_char) (src.g * dim);
	src.b = (u_char) (src.b * dim);

	u_int mask = -(col == XPM_TRANSPARENT);
	result = (src.raw & ~mask) | (col & mask);
	return (result);
}


static inline __attribute__((always_inline))
t_rgba_ps128	dim_colour2_unpvec(t_rgba_ps128 in, float dim)
{
	dim = fmaxf(0.0f, fminf(1.0f, dim));

	__m128 dim_vec = _mm_set1_ps(dim);
	__m128 alpha_vec = _mm_set1_ps((1.0f - dim) * 255.0f);

	t_rgba_ps128	out;

	out.g = _mm_mul_ps(in.g, dim_vec);
	out.r = _mm_mul_ps(in.r, dim_vec);
	out.a = _mm_mul_ps(in.a, dim_vec);

//	mc.mask = _mm_cmpeq_epi32(color_vec, mc.transparent);
//
//	out.b = alpha_vec;
//	out.g = _mm_blendv_epi8(in.g, out.g, mc.mask);
//	out.r = _mm_blendv_epi8(in.r, out.r, mc.mask);
//	out.a = _mm_blendv_epi8(in.a, out.a, mc.mask);
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
__m128i	dim_colour2_vec(__m128i color_vec, float dim)
{
	t_m128i			mc = {.src = color_vec};

	mc.transparent = _mm_set1_epi32(XPM_TRANSPARENT);
	dim = fmaxf(0.0f, fminf(1.0f, dim));

	__m128 dim_vec = _mm_set1_ps(dim);
	__m128 alpha_vec = _mm_set1_ps((1.0f - dim) * 255.0f);

	const __m128i shuffle = _mm_set_epi8(15, 11, 7, 3, 14, 10, 6, 2, 13, 9, 5, 1, 12, 8, 4, 0);

	__m128i grouped = _mm_shuffle_epi8(mc.src, shuffle);

	__m128i zero = _mm_setzero_si128();

	__m128i lo_16 = _mm_unpacklo_epi8(grouped, zero);
	__m128i hi_16 = _mm_unpackhi_epi8(grouped, zero);

	__m128i aa = _mm_cvttps_epi32(alpha_vec);
	__m128i rr = _mm_unpacklo_epi16(lo_16, zero);
	__m128i gg = _mm_unpackhi_epi16(lo_16, zero);
	__m128i bb = _mm_unpacklo_epi16(hi_16, zero);

	rr = _mm_cvtps_epi32(_mm_mul_ps(_mm_cvtepi32_ps(rr), dim_vec));
	gg = _mm_cvtps_epi32(_mm_mul_ps(_mm_cvtepi32_ps(gg), dim_vec));
	bb = _mm_cvtps_epi32(_mm_mul_ps(_mm_cvtepi32_ps(bb), dim_vec));

	// Repack back into 0xAARRGGBB
	__m128i rg = _mm_packs_epi32(rr, gg);
	__m128i ba = _mm_packs_epi32(bb, aa);
	__m128i rgba = _mm_packus_epi16(rg, ba);

	mc.dst = _mm_shuffle_epi8(rgba, shuffle);
	mc.mask = _mm_cmpeq_epi32(color_vec, mc.transparent);
	mc.blend = _mm_blendv_epi8(mc.dst, mc.src, mc.mask);
	return (mc.blend);
}

void draw_credits_row(t_info *app, t_vect l_pos, t_vect r_pos, int row)
{
	const t_tex		*tex = &app->shtex->credits;
	int				i;
	double			step_x;
	double			curr_x;
	t_vect			idx;
	double			dist = app->dummy->row_depths[row - 1];
	t_vect			lim = {-0.48, 0.48}; // Relative to 1 block on the map
	u_int *const	p_row = (u_int *)app->overlay->data + app->overlay->width * row;

	step_x = (r_pos.x - l_pos.x) / WIN_WIDTH;
	curr_x = l_pos.x;
	idx.y = (-l_pos.y) * tex->w;
	if (l_pos.y > 0 || idx.y > tex->h)
		return;
	i = -1;
	while (++i < WIN_WIDTH)
	{
		if (curr_x > lim.x && curr_x < lim.y)
		{
			idx.x = (0.5 + curr_x) * tex->w;
			p_row[i] = dim_colour_alpha(bilinear_filter(idx, tex),
										(dist - 1.5) * 6).raw;
		}
		curr_x += step_x;
	}
}

void update_rocks(t_info *app, t_dummy *dummy)
{
	t_rock *rock;
	t_list *current;

	current = dummy->rocks;
	while (current != NULL)
	{
		rock = current->data;
		place_tex_to_image_scale(app->canvas, rock->tex, round_vect(rock->pos),
								 rock->scale);
		rock->pos.x += rock->speed;
		current = current->next;
	}
}

int cmp_rock_speed(void *data1, void *data2)
{
	t_rock *rock1;
	t_rock *rock2;

	rock1 = data1;
	rock2 = data2;
	if (rock1->speed > rock2->speed)
		return (1);
	return (-1);
}

void spawn_rock(t_info *app, t_vect pos, t_tex *tex, double speed)
{
	t_rock *rock;

	rock = ft_calloc(1, sizeof(*rock));
	rock->pos = pos;
	rock->speed = speed;
	rock->tex = tex;
	ft_lstadd_back(&app->dummy->rocks, ft_lstnew(rock));
}

void spawn_random_rock(t_info *app, double speed)
{
	t_rock *rock;
	int index;

	rock = ft_calloc(1, sizeof(*rock));
	rock->scale = 3;
	rock->pos.x = rand_range(0, WIN_WIDTH - 50);
	if (speed == 0)
		speed = rand_range(-6.0, 6.0);
	if (fabs(speed) > 4)
	{
		index = rand_range(0, 2);
		rock->tex = &app->shtex->rocks[index];
		if (fabs(speed) > 5)
			rock->scale = 4;
	}
	else
	{
		index = rand_range(2, 6);
		rock->tex = &app->shtex->rocks[index];
		if (fabs(speed) > 3)
			rock->scale = 4;
		if (fabs(speed) < 1.5)
			rock->scale = 2;
	}
	rock->pos.y =
		((WIN_HEIGHT - 80) / 12.0) * fabs(speed) * 2 + rand_range(-100, 100);
	if (rock->pos.y < 0)
		rock->pos.y = rand_range(0, 100);
	if (rock->pos.y > WIN_HEIGHT)
		rock->pos.y = WIN_HEIGHT - rand_range(50, 150);
	rock->speed = speed;
	ft_lstadd_back(&app->dummy->rocks, ft_lstnew(rock));
}

void draw_credits(t_info *app, t_dummy *dummy)
{
	t_vect l_dir;
	t_vect r_dir;
	t_vect l_pos;
	t_vect r_pos;
	int row;

	row = 0;
	l_dir = rotate_vect(dummy->dir, app->fov_rad_half);
	r_dir = rotate_vect(dummy->dir, -app->fov_rad_half);
	update_rocks(app, dummy);
	while (++row < WIN_HEIGHT)
	{
		l_pos = add_vect(dummy->pos,
						 scale_vect(l_dir, dummy->row_depths[row - 1]));
		r_pos = add_vect(dummy->pos,
						 scale_vect(r_dir, dummy->row_depths[row - 1]));
		draw_credits_row(app, l_pos, r_pos, row);
	}
	place_img_alpha_sse(app->canvas, app->overlay, (t_point) {0, 0});
}

void draw_credits_sse4_unpacked(t_info *app, t_dummy *dummy)
{
	t_vect dir[2];
	t_vect pos[2];
	int row;


	int i;

	const t_tex *tex = &app->shtex->credits;

	__m128 half_ps = _mm_set1_ps(0.5f);
	__m128 one_ps = _mm_set1_ps(1.0f);
	__m128i one_epi32 = _mm_set1_epi32(1);
	__m128i max_val = _mm_set1_epi32(tex->w - 1);

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
		const __m128 weight_yy = _mm_set1_ps(fmodf(idx_y, 1.0f));

		if (pos[LEFT].y > 0)
			continue;
		if (idx_y > tex->h)
			break;

		__m128 pos_LEFT_x = _mm_set1_ps(pos[LEFT].x);

		float step_x = (pos[RIGHT].x - pos[LEFT].x) / WIN_WIDTH;

		const float falloff = (depth - 1.5f) * 6.0f;
		float inv = 1.0 / (falloff + DBL_EPSILON);
		float dim = 1.0 + (inv - 1.0) * (falloff >= 1.0);

		t_vect lim = {-0.48, 0.48}; // Relative to 1 block on the map

		u_int *const p_row = (u_int *) app->overlay->data + app->overlay->width * row;

		int y = (int) idx_y;
		int y1 = y + ((((tex->h - 1) - (y + 1)) >> 31) ^ 1); // y1 = MIN(y + 1, tex->h - 1);

		int start = MAX(0, (lim.x - pos[LEFT].x) / step_x);
		int stop = MIN(WIN_WIDTH, (lim.y - pos[LEFT].x) / step_x);

		i = start;
		while (i < stop - 3)
		{

			__m128i initial = _mm_setr_epi32(0, 1, 2, 3);
			__m128i ii = _mm_set1_epi32(i);

			__m128 step_xx = _mm_set1_ps(step_x);
			__m128 scaled_xx = _mm_mul_ps(step_xx, _mm_cvtepi32_ps( _mm_add_epi32(ii, initial)));

			__m128 currs_xx = _mm_add_ps(half_ps, _mm_add_ps(pos_LEFT_x, scaled_xx));
			__m128 idx_xx = _mm_mul_ps(currs_xx, _mm_cvtepi32_ps(_mm_set1_epi32(tex->w)));

			/* ===============bilinear_filter=============== */

			// fmodf(x, y) == (x - y * trunc(x / y))
			// https://hugeonotation.github.io/pblog/2024/06/07/fmod.html
			__m128 div = _mm_div_ps(idx_xx, one_ps);
			__m128 trunc_div = _mm_round_ps(div, _MM_FROUND_TO_ZERO | _MM_FROUND_NO_EXC);
			__m128 prod = _mm_mul_ps(one_ps, trunc_div);
			__m128 weight_xx = _mm_sub_ps(idx_xx, prod);

			__m128i xx = _mm_cvttps_epi32(idx_xx);
			__m128i xx1 = _mm_min_epi32(_mm_add_epi32(xx, one_epi32), max_val);

			// Load the 2x2 texels
			const int *row1 = (int *) tex->data + y * tex->w;
			const int *row2 = (int *) tex->data + y1 * tex->w;

			t_vec4i_sse source;

			source.r0 = _mm_i32gather_epi32(row1, xx, sizeof(int));
			source.r1 = _mm_i32gather_epi32(row1, xx1, sizeof(int));
			source.r2 = _mm_i32gather_epi32(row2, xx, sizeof(int));
			source.r3 = _mm_i32gather_epi32(row2, xx1, sizeof(int));

			t_rgba_ps128 src_a = unpack_rgba_bytes_to_floats(source.r0);
			t_rgba_ps128 src_b = unpack_rgba_bytes_to_floats(source.r1);
			t_rgba_ps128 src_c = unpack_rgba_bytes_to_floats(source.r2);
			t_rgba_ps128 src_d = unpack_rgba_bytes_to_floats(source.r3);

			t_rgba_ps128 dimmed_a = dim_colour2_unpvec(src_a, dim);
			t_rgba_ps128 dimmed_b = dim_colour2_unpvec(src_b, dim);
			t_rgba_ps128 dimmed_c = dim_colour2_unpvec(src_c, dim);
			t_rgba_ps128 dimmed_d = dim_colour2_unpvec(src_d, dim);

			t_rgba_ps128 top = lerp_biased_unpvec(dimmed_a, dimmed_b, weight_xx);
			t_rgba_ps128 bottom = lerp_biased_unpvec(dimmed_c, dimmed_d, weight_xx);
			t_rgba_ps128 outf = lerp_biased_unpvec(top, bottom, weight_yy);

			__m128i rgba = repack_rgba_floats_to_bytes_sse(outf);

			_mm_storeu_si128((__m128i_u *) &p_row[i], rgba);

			/* ============================================== */
			i += 4;
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
	place_img_alpha_avx2_soa(app->canvas, app->overlay, p);
}

void draw_credits_sse4(t_info *app, t_dummy *dummy)
{
	t_vect dir[2];
	t_vect pos[2];
	int row;


	int i;

	const t_tex *tex = &app->shtex->credits;

	__m128 half_ps = _mm_set1_ps(0.5f);
	__m128 one_ps = _mm_set1_ps(1.0f);
	__m128i one_epi32 = _mm_set1_epi32(1);
	__m128i max_val = _mm_set1_epi32(tex->w - 1);

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
		const __m128 weight_yy = _mm_set1_ps(fmodf(idx_y, 1.0f));

		if (pos[LEFT].y > 0)
			continue;
		if (idx_y > tex->h)
			break;

		__m128 pos_LEFT_x = _mm_set1_ps(pos[LEFT].x);

		float step_x = (pos[RIGHT].x - pos[LEFT].x) / WIN_WIDTH;

		const float falloff = (depth - 1.5f) * 6.0f;
		float inv = 1.0 / (falloff + DBL_EPSILON);
		float dim = 1.0 + (inv - 1.0) * (falloff >= 1.0);

		t_vect lim = {-0.48, 0.48}; // Relative to 1 block on the map

		u_int *const p_row = (u_int *) app->overlay->data + app->overlay->width * row;

		int y = (int) idx_y;
		int y1 = y + ((((tex->h - 1) - (y + 1)) >> 31) ^ 1); // y1 = MIN(y + 1, tex->h - 1);

		int start = MAX(0, (lim.x - pos[LEFT].x) / step_x);
		int stop = MIN(WIN_WIDTH, (lim.y - pos[LEFT].x) / step_x);

		i = start;
		while (i < stop - 3)
		{

			__m128i initial = _mm_setr_epi32(0, 1, 2, 3);
			__m128i ii = _mm_set1_epi32(i);

			__m128 step_xx = _mm_set1_ps(step_x);
			__m128 scaled_xx = _mm_mul_ps(step_xx, _mm_cvtepi32_ps( _mm_add_epi32(ii, initial)));

			__m128 currs_xx = _mm_add_ps(half_ps, _mm_add_ps(pos_LEFT_x, scaled_xx));
			__m128 idx_xx = _mm_mul_ps(currs_xx, _mm_cvtepi32_ps(_mm_set1_epi32(tex->w)));

			/* ===============bilinear_filter=============== */

			// fmodf(x, y) == (x - y * trunc(x / y))
			// https://hugeonotation.github.io/pblog/2024/06/07/fmod.html
			__m128 div = _mm_div_ps(idx_xx, one_ps);
			__m128 trunc_div = _mm_round_ps(div, _MM_FROUND_TO_ZERO | _MM_FROUND_NO_EXC);
			__m128 prod = _mm_mul_ps(one_ps, trunc_div);
			__m128 weight_xx = _mm_sub_ps(idx_xx, prod);

			__m128i xx = _mm_cvttps_epi32(idx_xx);
			__m128i x_plus_1 = _mm_add_epi32(xx, one_epi32);
			__m128i xx1 = _mm_min_epi32(x_plus_1, max_val);

			// Load the 2x2 texels
			const int *row1 = (int *) tex->data + y * tex->w;
			const int *row2 = (int *) tex->data + y1 * tex->w;

			__m128i col_tl = _mm_i32gather_epi32((const int *) row1, xx, sizeof(int));
			__m128i col_tr = _mm_i32gather_epi32((const int *) row1, xx1, sizeof(int));
			__m128i col_bl = _mm_i32gather_epi32((const int *) row2, xx, sizeof(int));
			__m128i col_br = _mm_i32gather_epi32((const int *) row2, xx1, sizeof(int));

			t_vec4i_sse dimmed;

			dimmed.r0 = dim_colour2_vec(col_tl, dim);
			dimmed.r1 = dim_colour2_vec(col_tr, dim);
			dimmed.r2 = dim_colour2_vec(col_bl, dim);
			dimmed.r3 = dim_colour2_vec(col_br, dim);

			__m128i top_vec = lerp_biased_vec(dimmed.r0, dimmed.r1, weight_xx);
			__m128i bottom_vec = lerp_biased_vec(dimmed.r2, dimmed.r3, weight_xx);
			__m128i out_vec = lerp_biased_vec(top_vec, bottom_vec, weight_yy);

			_mm_storeu_si128((__m128i_u *) &p_row[i], out_vec);

			/* ============================================== */
			i += 4;
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
	place_img_alpha_avx2_soa(app->canvas, app->overlay, p);
}
