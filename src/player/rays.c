/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rays.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/25 17:54:19 by fsmyth            #+#    #+#             */
/*   Updated: 2025/03/25 20:18:11 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/cub3d.h"

double	get_gradient_dir(t_vect dir)
{
	return (dir.y / dir.x);
}

double	get_gradient_angle(double angle)
{
	return (tan(angle));
}

double	get_y_intercept(t_vect pos, double gradient)
{
	return (pos.y - (gradient * pos.x));
}

t_vect	get_horizontal_int(double y, double gradient, double c)
{
	t_vect	out;

	out.y = y;
	out.x = (y - c) / gradient;
	return (out);
}

t_vect	get_vertical_int(double x, double gradient, double c)
{
	t_vect	out;

	out.x = x;
	out.y = (gradient * x) + c;
	return (out);
}

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

t_ray	get_horiz_boundary_intersect(t_data *map, t_player *player, double angle, int quadrant)
{
	double	gradient;
	double	c;
	t_ray	out;
	double	i;

	gradient = get_gradient_angle(angle);
	c = get_y_intercept(player->pos, gradient);
	if (quadrant < 2)
	{
		i = ceil(player->pos.y);
		out.face = SOUTH;
		while (i < map->height)
		{
			out.intcpt = get_horizontal_int(i, gradient, c);
			if (out.intcpt.x < map->width && out.intcpt.x >= 0)
			{
				if (map->map[(int)out.intcpt.y][(int)out.intcpt.x] == '1')
					break ;
				if (map->map[(int)out.intcpt.y][(int)out.intcpt.x] == 'D')
				{
					out.face = DOOR_S;
					break ;
				}
			}
			i += 1.0f;
		}
	}
	else
	{
		i = floor(player->pos.y);
		out.face = NORTH;
		while (i > 0)
		{
			out.intcpt = get_horizontal_int(i, gradient, c);
			// out.intcpt.y -= 0.001;
			if (out.intcpt.x < map->width && out.intcpt.x >= 0)
			{
				if (map->map[(int)out.intcpt.y - 1][(int)out.intcpt.x] == '1')
					break ;
				if (map->map[(int)out.intcpt.y - 1][(int)out.intcpt.x] == 'D')
				{
					out.face = DOOR_N;
					break ;
				}
			}
			i -= 1.0f;
		}
	}
	return (out);
}

t_ray	get_vert_boundary_intersect(t_data *map, t_player *player, double angle, int quadrant)
{
	double	gradient;
	double	c;
	t_ray	out;
	double	i;

	gradient = get_gradient_angle(angle);
	c = get_y_intercept(player->pos, gradient);
	if (quadrant == 0 || quadrant == 3)
	{
		i = ceil(player->pos.x);
		out.face = WEST;
		while (i < map->width)
		{
			out.intcpt = get_vertical_int(i, gradient, c);
			if (out.intcpt.y < map->height && out.intcpt.y >= 0)
			{
				if (map->map[(int)out.intcpt.y][(int)out.intcpt.x] == '1')
					break ;
				if (map->map[(int)out.intcpt.y][(int)out.intcpt.x] == 'D')
				{
					out.face = DOOR_W;
					break ;
				}
			}
			i += 1.0f;
		}
	}
	else
	{
		i = floor(player->pos.x);
		out.face = EAST;
		while (i > 0)
		{
			out.intcpt = get_vertical_int(i, gradient, c);
			if (out.intcpt.y < map->height && out.intcpt.y >= 0)
			{
				if (map->map[(int)out.intcpt.y][(int)out.intcpt.x - 1] == '1')
					break ;
				if (map->map[(int)out.intcpt.y][(int)out.intcpt.x - 1] == 'D')
				{
					out.face = DOOR_E;
					break ;
				}
			}
			i -= 1.0f;
		}
	}
	return (out);
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
	hor_dist2 = (pow(hor_int.intcpt.x - player->pos.x, 2) + pow(hor_int.intcpt.y - player->pos.y, 2));
	vert_dist2 = (pow(vert_int.intcpt.x - player->pos.x, 2) + pow(vert_int.intcpt.y - player->pos.y, 2));
	if (hor_dist2 > vert_dist2)
		return (vert_int);
	return (hor_int);
}

double	get_cam_distance(t_vect pos, double angle, t_vect intcpt)
{
	return (fabs((cos(angle) * (intcpt.y - pos.y)) - (sin(angle) * (intcpt.x - pos.x))));
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
		player->rays[i].distance = get_cam_distance(player->pos, player->angle + M_PI_2, player->rays[i].intcpt);
		i++;
	}
}

// void	cast_all_rays(t_data *map, t_player *player)
// {
// 	int		i;
// 	double	step;
// 	double	angle;
//
// 	step = M_PI_2 / WIN_WIDTH;
// 	player->angle = atan2(player->direction.y, player->direction.x);
// 	angle = player->angle + M_PI_4;
// 	i = 0;
// 	while (i < WIN_WIDTH)
// 	{
// 		player->rays[i] = find_ray_collision(map, player, angle);
// 		player->rays[i].distance = get_cam_distance(player->pos, player->angle + M_PI_2, player->rays[i].intcpt);
// 		angle -= step;
// 		i++;
// 	}
// }
