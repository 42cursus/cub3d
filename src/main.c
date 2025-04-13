/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 16:38:05 by abelov            #+#    #+#             */
/*   Updated: 2025/04/09 00:29:05 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "../include/cub3d.h"

int	main(int argc, char **argv)
{
	t_info *const	app = &(t_info){.title = (char *)"cub3d", .win = {
		.width = WIN_WIDTH, .height = WIN_HEIGHT
	}};

	while (app->state != STATE_END)
		app->state = run_state(app, argc, argv);
	cleanup(app);
	return (EXIT_SUCCESS);
}
