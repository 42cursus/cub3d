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

t_etex	get_open_boss_door_tex(t_anim *anim, t_info *app)
{
	ssize_t	frames;
	t_etex	tex_id;

	frames = (app->fr_last - anim->timestart) / 20000;
	if (frames > 19)
	{
		anim->active = 0;
		tex_id = anim->tex_idx + 1;
	}
	else
		// tex = &anim->tex[2 + (frames / 4)];
		tex_id = anim->tex_idx + 2 + (frames / 4);
	return (tex_id);
}

t_etex	get_open_door_tex(t_anim *anim, t_info *app)
{
	ssize_t	frames;
	t_etex	tex_id;

	frames = (app->fr_last - anim->timestart) / 20000;
	if (frames > 19)
	{
		anim->active = 0;
		// tex = &app->shtex->door_tex[1];
		tex_id = tex_DOOR + 1;
		anim->tex_idx = tex_DOOR;
	}
	else
		// tex = &anim->tex[2 + (frames / 4)];
		tex_id = anim->tex_idx + 2 + (frames / 4);
	return (tex_id);
}

t_etex	get_close_door_tex(t_anim *anim, t_info *app)
{
	ssize_t	frames;
	// t_tex	*door_tex;
	t_etex	door_tex_start;
	t_etex	tex_id;

	frames = (app->fr_last - anim->timestart) / 20000;
	door_tex_start = tex_DOOR;
	if (anim->tex_idx == tex_DOOR_BOSS)
		door_tex_start = anim->tex_idx;
	if (frames > 19)
	{
		anim->active = 0;
		tex_id = door_tex_start;
	}
	else
		// tex = &door_tex[2 + (4 - (frames / 4))];
		tex_id = door_tex_start + (6 - (frames / 4));
	return (tex_id);
}

t_etex	get_door_tex(t_anim *anim, t_info *app, char tile)
{
	if (!anim->active && tile != 'B')
		return anim->tex_idx + (tile == 'O');
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
