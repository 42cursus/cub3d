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

void	spawn_logo_piece(t_info *app, t_vect pos, t_vect dir, t_tex *texture)
{
	t_obj		*logo_piece;
	t_lvl *const	lvl = app->lvl;

	dir = rotate_vect(dir, rand_range(-M_PI_2, M_PI_2));
	// dir = rotate_vect(dir, rand_range(-M_PI_4 * 1.5, M_PI_4 * 1.5));
	dir = scale_vect(dir, 0.013);
	logo_piece = ft_calloc(1, sizeof(*logo_piece));
	logo_piece->end_pos = pos;
	logo_piece->pos = subtract_vect(pos, scale_vect(dir, 150));
	logo_piece->type = O_LOGO;
	logo_piece->texture = texture;
	logo_piece->dir = dir;
	printf("\e[34mpiece\e[m:\t%2ld\t\e[34mdir\e[m:\t\e[31m% f\e[m, \e[32m% f\e[m\n", texture - app->shtex->logo_tex, dir.x, dir.y);
	ft_lstadd_back(&lvl->logo, ft_lstnew(logo_piece));
}

int	handle_obj_logo(t_info *app, t_obj *obj)
{
	if (vector_distance(obj->pos, obj->end_pos) > 0.001)
	{
		obj->pos = add_vect(obj->pos, obj->dir);
		// obj->dir = scale_vect(obj->dir, 0.998);
		return (0);
	}
	return (1);
	(void)app;
}

void	update_logo_pieces(t_info *app, t_player *player, t_lvl *lvl)
{
	t_list		*current;
	t_obj	*obj;
	int			stopped;

	current = lvl->logo;
	stopped = 0;
	while (current != NULL)
	{
		obj = (t_obj *)current->data;
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

void	init_logo_pieces(t_info *app, t_vect pos)
{
	t_vect	pos2;
	t_vect	pos3;

	pos2 = add_vect(pos, (t_vect){0.75, 0});
	pos3 = add_vect(pos, (t_vect){1.6, 0});
	spawn_logo_piece(app, pos, (t_vect){0, 1}, &app->shtex->logo_tex[0]);
	spawn_logo_piece(app, pos, (t_vect){0, 1}, &app->shtex->logo_tex[1]);
	spawn_logo_piece(app, pos, (t_vect){0, 1}, &app->shtex->logo_tex[2]);
	spawn_logo_piece(app, pos2, (t_vect){0, 1}, &app->shtex->logo_tex[3]);
	spawn_logo_piece(app, pos2, (t_vect){0, 1}, &app->shtex->logo_tex[4]);
	spawn_logo_piece(app, pos2, (t_vect){0, 1}, &app->shtex->logo_tex[5]);
	spawn_logo_piece(app, pos2, (t_vect){0, 1}, &app->shtex->logo_tex[6]);
	spawn_logo_piece(app, pos3, (t_vect){0, 1}, &app->shtex->logo_tex[7]);
	spawn_logo_piece(app, pos3, (t_vect){0, 1}, &app->shtex->logo_tex[8]);
	spawn_logo_piece(app, pos3, (t_vect){0, 1}, &app->shtex->logo_tex[9]);
	spawn_logo_piece(app, pos3, (t_vect){0, 1}, &app->shtex->logo_tex[10]);
	spawn_logo_piece(app, pos3, (t_vect){0, 1}, &app->shtex->logo_tex[11]);
	spawn_logo_piece(app, pos3, (t_vect){0, 1}, &app->shtex->logo_tex[12]);
	spawn_logo_piece(app, pos3, (t_vect){0, 1}, &app->shtex->logo_tex[13]);
}
