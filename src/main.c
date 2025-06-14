/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 16:38:05 by abelov            #+#    #+#             */
/*   Updated: 2025/04/28 19:47:32 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "cub3d.h"

int main(int argc, char **argv)
{
	t_info *const app = &(t_info) {
		.title = (char *) "cub3d",
		.fullscreen = GO_TO_FULLSCREEN_ON_LOAD
	};

	printf("fov: %d halffov: %f\n", app->fov_deg, app->fov_rad_half);
	while (app->state != STATE_END)
		app->state = run_state(app, argc, argv);
	cleanup(app);
	return (EXIT_SUCCESS);
}
