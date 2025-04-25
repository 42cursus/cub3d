/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rays.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/25 17:54:19 by fsmyth            #+#    #+#             */
/*   Updated: 2025/04/04 20:34:48 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

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

void	add_in_front(t_ray *ray, int face, t_texarr *texture)
{
	t_ray	*in_front;
	t_ray	*new;

	new = ft_calloc(1, sizeof(*new));
	new->intcpt = ray->intcpt;
	new->face = face;
	new->texture = texture;
	in_front = ray->in_front;
	ray->in_front = new;
	new->in_front = in_front;
}

void	free_ray_children(t_ray *ray)
{
	if (ray->in_front != NULL)
		free_ray_children(ray->in_front);
	free(ray->in_front);
}

// t_ray	get_horiz_boundary_intersect(t_data *map, t_player *player, double angle, int quadrant)
// {
// 	double	gradient;
// 	double	c;
// 	t_ray	out;
// 	double	i;
//
// 	gradient = get_gradient_angle(angle);
// 	c = get_y_intercept(player->pos, gradient);
// 	out.in_front = NULL;
// 	if (quadrant < 2)
// 	{
// 		i = ceil(player->pos.y);
// 		out.face = SOUTH;
// 		while (i < map->height)
// 		{
// 			out.intcpt = get_horizontal_int(i, gradient, c);
// 			if (out.intcpt.x < map->width && out.intcpt.x >= 0)
// 			{
// 				if (map->map[(int)out.intcpt.y][(int)out.intcpt.x] == '1')
// 					break ;
// 				if (map->map[(int)out.intcpt.y][(int)out.intcpt.x] == 'D')
// 				{
// 					out.face = DOOR_S;
// 					break ;
// 				}
// 				if (map->map[(int)out.intcpt.y][(int)out.intcpt.x] == 'O' && out.in_front == NULL)
// 				// if (map->map[(int)out.intcpt.y][(int)out.intcpt.x] == 'O')
// 					add_in_front(&out, DOOR_S_OPEN, &map->door_tex[1]);
// 			}
// 			i += 1.0f;
// 		}
// 	}
// 	else
// 	{
// 		i = floor(player->pos.y);
// 		out.face = NORTH;
// 		while (i > 0)
// 		{
// 			out.intcpt = get_horizontal_int(i, gradient, c);
// 			// out.intcpt.y -= 0.001;
// 			if (out.intcpt.x < map->width && out.intcpt.x >= 0)
// 			{
// 				if (map->map[(int)out.intcpt.y - 1][(int)out.intcpt.x] == '1')
// 					break ;
// 				if (map->map[(int)out.intcpt.y - 1][(int)out.intcpt.x] == 'D')
// 				{
// 					out.face = DOOR_N;
// 					break ;
// 				}
// 				if (map->map[(int)out.intcpt.y - 1][(int)out.intcpt.x] == 'O' && out.in_front == NULL)
// 				// if (map->map[(int)out.intcpt.y - 1][(int)out.intcpt.x] == 'O')
// 					add_in_front(&out, DOOR_N_OPEN, &map->door_tex[1]);
// 			}
// 			i -= 1.0f;
// 		}
// 	}
// 	return (out);
// }
//
// t_ray	get_vert_boundary_intersect(t_data *map, t_player *player, double angle, int quadrant)
// {
// 	double	gradient;
// 	double	c;
// 	t_ray	out;
// 	double	i;
//
// 	gradient = get_gradient_angle(angle);
// 	c = get_y_intercept(player->pos, gradient);
// 	out.in_front = NULL;
// 	if (quadrant == 0 || quadrant == 3)
// 	{
// 		i = ceil(player->pos.x);
// 		out.face = WEST;
// 		while (i < map->width)
// 		{
// 			out.intcpt = get_vertical_int(i, gradient, c);
// 			if (out.intcpt.y < map->height && out.intcpt.y >= 0)
// 			{
// 				if (map->map[(int)out.intcpt.y][(int)out.intcpt.x] == '1')
// 					break ;
// 				if (map->map[(int)out.intcpt.y][(int)out.intcpt.x] == 'D')
// 				{
// 					out.face = DOOR_W;
// 					break ;
// 				}
// 				if (map->map[(int)out.intcpt.y][(int)out.intcpt.x] == 'O' && out.in_front == NULL)
// 				// if (map->map[(int)out.intcpt.y][(int)out.intcpt.x] == 'O')
// 					add_in_front(&out, DOOR_W_OPEN, &map->door_tex[1]);
// 			}
// 			i += 1.0f;
// 		}
// 	}
// 	else
// 	{
// 		i = floor(player->pos.x);
// 		out.face = EAST;
// 		while (i > 0)
// 		{
// 			out.intcpt = get_vertical_int(i, gradient, c);
// 			if (out.intcpt.y < map->height && out.intcpt.y >= 0)
// 			{
// 				if (map->map[(int)out.intcpt.y][(int)out.intcpt.x - 1] == '1')
// 					break ;
// 				if (map->map[(int)out.intcpt.y][(int)out.intcpt.x - 1] == 'D')
// 				{
// 					out.face = DOOR_E;
// 					break ;
// 				}
// 				if (map->map[(int)out.intcpt.y][(int)out.intcpt.x - 1] == 'O' && out.in_front == NULL)
// 				// if (map->map[(int)out.intcpt.y][(int)out.intcpt.x - 1] == 'O')
// 					add_in_front(&out, DOOR_E_OPEN, &map->door_tex[1]);
// 			}
// 			i -= 1.0f;
// 		}
// 	}
// 	return (out);
// }

void	handle_in_fronts(t_ray *ray1, t_ray *ray2, t_player *player)
{
	double	r1_infront_dist;
	double	r2_infront_dist;

	r1_infront_dist = 99999999999;
	r2_infront_dist = 99999999999;
	if (ray1->in_front != NULL)
		r1_infront_dist = (pow(ray1->in_front->intcpt.x - player->pos.x, 2) + pow(ray1->in_front->intcpt.y - player->pos.y, 2));
	if (ray2->in_front != NULL)
		r2_infront_dist = (pow(ray2->in_front->intcpt.x - player->pos.x, 2) + pow(ray2->in_front->intcpt.y - player->pos.y, 2));
	if (r1_infront_dist < r2_infront_dist)
	{
		free(ray2->in_front);
		ray2->in_front = ray1->in_front;
		if (r1_infront_dist > ray2->distance)
		{
			free(ray2->in_front);
			ray2->in_front = NULL;
		}
	}
	else
	{
		free(ray1->in_front);
		ray1->in_front = ray2->in_front;
		if (r2_infront_dist > ray1->distance)
		{
			free(ray1->in_front);
			ray1->in_front = NULL;
		}
	}
}

double	get_cam_distance(t_vect pos, double angle, t_vect intcpt)
{
	return (fabs((cos(angle) * (intcpt.y - pos.y)) - (sin(angle) * (intcpt.x - pos.x))));
}

void	cast_all_rays_alt(t_info *app, t_data *map, t_player *player)
{
	int		i;

	player->angle = atan2(player->dir.y, player->dir.x);
	i = 0;
	while (i < WIN_WIDTH)
	{
		player->rays[i] = ray_dda(app, map, player, player->angle_offsets[i]);
		i++;
	}
}
