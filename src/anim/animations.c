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

t_etex	handle_animation(t_info *app, t_anim anim)
{
	const int	frame_len = anim.duration / anim.frames;
	const int	timediff = app->fr_last - anim.timestart;
	int			index;

	if (anim.frames == 1)
		return (anim.tex_idx);
	if (anim.loop)
	{
		index = timediff % anim.duration / frame_len;
		return (anim.tex_idx + index);
	}
	else
	{
		index = timediff / frame_len;
		if (index >= anim.frames)
			return (tex_EMPTY);
		return (anim.tex_idx + index);
	}
}

void	select_projectile_tex(t_obj *obj, t_player *player, t_info *app)
{
	t_etex	tex_id;
	double	angle;
	int		index;

	if (obj->subtype == P_BEAM)
	{
		obj->tex_id = tex_PROJ;
		return ;
	}
	if (obj->subtype == P_SUPER)
		tex_id = tex_SUPER + 4;
	else
		tex_id = tex_MISSILE + 4;
	angle = vector_angle(obj->dir, add_vect(player->dir,
				normalise_vect(subtract_vect(obj->pos, player->pos))));
	index = (int)((angle + M_PI_4 / 2) / M_PI_4 + 8) % 8;
	obj->tex_id = tex_id + index;
	(void)app;
}
