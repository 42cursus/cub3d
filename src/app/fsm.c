/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fsm.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/13 16:58:10 by abelov            #+#    #+#             */
/*   Updated: 2025/06/05 00:24:23 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"
#include <X11/X.h>
#include <math.h>
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

void	destroy_map(t_lvl *map)
{
	mlx_destroy_image(map->app->mlx, map->minimap_xs);
	mlx_destroy_image(map->app->mlx, map->minimap_xl);
	mlx_destroy_image(map->app->mlx, map->help);
	mlx_destroy_image(map->app->mlx, map->overlay);
	if (map->texs[T_CEILING])
		mlx_destroy_image(map->app->mlx, map->texs[T_CEILING]);
	if (map->texs[T_FLOOR])
		mlx_destroy_image(map->app->mlx, map->texs[T_FLOOR]);
	free_map(map);
}

void	cleanup_maps(t_info *app)
{
	ft_lstclear(&app->lvl_cache, (void (*)(void *))destroy_map);
	get_pooled_ray(1);
}

t_ret_code do_state_initial(void *param, int argc, char **argv)
{
	t_info *const	app = param;

	app->endianness = check_endianness();
	app->mlx = mlx_init();

	set_audio(app);
	set_fonts(app);

	set_fov(app, 110);
	set_framerate(app, FRAMERATE);
	set_sensitivity(app, 7);
	init_audio(app);
	init_fonts(app);
	set_sound_volume(app, 100);
	set_music_volume(app, 100);
	srand(get_time_ms());
	printf("framerate: %ld frametime: %ld fr_scale: %f\n", app->fr_rate, app->fr_delay, app->fr_scale);
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
	app->win = mlx_new_window(app->mlx, WIN_WIDTH, WIN_HEIGHT, app->title);

	app->fr_last = get_time_us();
	app->fr_count = 0;
	app->fr_time = 5000;

	app->stillshot = mlx_new_image(app->mlx, WIN_WIDTH, WIN_HEIGHT);
	app->canvas = mlx_new_image(app->mlx, WIN_WIDTH, WIN_HEIGHT);
	app->pointer = mlx_new_image(app->mlx, 50, 50);
	replace_image(app, &app->bg, NULL);
	if (!app->canvas || !app->stillshot || !app->pointer)
		exit(((void) ft_printf(" !! KO !!\n"), cleanup(app), EXIT_FAILURE));
	toggle_fullscreen(app);
	
	if (SKIP_INTRO)
		return (ok);
	else
		return (extra);
}

t_ret_code do_state_intro(void *param)
{
	t_info *const app = param;
	t_aud *const aud = &app->audio;
	int				old_fps;

	Mix_PlayChannel(ch_music, aud->chunks[snd_intro], 0);
	old_fps = app->fr_rate;
	set_framerate(app, 60);
	app->fr_last = get_time_us();
	mlx_loop(app->mlx);
	set_framerate(app, old_fps);
	return (ok);
}

//t_info *const	app = param;
//t_aud *const	aud = &app->audio;
//int				old_fps;
//
//app->old_fov = app->fov_deg;
//old_fps = app->fr_rate;
//set_fov(app, 70);
//set_framerate(app, 30);
//calculate_credits_offset(app, app->dummy);
//mlx_mouse_hide(app->mlx, app->win);
//app->fr_last = get_time_us();
//mlx_loop(app->mlx);
//set_fov(app, app->old_fov);
//set_framerate(app, old_fps);
//mlx_mouse_show(app->mlx, app->win);


t_ret_code do_state_mmenu(void *param)
{
	t_info *const app = param;

	mlx_loop(app->mlx);
	return (app->rc);
}

t_ret_code do_state_load(void *param)
{
	t_info *const app = param;

	if (app->map->music)
		Mix_PlayChannel(ch_music, app->map->music, -1);
	mlx_loop(app->mlx);
	replace_sky(app, (char *) "./textures/skybox.xpm");

	return (ok);
	(void)app;
}

t_ret_code do_state_play(void *param)
{
	t_info *const app = param;


	mlx_mouse_hide(app->mlx, app->win);
	draw_sky_alt(app);
	draw_nav(app);
	calculate_offsets(app, app->player);
	app->fr_last = get_time_us();
	mlx_loop(app->mlx);
	mlx_mouse_show(app->mlx, app->win);

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

t_ret_code do_state_credits(void *param)
{
	t_info *const	app = param;
	int				old_fps;

	app->old_fov = app->fov_deg;
	old_fps = app->fr_rate;
	set_fov(app, 70);
	set_framerate(app, 30);
	calculate_credits_offset(app, app->dummy);
	mlx_mouse_hide(app->mlx, app->win);
	app->fr_last = get_time_us();
	mlx_loop(app->mlx);
	set_fov(app, app->old_fov);
	set_framerate(app, old_fps);
	mlx_mouse_show(app->mlx, app->win);

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

	replace_image(app, &app->bg, (char *) "./textures/wall.xpm");
	mlx_hook(app->win, DestroyNotify, 0, (void *)&exit_win, app);
	mlx_expose_hook(app->win, &expose_win, app);
	mlx_loop_hook(app->mlx, &render_mmenu, app);
	mlx_hook(app->win, KeyPress, KeyPressMask, (void *) &key_press_mmenu, app);
	app->menu_state.state = MAIN;
	app->menu_state.selected = 0;
	app->menu_state.no_items = 5;
}

void do_initial_to_end(void *param)
{
	t_info *const app = param;

	return ;
	(void)app;
}

void do_initial_to_intro(void *param)
{
	t_info *const app = param;

	app->fr_count = 0;
	app->map = init_map();
	if (parse_cub(app, (char *)"./maps/logo_test.cub"))
	{
		free_map(app->map);
		app->rc = fail;
		return ;
	}
	spawn_logo_piece(app, (t_vect) {14.9, 10}, rotate_vect((t_vect){0, 0.015}, rand_range(-M_PI_2, M_PI_2)), &app->shtex->logo_tex[0]);
	spawn_logo_piece(app, (t_vect) {14.9, 10}, rotate_vect((t_vect){0, 0.015}, rand_range(-M_PI_2, M_PI_2)), &app->shtex->logo_tex[1]);
	spawn_logo_piece(app, (t_vect) {14.9, 10}, rotate_vect((t_vect){0, 0.015}, rand_range(-M_PI_2, M_PI_2)), &app->shtex->logo_tex[2]);
	spawn_logo_piece(app, (t_vect) {14.9, 10}, rotate_vect((t_vect){0, 0.015}, rand_range(-M_PI_2, M_PI_2)), &app->shtex->logo_tex[3]);
	spawn_logo_piece(app, (t_vect) {14.9, 10}, rotate_vect((t_vect){0, 0.015}, rand_range(-M_PI_2, M_PI_2)), &app->shtex->logo_tex[4]);
	spawn_logo_piece(app, (t_vect) {15.9, 10}, rotate_vect((t_vect){0, 0.015}, rand_range(-M_PI_2, M_PI_2)), &app->shtex->logo_tex[5]);
	spawn_logo_piece(app, (t_vect) {15.9, 10}, rotate_vect((t_vect){0, 0.015}, rand_range(-M_PI_2, M_PI_2)), &app->shtex->logo_tex[6]);
	spawn_logo_piece(app, (t_vect) {15.9, 10}, rotate_vect((t_vect){0, 0.015}, rand_range(-M_PI_2, M_PI_2)), &app->shtex->logo_tex[7]);
	spawn_logo_piece(app, (t_vect) {15.9, 10}, rotate_vect((t_vect){0, 0.015}, rand_range(-M_PI_2, M_PI_2)), &app->shtex->logo_tex[8]);
	spawn_logo_piece(app, (t_vect) {15.9, 10}, rotate_vect((t_vect){0, 0.015}, rand_range(-M_PI_2, M_PI_2)), &app->shtex->logo_tex[9]);
	spawn_logo_piece(app, (t_vect) {15.9, 10}, rotate_vect((t_vect){0, 0.015}, rand_range(-M_PI_2, M_PI_2)), &app->shtex->logo_tex[10]);
	spawn_logo_piece(app, (t_vect) {15.9, 10}, rotate_vect((t_vect){0, 0.015}, rand_range(-M_PI_2, M_PI_2)), &app->shtex->logo_tex[11]);
	spawn_logo_piece(app, (t_vect) {15.9, 10}, rotate_vect((t_vect){0, 0.015}, rand_range(-M_PI_2, M_PI_2)), &app->shtex->logo_tex[12]);
	spawn_logo_piece(app, (t_vect) {16.9, 10}, rotate_vect((t_vect){0, 0.015}, rand_range(-M_PI_2, M_PI_2)), &app->shtex->logo_tex[13]);
	app->player = init_player(app);
	fill_with_colour(app->bg, 0x000000, 0x000000);
	// mlx_expose_hook(app->win, &expose_win, app);
	mlx_hook(app->win, KeyPress, KeyPressMask, (void *) &key_press_intro, app);
	mlx_hook(app->win, KeyRelease, KeyReleaseMask, (void *) &key_release_intro, app);
	mlx_hook(app->win, ButtonPress, 0, NULL, app);
	mlx_hook(app->win, ButtonRelease, 0, NULL, app);
	mlx_hook(app->win, MotionNotify, 0, NULL, app);

	mlx_hook(app->win, DestroyNotify, 0, (void *)&exit_win, app);

	mlx_loop_hook(app->mlx, &render_intro, app);
	app->mlx->end_loop = 0;
}

void do_intro_to_mmenu(void *param)
{
	t_info *const app = param;

	cleanup_maps(app);
	free(app->player);
	replace_image(app, &app->bg, (char *) "./textures/wall.xpm");
	mlx_hook(app->win, DestroyNotify, 0, (void *)&exit_win, app);
	mlx_expose_hook(app->win, &expose_win, app);
	mlx_loop_hook(app->mlx, &render_mmenu, app);
	mlx_hook(app->win, KeyPress, KeyPressMask, (void *) &key_press_mmenu, app);
	mlx_hook(app->win, KeyRelease, 0, NULL, app);
	app->mlx->end_loop = 0;
	app->menu_state.state = MAIN;
	app->menu_state.selected = 0;
	app->menu_state.no_items = 5;
}

void do_intro_to_end(void *param)
{
	t_info *const app = param;

	return ;
	(void)app;
}

void do_mmenu_to_load(void *param)
{
	t_info *const app = param;

	app->fr_count = 0;
	app->map = init_map();
	if (parse_cub(app, app->map_ids[app->current_level]))
	{
		free_map(app->map);
		app->rc = fail;
		return ;
	}
	app->player = init_player(app);
	app->player->total_pickups += count_collectables(app->map);
	mlx_loop_hook(app->mlx, &render_load, app);
	app->mlx->end_loop = 0;
	app->timer.total_ms = 0;
	app->timer.cur_lvl_start = 0;
	app->timer.stop_time = 0;
	return ;
	(void)app;
}

void	do_mmenu_to_credits(void *param)
{
	t_info *const app = param;
	t_dummy			*dummy;

	fill_with_colour(app->bg, 0x000000, 0x000000);
	mlx_loop_hook(app->mlx, &render_credits, app);
	app->mlx->end_loop = 0;
	dummy = ft_calloc(1, sizeof(*dummy));
	app->dummy = dummy;
	dummy->dir = (t_vect){0.0, 1.0};
	dummy->pos =  (t_vect){0.0, -0.6};
	dummy->speed = 0.002;
	mlx_hook(app->win, KeyPress, KeyPressMask, (void *) &key_press_credits, app);
	mlx_hook(app->win, KeyRelease, KeyReleaseMask, (void *) &key_release_credits, app);
	mlx_hook(app->win, ButtonPress, 0, NULL, app);
	mlx_hook(app->win, ButtonRelease, 0, NULL, app);
	mlx_hook(app->win, MotionNotify, 0, NULL, app);
}

void do_mmenu_to_intro(void *param)
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


void	do_credits_to_mmenu(void *param)
{
	t_info *const app = param;

	free(app->dummy);
	replace_image(app, &app->bg, (char *) "./textures/wall.xpm");
	ft_memset(app->keys, 0, sizeof(bool) * 16);
	mlx_loop_hook(app->mlx, &render_mmenu, app);
	app->mlx->end_loop = 0;
	mlx_hook(app->win, KeyPress, KeyPressMask, (void *) &key_press_mmenu, app);
	mlx_hook(app->win, ButtonPress, 0, NULL, app);
	mlx_hook(app->win, ButtonRelease, 0, NULL, app);
	mlx_hook(app->win, KeyRelease, 0, NULL, app);
	mlx_hook(app->win, MotionNotify, 0, NULL, app);
	app->menu_state.state = MAIN;
	app->menu_state.selected = 3;
	app->menu_state.no_items = 5;
}

void do_load_to_play(void *param)
{
	t_info *const app = param;

	replace_image(app, &app->bg, NULL);
	// fill_with_colour(app->bg, app->map->f_col, app->map->c_col);
	mlx_loop_hook(app->mlx, &render_play, app);
	app->mlx->end_loop = 0;
	mlx_hook(app->win, KeyPress, KeyPressMask, (void *) &key_press_play, app);
	mlx_hook(app->win, KeyRelease, KeyReleaseMask, (void *) &key_release_play, app);
	mlx_hook(app->win, ButtonPress, ButtonPressMask, (void *) &mouse_press_play, app);
	mlx_hook(app->win, ButtonRelease, ButtonReleaseMask, (void *) &mouse_release_play, app);
	mlx_hook(app->win, MotionNotify, PointerMotionMask, (void *) &mouse_move_play, app);
	app->timer.total_ms += app->timer.stop_time - app->timer.cur_lvl_start;
	app->timer.cur_lvl_start = get_time_ms();

	XGrabPointer(app->mlx->display, app->win->window, True, PointerMotionMask,
				 GrabModeAsync, GrabModeAsync, None, None, CurrentTime);
	mlx_mouse_move(app->mlx, app->win, WIN_WIDTH / 2, WIN_HEIGHT / 2);
	XUngrabPointer(app->mlx->display, CurrentTime);
}

void do_credits_to_end(void *param)
{
	t_info *const app = param;

	return ;
	(void)app;
}

void do_load_to_end(void *param)
{
	t_info *const app = param;

	return ;
	(void)app;
}

void do_play_to_pmenu(void *param)
{
	t_info *const app = param;

	app->mlx->end_loop = 0;

	app->timer.stop_time = get_time_ms();
	ft_memset(app->keys, 0, sizeof(bool) * 16);
	// free_ray_children(&app->player->rays[WIN_WIDTH / 2]);
	replace_frame(app);
	fast_memcpy_test((int *)app->stillshot->data, (int *)app->canvas->data, WIN_HEIGHT * WIN_WIDTH * sizeof(int));
	mlx_loop_hook(app->mlx, &render_pmenu, app);
	// mlx_hook(app->root, KeyPress, KeyPressMask, (void *) &key_press_pmenu, app);
	mlx_hook(app->win, KeyPress, KeyPressMask, (void *) &key_press_mmenu, app);

	mlx_hook(app->win, ButtonPress, 0, NULL, app);
	mlx_hook(app->win, ButtonRelease, 0, NULL, app);
	mlx_hook(app->win, KeyRelease, 0, NULL, app);
	mlx_hook(app->win, MotionNotify, 0, NULL, app);
	app->menu_state.state = PAUSE;
	app->menu_state.selected = 0;
	app->menu_state.no_items = 4;
}

void	do_play_to_load(void *param)
{
	t_info *const	app = param;
	char			*next_lvl;

	app->timer.stop_time = get_time_ms();
	next_lvl = ft_strdup(app->map->sublvls[app->current_sublevel]);
	// cleanup_map(app);
	// if (get_cached_lvl(app, app->map->sublvls[0]) == NULL)
	// 	ft_lstadd_back(&app->lvlcache, ft_lstnew(app->map));
	app->fr_count = 0;
	app->map->starting_pos = app->player->tele_pos;
	move_entity(&app->map->starting_pos, app->map, subtract_vect(app->player->pos, app->player->tele_pos));
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
		app->player->total_pickups += count_collectables(app->map);
	}
	free(next_lvl);
	// app->player = init_player(app);
	refresh_player(app, app->player);
	ft_memset(app->keys, 0, sizeof(bool) * 16);
	app->mlx->end_loop = 0;
	replace_image(app, &app->bg, (char *) "./textures/wall.xpm");
	mlx_loop_hook(app->mlx, &render_load, app);
	mlx_hook(app->win, KeyPress, 0, NULL, app);
	mlx_hook(app->win, KeyRelease, 0, NULL, app);
	mlx_hook(app->win, ButtonPress, 0, NULL, app);
	mlx_hook(app->win, ButtonRelease, 0, NULL, app);
	mlx_hook(app->win, MotionNotify, 0, NULL, app);
}

void do_play_to_win(void *param)
{
	t_info *const app = param;

	app->timer.stop_time = get_time_ms();
	app->timer.total_ms += app->timer.stop_time - app->timer.cur_lvl_start;
	ft_memset(app->keys, 0, sizeof(bool) * 16);
	app->mlx->end_loop = 0;
	replace_image(app, &app->bg, NULL);
	fill_with_colour(app->bg, MLX_LIME, MLX_GREEN);
	mlx_loop_hook(app->mlx, &render_win, app);
	mlx_hook(app->win, KeyPress, KeyPressMask, (void *) &key_press_mmenu, app);
	mlx_hook(app->win, KeyRelease, 0, (void *) &key_release_win, app);

	mlx_hook(app->win, ButtonPress, 0, NULL, app);
	mlx_hook(app->win, ButtonRelease, 0, NULL, app);
	mlx_hook(app->win, MotionNotify, 0, NULL, app);
	if (app->current_level < app->no_maps - 1)
		app->current_level++;
	else
		app->current_level = 0;
	app->menu_state.state = WIN;
	app->menu_state.selected = 0;
	app->menu_state.no_items = 3;
	printf("collected: %d total: %d pcnt: %d%%\n",app->player->pickups_collected, app->player->total_pickups, (app->player->pickups_collected * 100) / app->player->total_pickups);
}

void do_play_to_lose(void *param)
{
	t_info *const app = param;

	ft_memset(app->keys, 0, sizeof(bool) * 16);
	app->mlx->end_loop = 0;
	replace_image(app, &app->bg, NULL);
	fill_with_colour(app->bg, MLX_RED, MLX_LIGHT_RED);
	mlx_loop_hook(app->mlx, &render_lose, app);
	mlx_hook(app->win, KeyPress, KeyPressMask, (void *) &key_press_mmenu, app);
	mlx_hook(app->win, KeyRelease, 0, (void *) &key_release_lose, app);

	mlx_hook(app->win, ButtonPress, 0, NULL, app);
	mlx_hook(app->win, ButtonRelease, 0, NULL, app);
	mlx_hook(app->win, MotionNotify, 0, NULL, app);
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
	mlx_hook(app->win, KeyPress, KeyPressMask, (void *) &key_press_play, app);
	mlx_hook(app->win, ButtonPress, ButtonPressMask, (void *) &mouse_press_play, app);
	mlx_hook(app->win, ButtonRelease, ButtonReleaseMask, (void *) &mouse_release_play, app);
	mlx_hook(app->win, KeyRelease, KeyReleaseMask, (void *) &key_release_play, app);
	mlx_hook(app->win, MotionNotify, PointerMotionMask, (void *) &mouse_move_play, app);
	app->timer.total_ms += app->timer.stop_time - app->timer.cur_lvl_start;
	app->timer.cur_lvl_start = get_time_ms();
}

void do_pmenu_to_mmenu(void *param)
{
	t_info *const app = param;

	cleanup_maps(app);
	free(app->player);
	replace_image(app, &app->bg, (char *) "./textures/wall.xpm");

	mlx_loop_hook(app->mlx, &render_mmenu, app);
	app->mlx->end_loop = 0;

	mlx_hook(app->win, KeyPress, KeyPressMask, (void *) &key_press_mmenu, app);

	mlx_hook(app->win, ButtonPress, 0, NULL, app);
	mlx_hook(app->win, ButtonRelease, 0, NULL, app);
	mlx_hook(app->win, KeyRelease, 0, NULL, app);
	mlx_hook(app->win, MotionNotify, 0, NULL, app);
	app->menu_state.state = MAIN;
	app->menu_state.selected = 0;
	app->menu_state.no_items = 5;
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

	mlx_hook(app->win, KeyPress, KeyPressMask, (void *) &key_press_mmenu, app);

	mlx_hook(app->win, ButtonPress, 0, NULL, app);
	mlx_hook(app->win, ButtonRelease, 0, NULL, app);
	mlx_hook(app->win, KeyRelease, 0, NULL, app);
	mlx_hook(app->win, MotionNotify, 0, NULL, app);
	app->menu_state.state = MAIN;
	app->menu_state.selected = 0;
	app->menu_state.no_items = 5;
}

void do_lose_to_end(void *param)
{
	t_info *const app = param;

	cleanup_maps(app);
	free(app->player);
}

void	do_win_to_credits(void * param)
{
	t_info *const app = param;
	t_dummy			*dummy;

	cleanup_maps(app);
	free(app->player);
	fill_with_colour(app->bg, 0x000000, 0x000000);
	mlx_loop_hook(app->mlx, &render_credits, app);
	app->mlx->end_loop = 0;
	dummy = ft_calloc(1, sizeof(*dummy));
	app->dummy = dummy;
	dummy->dir = (t_vect){0.0, 1.0};
	dummy->pos =  (t_vect){0.0, -0.6};
	dummy->speed = 0.002;
	mlx_hook(app->win, KeyPress, KeyPressMask, (void *) &key_press_credits, app);
	mlx_hook(app->win, KeyRelease, KeyReleaseMask, (void *) &key_release_credits, app);
	mlx_hook(app->win, ButtonPress, 0, NULL, app);
	mlx_hook(app->win, ButtonRelease, 0, NULL, app);
	mlx_hook(app->win, MotionNotify, 0, NULL, app);
}

void do_win_to_mmenu(void *param)
{
	t_info *const app = param;

	cleanup_maps(app);
	free(app->player);
	replace_image(app, &app->bg, (char *) "./textures/wall.xpm");

	mlx_loop_hook(app->mlx, &render_mmenu, app);
	app->mlx->end_loop = 0;

	mlx_hook(app->win, KeyPress, KeyPressMask, (void *) &key_press_mmenu, app);

	mlx_hook(app->win, ButtonPress, 0, NULL, app);
	mlx_hook(app->win, ButtonRelease, 0, NULL, app);
	mlx_hook(app->win, KeyRelease, 0, NULL, app);
	mlx_hook(app->win, MotionNotify, 0, NULL, app);
	app->menu_state.state = MAIN;
	app->menu_state.selected = 0;
	app->menu_state.no_items = 5;
}

void do_win_to_load(void *param)
{
	t_info *const app = param;

	cleanup_maps(app);
	app->fr_count = 0;
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
	mlx_hook(app->win, KeyPress, 0, NULL, app);
	mlx_hook(app->win, KeyRelease, 0, NULL, app);
	mlx_hook(app->win, ButtonPress, 0, NULL, app);
	mlx_hook(app->win, ButtonRelease, 0, NULL, app);
	mlx_hook(app->win, MotionNotify, 0, NULL, app);
	app->timer.total_ms = 0;
	app->timer.cur_lvl_start = 0;
	app->timer.stop_time = 0;
}

void do_lose_to_load(void *param)
{
	t_info *const app = param;

	cleanup_maps(app);
	free(app->player);
	app->map = init_map();
	app->fr_count = 0;
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
	mlx_hook(app->win, KeyPress, 0, NULL, app);
	mlx_hook(app->win, KeyRelease, 0, NULL, app);
	mlx_hook(app->win, ButtonPress, 0, NULL, app);
	mlx_hook(app->win, ButtonRelease, 0, NULL, app);
	mlx_hook(app->win, MotionNotify, 0, NULL, app);
	app->timer.total_ms = 0;
	app->timer.cur_lvl_start = 0;
	app->timer.stop_time = 0;
}

void do_win_to_end(void *param)
{
	t_info *const app = param;

	cleanup_maps(app);
	free(app->player);
}
