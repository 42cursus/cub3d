/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dda_init.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/15 15:21:45 by fsmyth            #+#    #+#             */
/*   Updated: 2025/05/15 15:22:41 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

static inline __attribute__((always_inline))
void	setup_dda_x(t_dda *dda, t_ray *ray, t_player *player, t_lvl *lvl)
{
	if (dda->dir.x < 0)
	{
		dda->step.x = -1;
		dda->norm.x = 1;
		dda->side_dist.x = (player->pos.x - ray->intcpt.x) * dda->delta_dist.x;
		dda->faces[0] = EAST;
		dda->textures[0] = &lvl->e_tex;
	}
	else
	{
		dda->step.x = 1;
		dda->norm.x = 0;
		dda->side_dist.x = (ray->intcpt.x + 1.0 - player->pos.x)
			* dda->delta_dist.x;
		dda->faces[0] = WEST;
		dda->textures[0] = &lvl->w_tex;
	}
}

static inline __attribute__((always_inline))
void	setup_dda_y(t_dda *dda, t_ray *ray, t_player *player, t_lvl *lvl)
{
	if (dda->dir.y < 0)
	{
		dda->step.y = -1;
		dda->norm.y = 1;
		dda->side_dist.y = (player->pos.y - ray->intcpt.y) * dda->delta_dist.y;
		dda->faces[1] = NORTH;
		dda->textures[1] = &lvl->n_tex;
	}
	else
	{
		dda->step.y = 1;
		dda->norm.y = 0;
		dda->side_dist.y = (ray->intcpt.y + 1.0 - player->pos.y)
			* dda->delta_dist.y;
		dda->faces[1] = SOUTH;
		dda->textures[1] = &lvl->s_tex;
	}
}

void	init_dda(t_dda *dda, t_info *app, t_ray *ray, double angle)
{
	t_vect		dir;
	t_player	*player;

	player = app->player;
	ray->intcpt.x = floor(player->pos.x);
	ray->intcpt.y = floor(player->pos.y);
	dda->dir = rotate_vect(player->dir, angle);
	dir = dda->dir;
	if (dda->dir.x == 0)
		dir.x = __DBL_EPSILON__;
	if (dda->dir.y == 0)
		dir.y = __DBL_EPSILON__;
	dda->delta_dist.x = fabs(1.0 / dir.x);
	dda->delta_dist.y = fabs(1.0 / dir.y);
	dda->gradient = get_gradient_angle(player->angle + angle);
	dda->c = get_y_intercept(player->pos, dda->gradient);
	ray->in_front = NULL;
	setup_dda_x(dda, ray, player, app->lvl);
	setup_dda_y(dda, ray, player, app->lvl);
}
