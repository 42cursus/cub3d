/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   credits.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 16:14:27 by fsmyth            #+#    #+#             */
/*   Updated: 2025/06/04 16:58:16 by abelov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <math.h>
#include <sys/param.h>
#include "cub3d.h"

void update_rocks(t_info *app, t_dummy *dummy)
{
	t_rock *rock;
	t_list *current;

	current = dummy->rocks;
	while (current != NULL)
	{
		rock = current->data;
		place_tex_to_image_scale(app->canvas, rock->tex, round_vect(rock->pos),
								 rock->scale);
		rock->pos.x += rock->speed;
		current = current->next;
	}
}

void spawn_random_rock(t_info *app, double speed)
{
	t_rock *rock;
	int index;

	rock = ft_calloc(1, sizeof(*rock));
	rock->scale = 3;
	rock->pos.x = rand_range(0, WIN_WIDTH - 50);
	if (speed == 0)
		speed = rand_range(-6.0, 6.0);
	if (fabs(speed) > 4)
	{
		index = rand_range(0, 2);
		rock->tex = &app->shtex->rocks[index];
		if (fabs(speed) > 5)
			rock->scale = 4;
	}
	else
	{
		index = rand_range(2, 6);
		rock->tex = &app->shtex->rocks[index];
		if (fabs(speed) > 3)
			rock->scale = 4;
		if (fabs(speed) < 1.5)
			rock->scale = 2;
	}
	rock->pos.y =
		((WIN_HEIGHT - 80) / 12.0) * fabs(speed) * 2 + rand_range(-100, 100);
	if (rock->pos.y < 0)
		rock->pos.y = rand_range(0, 100);
	if (rock->pos.y > WIN_HEIGHT)
		rock->pos.y = WIN_HEIGHT - rand_range(50, 150);
	rock->speed = speed;
	ft_lstadd_back(&app->dummy->rocks, ft_lstnew(rock));
}
