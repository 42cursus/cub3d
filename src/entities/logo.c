/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 19:51:25 by fsmyth            #+#    #+#             */
/*   Updated: 2025/06/05 09:33:49 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	spawn_logo_piece(t_info *app, t_vect pos, t_vect dir, t_texture *texture)
{
	t_object	*logo_piece;
	t_lvl		*map;

	map = app->map;
	logo_piece = ft_calloc(1, sizeof(*logo_piece));
	logo_piece->end_pos = pos;
	logo_piece->pos = subtract_vect(pos, scale_vect(dir, 150));
	logo_piece->type = O_LOGO;
	logo_piece->texture = texture;
	logo_piece->dir = dir;
	printf("\e[34mpiece\e[m:\t%2ld\t\e[34mdir\e[m:\t\e[31m% f\e[m, \e[32m% f\e[m\n", texture - app->shtex->logo_tex, dir.x, dir.y);
	ft_lstadd_back(&map->logo, ft_lstnew(logo_piece));
}

int	handle_obj_logo(t_info *app, t_object *obj)
{
	if (vector_distance(obj->pos, obj->end_pos) > 0.001)
	{
		obj->pos = add_vect(obj->pos, obj->dir);
		return (0);
	}
	return (1);
	(void)app;
}

void	update_logo_pieces(t_info *app, t_player *player, t_lvl *map)
{
	t_list		*current;
	t_object	*obj;
	int			stopped;

	current = map->logo;
	stopped = 0;
	while (current != NULL)
	{
		obj = (t_object *)current->data;
		stopped += handle_obj_logo(app, obj);
		obj->norm = rotate_vect(scale_vect(player->dir, 0.5), M_PI_2);
		obj->p2 = add_vect(obj->pos, obj->norm);
		current = current->next;
	}
	if (player->dead == 0 && stopped == 14)
	{
		player->dead = 1;
		app->timer.cur_lvl_start = get_time_ms();
	}
}
