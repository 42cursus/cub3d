/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rays.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/25 17:54:19 by fsmyth            #+#    #+#             */
/*   Updated: 2025/07/13 18:59:57 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

int	get_quadrant(double angle)
{
	if (angle > M_PI)
		angle -= 2 * M_PI;
	else if (angle < -M_PI)
		angle += 2 * M_PI;
	if (angle > M_PI_2)
		return (1);
	if (angle > 0)
		return (0);
	if (angle <= -M_PI_2)
		return (2);
	if (angle <= 0)
		return (3);
	return (-1);
}

t_ray	get_horiz_boundary_intersect(t_data *map,
			t_player *player, double angle, int quadrant)
{
	double	gradient;
	double	c;
	t_ray	out;

	gradient = get_gradient_angle(angle);
	c = get_y_intercept(player->pos, gradient);
	if (quadrant < 2)
		return (get_horiz_intersect_south(player, map, gradient, c));
	else
		return (get_horiz_intersect_north(player, map, gradient, c));
	return (out);
}

t_ray	get_vert_boundary_intersect(t_data *map,
			t_player *player, double angle, int quadrant)
{
	double	gradient;
	double	c;

	gradient = get_gradient_angle(angle);
	c = get_y_intercept(player->pos, gradient);
	if (quadrant == 0 || quadrant == 3)
		return (get_vert_intersect_west(player, map, gradient, c));
	else
		return (get_vert_intersect_east(player, map, gradient, c));
}

t_ray	find_ray_collision(t_data *map, t_player *player, double angle)
{
	t_ray	hor_int;
	t_ray	vert_int;
	double	hor_dist2;
	double	vert_dist2;
	int		quadrant;

	quadrant = get_quadrant(angle);
	hor_int = get_horiz_boundary_intersect(map, player, angle, quadrant);
	vert_int = get_vert_boundary_intersect(map, player, angle, quadrant);
	hor_dist2 = (pow(hor_int.intcpt.x - player->pos.x, 2)
			+ pow(hor_int.intcpt.y - player->pos.y, 2));
	vert_dist2 = (pow(vert_int.intcpt.x - player->pos.x, 2)
			+ pow(vert_int.intcpt.y - player->pos.y, 2));
	if (hor_dist2 > vert_dist2)
		return (vert_int);
	return (hor_int);
}

void	cast_all_rays(t_data *map, t_player *player)
{
	int		i;
	double	angle;

	player->angle = atan2(player->direction.y, player->direction.x);
	i = 0;
	while (i < WIN_WIDTH)
	{
		angle = player->angle + player->angle_offsets[i];
		player->rays[i] = find_ray_collision(map, player, angle);
		player->rays[i].distance = get_cam_distance(player->pos,
				player->angle + M_PI_2, player->rays[i].intcpt);
		i++;
	}
}
