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

t_vect	get_horiz_boundary_intersect(t_data *map, t_player *player, double angle, int quadrant)
{
	double	gradient;
	double	c;
	t_vect	out;
	double	i;

	gradient = get_gradient_angle(angle);
	c = get_y_intercept(player->pos, gradient);
	if (quadrant < 2)
	{
		i = ceil(player->pos.y);
		while (i < map->height)
		{
			out = get_horizontal_int(i, gradient, c);
			if (out.x < map->width && out.x >= 0 && map->map[(int)out.y][(int)out.x] == '1')
				break ;
			i += 1.0f;
		}
	}
	else
	{
		i = floor(player->pos.y);
		while (i > 0)
		{
			out = get_horizontal_int(i, gradient, c);
			out.y -= 0.001;
			if (out.x < map->width && out.x >= 0 && map->map[(int)out.y][(int)out.x] == '1')
				break ;
			i -= 1.0f;
		}
	}
	return (out);
}

t_vect	get_vert_boundary_intersect(t_data *map, t_player *player, double angle, int quadrant)
{
	double	gradient;
	double	c;
	t_vect	out;
	double	i;

	gradient = get_gradient_angle(angle);
	c = get_y_intercept(player->pos, gradient);
	if (quadrant == 0 || quadrant == 3)
	{
		i = ceil(player->pos.x);
		while (i < map->width)
		{
			out = get_vertical_int(i, gradient, c);
			if (out.y < map->height && out.y >= 0 && map->map[(int)out.y][(int)out.x] == '1')
				break ;
			i += 1.0f;
		}
	}
	else
	{
		i = floor(player->pos.x);
		while (i > 0)
		{
			out = get_vertical_int(i, gradient, c);
			out.x -= 0.001;
			if (out.y < map->height && out.y >= 0 && map->map[(int)out.y][(int)out.x] == '1')
				break ;
			i -= 1.0f;
		}
	}
	return (out);
}

t_vect	find_ray_collision(t_data *map, t_player *player, double angle)
{
	t_vect	hor_int;
	t_vect	vert_int;
	double	hor_dist2;
	double	vert_dist2;
	int		quadrant;

	quadrant = get_quadrant(angle);
	hor_int = get_horiz_boundary_intersect(map, player, angle, quadrant);
	vert_int = get_vert_boundary_intersect(map, player, angle, quadrant);
	hor_dist2 = (pow(hor_int.x - player->pos.x, 2) + pow(hor_int.y - player->pos.y, 2));
	vert_dist2 = (pow(vert_int.x - player->pos.x, 2) + pow(vert_int.y - player->pos.y, 2));
	if (hor_dist2 > vert_dist2)
		return (vert_int);
	return (hor_int);
}

void	cast_all_rays(t_data *map, t_player *player)
{
	int		i;
	double	step;
	double	angle;

	step = M_PI_2 / 800;
	angle = player->angle + M_PI_4;
	i = 0;
	while (i < 800)
	{
		player->rays[i] = find_ray_collision(map, player, angle);
		angle -= step;
		i++;
	}
}

int	determine_face(t_vect intersect)
{
	double	modx;
	double	mody;

	modx = fmod(intersect.x, 1);
	mody = fmod(intersect.y, 1);
	if (modx == 0)
		return (WEST);
	if (mody == 0)
		return (SOUTH);
	if (modx >= 0.9989 && modx < 0.9991)
		return (EAST);
	return (NORTH);
}
