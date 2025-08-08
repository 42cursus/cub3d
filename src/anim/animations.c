/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   animations.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 18:41:29 by fsmyth            #+#    #+#             */
/*   Updated: 2025/08/08 18:02:59 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

t_tex	*handle_animation(t_info *app, t_anim anim)
{
	const int	frame_len = anim.duration / anim.frames;
	const int	timediff = app->fr_last - anim.timestart;
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

void	select_projectile_tex(t_obj *obj, t_player *player, t_info *app)
{
	t_tex	*tex;
	double	angle;
	int		index;

	if (obj->subtype == P_BEAM)
	{
		obj->texture = &app->shtex->proj_tex[0];
		return ;
	}
	if (obj->subtype == P_SUPER)
		tex = app->shtex->super_tex + 4;
	else
		tex = app->shtex->missile_tex + 4;
	angle = vector_angle(obj->dir, add_vect(player->dir,
				normalise_vect(subtract_vect(obj->pos, player->pos))));
	index = (int)((angle + M_PI_4 / 2) / M_PI_4 + 8) % 8;
	obj->texture = &tex[index];
}
