/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   refresh_map.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/08 14:47:23 by fsmyth            #+#    #+#             */
/*   Updated: 2025/08/08 14:48:12 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	respawn_enemies(t_info *app, t_lvl *lvl)
{
	t_list	*cur_node;
	t_enpos	*cur_pos;
	t_vect	dir;
	t_vect	def;

	def = (t_vect){0, 1};
	ft_lstclear(&lvl->enemies, free);
	cur_node = lvl->enemy_pos;
	while (cur_node != NULL)
	{
		cur_pos = (t_enpos *)cur_node->data;
		if (cur_pos->type != E_PHANTOON)
		{
			dir = rotate_vect(def, rand_range(-M_PI, M_PI));
			spawn_enemy(app, cur_pos->pos, dir, cur_pos->type);
		}
		else if (lvl->boss_obj != NULL)
			lvl->boss_obj = spawn_enemy(app, cur_pos->pos, def, E_PHANTOON);
		cur_node = cur_node->next;
	}
}

int	is_drop(t_list *node)
{
	return (((t_obj *)node->content)->subtype >= I_AMMO_M
		&& ((t_obj *)node->content)->subtype <= I_HEALTH);
}

void	remove_drops(t_lvl *lvl)
{
	t_list	*current;
	t_list	*temp;

	current = lvl->items;
	if (current == NULL)
		return ;
	while (is_drop(current))
	{
		lvl->items = current->next;
		ft_lstdelone(current, free);
		current = lvl->items;
		if (current == NULL)
			return ;
	}
	while (current->next != NULL)
	{
		if (is_drop(current->next))
		{
			temp = current->next->next;
			ft_lstdelone(current->next, free);
			current->next = temp;
			continue ;
		}
		current = current->next;
	}
}

void	reset_doors(t_lvl *lvl)
{
	int	i;
	int	j;

	i = -1;
	while (++i < lvl->height)
	{
		j = -1;
		while (++j < lvl->width)
		{
			if (lvl->map[i][j] == 'O')
				lvl->map[i][j] = 'D';
		}
	}
}

void	refresh_map(t_info *app, t_lvl *lvl)
{
	respawn_enemies(app, lvl);
	ft_lstclear(&lvl->projectiles, free);
	remove_drops(lvl);
	reset_doors(lvl);
	reset_anims(app, lvl);
}
