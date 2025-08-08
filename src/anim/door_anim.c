/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   door_anim.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/08 18:02:32 by fsmyth            #+#    #+#             */
/*   Updated: 2025/08/08 18:03:48 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

t_tex	*get_open_boss_door_tex(t_anim *anim, t_info *app)
{
	ssize_t	frames;
	t_tex	*tex;

	frames = (app->fr_last - anim->timestart) / 20000;
	if (frames > 19)
	{
		anim->active = 0;
		tex = &anim->tex[1];
	}
	else
		tex = &anim->tex[2 + (frames / 4)];
	return (tex);
}

t_tex	*get_open_door_tex(t_anim *anim, t_info *app)
{
	ssize_t	frames;
	t_tex	*tex;

	frames = (app->fr_last - anim->timestart) / 20000;
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

t_tex	*get_close_door_tex(t_anim *anim, t_info *app)
{
	ssize_t	frames;
	t_tex	*door_tex;
	t_tex	*tex;

	frames = (app->fr_last - anim->timestart) / 20000;
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

t_tex	*get_door_tex(t_anim *anim, t_info *app, char tile)
{
	if (!anim->active && tile != 'B')
		return (&anim->tex[tile == 'O']);
	if (tile == 'O')
		return (get_open_door_tex(anim, app));
	else if (tile == 'B')
	{
		if (app->lvl->boss_active)
			return (get_close_door_tex(anim, app));
		else
			return (get_open_boss_door_tex(anim, app));
	}
	return (get_close_door_tex(anim, app));
}
