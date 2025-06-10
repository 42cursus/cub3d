/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   projectile.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 13:44:36 by fsmyth            #+#    #+#             */
/*   Updated: 2025/05/16 14:25:47 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	setup_projectile(t_object *projectile, t_info *app,
			t_player *player, int subtype)
{
	if (subtype == pr_BEAM)
	{
		projectile->dir = scale_vect(player->dir, 0.5 / app->fr_scale);
		projectile->anim2.tex = &app->shtex->proj_tex[1];
		projectile->anim2.frames = 4;
	}
	else
	{
		player->ammo[subtype] -= 1;
		if (player->ammo[subtype] == 0)
			player->equipped = pr_BEAM;
		projectile->dir = scale_vect(player->dir, 0.2 / app->fr_scale);
		projectile->anim2.tex = &app->shtex->proj_tex[5];
		projectile->anim2.frames = 5;
		if (subtype == pr_MISSILE)
			projectile->anim2.tex = &app->shtex->explode_tex[12];
	}
}

void	spawn_projectile(t_info *app, t_player *player, t_lvl *map, t_pr_type subtype)
{
	t_object		*projectile;
	t_aud *const	aud = &app->audio;

	player->hud.active = 1;
	player->hud.timestart = app->fr_last;
	projectile = ft_calloc(1, sizeof(*projectile));
	projectile->subtype = subtype;
	projectile->pos = add_vect(player->pos, scale_vect(player->dir, 0.2));
	projectile->texture = &app->shtex->proj_tex[0];
	projectile->anim2.duration = 350000;
	setup_projectile(projectile, app, player, subtype);
	projectile->type = O_PROJ;
	projectile->anim.active = 0;
	ft_lstadd_back(&map->projectiles, ft_lstnew(projectile));

	if (subtype == pr_BEAM)
	{
		// Mix_PlayChannel(-1, aud->chunks[snd_door], 0);
		Mix_PlayChannel(ch_weapons, aud->chunks[snd_gun], 0);
	}
	else
		Mix_PlayChannel(ch_weapons, aud->chunks[snd_rocket], 0);
}

int	handle_projectile_death(t_info *app, t_object *obj, t_list **current)
{
	t_object	*closest;

	if (obj->anim2.active == 1)
	{
		obj->texture = handle_animation(app, obj->anim2);
		if (obj->texture == NULL)
		{
			*current = delete_object(&app->map->projectiles, *current);
			return (1);
		}
		return (0);
	}
	closest = check_obj_proximity(obj->pos, app->map);
	if (closest != NULL)
	{
		start_obj_death(obj, app);
		if (obj->subtype == pr_BEAM)
			damage_enemy(app, closest, 10);
		else if (obj->subtype == pr_SUPER)
			damage_enemy(app, closest, 50);
		else if (obj->subtype == pr_MISSILE)
			damage_enemy(app, closest, 30);
		return (0);
	}
	return (-1);
}

void	handle_door_projectile(t_info *app, t_object *obj,
			char *tile, t_anim *anim)
{
	if (*tile == 'D')
	{
		*tile = 'O';
		Mix_PlayChannel(ch_door, app->audio.chunks[snd_door_open], 0);
		anim->active = 1;
		anim->timestart = app->fr_last;
	}
	else if (*tile == 'L')
	{
		if (obj->subtype == pr_SUPER)
		{
			*tile = 'O';
			Mix_PlayChannel(ch_door, app->audio.chunks[snd_door_open], 0);
			anim->active = 1;
			anim->timestart = app->fr_last;
		}
	}
	else if (*tile == 'M')
	{
		if (obj->subtype != pr_BEAM)
		{
			*tile = 'O';
			Mix_PlayChannel(ch_door, app->audio.chunks[snd_door_open], 0);
			anim->active = 1;
			anim->timestart = app->fr_last;
		}
	}
}

int	handle_obj_projectile(t_info *app, t_object *obj, t_list **current)
{
	char		*tile;
	t_anim		*anim;
	t_vect		new_pos;
	int			retval;

	retval = handle_projectile_death(app, obj, current);
	if (retval != -1)
		return (retval);
	select_projectile_tex(obj, app->player, app);
	new_pos = add_vect(obj->pos, obj->dir);
	if (!point_oob_global(new_pos, app->map))
	{
		tile = &app->map->map[(int) new_pos.y][(int) new_pos.x];
		if (!check_tile_open(*tile, app->map))
		{
			anim = &app->map->anims[(int) new_pos.y][(int) new_pos.x];
			handle_door_projectile(app, obj, tile, anim);
			start_obj_death(obj, app);
		}
		else
			obj->pos = new_pos;
	}
	return (0);
}
