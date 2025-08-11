/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   teles.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/08 17:49:58 by fsmyth            #+#    #+#             */
/*   Updated: 2025/08/11 18:33:28 by abelov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

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
		tele->tex_id = tex_TELE;
		ft_lstadd_back(&lvl->triggers, ft_lstnew(tele));
	}
}

t_obj	*find_matching_tele(t_lvl *lvl, t_obj *key)
{
	t_list	*curr;
	t_obj	*cur_trig;

	curr = lvl->triggers;
	while (curr != NULL)
	{
		cur_trig = curr->data;
		if (cur_trig->type == O_TELE && cur_trig->subtype == key->subtype)
			return (cur_trig);
		curr = curr->next;
	}
	return (NULL);
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
		key->anim.tex_idx = tex_KEY;
		key->anim.timestart = app->fr_last;
		ft_lstadd_back(&lvl->items, ft_lstnew(key));
	}
}

int	handle_key(t_info *app, t_obj *key, t_list **current)
{
	t_obj	*tele;

	key->tex_id = handle_animation(app, key->anim);
	if (vector_distance(app->player->pos, key->pos) < 0.4)
	{
		tele = find_matching_tele(app->lvl, key);
		if (tele != NULL)
			tele->attacking = 0;
		Mix_PlayChannel(ch_item, app->audio.chunks[snd_pickup_ammo], 0);
		app->msg_to_show = MSG_FOUND_KEY_1 + key->subtype - 1;
		app->msg_last_time = app->fr_last;
		*current = delete_object(&app->lvl->items, *current);
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
			app->msg_to_show = MSG_NOKEY_1 + tele->subtype - 1;
			app->msg_last_time = app->fr_last;
		}
	}
	else if (tele->attacking == 0
		&& distance > 1.5)
		tele->dead = 0;
}
