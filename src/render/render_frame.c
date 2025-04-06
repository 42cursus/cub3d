/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_frame.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/04 18:07:08 by fsmyth            #+#    #+#             */
/*   Updated: 2025/04/04 22:55:39 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/cub3d.h"
#include <sys/time.h>

size_t	get_time_ms(void)
{
	struct timeval	current_time;

	gettimeofday(&current_time, NULL);
	return (current_time.tv_sec * 1000 + current_time.tv_usec / 1000);
}

void	update_objects(t_player *player, t_data *map)
{
	t_list		*current;
	t_object	*obj;

	current = map->objects;
	while (current != NULL)
	{
		obj = (t_object *)current->data;
		obj->pos = add_vect(obj->pos, obj->dir);
		obj->norm = rotate_vect(player->direction, -M_PI_2);
		obj->p2 = add_vect(obj->pos, obj->norm);
		current = current->next;
	}
}

int	render_next_frame(void *param)
{
	t_info	*app;

	app = param;
	free_ray_children(&app->player->rays[WIN_WIDTH / 2]);
	// app->map->testobj.norm = rotate_vect(app->player->direction, -M_PI_2);
	// app->map->testobj.p2 = add_vect(app->map->testobj.pos, app->map->testobj.norm);
	update_objects(app->player, app->map);
	replace_image(app);
	// printf("player_pos:\t(%f, %f)\n", app->player->pos.x, app->player->pos.y);
	// exit(0);
	while (get_time_ms() - app->last_frame < 20)
		usleep(200);
	app->last_frame = get_time_ms();
	app->framecount++;
	on_expose(app);
	return (0);
}
