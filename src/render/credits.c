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
	t_colour	result;
	uint32_t	mask;

	mask = -(a.a < b.a);
	// Choose RGB from more opaque color (lower alpha)
	result.raw = ((a.raw & mask) | (b.raw & ~mask));
	result.a = (unsigned char) ((b.a - a.a) * t + a.a);
	return result;
}

static inline __attribute((always_inline, unused))
u_int lerp_biased(u_int aa, u_int bb, float t)
{
	t_colour	result;
	uint32_t	mask;

	t_colour a = {.raw = aa};
	t_colour b = {.raw = bb};

	mask = -(a.a < b.a);
	// Choose RGB from more opaque color (lower alpha)
	result.raw = ((a.raw & mask) | (b.raw & ~mask));
	result.a = (u_char) ((b.a - a.a) * t + a.a);
	return result.raw;
}

static inline __attribute__((always_inline, unused))
t_colour	linear_filter(t_vect idx, const t_tex *tex)
{
	const double	frac = fmod(idx.x, 1.0);

	int x = (int) idx.y * tex->w + (int) (idx.x);

	t_colour left = *(t_colour *)&tex->data[x];
	t_colour right = *(t_colour *)&tex->data[x + 1];

	t_colour out = lerp_biased_c(left, right, frac);
	return (out);
}


static inline __attribute__((always_inline, unused))
int	interpolate_colour_inline(int col1, int col2, double frac)
{
	int	r;
	int	g;
	int	b;

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
t_colour	bilinear_filter(t_vect idx, const t_tex *tex)
{
	const int x = (int)idx.x;
	const int y = (int)idx.y;

	const double frac_x = idx.x - x;
	const double frac_y = idx.y - y;

	int x1 = x + (((tex->w - 2 - x) >> 31) ^ 1); // x1 = MIN(x + 1, tex->w - 1);
	int y1 = y + (((tex->h - 2 - y) >> 31) ^ 1); // y1 = MIN(y + 1, tex->h - 1);

	// Load the 2x2 texels
	u_int *row1 = tex->data + y * tex->w;
	u_int *row2 = tex->data + y1 * tex->w;

	u_int top = lerp_biased(row1[x], row1[x1], frac_x);
	u_int bottom  = lerp_biased(row2[x], row2[x1], frac_x);
	t_colour out = {.raw = lerp_biased(top, bottom, frac_y)};

	return (out);
}

static inline __attribute__((always_inline, unused))
int	bilinear_filter_old(double x, double y, const t_tex *tex)
{
	int		x_lower;
	int		x_upper;
	int		y_lower;
	int		y_upper;

	x_lower = (int)x;
	y_lower = (int)y;
	x_upper = x_lower + 1;
	y_upper = y_lower + 1;

	int interp;

	if (x_upper == tex->w)
		x_upper = 0;

	int col1 = (int)tex->data[y_lower * tex->w + x_lower];
	int col2 = (int)tex->data[y_lower * tex->w + x_upper];
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
t_colour	dim_colour_alpha(t_colour src, double falloff)
{
	if (falloff < 1 || src.raw == XPM_TRANSPARENT)
		return (src);

	const double dim = 1.0 / falloff;

	double alpha = (1 - dim);

	alpha = alpha > 228 ? 255 : alpha;

	src.a = (u_char) alpha;
	src.r = (u_char)(src.r * dim);
	src.g = (u_char)(src.g / falloff);
	src.b = (u_char)(src.b / falloff);
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
static inline __attribute__((always_inline))
u_int dim_colour2(u_int col, double dim)
{
	t_colour src = {.raw = col};
	u_int mask = -(col == XPM_TRANSPARENT);

	dim = fmax(0.0, fmin(1.0, dim));

	double alpha = (1.0 - dim) * 255;

	src.r = (u_char)(src.r * dim);
	src.g = (u_char)(src.g * dim);
	src.b = (u_char)(src.b * dim);
	src.a = (u_char)(alpha);

	return (src.raw & ~mask) | (col & mask);
}

void	draw_credits_row(t_info *app, t_vect l_pos, t_vect r_pos, int row)
{
	const t_tex		*tex = &app->shtex->credits;
	int				i;
	double			step_x;
	double			curr_x;
	t_vect			idx;
	double			dist = app->dummy->row_depths[row - 1];
	t_vect			lim = {-0.48,  0.48}; // Relative to 1 block on the map
	u_int *const	p_row = (u_int *)app->overlay->data + app->overlay->width * row;

	step_x = (r_pos.x - l_pos.x) / WIN_WIDTH;
	curr_x = l_pos.x;
	idx.y = (-l_pos.y) * tex->w;
	if (l_pos.y > 0 || idx.y > tex->h)
		return ;
	i = -1;
	while (++i < WIN_WIDTH)
	{
		if (curr_x > lim.x && curr_x < lim.y)
		{
			idx.x = (0.5 + curr_x) * tex->w;
			p_row[i] = dim_colour_alpha(bilinear_filter(idx, tex), (dist - 1.5) * 6).raw;
		}
		curr_x += step_x;
	}
}

void	update_rocks(t_info *app, t_dummy *dummy)
{
	t_rock	*rock;
	t_list	*current;

	current = dummy->rocks;
	while (current != NULL)
	{
		rock = current->data;
		place_tex_to_image_scale(app->canvas, rock->tex, round_vect(rock->pos), rock->scale);
		rock->pos.x += rock->speed;
		current = current->next;
	}
}

int	cmp_rock_speed(void *data1, void *data2)
{
	t_rock	*rock1;
	t_rock	*rock2;

	rock1 = data1;
	rock2 = data2;
	if (rock1->speed > rock2->speed)
		return (1);
	return (-1);
}

void	spawn_rock(t_info *app, t_vect pos, t_tex *tex, double speed)
{
	t_rock	*rock;
	
	rock = ft_calloc(1, sizeof(*rock));
	rock->pos = pos;
	rock->speed = speed;
	rock->tex = tex;
	ft_lstadd_back(&app->dummy->rocks, ft_lstnew(rock));
}

void	spawn_random_rock(t_info *app, double speed)
{
	t_rock	*rock;
	int		index;

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
	rock->pos.y = ((WIN_HEIGHT - 80) / 12.0) * fabs(speed) * 2 + rand_range(-100, 100);
	if (rock->pos.y < 0)
		rock->pos.y = rand_range(0, 100);
	if (rock->pos.y > WIN_HEIGHT)
		rock->pos.y = WIN_HEIGHT - rand_range(50, 150);
	rock->speed = speed;
	ft_lstadd_back(&app->dummy->rocks, ft_lstnew(rock));
}

void	draw_credits(t_info *app, t_dummy *dummy)
{
	t_vect	l_dir;
	t_vect	r_dir;
	t_vect	l_pos;
	t_vect	r_pos;
	int		row;

	row = 0;
	l_dir = rotate_vect(dummy->dir, app->fov_rad_half);
	r_dir = rotate_vect(dummy->dir, -app->fov_rad_half);
	update_rocks(app, dummy);
	while (++row < WIN_HEIGHT)
	{
		l_pos = add_vect(dummy->pos, scale_vect(l_dir, dummy->row_depths[row - 1]));
		r_pos = add_vect(dummy->pos, scale_vect(r_dir, dummy->row_depths[row - 1]));
		draw_credits_row(app, l_pos, r_pos, row);
	}
	place_img_alpha_sse(app->canvas, app->overlay, (t_point) {0, 0});
}

void	draw_credits_avx2(t_info *app, t_dummy *dummy)
{
	t_vect	dir[2];
	t_vect	pos[2];
	int		row;



	int		i;

//	double	depth;
	const t_tex *tex = &app->shtex->credits;
	row = 0;
	dir[LEFT] = rotate_vect(dummy->dir, app->fov_rad_half);
	dir[RIGHT] = rotate_vect(dummy->dir, -app->fov_rad_half);
	update_rocks(app, dummy);
	while (++row < WIN_HEIGHT)
	{
		double depth = app->dummy->row_depths[row - 1];

		pos[LEFT] = add_vect(dummy->pos, scale_vect(dir[LEFT], depth));
		pos[RIGHT] = add_vect(dummy->pos, scale_vect(dir[RIGHT], depth));

		__m128 pos_LEFT_x = _mm_set1_ps(pos[LEFT].x);
		__m128 half_ps = _mm_set1_ps(0.5);

		float step_x;
//		double step_y;
		float curr_x;
		float idx_xs[4];
		float idx_ys[4];
//		double curr_y;


		const double falloff = (depth - 1.5) * 6.0;

//		double dim = falloff >= 1.0 ? 1.0 / falloff : 1.0;

		float inv = 1.0 / (falloff + DBL_EPSILON);
		float dim = 1.0 + (inv - 1.0) * (falloff >= 1.0);

		t_vect lim = {-0.48, 0.48}; // Relative to 1 block on the map

		u_int *const p_row = (u_int *) app->overlay->data + app->overlay->width * row;

		step_x = (pos[RIGHT].x - pos[LEFT].x) / WIN_WIDTH;
//		step_y = 0;
		curr_x = pos[LEFT].x;
//		curr_y = pos[LEFT].y * 0;
		float idx_y = (-pos[LEFT].y) * tex->w;
		__m128i  idx_yy = _mm_set1_ps(idx_y);
		float weight_y = fmodf(idx_y, 1.0f);

		if (pos[LEFT].y > 0)
			continue;
		if (idx_y > tex->h)
			break ;

		int y = (int)idx_y;
		int y1 = y + ((((tex->h - 1) - (y + 1)) >> 31) ^ 1); // y1 = MIN(y + 1, tex->h - 1);

		int start = MAX(0, (lim.x - pos[LEFT].x) / step_x );
		int stop = MIN(WIN_WIDTH, (lim.y - pos[LEFT].x) / step_x);
//		int stop = MIN(WIN_WIDTH, (int)(step_x * WIN_WIDTH));

//		while (i < WIN_WIDTH && (curr_x > lim.x && curr_x < lim.y))

		i = start;
		while (i < stop - 3)
		{
			curr_x = pos[LEFT].x + (step_x * i);

			__m128i  initial = _mm_setr_epi32(0, 1, 2, 3);
			__m128i  ii = _mm_set1_epi32(i);

			__m128  step_xx = _mm_set1_ps(step_x);
			__m128  scaled_xx = _mm_mul_ps(step_xx, _mm_cvtepi32_ps(_mm_add_epi32(ii, initial)));

			__m128 currs_xx = _mm_add_ps(half_ps, _mm_add_ps(pos_LEFT_x, scaled_xx));
			__m128 idx_xx = _mm_mul_ps(currs_xx, _mm_cvtepi32_ps(_mm_set1_epi32(tex->w)));

			_mm_storeu_si128((__m128i_u *) idx_xs, idx_xx);
			_mm_storeu_si128((__m128i_u *) idx_ys, idx_yy);

			/* ===============bilinear_filter=============== */

			int xs[4];

			xs[0] = (int) idx_xs[0];
			xs[1] = (int) idx_xs[1];
			xs[2] = (int) idx_xs[2];
			xs[3] = (int) idx_xs[3];


			float weight_xs[4];
			float weight_ys[4];

			weight_xs[0] = idx_xs[0] - (float)xs[0];
			weight_xs[1] = idx_xs[1] - (float)xs[1];
			weight_xs[2] = idx_xs[2] - (float)xs[2];
			weight_xs[3] = idx_xs[3] - (float)xs[3];

			weight_ys[0] = weight_y;
			weight_ys[1] = weight_y;
			weight_ys[2] = weight_y;
			weight_ys[3] = weight_y;


			int x1s[4];

			x1s[0] = xs[0] + ((((tex->w - 1) - (xs[0] + 1)) >> 31) ^ 1); // x1 = MIN(x + 1, tex->w - 1);
			x1s[1] = xs[1] + ((((tex->w - 1) - (xs[1] + 1)) >> 31) ^ 1); // x1 = MIN(x + 1, tex->w - 1);
			x1s[2] = xs[2] + ((((tex->w - 1) - (xs[2] + 1)) >> 31) ^ 1); // x1 = MIN(x + 1, tex->w - 1);
			x1s[3] = xs[3] + ((((tex->w - 1) - (xs[3] + 1)) >> 31) ^ 1); // x1 = MIN(x + 1, tex->w - 1);

			// Load the 2x2 texels
			u_int *row1 = tex->data + y * tex->w;
			u_int *row2 = tex->data + y1 * tex->w;

			u_int col_lt[4];
			u_int col_rt[4];
			u_int col_lb[4];
			u_int col_rb[4];

			col_lt[0] = row1[xs[0]];
			col_lt[1] = row1[xs[1]];
			col_lt[2] = row1[xs[2]];
			col_lt[3] = row1[xs[3]];

			col_rt[0] = row1[x1s[0]];
			col_rt[1] = row1[x1s[1]];
			col_rt[2] = row1[x1s[2]];
			col_rt[3] = row1[x1s[3]];

			col_lb[0] = row2[xs[0]];
			col_lb[1] = row2[xs[1]];
			col_lb[2] = row2[xs[2]];
			col_lb[3] = row2[xs[3]];

			col_rb[0] = row2[x1s[0]];
			col_rb[1] = row2[x1s[1]];
			col_rb[2] = row2[x1s[2]];
			col_rb[3] = row2[x1s[3]];

			u_int a[4];
			u_int b[4];
			u_int c[4];
			u_int d[4];

			a[0] = dim_colour2(col_lt[0], dim);
			a[1] = dim_colour2(col_lt[1], dim);
			a[2] = dim_colour2(col_lt[2], dim);
			a[3] = dim_colour2(col_lt[3], dim);

			b[0] = dim_colour2(col_rt[0], dim);
			b[1] = dim_colour2(col_rt[1], dim);
			b[2] = dim_colour2(col_rt[2], dim);
			b[3] = dim_colour2(col_rt[3], dim);

			c[0] = dim_colour2(col_lb[0], dim);
			c[1] = dim_colour2(col_lb[1], dim);
			c[2] = dim_colour2(col_lb[2], dim);
			c[3] = dim_colour2(col_lb[3], dim);

			d[0] = dim_colour2(col_rb[0], dim);
			d[1] = dim_colour2(col_rb[1], dim);
			d[2] = dim_colour2(col_rb[2], dim);
			d[3] = dim_colour2(col_rb[3], dim);

			u_int top[4];
			u_int bottom[4];
			u_int out[4];

			top[0] = lerp_biased(a[0], b[0], weight_xs[0]);
			top[1] = lerp_biased(a[1], b[1], weight_xs[1]);
			top[2] = lerp_biased(a[2], b[2], weight_xs[2]);
			top[3] = lerp_biased(a[3], b[3], weight_xs[3]);

			bottom[0] = lerp_biased(c[0], d[0], weight_xs[0]);
			bottom[1] = lerp_biased(c[1], d[1], weight_xs[1]);
			bottom[2] = lerp_biased(c[2], d[2], weight_xs[2]);
			bottom[3] = lerp_biased(c[3], d[3], weight_xs[3]);

			out[0] = lerp_biased(top[0], bottom[0], weight_y);
			out[1] = lerp_biased(top[1], bottom[1], weight_y);
			out[2] = lerp_biased(top[2], bottom[2], weight_y);
			out[3] = lerp_biased(top[3], bottom[3], weight_y);

			/* ================dim_colour_alpha============== */
//					p_row[i] = dim_colour_alpha(src, falloff).raw;
			p_row[i + 0] = out[0];
			p_row[i + 1] = out[1];
			p_row[i + 2] = out[2];
			p_row[i + 3] = out[3];
			/* ============================================== */

//			curr_y += step_y;
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
