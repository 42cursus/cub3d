/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hud_8.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 15:26:49 by abelov            #+#    #+#             */
/*   Updated: 2025/08/11 15:26:50 by abelov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

t_mcol		get_mc(t_ivect3 p, int alpha, t_ivect3 it, t_cdata cd);
uint32_t	get_tile_pix(int x, int y, int idx);

inline __attribute__((always_inline, used, externally_visible))
t_img	cvttex_img(t_tex tex)
{
	t_img	img;

	img.width = tex.w;
	img.height = tex.h;
	img.data = (char *)tex.data;
	img.size_line = (int)tex.sl;
	return (img);
}

inline __attribute__((always_inline, used, externally_visible))
void	place_char_alpha(char c, t_info *app, t_ivect3 p, int alpha)
{
	t_img *const	cnvs = app->canvas;
	t_tex const		alph = app->shtex->alphabet;
	t_ivect3		it;
	t_cdata			cd;

	if (!ft_isprint(c) || p.z < 1)
		return ;
	it.z = (c - ' ') * CHAR_WIDTH;
	it.y = -1;
	while (++it.y < CHAR_WIDTH * p.z)
	{
		cd.src = (int *)alph.data + ((it.y / p.z) * alph.w) + it.z;
		cd.dst = (int *)cnvs->data + ((it.y + p.y) * cnvs->width) + p.x;
		it.x = -1;
		while (++it.x < CHAR_WIDTH * p.z)
			cd.dst[it.x] = (int)get_mc(p, alpha, it, cd).colour;
	}
}

/**
 * blend	Amount of original alpha preserved
 *
 * added_alpha = 0 => no fade => blend = 1.0 => keep all existing alpha.
 * added_alpha = 255 => fully transparent => blend = 0.0 => alpha becomes 255.
 *
 * @param img
 * @param alpha
 */
inline __attribute__((always_inline, used, externally_visible))
void	apply_inverted_alpha(t_img *img, u_char added_alpha)
{
	t_ivect			it;
	t_colour		*row;
	const double	blend = (255.0 - added_alpha) / 255.0;

	it.y = -1;
	while (++it.y < img->height)
	{
		row = (t_colour *)img->data + (it.y * img->width);
		it.x = -1;
		while (++it.x < img->width)
			row[it.x].a = (u_char)(added_alpha + row[it.x].a * blend);
	}
}

inline __attribute__((always_inline, used, externally_visible))
t_tex	get_tile(int idx)
{
	t_ivect			it;
	t_tex			*tex;
	u_int32_t		*row;
	static t_tex	tiles[256] = {0x00};
	static char		tiles_data[256][MMAP_TILE_SL * MMAP_TILE_H] = {0x00};

	tex = &tiles[15];
	if (idx < 0 || idx >= 0xFF)
		return (*tex);
	if (tiles[idx].data)
		return (tiles[idx]);
	tex = &tiles[idx];
	*tex = (t_tex){.w = MMAP_TILE_W, .h = MMAP_TILE_H, .sl = MMAP_TILE_SL};
	tex->data = (u_int *) tiles_data[idx];
	if (tex->data != NULL)
	{
		it.y = -1;
		while (++it.y < MMAP_TILE_H)
		{
			row = tex->data + it.y * MMAP_TILE_W;
			it.x = -1;
			while (++it.x < MMAP_TILE_W)
				row[it.x] = get_tile_pix(it.x, it.y, idx);
		}
	}
	return (*tex);
}

/**
 * 0 => 3: Direct neighbors
 * 4 => 7: Diagonal neighbors
 * @param map
 * @param i
 * @param j
 * @return
 */
inline __attribute__((always_inline, used, externally_visible))
int	get_tile_idx(char **map, int i, int j)
{
	int	index;

	index = 0;
	index += (map[i - 0][j - 1] - '0' != 0) << 0;
	index += (map[i - 1][j + 0] - '0' != 0) << 1;
	index += (map[i - 0][j + 1] - '0' != 0) << 2;
	index += (map[i + 1][j + 0] - '0' != 0) << 3;
	index += (map[i - 1][j - 1] - '0' != 0) << 4;
	index += (map[i - 1][j + 1] - '0' != 0) << 5;
	index += (map[i + 1][j - 1] - '0' != 0) << 6;
	index += (map[i + 1][j + 1] - '0' != 0) << 7;
	return (index);
}
