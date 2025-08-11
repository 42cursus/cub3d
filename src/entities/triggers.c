/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   triggers.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 16:30:01 by fsmyth            #+#    #+#             */
/*   Updated: 2025/08/08 17:51:12 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	spawn_trigger(t_info *app, t_vect pos, t_subtype subtype)
{
	t_obj			*trigger;
	t_lvl *const	lvl = app->lvl;

	if (subtype > 0 && subtype < 5)
		return (spawn_teleporter(app, pos, subtype));
	trigger = ft_calloc(1, sizeof(*trigger));
	trigger->pos = pos;
	trigger->type = O_TRIGGER;
	trigger->subtype = subtype;
	trigger->tex_id = tex_EMPTY;
	ft_lstadd_back(&lvl->triggers, ft_lstnew(trigger));
}

int	handle_trigger(t_info *app, t_obj *obj, t_list **current)
{
	t_lvl *const	lvl = app->lvl;

	if (obj->subtype == T_BOSS)
	{
		if (vector_distance(obj->pos, app->player->pos) < 1.5)
		{
			Mix_PlayChannel(ch_music1, app->audio.chunks[snd_music_boss], -1);
			toggle_boss_doors(app);
			lvl->boss_active = 1;
			lvl->boss_obj->anim.active = 1;
			lvl->boss_obj->dir = (t_vect){0.0, 1.0};
			*current = delete_object(&lvl->triggers, *current);
			return (1);
		}
	}
	return (0);
}

void	toggle_boss_doors(t_info *app)
{
	// t_anim	**anims;
	// char	**map;
	// int		i;
	// int		j;
	//
	// anims = app->lvl->anims;
	// map = app->lvl->map;
	// i = -1;
	// while (++i < app->lvl->height)
	// {
	// 	j = -1;
	// 	while (++j < app->lvl->width)
	// 	{
	// 		if (map[i][j] == 'B')
	// 		{
	// 			anims[i][j].active = 1;
	// 			anims[i][j].timestart = app->fr_last;
	// 		}
	// 	}
	// }
	(void)app;
}
