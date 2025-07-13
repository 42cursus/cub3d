/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   triggers.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 16:30:01 by fsmyth            #+#    #+#             */
/*   Updated: 2025/05/19 15:25:26 by fsmyth           ###   ########.fr       */
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
	trigger->texture = &app->shtex->empty;
	ft_lstadd_back(&lvl->triggers, ft_lstnew(trigger));
}

void	spawn_teleporter(t_info *app, t_vect pos, int level)
{
	t_obj			*tele;
	t_lvl *const	lvl = app->lvl;

	if (lvl->sublvls[level - 1] != NULL)
	{
		tele = ft_calloc(1, sizeof(*tele));
		tele->pos = pos;
		tele->type = O_TELE;
		tele->dead = 1;
		tele->attacking = 0;
		tele->subtype = level;
		tele->texture = &app->shtex->tele;
		ft_lstadd_back(&lvl->triggers, ft_lstnew(tele));
	}
}

void	spawn_key(t_info *app, t_vect pos, int level)
{
	t_obj			*key;
	t_lvl *const	lvl = app->lvl;

	if (lvl->sublvls[level - 1] != NULL)
	{
		key = ft_calloc(1, sizeof(*key));
		key->pos = pos;
		key->type = O_KEY;
		key->subtype = level;
		key->anim.active = 1;
		key->anim.loop = 1;
		key->anim.frames = 6;
		key->anim.duration = 600000;
		key->anim.tex = app->shtex->key_tex;
		key->anim.timestart = app->fr_last;
		ft_lstadd_back(&lvl->triggers, ft_lstnew(key));
	}
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

int	handle_key(t_info *app, t_obj *key, t_list **current)
{
	t_list	*cur_trig;
	t_obj	*tele;

	key->texture = handle_animation(app, key->anim);
	if (vector_distance(app->player->pos, key->pos) < 0.4)
	{
		tele = find_matching_tele(app->lvl, key);
		if (tele != NULL)
			tele->attacking = 0;
		*current = delete_object(&app->lvl->triggers, *current);
		return (1);
	}
	return (0);
}

void	handle_tele(t_info *app, t_obj *tele)
{
	t_aud *const	aud = &app->audio;
	const double	distance = vector_distance(app->player->pos, tele->pos);

	if (distance < 0.4)
	{
		if (tele->dead == 0)
		{
			tele->dead = 1;
			app->current_sublevel = tele->subtype;
			app->rc = extra;
			app->mlx->end_loop = 1;
			app->player->tele_pos = tele->pos;
			Mix_PlayChannel(ch_tele, aud->chunks[snd_portal], 0);
		}
		else if (tele->attacking == 1)
		{
			app->msg_to_show = MSG_NOKEY;
			app->msg_last_time = app->fr_last;
		}
	}
	else if (tele->attacking == 0
		&& distance > 1.5)
		tele->dead = 0;
}

void	toggle_boss_doors(t_info *app)
{
	t_anim	**anims;
	char	**map;
	int		i;
	int		j;

	anims = app->lvl->anims;
	map = app->lvl->map;
	i = -1;
	while (++i < app->lvl->height)
	{
		j = -1;
		while (++j < app->lvl->width)
		{
			if (map[i][j] == 'B')
			{
				anims[i][j].active = 1;
				anims[i][j].timestart = app->fr_last;
			}
		}
	}
}
