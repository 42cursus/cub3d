/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vector.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/13 14:53:49 by fsmyth            #+#    #+#             */
/*   Updated: 2025/08/08 16:27:42 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

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

t_ivect	round_vect(t_vect vect)
{
	t_ivect	out;

	out.x = vect.x;
	out.y = vect.y;
	return (out);
}

char	get_max_direction(t_vect vect)
{
	double	absx;
	double	absy;

	absx = fabs(vect.x);
	absy = fabs(vect.y);
	if (absx > absy)
		return ('x');
	return ('y');
}

/**
 * 	ft_memcpy_avx2()
 * 	copies chunks of 8 integers using avx2 instructions
 *
 * @var stride = 32; // 256-bit = 32 bytes
 * @var prefetch_distance = 256; // ahead by 256 bytes
 * @param dst
 * @param src
 * @param count
 * @return dst
 */
void	*ft_memcpy_avx2(void *dst, const void *src, size_t count)
{
	size_t			i;
	__m256i			chunk;
	const size_t	stride = 32;
	uint8_t			*out;
	const uint8_t	*in = (const uint8_t *) src;

	i = 0;
	while (i + stride - 1 < count)
	{
		chunk = _mm256_loadu_si256((const __m256i *)(src + i));
		_mm256_storeu_si256((__m256i *)(dst + i), chunk);
		i += stride;
	}
	out = (uint8_t *) dst;
	i--;
	while (++i < count)
		out[i] = in[i];
	return (dst);
}

//void *fast_memcpy_test(void *dst, const void *src, size_t size)
//{
//	if (__builtin_cpu_supports("avx2"))
//		memcpy_avx2_nt(dst, src, size);
//	else if (__builtin_cpu_supports("sse2"))
//		memcpy_sse2(dst, src, size);
//	else
//		ft_memcpy(dst, src, size);
//	return (dst);
//}
