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

int	main(int argc, char **argv)
{
	t_info *const	app = &(t_info){
		.title = (char *) "cub3d",
		.fullscreen = GO_TO_FULLSCREEN_ON_LOAD
	};


	printf("fov: %d halffov: %f\n", app->fov_deg, app->fov_rad_half);
	printf("n_textures: %f\n", (double)sizeof(t_shtex) / sizeof(t_tex));
	printf("tex_MAX: %d\n", TEX_MAX);
	printf("sizeof entity: %lu Bytes\n", sizeof(t_obj));
	printf("sizeof sobj: %lu Bytes\n", sizeof(t_sobj));
	printf("sizeof sdoor: %lu Bytes\n", sizeof(t_sdoor));
	printf("sizeof servermsg: %lu Bytes\n", sizeof(t_servermsg));
	// printf("sizeof combined servermsg: %lu Bytes\n", sizeof(t_sobj) * SRV_MAX_OBJECTS + sizeof(t_sdoor) * SRV_MAX_DOORS + 8);
	printf("sizeof clientmsg: %lu Bytes\n", sizeof(t_clientmsg));

	// t_playermult *tree = playermult_new("fintan");
	// playertree_add(&tree, playermult_new("bob"));
	// playertree_add(&tree, playermult_new("fred"));
	// playertree_add(&tree, playermult_new("tom"));
	// playertree_add(&tree, playermult_new("dick"));
	// playertree_add(&tree, playermult_new("harry"));
	//
	// traverse_playertree(tree, IN_ORD, (void (*)(void *))print_playermult);
	// clear_playertree(&tree);
	// return 1;

	while (app->state != STATE_END)
		app->state = run_state(app, argc, argv);
	cleanup(app);
	return (EXIT_SUCCESS);
}
