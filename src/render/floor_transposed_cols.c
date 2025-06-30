/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fill_ceil_transposed_cols.c                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/30 17:33:50 by abelov            #+#    #+#             */
/*   Updated: 2025/06/30 17:33:50 by abelov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

// __attribute__((optnone))
void	fill_ceil_transposed_cols(t_info *app, t_player *player)
{

	return ;
	(void) app;
	(void) player;
}

__attribute__((optnone))
void	fill_floor_transposed_cols(t_info *app, t_player *player)
{
	t_vect	dir[2];
	t_vect	pos[2];
	t_vect	step, curr;
	t_ivect	idxs[WIN_HEIGHT / 2][WIN_WIDTH - 1];

	t_ivect idx;
	t_cdata row;
	t_ivect iter;

	int y;
	double	depth;
	t_img	tex;
	float *depths;

	dir[LEFT] = rotate_vect(player->dir, app->fov_rad_half);
	dir[RIGHT] = rotate_vect(player->dir, -app->fov_rad_half);

	tex = *app->lvl->planes[T_FLOOR];

	depths = &player->row_depths[WIN_HEIGHT / 2 - 1];

	row.dst = (int *)app->canvas_r->data;
	row.src = (int *)tex.data;

	iter.y = -1;
	while (++iter.y < WIN_HEIGHT / 2)
	{
		depth = depths[iter.y * -1];

		pos[LEFT] = add_vect(player->pos, scale_vect(dir[LEFT], depth));
		pos[RIGHT] = add_vect(player->pos, scale_vect(dir[RIGHT], depth));

		step.x = (pos[RIGHT].x - pos[LEFT].x) / WIN_WIDTH * 2;
		step.y = (pos[RIGHT].y - pos[LEFT].y) / WIN_WIDTH * 2;

		curr = pos[LEFT];

		iter.x = 0;
		while (iter.x < WIN_WIDTH - 1)
		{
			idx.x = ((int)(curr.x * tex.width)) & (tex.width - 1);
			idx.y = ((int)(curr.y * tex.height)) & (tex.height - 1);
			idxs[iter.y][iter.x] = idx;

			curr.x += step.x;
			curr.y += step.y;

			iter.x += 2;
		}
	}
	iter.x = 0;
	while (iter.x < WIN_WIDTH - 1)
	{
		iter.y = -1;
		while (++iter.y < WIN_HEIGHT / 2)
		{
			y = iter.y + WIN_HEIGHT / 2;
			idx = idxs[iter.y][iter.x];

			int result = row.src[idx.y * tex.width + idx.x];
			int	*dst = row.dst + iter.x * WIN_HEIGHT + y;

			dst[0] = result;
			dst[WIN_HEIGHT] = result;
		}
		iter.x += 2;
	}
}

