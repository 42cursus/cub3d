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

void	add_serialobj(t_obj *obj, t_lvl *lvl)
{
	t_sobj *sobj = &lvl->serialdata.serialobjs[lvl->serialdata.n_serialobjs++];

	sobj->pos.x = obj->pos.x;
	sobj->pos.y = obj->pos.y;
	sobj->last_damaged = obj->last_damaged;
	sobj->tex_id = obj->tex_id;
}

void	add_serialdoor(t_obj *obj, t_lvl *lvl)
{
	t_sdoor *sdoor = &lvl->serialdata.sdoors[lvl->serialdata.n_serialdoors++];

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
		add_serialobj(obj, lvl);
		current = current->next;
	}
	(void)player;
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
		add_serialobj(obj, lvl);
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
		add_serialobj(obj, lvl);
		current = current->next;
	}
	(void)player;
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
		add_serialobj(obj, lvl);
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
	lvl->serialdata.n_serialobjs = 0;
	lvl->serialdata.n_serialdoors = 0;

	update_enemies(app, player, lvl);
	update_projectiles(app, player, lvl);
	update_items(app, player, lvl);
	update_triggers(app, player, lvl);
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
		t_vect	norm = rotate_vect(scale_vect(player->dir, 0.5), M_PI_2);
		obj->p2 = add_vect(obj->pos, norm);
	}
}
