/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   floor_transposed.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/30 17:16:32 by abelov            #+#    #+#             */
/*   Updated: 2025/06/30 17:16:33 by abelov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	fill_ceil_transposed(t_info *app, t_player *player)
{
	t_vect	dir[2];
	t_vect	pos[2];
	t_vect	step, curr;
	t_ivect idx;
	t_cdata row;
	t_ivect iter;

	int depths_step;
	int y;
	int y_start;
	int depths_idx_start;
	double	depth;
	t_img	tex;

	dir[LEFT] = rotate_vect(player->dir, app->fov_rad_half);
	dir[RIGHT] = rotate_vect(player->dir, -app->fov_rad_half);

	tex = *app->lvl->planes[T_CEILING];
	y_start = 0;
	depths_idx_start = 0;
	depths_step = 1;

	row.dst = (int *)app->canvas_r->data;
	row.src = (int *)tex.data;

	iter.y = -1;
	while (++iter.y < WIN_HEIGHT / 2)
	{
		y = y_start + iter.y;
		depth = player->row_depths[iter.y * depths_step + depths_idx_start];

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

			row.dst[iter.x * WIN_HEIGHT + y] = row.src[idx.y * tex.width + idx.x];
			row.dst[(iter.x + 1) * WIN_HEIGHT + y] = row.src[idx.y * tex.width + idx.x];

			curr.x += step.x;
			curr.y += step.y;

			iter.x += 2;
		}
	}
}

void	fill_floor_transposed(t_info *app, t_player *player)
{
	t_vect	dir[2];
	t_vect	pos[2];
	t_vect	step, curr;
	t_ivect idx;
	t_cdata row;
	t_ivect iter;

	int y;
	double	depth;
	t_img	tex;

	dir[LEFT] = rotate_vect(player->dir, app->fov_rad_half);
	dir[RIGHT] = rotate_vect(player->dir, -app->fov_rad_half);

	tex = *app->lvl->planes[T_FLOOR];

	row.dst = (int *)app->canvas_r->data;
	row.src = (int *)tex.data;

	t_ivect	idxs[WIN_HEIGHT / 2][WIN_WIDTH - 1];

	iter.y = -1;
	while (++iter.y < WIN_HEIGHT / 2)
	{
		depth = player->row_depths[iter.y * -1 + (WIN_HEIGHT / 2 - 1)];
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
			iter.x += 2;
			curr.x += step.x;
			curr.y += step.y;
		}
	}
	iter.y = -1;
	while (++iter.y < WIN_HEIGHT / 2)
	{
		y = iter.y + WIN_HEIGHT / 2;

		iter.x = 0;
		while (iter.x < WIN_WIDTH - 1)
		{
			idx = idxs[iter.y][iter.x];

			row.dst[iter.x * WIN_HEIGHT + y] = row.src[idx.y * tex.width + idx.x];
			row.dst[(iter.x + 1) * WIN_HEIGHT + y] = row.src[idx.y * tex.width + idx.x];

			iter.x += 2;
		}
	}
}
