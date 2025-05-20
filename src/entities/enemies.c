/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   enemies.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 16:18:58 by fsmyth            #+#    #+#             */
/*   Updated: 2025/05/19 15:41:42 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	init_zoomer(t_object *enemy, t_info *app);
void	init_atomic(t_object *enemy, t_info *app);
void	init_reo(t_object *enemy, t_info *app);
void	init_holtz(t_object *enemy, t_info *app);
void	init_phantoon(t_object *enemy, t_info *app);

t_object	*spawn_enemy(t_info *app, t_vect pos, t_vect dir, int subtype)
{
	t_object	*enemy;

	enemy = ft_calloc(1, sizeof(*enemy));
	enemy->pos = pos;
	enemy->dir = dir;
	enemy->type = O_ENTITY;
	enemy->subtype = subtype;
	enemy->anim2.frames = 6;
	enemy->anim2.duration = 420000;
	enemy->anim2.tex_arr = &app->shtex->explode_tex[6];
	if (subtype == E_ZOOMER)
		init_zoomer(enemy, app);
	if (subtype == E_ATOMIC)
		init_atomic(enemy, app);
	if (subtype == E_REO)
		init_reo(enemy, app);
	if (subtype == E_HOLTZ)
		init_holtz(enemy, app);
	else if (subtype == E_PHANTOON)
		init_phantoon(enemy, app);
	enemy->anim.active = 1;
	enemy->anim.timestart = app->last_frame;
	enemy->anim.loop = 1;
	ft_lstadd_back(&app->map->enemies, ft_lstnew(enemy));
	return (enemy);
}

int	handle_enemy_death(t_info *app, t_object *obj, t_list **current)
{
	if (obj->dead == 1)
	{
		obj->texture = handle_animation(app, obj->anim2);
		if (obj->texture == NULL)
		{
			if (obj->subtype == E_PHANTOON)
			{
				spawn_drops(app, obj, 15);
				app->map->boss_obj = NULL;
			}
			else if (obj->subtype == E_HOLTZ)
				spawn_drops(app, obj, 3);
			else
				spawn_drops(app, obj, 1);
			*current = delete_object(&app->map->enemies, *current);
			return (1);
		}
		return (0);
	}
	return (-1);
}

void	handle_enemy_ai(t_info *app, t_object *obj)
{
	if (obj->subtype == E_ZOOMER)
		zoomer_ai(app, obj);
	else if (obj->subtype == E_REO)
		reo_ai(app, obj);
	else if (obj->subtype == E_PHANTOON)
		phantoon_ai(app, obj);
	else if (obj->subtype == E_ATOMIC)
		atomic_ai(app, obj);
	else if (obj->subtype == E_HOLTZ)
		holtz_ai(app, obj, app->player);
}

int	handle_obj_entity(t_info *app, t_object *obj, t_list **current)
{
	int	retval;

	retval = handle_enemy_death(app, obj, current);
	if (retval != -1)
		return (retval);
	handle_enemy_ai(app, obj);
	obj->texture = handle_animation(app, obj->anim);
	if (vector_distance(obj->pos, app->player->pos) < 0.5 && !app->player->dead)
	{
		if (obj->subtype != E_ATOMIC)
			subtract_health(app, app->player, 35);
		else
		{
			subtract_health(app, app->player, 80);
			damage_enemy(app, obj, 100);
		}
		move_entity(&app->player->pos, app->map,
			scale_vect(subtract_vect(app->player->pos, obj->pos), 1));
		app->player->dmg_dir = (subtract_vect(obj->pos, app->player->pos));
	}
	return (0);
}
