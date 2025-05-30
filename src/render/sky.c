/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sky.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 17:53:25 by fsmyth            #+#    #+#             */
/*   Updated: 2025/05/19 15:42:38 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	replace_sky(t_info *app, char *tex_file)
{
	t_img	**img;
	t_img	*new;
	t_img	tmp;
	int		new_x;

	img = &app->skybox;
	if (*img != NULL)
		mlx_destroy_image(app->mlx, *img);
	new = mlx_xpm_file_to_image(app->mlx, tex_file, &tmp.width, &tmp.height);
	new_x = WIN_WIDTH * 360.0 / app->fov_deg;
	new->height *= ((9 * WIN_WIDTH) / 12) / WIN_HEIGHT;
	new = scale_image(app, new, new_x, WIN_HEIGHT / 2);
	*img = new;
}

static inline __attribute__((always_inline))
void	copy_sky_full(t_img *const sky, t_img const *bg, t_ivect boundary)
{
	int	i;
	int	height;

	u_int (*const pixels_sky)[sky->height][sky->width] = (void *)sky->data;
	u_int (*const pixels_bg)[bg->height][bg->width] = (void *)bg->data;
	height = sky->height;
	i = -1;
	while (++i < height)
		ft_memcpy(&(*pixels_bg)[i][0], &(*pixels_sky)[i][boundary.x],
			WIN_WIDTH * sizeof(u_int));
}

static inline __attribute__((always_inline))
void	copy_sky_split(t_img *const sky, t_img const *bg, t_ivect boundary)
{
	int	i;
	int	height;
	int	copy_width;

	u_int (*const pixels_sky)[sky->height][sky->width] = (void *)sky->data;
	u_int (*const pixels_bg)[bg->height][bg->width] = (void *)bg->data;
	height = sky->height;
	i = -1;
	copy_width = sky->width - boundary.x;
	while (++i < height)
	{
		ft_memcpy(&(*pixels_bg)[i][0], &(*pixels_sky)[i][boundary.x],
			copy_width * sizeof(u_int));
		ft_memcpy(&(*pixels_bg)[i][copy_width], &(*pixels_sky)[i][0],
			(boundary.y + 1) * sizeof(u_int));
	}
}

void	draw_sky_alt(t_info *const app)
{
	const double	angle = atan2(app->player->dir.y, app->player->dir.x);
	t_img *const	sky = app->skybox;
	t_img *const	bg = app->bg;
	int				offset;
	t_ivect			boundary;

	app->player->angle = angle;
	offset = (int)((angle - app->fov_rad_half * 2) * (sky->width / M_PI)) / 2;
	boundary.x = (0 - offset + sky->width) % sky->width;
	boundary.y = (WIN_WIDTH - 1 - offset + sky->width) % sky->width;
	if (boundary.y > boundary.x)
		copy_sky_full(sky, bg, boundary);
	else
		copy_sky_split(sky, bg, boundary);
}
