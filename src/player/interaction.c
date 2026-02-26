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
	if (app->lvl->outside)
		draw_sky_transposed_avx2(app);
	draw_nav(app);
}

void	handle_open_door(t_info *app, t_ray *crosshair)
{
	// t_anim	*anim;
	// char	*doortile;
	//
	// if (crosshair->distance < 1.0)
	// {
	// 	doortile = &app->lvl->map[crosshair->maptile.y][crosshair->maptile.x];
	// 	anim = &app->lvl->doors[crosshair->maptile.y][crosshair->maptile.x];
	// 	if (*doortile == 'D')
	// 	{
	// 		*doortile = 'O';
	// 		Mix_PlayChannel(ch_door, app->audio.chunks[snd_door_open], 0);
	// 	}
	// 	else if (*doortile == 'O')
	// 	{
	// 		*doortile = 'D';
	// 		Mix_PlayChannel(ch_door, app->audio.chunks[snd_door_close], 0);
	// 	}
	// 	else
	// 		return ;
	// 	anim->active = 1;
	// 	anim->timestart = app->fr_last;
	// }
	// if (crosshair->in_front != NULL)
	// 	handle_open_door(app, crosshair->in_front);
	(void)app;
	(void)crosshair;
}

void	handle_open_door_client(t_info *app, t_ray *crosshair)
{
	char	*doortile;

	if (crosshair->distance < 1.0)
	{
		// printf("interacting with object!\n");
		doortile = &app->lvl->map[crosshair->maptile.y][crosshair->maptile.x];
		if (*doortile == 'D')
			Mix_PlayChannel(ch_door, app->audio.chunks[snd_door_open], 0);
		else if (*doortile == 'O')
			Mix_PlayChannel(ch_door, app->audio.chunks[snd_door_close], 0);
		else
			return ;
		// printf("object is a door!\n");
		client_send_door(app, crosshair->maptile);
	}
	if (crosshair->in_front != NULL)
		handle_open_door_client(app, crosshair->in_front);
}

void	handle_open_door_server(t_info *app, t_ivect pos)
{
	char	*doortile = &app->lvl->map[pos.y][pos.x];
	int		open;

	if (*doortile == 'D')
	{
		*doortile = 'O';
		open = 1;
	}
	else if (*doortile == 'O')
	{
		*doortile = 'D';
		open = 0;
	}

	t_list *current = app->lvl->doors;
	t_obj	*door;
	while (current != NULL)
	{
		door = current->data;
		if (pos.x == door->coords.x && pos.y == door->coords.y)
			break ;
	}
	if (current == NULL)
		return ;

	door->dead = open;
	door->anim.active = 1;
	door->anim.timestart = app->fr_last;
}
