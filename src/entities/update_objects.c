/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   update_objects.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 18:33:14 by fsmyth            #+#    #+#             */
/*   Updated: 2025/08/11 18:32:44 by abelov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	update_logo_pieces(t_info *app, t_player *player, t_lvl *lvl);

void	add_serialobj(t_info *app, t_obj *obj, t_lvl *lvl)
{
	t_servermsg	*msg = &lvl->serialdata[SMT_OBJS];
	t_sobj *sobj = &msg->payload.serialobjs[msg->payload.n_serialobjs++];

	sobj->pos = vect_to_fvect(obj->pos);
	sobj->damaged = app->fr_last - obj->last_damaged < 100000 ? 1 : 0;
	sobj->tex_id = obj->tex_id;
	sobj->id = -1;
}

void	add_serialplayer(t_clientmsg *cmsg, t_lvl *lvl)
{
	t_sobj *sobj = &lvl->serialdata[SMT_OBJS].payload.serialobjs[cmsg->id];

	sobj->pos = vect_to_fvect(cmsg->player.pos);
	sobj->damaged = 0;
	sobj->tex_id = tex_PHANTOON;
	sobj->id = cmsg->id;
}

void	add_serialdoor(t_obj *obj, t_lvl *lvl)
{
	t_servermsg	*msg = &lvl->serialdata[SMT_DOORS];
	t_sdoor *sdoor = &msg->payload.sdoors[msg->payload.n_serialdoors++];

	sdoor->pos = obj->coords;
	sdoor->tex_id = obj->tex_id;
	switch (obj->dead) {
		case (0):
			sdoor->open = false;
			break ;
		case (1):
			sdoor->open = true;
			break ;
	}
	sdoor->type = obj->doortype;
}

void	update_enemies(t_info *app, t_player *player, t_lvl *lvl)
{
	t_list	*current;
	t_obj	*obj;

	current = lvl->enemies;
	while (current != NULL)
	{
		obj = (t_obj *)current->data;
		if (obj->type == O_ENTITY && handle_obj_entity(app, obj, &current))
			continue ;
		// obj->norm = rotate_vect(scale_vect(player->dir, 0.5), M_PI_2);
		// obj->norm = rotate_vect((t_vect){0.5, 0}, M_PI_2);
		// obj->p2 = add_vect(obj->pos, obj->norm);
		add_serialobj(app, obj, lvl);
		current = current->next;
	}
	(void)player;
}

void	update_enemies_mult(t_info *app, t_lvl *lvl)
{
	t_list	*current;
	t_obj	*obj;

	current = lvl->enemies;
	while (current != NULL)
	{
		obj = (t_obj *)current->data;
		if (obj->type == O_ENTITY && handle_obj_entity_mult(app, obj, &current))
			continue ;
		// obj->norm = rotate_vect(scale_vect(player->dir, 0.5), M_PI_2);
		// obj->norm = rotate_vect((t_vect){0.5, 0}, M_PI_2);
		// obj->p2 = add_vect(obj->pos, obj->norm);
		add_serialobj(app, obj, lvl);
		current = current->next;
	}
}

void	update_projectiles(t_info *app, t_player *player, t_lvl *lvl)
{
	t_list	*current;
	t_obj	*obj;

	current = lvl->projectiles;
	while (current != NULL)
	{
		obj = (t_obj *)current->data;
		if (obj->type == O_PROJ && handle_obj_projectile(app, obj, &current))
			continue ;
		else if (obj->type == O_EPROJ
			&& handle_enemy_projectile(app, obj, &current))
			continue ;
		// obj->norm = rotate_vect(scale_vect(player->dir, 0.5), M_PI_2);
		// obj->p2 = add_vect(obj->pos, obj->norm);
		add_serialobj(app, obj, lvl);
		current = current->next;
	}
	(void)player;
}

void	update_projectiles_mult(t_info *app, t_player *player, t_lvl *lvl)
{
	t_list	*current;
	t_obj	*obj;

	current = lvl->projectiles;
	while (current != NULL)
	{
		obj = (t_obj *)current->data;
		if (obj->type == O_PROJ && handle_obj_projectile_mult(app, obj, &current))
			continue ;
		else if (obj->type == O_EPROJ
			&& handle_enemy_projectile(app, obj, &current))
			continue ;
		// obj->norm = rotate_vect(scale_vect(player->dir, 0.5), M_PI_2);
		// obj->p2 = add_vect(obj->pos, obj->norm);
		add_serialobj(app, obj, lvl);
		current = current->next;
	}
	(void)player;
}

void	update_items(t_info *app, t_player *player, t_lvl *lvl)
{
	t_list	*current;
	t_obj	*obj;

	current = lvl->items;
	while (current != NULL)
	{
		obj = (t_obj *)current->data;
		if (obj->type == O_ITEM && handle_obj_item(app, obj, &current))
			continue ;
		else if (obj->type == O_DECORATIVE)
			handle_decorative(app, obj);
		else if (obj->type == O_KEY && handle_key(app, obj, &current))
			continue ;
		// obj->norm = rotate_vect(scale_vect(player->dir, 0.5), M_PI_2);
		// obj->p2 = add_vect(obj->pos, obj->norm);
		add_serialobj(app, obj, lvl);
		current = current->next;
	}
	(void)player;
}

void	update_items_mult(t_info *app, t_lvl *lvl)
{
	t_list	*current;
	t_obj	*obj;

	current = lvl->items;
	while (current != NULL)
	{
		obj = (t_obj *)current->data;
		if (obj->type == O_ITEM && handle_obj_item_mult(app, obj, &current))
			continue ;
		else if (obj->type == O_DECORATIVE)
			handle_decorative(app, obj);
		// else if (obj->type == O_KEY && handle_key(app, obj, &current))
		// 	continue ;
		// obj->norm = rotate_vect(scale_vect(player->dir, 0.5), M_PI_2);
		// obj->p2 = add_vect(obj->pos, obj->norm);
		add_serialobj(app, obj, lvl);
		current = current->next;
	}
}

void	update_triggers(t_info *app, t_player *player, t_lvl *lvl)
{
	t_list	*current;
	t_obj	*obj;

	current = lvl->triggers;
	while (current != NULL)
	{
		obj = (t_obj *)current->data;
		if (obj->type == O_TRIGGER && handle_trigger(app, obj, &current))
			continue ;
		if (obj->type == O_TELE)
			handle_tele(app, obj);
		// obj->norm = rotate_vect(scale_vect(player->dir, 0.5), M_PI_2);
		// obj->p2 = add_vect(obj->pos, obj->norm);
		add_serialobj(app, obj, lvl);
		current = current->next;
	}
	(void)player;
}

void update_doors(t_info *app, t_lvl *lvl)
{
	t_list	*current;
	t_obj	*obj;
	char	tile;

	current = lvl->doors;
	while (current != NULL)
	{
		obj = (t_obj *)current->data;
		tile = lvl->map[obj->coords.y][obj->coords.x];
		obj->tex_id = get_door_tex(&obj->anim, app, tile);
		add_serialdoor(obj, lvl);
		current = current->next;
	}
	(void)obj;
	(void)tile;
	(void)app;
}

void	update_objects(t_info *app, t_player *player, t_lvl *lvl)
{
	lvl->serialdata[SMT_OBJS].payload.n_serialobjs = app->srv == NULL ? 0 : app->srv->n_clients;
	lvl->serialdata[SMT_DOORS].payload.n_serialdoors = 0;

	update_enemies(app, player, lvl);
	update_projectiles(app, player, lvl);
	update_items(app, player, lvl);
	update_triggers(app, player, lvl);
	update_logo_pieces(app, player, lvl);
	update_doors(app, lvl);
}

void	update_objects_mult(t_info *app, t_player *player, t_lvl *lvl)
{
	lvl->serialdata[SMT_OBJS].payload.n_serialobjs = app->srv == NULL ? 0 : app->srv->n_clients;
	lvl->serialdata[SMT_DOORS].payload.n_serialdoors = 0;
	for (int i = 0; i < app->srv->n_clients; i++)
		app->srv->clients[i].event = EVENT_NONE;

	update_enemies_mult(app, lvl);
	update_projectiles_mult(app, player, lvl);
	update_items_mult(app, lvl);
	// update_triggers(app, player, lvl);
	update_logo_pieces(app, player, lvl);
	update_doors(app, lvl);
}

void	deserialise_doors(t_sdoor *serialdoors, int n_sdoors, t_lvl *lvl)
{
	for (int i = 0; i < n_sdoors; i++)
	{
		t_ivect coords = serialdoors[i].pos;
		lvl->door_tex[coords.y * lvl->width + coords.x] = serialdoors[i].tex_id;

		if (serialdoors[i].open)
			lvl->map[coords.y][coords.x] = 'O';
		else
		{
			switch (serialdoors[i].type) {
				case (D_NORMAL):
					lvl->map[coords.y][coords.x] = 'D';
					break ;
				case (D_MISSILE):
					lvl->map[coords.y][coords.x] = 'M';
					break ;
				case (D_SUPER):
					lvl->map[coords.y][coords.x] = 'L';
					break ;
				default:
					break;
			}
		}
	}
}

void	deserialise_objs(t_sobj *serialobjs, int n_sobjs, t_player *player)
{
	for (int i = 0; i < n_sobjs; i++)
	{
		t_sobj	*obj = &serialobjs[i];
		// printf("id: %d pos: (%f, %f)\n", obj->id, obj->pos.x, obj->pos.y);
		t_vect	norm;
		// if (obj->id == -1)
			norm = rotate_vect(scale_vect(player->dir, 0.5), M_PI_2);
		// else
		// 	norm = rotate_vect(scale_vect(fvect_to_vect(obj->p2), 0.5), M_PI_2);
		player->obj_p2s[i] = add_vect(fvect_to_vect(obj->pos), norm);
	}
}

void deserialise_player_state(t_info *app, t_servermsg *smsg)
{
	t_player	*player = app->player;

	player->health = smsg->payload.player.health;
	player->max_health = smsg->payload.player.max_health;
	player->ammo[P_MISSILE] = smsg->payload.player.ammo[P_MISSILE];
	player->ammo[P_SUPER] = smsg->payload.player.ammo[P_SUPER];
	player->max_ammo[P_MISSILE] = smsg->payload.player.max_ammo[P_MISSILE];
	player->max_ammo[P_SUPER] = smsg->payload.player.max_ammo[P_SUPER];
	if (player->ammo[player->equipped] == 0)
		player->equipped = P_BEAM;
	if (smsg->payload.player.event & EVENT_DAMAGE)
	{
		player->dmg_dir = smsg->payload.player.dmg_dir;
		player->dmg_time = app->fr_last;
		Mix_PlayChannel(ch_player, app->audio.chunks[snd_player_damage], 0);
	}
}
