/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vector.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/13 14:53:49 by fsmyth            #+#    #+#             */
/*   Updated: 2025/04/10 21:59:35 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

t_vect	vect(double x, double y)
{
	t_vect	out;

	out.x = x;
	out.y = y;
	return (out);
}

t_vect	scale_vect(t_vect vect, double scalar)
{
	t_vect	out;

	out.x = vect.x * scalar;
	out.y = vect.y * scalar;
	return (out);
}

t_ivect	scale_ivect(t_ivect vect, int scalar)
{
	t_ivect	out;

	out.x = vect.x * scalar;
	out.y = vect.y * scalar;
	return (out);
}

void	rotate_vect_inplace(t_vect *vect, double angle)
{
	double temp_x;
	double temp_y;

	temp_x = (vect->x * cos(angle)) - (vect->y * sin(angle));
	temp_y = (vect->x * sin(angle)) + (vect->y * cos(angle));
	vect->x = temp_x;
	vect->y = temp_y;
}

t_vect	rotv(t_vect vect, double angle)
{
	t_vect out;

	out = vect;
	rotate_vect_inplace(&out, angle);
	return (out);
}


t_vect	rotate_vect(t_vect vect, double angle)
{
	t_vect out;

	out = vect;
	rotate_vect_inplace(&out, angle);
	return (out);
}

t_vect	add_vect(const t_vect v1, t_vect v2)
{
	t_vect out;

	out.x = v1.x + v2.x;
	out.y = v1.y + v2.y;
	return (out);
}

t_vect	addi_vect(const t_vect v1, t_ivect v2)
{
	t_vect out;

	out.x = v1.x + v2.x;
	out.y = v1.y + v2.y;
	return (out);
}

t_vect	subtract_vect(t_vect v1, t_vect v2)
{
	t_vect out;

	out.x = v1.x - v2.x;
	out.y = v1.y - v2.y;
	return (out);
}

double	vector_distance(t_vect v1, t_vect v2)
{
	t_vect diff;

	diff.x = v2.x - v1.x;
	diff.y = v2.y - v1.y;
	return (sqrt(diff.x * diff.x + diff.y * diff.y));
}

char	get_max_direction(t_vect vect)
{
	double absx;
	double absy;

	absx = fabs(vect.x);
	absy = fabs(vect.y);
	if (absx > absy)
		return ('x');
	return ('y');
}

double	vector_magnitude(t_vect vect)
{
	return (sqrt(vect.x * vect.x + vect.y * vect.y));
}

double	dot_product(t_vect v1, t_vect v2)
{
	return (v1.x * v2.x + v1.y * v2.y);
}

t_vect	normalise_vect(t_vect vect)
{
	double mag;
	t_vect out;

	mag = vector_magnitude(vect);
	out.x = vect.x / mag;
	out.y = vect.y / mag;
	return (out);
}

double	vector_angle(t_vect v1, t_vect v2)
{
	double	dot;
	double	det;

	dot = dot_product(v1, v2);
	det = v1.x * v2.y - v1.y * v2.x;
	return atan2(det, dot);
}

/**
 * getting hypotenuse length - Pythagoras' theorem
 * @param len1
 * @param len2
 * @return
 */
double	get_hyp_len(double len1, double len2)
{
	return (sqrt(len1 * len1 + len2 * len2));
}

#include <immintrin.h>
#include <stdint.h>

void memcpy_avx2_nt(void *dst, const void *src, size_t count)
{
	size_t i = 0;
	const size_t stride = 32; // 256-bit = 32 bytes
	const size_t prefetch_distance = 256; // ahead by 256 bytes

	// Process 8 integers (256 bits) at a time
	if (((uintptr_t) src % 32 == 0) && ((uintptr_t) dst % 32 == 0))
	{
		while (i + stride - 1 < count) // Use non-temporal store
		{
//			if (i + prefetch_distance < count)
			_mm_prefetch((const char *) (src + i + prefetch_distance), _MM_HINT_T0);
			__m256i chunk = _mm256_load_si256((const __m256i *) (src + i));
			_mm256_stream_si256((__m256i *) (dst + i), chunk);
			i += stride;
		}
		_mm_sfence();        // Ensure the stores are globally visible
	}
	else  // Fallback to unaligned AVX2
	{
		while (i + stride - 1 < count)
		{
			__m256i chunk = _mm256_loadu_si256((const __m256i *) (src + i));
			_mm256_storeu_si256((__m256i *) (dst + i), chunk);
			i += stride;
		}
	}

	i--;
	while (++i < count)
		((uint8_t *)dst)[i] = ((const uint8_t *)src)[i];
}

void memcpy_sse2(void *dst_void, const void *src_void, size_t size)
{
	uint8_t			*dst = (uint8_t *)dst_void;
	const uint8_t	*src = (const uint8_t *)src_void;

	size_t			i = 0;
	const size_t	stride = 16;

	while (i + stride - 1 < size)
	{
		__m128i chunk = _mm_loadu_si128((const __m128i *)(src + i));
		_mm_storeu_si128((__m128i *)(dst + i), chunk);
		i += stride;
	}
	i--;
	while (++i < size)
		((uint8_t *)dst)[i] = ((const uint8_t *)src)[i];
}

void *fast_memcpy_test(int *dst, const int *src, size_t size)
{
	if (__builtin_cpu_supports("avx2"))
		memcpy_avx2_nt(dst, src, size);
	else if (__builtin_cpu_supports("sse2"))
		memcpy_sse2(dst, src, size);
	else
		ft_memcpy(dst, src, size);
	return (dst);
}
