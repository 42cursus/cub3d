/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   settings.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/19 15:13:37 by fsmyth            #+#    #+#             */
/*   Updated: 2025/05/19 15:13:59 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	set_sensitivity(t_info *app, int sensitivity)
{
	if (sensitivity < 1)
		sensitivity = 1;
	if (sensitivity > 10)
		sensitivity = 10;
	app->sensitivity = sensitivity;
}

void	set_fov(t_info *app, int fov)
{
	if (fov < 45)
		fov = 45;
	else if (fov > 140)
		fov = 140;
	app->fov_deg = fov;
	app->fov_rad_half = ((double)fov / 360.0) * M_PI;
	app->fov_opp_len = tan(app->fov_rad_half);
}

void	set_framerate(t_info *app, size_t framerate)
{
	if (framerate < 30)
		framerate = 30;
	else if (framerate > 500)
		framerate = 500;
	app->framerate = framerate;
	app->fr_delay = 1000000 / framerate;
	app->fr_scale = framerate / 50.0;
	printf("framerate: %ld frametime: %ld fr_scale: %f\n", app->framerate, app->fr_delay, app->fr_scale);
}
