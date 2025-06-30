/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   floor_avx2.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/30 17:44:29 by abelov            #+#    #+#             */
/*   Updated: 2025/06/30 19:35:39 by abelov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

/**
 * This function maps a continuous coordinate (like pos = 0.5)
 * to an integer pixel index in a texture of size dim.
 *
 * The modulo trick ensures the result is always between 0 and dim - 1,
 * even if whole is negative
 *
 * @param pos
 * @param dim
 * @return
 */
static inline __attribute__((always_inline, unused))
int	get_tex_index_float(double pos, int dim)
{
	// int	whole;
	//
	// whole = pos * dim;
	// return ((whole % dim + dim) % dim);
	return ((int)(pos * dim) % dim);
}

/**
 * Checks if the position pos is outside the boundaries of the map
 * oob stands for "out of bounds"
 * @param pos
 * @param lvl
 * @return
 */
static inline __attribute__((always_inline, unused))
int	point_oob(t_vect pos, t_lvl *lvl)
{
	return ((pos.x < 0 || pos.x > lvl->width)
			|| (pos.y < 0 || pos.y > lvl->height));
}

static inline __attribute__((always_inline, unused))
void	draw_floor_row(t_vect pos[2], u_int (*const dst), t_img *tex)
{
	int				i;
	t_vect			step;
	t_vect			curr;
	t_ivect			idx;
	u_int			*src = (u_int *)tex->data;

	int width = tex->width;
	int height = tex->height;

	curr = (t_vect)(pos[LEFT]);
	step.x = (pos[RIGHT].x - pos[LEFT].x) / WIN_WIDTH * 2;
	step.y = (pos[RIGHT].y - pos[LEFT].y) / WIN_WIDTH * 2;

	i = 0;
	while (i < WIN_WIDTH - 1)
	{
		idx.x = ((int)(curr.x * width)) & (width - 1);
		idx.y = ((int)(curr.y * height)) & (height - 1);

		dst[i] = src[idx.y * width + idx.x];
		dst[i + 1] = src[idx.y * width + idx.x];

		curr.x = curr.x + step.x;
		curr.y = curr.y + step.y;
		i += 2;
	}
}

/**
 * floor-drawing logic using planar projection and nearest-neighbor sampling
 *
 * can be improved with bi-linear interpolation
 *
 * `& (tex.width - 1)` is faster then `% tex.width`
 * @param app
 * @param map
 * @param player
 */
inline __attribute__((always_inline))
void	fill_floor_sse4x4(t_info *app, t_player *player)
{
	t_vect	dir[2];
	t_vect	pos[2];

	t_ivect it;

	t_cdata row;
	double	depth;
	t_img	tex;

	dir[LEFT] = rotate_vect(player->dir, app->fov_rad_half);
	dir[RIGHT] = rotate_vect(player->dir, -app->fov_rad_half);

	t_vect	step;
	t_vect	curr;
	t_vect	currs[8];
//	t_ivect	idx;
	t_ivect	idxs[4];
	int		src_pixels[4];
	int		full_src_pixels[8];

	tex = *app->lvl->planes[T_FLOOR];

	it.y = -1;
	while (++it.y < WIN_HEIGHT / 2)
	{
		depth = player->row_depths[it.y + (WIN_HEIGHT / 2 - 1)];

		pos[LEFT] = add_vect(player->pos, scale_vect(dir[LEFT], depth));
		pos[RIGHT] = add_vect(player->pos, scale_vect(dir[RIGHT], depth));

		row.dst = (int *)app->canvas->data + (it.y + WIN_HEIGHT / 2) * app->canvas->width;
		row.src = (int *)tex.data;

		step.x = ((pos[RIGHT].x - pos[LEFT].x) / WIN_WIDTH);
		step.y = ((pos[RIGHT].y - pos[LEFT].y) / WIN_WIDTH);


		curr = pos[LEFT];

		it.x = 0;
		while (it.x < WIN_WIDTH - 1)
		{
			currs[0] = add_vect(curr, scale_vect(step, it.x + 0));

			currs[1] = add_vect(curr, scale_vect(step, it.x + 2));

			currs[2] = add_vect(curr, scale_vect(step, it.x + 4));
			currs[3] = add_vect(curr, scale_vect(step, it.x + 6));

			idxs[0].x = ((int) (currs[0].x * tex.width)) & (tex.width - 1);
			idxs[1].x = ((int) (currs[1].x * tex.width)) & (tex.width - 1);
			idxs[2].x = ((int) (currs[2].x * tex.width)) & (tex.width - 1);
			idxs[3].x = ((int) (currs[3].x * tex.width)) & (tex.width - 1);

			idxs[0].y = ((int) (currs[0].y * tex.height)) & (tex.height - 1);
			idxs[1].y = ((int) (currs[1].y * tex.height)) & (tex.height - 1);
			idxs[2].y = ((int) (currs[2].y * tex.height)) & (tex.height - 1);
			idxs[3].y = ((int) (currs[3].y * tex.height)) & (tex.height - 1);


			src_pixels[0] = row.src[idxs[0].y * tex.width + idxs[0].x];
			src_pixels[1] = row.src[idxs[1].y * tex.width + idxs[1].x];
			src_pixels[2] = row.src[idxs[2].y * tex.width + idxs[2].x];
			src_pixels[3] = row.src[idxs[3].y * tex.width + idxs[3].x];

			full_src_pixels[7] = src_pixels[3];
			full_src_pixels[6] = src_pixels[3];
			full_src_pixels[5] = src_pixels[2];
			full_src_pixels[4] = src_pixels[2];
			full_src_pixels[3] = src_pixels[1];
			full_src_pixels[2] = src_pixels[1];
			full_src_pixels[1] = src_pixels[0];
			full_src_pixels[0] = src_pixels[0];

			int *dst = &row.dst[it.x];

			ft_memcpy(&dst[0], &full_src_pixels[0], 4 * sizeof(int));
			ft_memcpy(&dst[4], &full_src_pixels[4], 4 * sizeof(int));

			it.x += 2 * 4;
		}

//		while (it.x < WIN_WIDTH - 1)
//		{
//
//			idx.x = ((int) (curr.x * tex.width)) & (tex.width - 1);
//			idx.y = ((int) (curr.y * tex.height)) & (tex.height - 1);
//
//			row.dst[it.x] = row.src[idx.y * tex.width + idx.x];
//			row.dst[it.x + 1] = row.src[idx.y * tex.width + idx.x];
//
//			curr.x = curr.x + step.x;
//			curr.y = curr.y + step.y;
//			it.x += 2;
//		}

	}
}
