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
__m128i repack_rgba_floats_to_bytes_sse_clamped(t_rgba_ps128 blended)
{
	__m128i			out;
	t_rgba_si128	rgba;

	const __m128i	shuffle = _mm_set_epi8(15, 11, 7, 3, 14, 10, 6, 2, 13, 9, 5, 1, 12, 8, 4, 0);

	rgba.b = _mm_cvtps_epi32(blended.b);
	rgba.g = _mm_cvtps_epi32(blended.g);
	rgba.r = _mm_cvtps_epi32(blended.r);
	rgba.a = _mm_cvtps_epi32(blended.a);

	// Clamp channels to [0, 255]
	const __m128i	zero = _mm_setzero_si128();
	const __m128i	max255 = _mm_set1_epi32(255);
	rgba.b = _mm_min_epi32(_mm_max_epi32(rgba.b, zero), max255);
	rgba.g = _mm_min_epi32(_mm_max_epi32(rgba.g, zero), max255);
	rgba.r = _mm_min_epi32(_mm_max_epi32(rgba.r, zero), max255);
	rgba.a = _mm_min_epi32(_mm_max_epi32(rgba.a, zero), max255);

	__m128i ra_b = _mm_packs_epi32(rgba.r, rgba.a);
	__m128i bg_b = _mm_packs_epi32(rgba.b, rgba.g);

	__m128i rgba_a = _mm_packus_epi16(bg_b, ra_b);

	out = _mm_shuffle_epi8(rgba_a, shuffle);
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
