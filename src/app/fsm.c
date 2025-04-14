/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fsm.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/13 16:58:10 by abelov            #+#    #+#             */
/*   Updated: 2025/04/13 16:58:12 by abelov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/cub3d.h"
#include "../../include/fsm.h"

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

t_ret_code do_state_initial(void *param, int argc, char **argv)
{
	t_info *const	app = param;
	int				cubfd;

	printf("framerate: %d frametime: %d fr_scale: %d\n", FRAMERATE, FRAMETIME, FR_SCALE);
	if (argc != 2)
		return (printf("Error: incorrect arg no\n"), fail);
	cubfd = open(argv[1], O_RDONLY);
	if (cubfd == -1)
		return (printf("Error: failed to open map\n"), fail);
	app->map = init_map();
	app->endianness = check_endianness();
	app->mlx = mlx_init();

	if (app->mlx == NULL)
		return (printf("Error: failed to open map\n"), fail);
	if (parse_cub(app, cubfd))
		return (free_map(app->map), fail);
	app->player = init_player(app->map);

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
	return (ok);
	(void)app;
}

t_ret_code do_state_play(void *param)
{
	t_info *const app = param;


	mlx_mouse_hide(app->mlx, app->root);
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
	return (ok);
	(void)app;
}

t_ret_code do_state_loose(void *param)
{
	t_info *const app = param;

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
//				.hook = (void *) &key_press_loose,
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

	return ;
	(void)app;

}

void do_load_to_play(void *param)
{
	t_info *const app = param;

	struct s_thing
	{
		t_vect		pos;
		t_vect		dir;
		t_etype		type;
		t_subtype	subtype;
	} things[] = {
		{{28.0, 10.5}, {0.02, 0.02}, O_ENTITY, E_ZOOMER},
		{{24.0, 10.5}, {0.0, -0.03}, O_ENTITY, E_ZOOMER},
		{{15.0, 10.5}, {0.02, 0.01}, O_ENTITY, E_ZOOMER},
		{{ 5.0,  5.5}, {0.02, 0.0}, O_ENTITY, E_ZOOMER},
		{{12.5,  1.5}, {0.0, 0.03}, O_ENTITY, E_ZOOMER},
		{{10.5,  5.5}, {0.0, -0.03}, O_ENTITY, E_ZOOMER},
		{{18.5,  4.5}, {0.03, 0.0}, O_ENTITY, E_ZOOMER},
		{.pos = {20.5, 2.5}, .type = O_ITEM, .subtype = I_ETANK},
		{.pos = {18.5, 2.5 }, .type = O_ITEM, .subtype = I_SUPER},
		{.pos = {23.5, 2.5 }, .type = O_ITEM, .subtype = I_MISSILE},
		{.pos = {10.5, 10.5}, .type = O_ITEM, .subtype = I_ETANK},
		{.pos = {6.5, 1.5}, .type = O_ITEM, .subtype = I_TROPHY},
	};

	int i = -1;
	while (++i < (int)(sizeof(things) / sizeof(things[0])))
	{
		struct s_thing *thing = &things[i];
		if (thing->type == O_ENTITY)
			spawn_enemy(app,  thing->pos, thing->dir, thing->subtype);
		else
			spawn_item(app, thing->pos, thing->subtype);
	}
	replace_bg(app, NULL);
	fill_bg(&app->bg, app->map);
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
	mlx_loop_hook(app->mlx, &render_pmenu, app);
	mlx_hook(app->root, KeyPress, KeyPressMask, (void *) &key_press_pmenu, app);

	mlx_hook(app->root, ButtonPress, 0, NULL, app);
	mlx_hook(app->root, ButtonRelease, 0, NULL, app);
	mlx_hook(app->root, KeyRelease, 0, NULL, app);
	mlx_hook(app->root, MotionNotify, 0, NULL, app);
}

void do_play_to_win(void *param)
{
	t_info *const app = param;
	return ;
	(void)app;
}

void do_play_to_loose(void *param)
{
	t_info *const app = param;

	app->mlx->end_loop = 0;
	replace_bg(app, (char *) "./textures/wall.xpm");
	fill_everything_with_blood(&app->bg);
	mlx_loop_hook(app->mlx, &render_loose, app);
	mlx_hook(app->root, KeyPress, KeyPressMask, (void *) &key_press_loose, app);

	mlx_hook(app->root, ButtonPress, 0, NULL, app);
	mlx_hook(app->root, ButtonRelease, 0, NULL, app);
	mlx_hook(app->root, KeyRelease, 0, NULL, app);
	mlx_hook(app->root, MotionNotify, 0, NULL, app);
}

void do_play_to_end(void *param)
{
	t_info *const app = param;
	return ;
	(void)app;
}

void do_pmenu_to_play(void *param)
{
	t_info *const app = param;

	replace_bg(app, NULL);
	fill_bg(&app->bg, app->map);
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

	replace_bg(app, (char *)"./textures/wall.xpm");
	fill_bg(&app->bg, app->map);
	mlx_loop_hook(app->mlx, &render_mmenu, app);
	app->mlx->end_loop = 0;

	mlx_hook(app->root, KeyPress, KeyPressMask, (void *) &key_press_mmenu, app);

	mlx_hook(app->root, ButtonPress, 0, NULL, app);
	mlx_hook(app->root, ButtonRelease, 0, NULL, app);
	mlx_hook(app->root, KeyRelease, 0, NULL, app);
	mlx_hook(app->root, MotionNotify, 0, NULL, app);
}

void do_pmenu_to_end(void *param)
{

	t_info *const app = param;

	return ;
	(void)app;
}

void do_loose_to_mmenu(void *param)
{
	t_info *const app = param;
	return ;
	(void)app;
}

void do_loose_to_end(void *param)
{
	t_info *const app = param;
	return ;
	(void)app;
}

void do_win_to_mmenu(void *param)
{
	t_info *const app = param;
	return ;
	(void)app;
}

void do_win_to_end(void *param)
{
	t_info *const app = param;
	return ;
	(void)app;
}
