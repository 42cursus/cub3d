/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   update_objects.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 18:33:14 by fsmyth            #+#    #+#             */
/*   Updated: 2025/06/04 20:31:24 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	update_logo_pieces(t_info *app, t_player *player, t_lvl *lvl);

void	update_enemies(t_info *app, t_player *player, t_lvl *lvl)
{
	t_list		*current;
	t_obj	*obj;

	current = lvl->enemies;
	while (current != NULL)
	{
		obj = (t_obj *)current->data;
		if (obj->type == O_ENTITY && handle_obj_entity(app, obj, &current))
			continue ;
		obj->norm = rotate_vect(scale_vect(player->dir, 0.5), M_PI_2);
		obj->p2 = add_vect(obj->pos, obj->norm);
		current = current->next;
	}
}

void	update_projectiles(t_info *app, t_player *player, t_lvl *lvl)
{
	t_list		*current;
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
		obj->norm = rotate_vect(scale_vect(player->dir, 0.5), M_PI_2);
		obj->p2 = add_vect(obj->pos, obj->norm);
		current = current->next;
	}
}

void	update_items(t_info *app, t_player *player, t_lvl *lvl)
{
	t_list		*current;
	t_obj	*obj;

	current = lvl->items;
	while (current != NULL)
	{
		obj = (t_obj *)current->data;
		if (obj->type == O_ITEM && handle_obj_item(app, obj, &current))
			continue ;
		obj->norm = rotate_vect(scale_vect(player->dir, 0.5), M_PI_2);
		obj->p2 = add_vect(obj->pos, obj->norm);
		current = current->next;
	}
}

void	update_triggers(t_info *app, t_player *player, t_lvl *lvl)
{
	t_list		*current;
	t_obj	*obj;

	current = lvl->triggers;
	while (current != NULL)
	{
		obj = (t_obj *)current->data;
		if (obj->type == O_TRIGGER && handle_trigger(app, obj, &current))
			continue ;
		if (obj->type == O_TELE)
			handle_tele(app, obj);
		obj->norm = rotate_vect(scale_vect(player->dir, 0.5), M_PI_2);
		obj->p2 = add_vect(obj->pos, obj->norm);
		current = current->next;
	}
}

void	update_objects(t_info *app, t_player *player, t_lvl *lvl)
{
	update_enemies(app, player, lvl);
	update_projectiles(app, player, lvl);
	update_items(app, player, lvl);
	update_triggers(app, player, lvl);
	update_logo_pieces(app, player, lvl);
}
