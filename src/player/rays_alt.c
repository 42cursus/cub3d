/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rays_alt.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/04 14:58:41 by fsmyth            #+#    #+#             */
/*   Updated: 2025/04/04 22:02:39 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/cub3d.h"

int		get_quadrant(double angle);
double	get_gradient_angle(double angle);
double	get_y_intercept(t_fvect pos, double gradient);
t_fvect	get_vertical_int(double x, double gradient, double c);
t_fvect	get_horizontal_int(double y, double gradient, double c);
double	get_cam_distance(t_fvect pos, double angle, t_fvect intcpt);
void	add_in_front(t_ray *ray, int face);

void	calculate_ray_stuff(t_ray *ray, t_player *player, double gradient, double c)
{
	int	face_mod;

	face_mod = ray->face % 4;
	if (face_mod == 3 || face_mod == 0)
		ray->intcpt = get_horizontal_int(ray->intcpt.y, gradient, c);
	else
		ray->intcpt = get_vertical_int(ray->intcpt.x, gradient, c);
	ray->distance = get_cam_distance(player->pos, player->angle + M_PI_2, ray->intcpt);
	if (ray->distance < 0.00001)
		ray->distance = 0.00001;
	// (void)player;
}

t_ray	ray_dda(t_data *map, t_player *player, double angle)
{
	t_ray	ray;
	ray.intcpt.x = floor(player->pos.x);
	ray.intcpt.y = floor(player->pos.y);
	t_fvect	dir = rotate_vect(player->direction, angle);

	double	sideDistX;
	double	sideDistY;
	double	deltaDistX = fabs(1.0 / dir.x);
	double	deltaDistY = fabs(1.0 / dir.y);
	int		stepX;
	int		stepY;
	int		normX;
	int		normY;
	int		faces[2];
	double	gradient;
	double	c;
	gradient = get_gradient_angle(player->angle + angle);
	c = get_y_intercept(player->pos, gradient);
	ray.in_front = NULL;
	if (dir.x < 0)
	{
		stepX = -1;
		normX = 1;
		sideDistX = (player->pos.x - ray.intcpt.x) * deltaDistX;
		faces[0] = EAST;
	}
	else
	{
		stepX = 1;
		normX = 0;
		sideDistX = (ray.intcpt.x + 1.0 - player->pos.x) * deltaDistX;
		faces[0] = WEST;
	}
	if (dir.y < 0)
	{
		stepY = -1;
		normY = 1;
		sideDistY = (player->pos.y - ray.intcpt.y) * deltaDistY;
		faces[1] = NORTH;
	}
	else
	{
		stepY = 1;
		normY = 0;
		sideDistY = (ray.intcpt.y + 1.0 - player->pos.y) * deltaDistY;
		faces[1] = SOUTH;
	}
	while (1)
	{
		if (sideDistX < sideDistY)
		{
			sideDistX += deltaDistX;
			ray.intcpt.x += stepX;
			ray.face = faces[0];
			// ray.distance = sideDistX - deltaDistX;
		}
		else
		{
			sideDistY += deltaDistY;
			ray.intcpt.y += stepY;
			ray.face = faces[1];
			// ray.distance = sideDistY - deltaDistY;
		}
		if (map->map[(int)ray.intcpt.y][(int)ray.intcpt.x] == '1')
			break ;
		else if (map->map[(int)ray.intcpt.y][(int)ray.intcpt.x] >= 'D')
		{
			if (map->map[(int)ray.intcpt.y][(int)ray.intcpt.x] == 'O')
				add_in_front(&ray, ray.face + 8);
			else
				add_in_front(&ray, ray.face + 4);
			ray.in_front->maptile.x = (int)ray.intcpt.x;
			ray.in_front->maptile.y = (int)ray.intcpt.y;
			ray.in_front->intcpt.x += normX;
			ray.in_front->intcpt.y += normY;
			calculate_ray_stuff(ray.in_front, player, gradient, c);
		}
	}
	ray.maptile.x = (int)ray.intcpt.x;
	ray.maptile.y = (int)ray.intcpt.y;
	ray.intcpt.x += normX;
	ray.intcpt.y += normY;
	calculate_ray_stuff(&ray, player, gradient, c);
	return (ray);
}
