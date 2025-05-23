/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   animations.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 18:41:29 by fsmyth            #+#    #+#             */
/*   Updated: 2025/05/17 17:19:49 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

t_texture	*handle_animation(t_info *app, t_anim anim)
{
	const int	frame_len = anim.duration / anim.frames;
	const int	timediff = app->last_frame - anim.timestart;
	int			index;

	if (anim.frames == 1)
		return (anim.tex);
	if (anim.loop)
	{
		index = timediff % anim.duration / frame_len;
		return (&anim.tex[index]);
	}
	else
	{
		index = timediff / frame_len;
		if (index >= anim.frames)
			return (NULL);
		return (&anim.tex[index]);
	}
}

t_texture	*get_open_door_tex(t_anim *anim, t_info *app)
{
	size_t		frames;
	t_texture	*tex;

	frames = (app->last_frame - anim->timestart) / 20000;
	if (frames > 19)
	{
		anim->active = 0;
		tex = &app->shtex->door_tex[1];
		anim->tex = app->shtex->door_tex;
	}
	else
		tex = &anim->tex[2 + (frames / 4)];
	return (tex);
}

t_texture	*get_close_door_tex(t_anim *anim, t_info *app)
{
	size_t		frames;
	t_texture	*door_tex;
	t_texture	*tex;

	frames = (app->last_frame - anim->timestart) / 20000;
	door_tex = app->shtex->door_tex;
	if (anim->tex == app->shtex->door_boss_tex)
		door_tex = anim->tex;
	if (frames > 19)
	{
		anim->active = 0;
		tex = &door_tex[0];
	}
	else
		tex = &door_tex[2 + (4 - (frames / 4))];
	return (tex);
}

void	select_projectile_tex(t_object *obj, t_player *player, t_info *app)
{
	t_texture	*tex;
	double		angle;
	int			index;

	if (obj->subtype == BEAM)
	{
		obj->texture = &app->shtex->proj_tex[0];
		return ;
	}
	if (obj->subtype == BEAM)
		obj->texture = &app->shtex->proj_tex[0];
	if (obj->subtype == SUPER)
		tex = app->shtex->super_tex + 4;
	else
		tex = app->shtex->missile_tex + 4;
	angle = vector_angle(obj->dir, add_vect(player->dir,
				normalise_vect(subtract_vect(obj->pos, player->pos))));
	index = (int)((angle + M_PI_4 / 2) / M_PI_4 + 8) % 8;
	obj->texture = &tex[index];
}
