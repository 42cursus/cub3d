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
#include <wchar.h>

void	setup_projectile(t_obj *projectile, t_info *app,
							t_player *player, int subtype)
{
	if (subtype == P_BEAM)
	{
		projectile->dir = scale_vect(player->dir, 0.5 / app->fr_scale);
		projectile->anim2.tex_idx = tex_PROJ + 1;
		projectile->anim2.frames = 4;
	}
	else
	{
		player->ammo[subtype] -= 1;
		if (player->ammo[subtype] == 0)
			player->equipped = P_BEAM;
		projectile->dir = scale_vect(player->dir, 0.2 / app->fr_scale);
		projectile->anim2.tex_idx = tex_PROJ + 5;
		projectile->anim2.frames = 5;
		if (subtype == P_MISSILE)
			projectile->anim2.tex_idx = tex_EXPLODE + 12;
	}
}

void	spawn_projectile(t_info *app, t_player *player,
							t_lvl *lvl, t_subtype subtype)
{
	t_obj			*projectile;
	t_snd			snd;
	t_aud *const	aud = &app->audio;

	player->hud.active = 1;
	player->hud.timestart = app->fr_last;
	projectile = ft_calloc(1, sizeof(*projectile));
	projectile->subtype = subtype;
	projectile->pos = add_vect(player->pos, scale_vect(player->dir, 0.2));
	projectile->tex_id = tex_PROJ;
	projectile->anim2.duration = 350000;
	setup_projectile(projectile, app, player, subtype);
	projectile->type = O_PROJ;
	projectile->anim.active = 0;
	ft_lstadd_back(&lvl->projectiles, ft_lstnew(projectile));
	snd = ({if (subtype == P_BEAM) snd = snd_gun; else snd = snd_rocket; snd;});
	Mix_PlayChannel(-1, aud->chunks[snd], 0);
}

void	spawn_projectile_server(t_info *app, t_vect pos, t_vect dir, t_lvl *lvl, t_subtype subtype, int player_id)
{
	t_obj			*projectile;

	projectile = ft_calloc(1, sizeof(*projectile));
	projectile->subtype = subtype;
	projectile->pos = add_vect(pos, scale_vect(dir, 0.2));
	projectile->tex_id = tex_PROJ;
	projectile->anim2.duration = 350000;
	if (subtype == P_BEAM)
	{
		projectile->dir = scale_vect(dir, 0.5 / app->fr_scale);
		projectile->anim2.tex_idx = tex_PROJ + 1;
		projectile->anim2.frames = 4;
	}
	else
	{
		projectile->dir = scale_vect(dir, 0.2 / app->fr_scale);
		projectile->anim2.tex_idx = tex_PROJ + 5;
		projectile->anim2.frames = 5;
		if (subtype == P_MISSILE)
			projectile->anim2.tex_idx = tex_EXPLODE + 12;
	}
	projectile->type = O_PROJ;
	projectile->player_id = player_id;
	projectile->anim.active = 0;
	ft_lstadd_back(&lvl->projectiles, ft_lstnew(projectile));
}

void	spawn_projectile_client(t_info *app, t_player *player)
{
	t_aud *const	aud = &app->audio;
	t_snd			snd;
	t_eproj			proj;

	switch (player->equipped) {
		case (P_BEAM):
			proj = PROJ_BEAM;
			break;
		case (P_MISSILE):
			proj = PROJ_MISSILE;
			break;
		case (P_SUPER):
			proj = PROJ_SUPER;
			break;
	}
	snd = ({if (proj == PROJ_BEAM) snd = snd_gun; else snd = snd_rocket; snd;});
	client_send_proj(app, proj);
	Mix_PlayChannel(-1, aud->chunks[snd], 0);
}

int	handle_projectile_death(t_info *app, t_obj *obj, t_list **current)
{
	t_obj	*closest;

	if (obj->anim2.active == 1)
	{
		obj->tex_id = handle_animation(app, obj->anim2);
		if (obj->tex_id == tex_EMPTY)
		{
			*current = delete_object(&app->lvl->projectiles, *current);
			return (1);
		}
		return (0);
	}
	closest = check_obj_proximity(obj->pos, app->lvl);
	if (closest != NULL)
	{
		start_obj_death(obj, app);
		if (obj->subtype == P_BEAM)
			damage_enemy(app, closest, 10);
		else if (obj->subtype == P_SUPER)
			damage_enemy(app, closest, 50);
		else if (obj->subtype == P_MISSILE)
			damage_enemy(app, closest, 30);
		return (0);
	}
	return (-1);
}

int	check_player_proximity(t_vect pos, t_playermult *players, int n_players)
{
	for (int i = 0; i < n_players; i++)
	{
		if (players[i].dead != 1)
		{
			if (vector_distance(pos, players[i].pos) < 0.3)
				return (i);
		}
	}
	return (-1);
}

int	handle_projectile_death_mult(t_info *app, t_obj *obj, t_list **current)
{
	t_obj	*closest_obj;

	if (obj->anim2.active == 1)
	{
		obj->tex_id = handle_animation(app, obj->anim2);
		if (obj->tex_id == tex_EMPTY)
		{
			*current = delete_object(&app->lvl->projectiles, *current);
			return (1);
		}
		return (0);
	}
	closest_obj = check_obj_proximity(obj->pos, app->lvl);
	if (closest_obj != NULL)
	{
		start_obj_death(obj, app);
		if (obj->subtype == P_BEAM)
			damage_enemy(app, closest_obj, 10);
		else if (obj->subtype == P_SUPER)
			damage_enemy(app, closest_obj, 50);
		else if (obj->subtype == P_MISSILE)
			damage_enemy(app, closest_obj, 30);
		return (0);
	}

	int player_id = check_player_proximity(obj->pos, app->srv->clients, app->srv->n_clients);
	if (player_id != -1 && player_id != obj->player_id)
	{
		start_obj_death(obj, app);
		if (obj->subtype == P_BEAM)
			subtract_health_mult(app, &app->srv->clients[player_id], 10);
		else if (obj->subtype == P_SUPER)
			subtract_health_mult(app, &app->srv->clients[player_id], 50);
		else if (obj->subtype == P_MISSILE)
			subtract_health_mult(app, &app->srv->clients[player_id], 30);
		return (0);
	}
	return (-1);
}

void	handle_door_projectile(t_info *app, t_obj *obj, char *tile,
								t_anim *anim)
{
	if (*tile == 'D')
	{
		*tile = 'O';
		Mix_PlayChannel(ch_door, app->audio.chunks[snd_door_open], 0);
		anim->active = 1;
		anim->timestart = app->fr_last;
	}
	else if (*tile == 'L' && obj->subtype == P_SUPER)
	{
		*tile = 'O';
		Mix_PlayChannel(ch_door, app->audio.chunks[snd_door_open], 0);
		anim->active = 1;
		anim->timestart = app->fr_last;
	}
	else if (*tile == 'M' && obj->subtype != P_BEAM)
	{
		*tile = 'O';
		Mix_PlayChannel(ch_door, app->audio.chunks[snd_door_open], 0);
		anim->active = 1;
		anim->timestart = app->fr_last;
	}
}

void	handle_door_projectile_alt(t_info *app, t_obj *obj, char *tile,
								t_obj *door)
{
	if (*tile == 'D')
		*tile = 'O';
	else if (*tile == 'L' && obj->subtype == P_SUPER)
		*tile = 'O';
	else if (*tile == 'M' && obj->subtype != P_BEAM)
		*tile = 'O';
	else
		return ;
	// *tile = 'D';
	Mix_PlayChannel(ch_door, app->audio.chunks[snd_door_open], 0);
	door->dead = 1;
	door->anim.active = 1;
	door->anim.timestart = app->fr_last;
}

int	handle_obj_projectile(t_info *app, t_obj *obj, t_list **current)
{
	char		*tile;
	// t_anim		*anim;
	t_vect		new_pos;
	int			retval;

	retval = handle_projectile_death(app, obj, current);
	if (retval != -1)
		return (retval);
	select_projectile_tex(obj, app->player, app);
	new_pos = add_vect(obj->pos, obj->dir);
	if (!point_oob_global(new_pos, app->lvl))
	{
		tile = &app->lvl->map[(int) new_pos.y][(int) new_pos.x];
		if (!check_tile_open(*tile, app->lvl))
		{
			t_list	*current = app->lvl->doors;
			t_obj	*door;
			while (current != NULL)
			{
				door = (t_obj *)current->data;
				if (door->coords.x == (int)new_pos.x && door->coords.y == (int)new_pos.y)
					break ;
				current = current->next;
			}
			if (current == NULL)
			{
				// obj->pos = new_pos;
				start_obj_death(obj, app);
				return (0);
			}
			handle_door_projectile_alt(app, obj, tile, door);
			start_obj_death(obj, app);
		}
		else
			obj->pos = new_pos;
	}
	return (0);
}

int	handle_obj_projectile_mult(t_info *app, t_obj *obj, t_list **current)
{
	char		*tile;
	t_vect		new_pos;
	int			retval;

	retval = handle_projectile_death_mult(app, obj, current);
	if (retval != -1)
		return (retval);
	select_projectile_tex(obj, app->player, app);
	new_pos = add_vect(obj->pos, obj->dir);
	if (!point_oob_global(new_pos, app->lvl))
	{
		tile = &app->lvl->map[(int) new_pos.y][(int) new_pos.x];
		if (!check_tile_open(*tile, app->lvl))
		{
			t_list	*current = app->lvl->doors;
			t_obj	*door;
			while (current != NULL)
			{
				door = (t_obj *)current->data;
				if (door->coords.x == (int)new_pos.x && door->coords.y == (int)new_pos.y)
					break ;
				current = current->next;
			}
			if (current == NULL)
			{
				// obj->pos = new_pos;
				start_obj_death(obj, app);
				return (0);
			}
			handle_door_projectile_alt(app, obj, tile, door);
			start_obj_death(obj, app);
		}
		else
			obj->pos = new_pos;
	}
	return (0);
}
