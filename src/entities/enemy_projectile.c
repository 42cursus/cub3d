/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   enemy_projectile.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 16:02:56 by fsmyth            #+#    #+#             */
/*   Updated: 2025/05/16 16:15:00 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	spawn_enemy_projectile(t_info *app, t_obj *enemy,
							   t_vect dir, int subtype)
{
	t_obj	*projectile;

	projectile = ft_calloc(1, sizeof(*projectile));
	projectile->type = O_EPROJ;
	projectile->pos = enemy->pos;
	projectile->dir = dir;
	projectile->subtype = subtype; //FIXME: `int` subtype vs `t_pr_type` vs `t_subtype`
	projectile->anim2.frames = 4;
	projectile->anim2.duration = 320000;
	if (subtype == P_PHANTOON)
	{
		projectile->anim.loop = 1;
		projectile->anim.frames = 3;
		projectile->anim.duration = 240000;
		projectile->anim.tex = &app->shtex->phantoon_proj[0];
		projectile->anim2.tex = &app->shtex->phantoon_proj[2];
	}
	else if (subtype == P_HOLTZ)
	{
		projectile->anim.frames = 1;
		projectile->anim.tex = app->shtex->proj_green_tex;
		projectile->anim2.tex = app->shtex->proj_green_tex;
	}
	ft_lstadd_back(&app->lvl->projectiles, ft_lstnew(projectile));
}

int	handle_enemy_projectile(t_info *app, t_obj *obj, t_list **current)
{
	t_vect	new_pos;

	if (obj->anim2.active == 1)
	{
		obj->texture = handle_animation(app, obj->anim2);
		if (obj->texture == NULL)
		{
			*current = delete_object(&app->lvl->projectiles, *current);
			return (1);
		}
		return (0);
	}
	obj->texture = handle_animation(app, obj->anim);
	if (vector_distance(obj->pos, app->player->pos) < 0.2)
	{
		subtract_health(app, app->player, 20);
		app->player->dmg_dir = (subtract_vect(obj->pos, app->player->pos));
		return (start_obj_death(obj, app), 0);
	}
	new_pos = add_vect(obj->pos, obj->dir);
	if (ft_strchr("1LDM", app->lvl->map[(int)new_pos.y][(int)new_pos.x]))
		return (start_obj_death(obj, app), 0);
	obj->pos = new_pos;
	return (0);
}
