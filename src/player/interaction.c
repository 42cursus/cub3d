/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   interaction.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/19 15:14:22 by fsmyth            #+#    #+#             */
/*   Updated: 2025/05/19 15:29:04 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	rotate_player(t_info *app, t_player *player,
			int direction, double sensitivity)
{
	if (direction == 0)
		rotate_vect_inplace(&player->dir,
			M_PI_4 / (sensitivity * app->fr_scale));
	else
		rotate_vect_inplace(&player->dir,
			-M_PI_4 / (sensitivity * app->fr_scale));
	app->player->angle = atan2(app->player->dir.y, app->player->dir.x);
	if (app->map->outside)
		draw_sky_alt(app);
	draw_nav(app);
}

void	handle_open_door(t_info *app, t_ray *crosshair)
{
	t_anim	*anim;
	char	*doortile;

	if (crosshair->distance < 1.0)
	{
		doortile = &app->map->map[crosshair->maptile.y][crosshair->maptile.x];
		anim = &app->map->anims[crosshair->maptile.y][crosshair->maptile.x];
		if (*doortile == 'D')
			*doortile = 'O';
		else if (*doortile == 'O')
			*doortile = 'D';
		else
			return ;
		anim->active = 1;
		anim->timestart = app->last_frame;
	}
	if (crosshair->in_front != NULL)
		handle_open_door(app, crosshair->in_front);
}
