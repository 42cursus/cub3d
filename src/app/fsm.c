/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fsm.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/13 16:58:10 by abelov            #+#    #+#             */
/*   Updated: 2025/04/28 19:56:22 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"
#include <math.h>
#include <signal.h>
#include "fsm.h"

t_state run_state(t_info *app, int argc, char **argv)
{
	state_func_t		*state_fun;
	t_transition_func	*transition_func;
	t_transition		transition;
	t_ret_code			rc;
	size_t				i;


	if (app->state == STATE_INITIAL)
		rc = do_state_initial(app, argc, argv);
	else
	{
		state_fun = state_table[app->state];
		rc = state_fun(app);
	}
	i = 0;
	transition.dst_state = app->state;
	while (i < (int) (sizeof(state_transitions) / sizeof(state_transitions[0])))
	{
		transition = state_transitions[i++];
		if (transition.src_state == app->state && transition.ret_code == rc)
			break;
	}
	transition_func = transition_table[app->state][transition.dst_state];
	if (transition_func)
		transition_func(app);
	return (transition.dst_state);
}

int	exit_win(t_info *const	app)
{
	exit(cleanup(app));
}

void	destroy_map(t_data *map)
{
	mlx_destroy_image(map->app->mlx, map->minimap);
	free_map(map);
}

void	cleanup_maps(t_info *app)
{
	ft_lstclear(&app->lvlcache, (void (*)(void *))destroy_map);
	// free_ray_children(&app->player->rays[WIN_WIDTH / 2]);
	get_pooled_ray_alt(1);
	// free(app->player);
}

t_ret_code do_state_initial(void *param, int argc, char **argv)
{
	t_info *const	app = param;

	app->endianness = check_endianness();
	app->mlx = mlx_init();

	set_fov(app, 90);
	set_framerate(app, FRAMERATE);
	printf("framerate: %ld frametime: %ld fr_scale: %f\n", app->framerate, app->fr_delay, app->fr_scale);
	app->map_ids = ft_calloc(argc, sizeof(char *));
	int	i = 0;
	while (++i < argc)
	{
		if (argv[i][ft_strlen(argv[i]) - 1] == '/')
			app->map_ids[i - 1] = ft_strjoin(argv[i], "start.cub");
		else
			app->map_ids[i - 1] = ft_strdup(argv[i]);
	}
	app->no_maps = argc - 1;
	if (app->mlx == NULL)
		return (printf("Error: failed to open map\n"), fail);
	load_shtex(app);
	app->root = mlx_new_window(app->mlx, app->win.width,
							   app->win.height, app->title);

	app->last_frame = get_time_us();
	app->framecount = 0;
	// app->last_frame_us = get_time_us();
	app->frametime = 5000;
	return (ok);
}

t_ret_code do_state_mmenu(void *param)
{
	t_info *const app = param;

	mlx_loop(app->mlx);
	return (app->rc);
}

t_ret_code do_state_load(void *param)
{
	t_info *const app = param;

	mlx_loop(app->mlx);
	replace_sky(app, (char *) "./textures/skybox.xpm");

	return (ok);
	(void)app;
}

t_ret_code do_state_play(void *param)
{
	t_info *const app = param;


	mlx_mouse_hide(app->mlx, app->root);
	draw_sky_alt(app);
	calculate_offsets(app, app->player);
	app->last_frame = get_time_us();
	mlx_loop(app->mlx);
	mlx_mouse_show(app->mlx, app->root);

	return (app->rc);
}


t_ret_code do_state_pmenu(void *param)
{
	t_info *const app = param;

	mlx_loop(app->mlx);
	return (app->rc);
}

t_ret_code do_state_win(void *param)
{
	t_info *const app = param;

	replace_sky(app, (char *)"./textures/skybox1.xpm");
	draw_sky_alt(app);
	mlx_loop(app->mlx);
	return (app->rc);
}

t_ret_code do_state_lose(void *param)
{
	t_info *const app = param;

	replace_sky(app, (char *)"./textures/skybox1.xpm");
	draw_sky_alt(app);
	mlx_loop(app->mlx);
	return (app->rc);
}


//	t_event_list		hooks[3][MLX_MAX_EVENT] = {
//		[INITIAL] = {
//			[KeyPress] = {
//				.mask = KeyPressMask,
//				.hook = (void *) &key_press_initial,
//				.param = app,
//			},
//			[Expose] = {
//				.mask = ExposureMask,
//				.hook = (void *) &expose_win,
//				.param = app,
//			},
//		},
//		[PLAY] = {
//			[KeyPress] = {
//				.mask = KeyPressMask,
//				.hook = (void *) &key_press_play,
//				.param = app,
//			},
//			[KeyRelease] = {
//				.mask = KeyReleaseMask,
//				.hook = (void *) &key_release_play,
//				.param = app,
//			},
//			[ButtonPress] = {
//				.mask = ButtonPressMask,
//				.hook = (void *) &mouse_press_play,
//				.param = app,
//			},
//			[ButtonRelease] = {
//				.mask = ButtonReleaseMask,
//				.hook = (void *) &mouse_release_play,
//				.param = app,
//			},
//			[MotionNotify] = {
//				.mask = PointerMotionMask,
//				.hook = (void *) &mouse_move_play,
//				.param = app,
//			},
//			[Expose] = {
//				.mask = ExposureMask,
//				.hook = (void *) &expose_win,
//				.param = app,
//			},
//		},
//		[GAME_OVER] = {
//			[KeyPress] = {
//				.mask = KeyPressMask,
//				.hook = (void *) &key_press_lose,
//				.param = app,
//			}
//		}
//	};

//	ft_memcpy(app->root->hooks, &hooks[new_state], MLX_MAX_EVENT * sizeof(t_event_list));
void do_initial_to_mmenu(void *param)
{
	t_info *const app = param;

	mlx_hook(app->root, DestroyNotify, 0, (void *)&exit_win, app);
	mlx_expose_hook(app->root, &expose_win, app);
	mlx_loop_hook(app->mlx, &render_mmenu, app);
	mlx_hook(app->root, KeyPress, KeyPressMask, (void *) &key_press_mmenu, app);
	app->menu_state.state = MAIN;
	app->menu_state.selected = 0;
	app->menu_state.no_items = 4;
}

void do_initial_to_end(void *param)
{
	t_info *const app = param;

	return ;
	(void)app;
}

void do_mmenu_to_load(void *param)
{
	t_info *const app = param;

	app->framecount = 0;
	app->map = init_map();
	if (parse_cub(app, app->map_ids[app->current_level]))
	{
		free_map(app->map);
		app->rc = fail;
		return ;
	}
	app->player = init_player(app);
//	replace_bg(app, (char *) "./textures/wall.xpm");
	mlx_loop_hook(app->mlx, &render_load, app);
	app->mlx->end_loop = 0;
	return ;
	(void)app;

}

void do_load_to_play(void *param)
{
	t_info *const app = param;

	// struct s_thing
	// {
	// 	t_vect		pos;
	// 	t_vect		dir;
	// 	t_etype		type;
	// 	t_subtype	subtype;
	// } things[] = {
	// 	{{28.0, 10.5}, {0.02, 0.02}, O_ENTITY, E_ZOOMER},
	// 	{{24.0, 10.5}, {0.0, -0.03}, O_ENTITY, E_ZOOMER},
	// 	{{15.0, 10.5}, {0.02, 0.01}, O_ENTITY, E_ZOOMER},
	// 	{{ 5.0,  5.5}, {0.02, 0.0}, O_ENTITY, E_ZOOMER},
	// 	{{12.5,  1.5}, {0.0, 0.03}, O_ENTITY, E_ZOOMER},
	// 	{{10.5,  5.5}, {0.0, -0.03}, O_ENTITY, E_ZOOMER},
	// 	{{18.5,  4.5}, {0.03, 0.0}, O_ENTITY, E_ZOOMER},
	// 	{{26.5,  11.5}, {0.0, 0.03}, O_ENTITY, E_PHANTOON},
	// 	{.pos = {20.5, 2.5}, .type = O_ITEM, .subtype = I_ETANK},
	// 	{.pos = {18.5, 2.5 }, .type = O_ITEM, .subtype = I_SUPER},
	// 	{.pos = {23.5, 2.5 }, .type = O_ITEM, .subtype = I_MISSILE},
	// 	{.pos = {10.5, 10.5}, .type = O_ITEM, .subtype = I_ETANK},
	// 	{.pos = {6.5, 1.5}, .type = O_ITEM, .subtype = I_TROPHY},
	// };
	//
	// int i = -1;
	// while (++i < (int)(sizeof(things) / sizeof(things[0])))
	// {
	// 	struct s_thing *thing = &things[i];
	// 	if (thing->type == O_ENTITY)
	// 		spawn_enemy(app,  thing->pos, thing->dir, thing->subtype);
	// 	else
	// 		spawn_item(app, thing->pos, thing->subtype);
	// }
	replace_image(app, &app->bg, NULL);
	// fill_with_colour(app->bg, app->map->f_col, app->map->c_col);
	mlx_loop_hook(app->mlx, &render_play, app);
	app->mlx->end_loop = 0;
	mlx_hook(app->root, KeyPress, KeyPressMask, (void *) &key_press_play, app);
	mlx_hook(app->root, ButtonPress, ButtonPressMask, (void *) &mouse_press_play, app);
	mlx_hook(app->root, ButtonRelease, ButtonReleaseMask, (void *) &mouse_release_play, app);
	mlx_hook(app->root, KeyRelease, KeyReleaseMask, (void *) &key_release_play, app);
	mlx_hook(app->root, MotionNotify, PointerMotionMask, (void *) &mouse_move_play, app);
}

void do_load_to_end(void *param)
{
	t_info *const app = param;

	return ;
	(void)app;

}

void do_mmenu_to_end(void *param)
{
	t_info *const app = param;
	return ;
	(void)app;
}

void do_play_to_pmenu(void *param)
{
	t_info *const app = param;

	app->mlx->end_loop = 0;

	ft_memset(app->keys, 0, sizeof(bool) * 16);
	// free_ray_children(&app->player->rays[WIN_WIDTH / 2]);
	replace_frame(app);
	fast_memcpy_test((int *)app->stillshot->data, (int *)app->canvas->data, WIN_HEIGHT * WIN_WIDTH * sizeof(int));
	mlx_loop_hook(app->mlx, &render_pmenu, app);
	// mlx_hook(app->root, KeyPress, KeyPressMask, (void *) &key_press_pmenu, app);
	mlx_hook(app->root, KeyPress, KeyPressMask, (void *) &key_press_mmenu, app);

	mlx_hook(app->root, ButtonPress, 0, NULL, app);
	mlx_hook(app->root, ButtonRelease, 0, NULL, app);
	mlx_hook(app->root, KeyRelease, 0, NULL, app);
	mlx_hook(app->root, MotionNotify, 0, NULL, app);
	app->menu_state.state = PAUSE;
	app->menu_state.selected = 0;
	app->menu_state.no_items = 4;
}

void	do_play_to_load(void *param)
{
	t_info *const	app = param;
	char			*next_lvl;

	next_lvl = ft_strdup(app->map->sublvls[app->current_sublevel]);
	// cleanup_map(app);
	// if (get_cached_lvl(app, app->map->sublvls[0]) == NULL)
	// 	ft_lstadd_back(&app->lvlcache, ft_lstnew(app->map));
	app->framecount = 0;
	app->map->starting_pos = app->player->tele_pos;
	move_entity(app, &app->map->starting_pos, app->map, scale_vect(subtract_vect(app->player->pos, app->player->tele_pos), 2));
	// app->map->starting_pos = add_vect(app->player->pos, scale_vect(subtract_vect(app->player->pos, app->player->tele_pos), 2));
	// app->map->starting_dir = normalise_vect(subtract_vect(app->player->tele_pos, app->player->pos));
	app->map->starting_dir = rotate_vect(app->player->dir, M_PI);
	refresh_map(app, app->map);
	app->map = get_cached_lvl(app, next_lvl);
	if (app->map == NULL)
	{
		app->map = init_map();
		if (parse_cub(app, next_lvl))
		{
			free_map(app->map);
			free(next_lvl);
			app->rc = fail;
			return ;
		}
	}
	free(next_lvl);
	// app->player = init_player(app);
	refresh_player(app, app->player);
	ft_memset(app->keys, 0, sizeof(bool) * 16);
	app->mlx->end_loop = 0;
	replace_image(app, &app->bg, (char *) "./textures/wall.xpm");
	mlx_loop_hook(app->mlx, &render_load, app);
	mlx_hook(app->root, KeyPress, 0, NULL, app);
	mlx_hook(app->root, KeyRelease, 0, NULL, app);
	mlx_hook(app->root, ButtonPress, 0, NULL, app);
	mlx_hook(app->root, ButtonRelease, 0, NULL, app);
	mlx_hook(app->root, MotionNotify, 0, NULL, app);
}

void do_play_to_win(void *param)
{
	t_info *const app = param;

	ft_memset(app->keys, 0, sizeof(bool) * 16);
	app->mlx->end_loop = 0;
	replace_image(app, &app->bg, NULL);
	fill_with_colour(app->bg, MLX_LIME, MLX_GREEN);
	mlx_loop_hook(app->mlx, &render_win, app);
	mlx_hook(app->root, KeyPress, KeyPressMask, (void *) &key_press_mmenu, app);
	mlx_hook(app->root, KeyRelease, 0, (void *) &key_release_win, app);

	mlx_hook(app->root, ButtonPress, 0, NULL, app);
	mlx_hook(app->root, ButtonRelease, 0, NULL, app);
	mlx_hook(app->root, MotionNotify, 0, NULL, app);
	if (app->current_level < app->no_maps - 1)
		app->current_level++;
	app->menu_state.state = WIN;
	app->menu_state.selected = 0;
	app->menu_state.no_items = 3;
}

void do_play_to_lose(void *param)
{
	t_info *const app = param;

	ft_memset(app->keys, 0, sizeof(bool) * 16);
	app->mlx->end_loop = 0;
	replace_image(app, &app->bg, NULL);
	fill_with_colour(app->bg, MLX_RED, MLX_LIGHT_RED);
	mlx_loop_hook(app->mlx, &render_lose, app);
	mlx_hook(app->root, KeyPress, KeyPressMask, (void *) &key_press_mmenu, app);
	mlx_hook(app->root, KeyRelease, 0, (void *) &key_release_lose, app);

	mlx_hook(app->root, ButtonPress, 0, NULL, app);
	mlx_hook(app->root, ButtonRelease, 0, NULL, app);
	mlx_hook(app->root, MotionNotify, 0, NULL, app);
	app->menu_state.state = LOSE;
	app->menu_state.selected = 0;
	app->menu_state.no_items = 3;
}

void do_play_to_end(void *param)
{
	t_info *const app = param;

	cleanup_maps(app);
	free(app->player);
}

void do_pmenu_to_play(void *param)
{
	t_info *const app = param;

	replace_image(app, &app->bg, NULL);
	// fill_with_colour(app->bg, app->map->f_col, app->map->c_col);
	mlx_loop_hook(app->mlx, &render_play, app);
	app->mlx->end_loop = 0;
	mlx_hook(app->root, KeyPress, KeyPressMask, (void *) &key_press_play, app);
	mlx_hook(app->root, ButtonPress, ButtonPressMask, (void *) &mouse_press_play, app);
	mlx_hook(app->root, ButtonRelease, ButtonReleaseMask, (void *) &mouse_release_play, app);
	mlx_hook(app->root, KeyRelease, KeyReleaseMask, (void *) &key_release_play, app);
	mlx_hook(app->root, MotionNotify, PointerMotionMask, (void *) &mouse_move_play, app);
}

void do_pmenu_to_mmenu(void *param)
{
	t_info *const app = param;

	cleanup_maps(app);
	free(app->player);
	replace_image(app, &app->bg, (char *) "./textures/wall.xpm");

	mlx_loop_hook(app->mlx, &render_mmenu, app);
	app->mlx->end_loop = 0;

	mlx_hook(app->root, KeyPress, KeyPressMask, (void *) &key_press_mmenu, app);

	mlx_hook(app->root, ButtonPress, 0, NULL, app);
	mlx_hook(app->root, ButtonRelease, 0, NULL, app);
	mlx_hook(app->root, KeyRelease, 0, NULL, app);
	mlx_hook(app->root, MotionNotify, 0, NULL, app);
	app->menu_state.state = MAIN;
	app->menu_state.selected = 0;
	app->menu_state.no_items = 4;
}

void do_pmenu_to_end(void *param)
{

	t_info *const app = param;

	cleanup_maps(app);
	free(app->player);
}

void do_lose_to_mmenu(void *param)
{
	t_info *const app = param;

	cleanup_maps(app);
	free(app->player);
	replace_image(app, &app->bg, (char *) "./textures/wall.xpm");

	mlx_loop_hook(app->mlx, &render_mmenu, app);
	app->mlx->end_loop = 0;

	mlx_hook(app->root, KeyPress, KeyPressMask, (void *) &key_press_mmenu, app);

	mlx_hook(app->root, ButtonPress, 0, NULL, app);
	mlx_hook(app->root, ButtonRelease, 0, NULL, app);
	mlx_hook(app->root, KeyRelease, 0, NULL, app);
	mlx_hook(app->root, MotionNotify, 0, NULL, app);
	app->menu_state.state = MAIN;
	app->menu_state.selected = 0;
	app->menu_state.no_items = 4;
}

void do_lose_to_end(void *param)
{
	t_info *const app = param;

	cleanup_maps(app);
	free(app->player);
}

void do_win_to_mmenu(void *param)
{
	t_info *const app = param;

	cleanup_maps(app);
	free(app->player);
	replace_image(app, &app->bg, (char *) "./textures/wall.xpm");

	mlx_loop_hook(app->mlx, &render_mmenu, app);
	app->mlx->end_loop = 0;

	mlx_hook(app->root, KeyPress, KeyPressMask, (void *) &key_press_mmenu, app);

	mlx_hook(app->root, ButtonPress, 0, NULL, app);
	mlx_hook(app->root, ButtonRelease, 0, NULL, app);
	mlx_hook(app->root, KeyRelease, 0, NULL, app);
	mlx_hook(app->root, MotionNotify, 0, NULL, app);
	app->menu_state.state = MAIN;
	app->menu_state.selected = 0;
	app->menu_state.no_items = 4;
}

void do_win_to_load(void *param)
{
	t_info *const app = param;

	cleanup_maps(app);
	app->framecount = 0;
	app->map = init_map();
	if (parse_cub(app, app->map_ids[app->current_level]))
	{
		free_map(app->map);
		app->rc = fail;
		return ;
	}
	// app->player = init_player(app);
	refresh_player(app, app->player);
	ft_memset(app->keys, 0, sizeof(bool) * 16);
	app->mlx->end_loop = 0;
	replace_image(app, &app->bg, (char *) "./textures/wall.xpm");
	mlx_loop_hook(app->mlx, &render_load, app);
	mlx_hook(app->root, KeyPress, 0, NULL, app);
	mlx_hook(app->root, KeyRelease, 0, NULL, app);
	mlx_hook(app->root, ButtonPress, 0, NULL, app);
	mlx_hook(app->root, ButtonRelease, 0, NULL, app);
	mlx_hook(app->root, MotionNotify, 0, NULL, app);
}

void do_lose_to_load(void *param)
{
	t_info *const app = param;

	cleanup_maps(app);
	free(app->player);
	app->map = init_map();
	app->framecount = 0;
	if (parse_cub(app, app->map_ids[app->current_level]))
	{
		free_map(app->map);
		app->rc = fail;
		return ;
	}
	app->player = init_player(app);
	ft_memset(app->keys, 0, sizeof(bool) * 16);
	app->mlx->end_loop = 0;
	replace_image(app, &app->bg, (char *) "./textures/wall.xpm");
	mlx_loop_hook(app->mlx, &render_load, app);
	mlx_hook(app->root, KeyPress, 0, NULL, app);
	mlx_hook(app->root, KeyRelease, 0, NULL, app);
	mlx_hook(app->root, ButtonPress, 0, NULL, app);
	mlx_hook(app->root, ButtonRelease, 0, NULL, app);
	mlx_hook(app->root, MotionNotify, 0, NULL, app);
}

void do_win_to_end(void *param)
{
	t_info *const app = param;

	cleanup_maps(app);
	free(app->player);
}
