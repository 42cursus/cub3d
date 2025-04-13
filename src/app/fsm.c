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

t_state run_state(t_info *app, int argc, char **argv)
{
	state_func_t *state_fun;
	t_transition_func *transition_func;
	t_transition transition;
	t_ret_code rc;
	size_t i;

	state_fun = state_table[app->state];
	if (state_fun == (void *) do_state_initial)
		rc = do_state_initial(app, argc, argv);
	else
		rc = state_fun(app);
	i = 0;
	transition.dst_state = app->state;
	while (++i <
		   (int) (sizeof(state_transitions) / sizeof(state_transitions[0])))
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
	t_info *const app = param;

	printf("framerate: %d frametime: %d fr_scale: %d\n", FRAMERATE, FRAMETIME, FR_SCALE);

	int			cubfd;
	cubfd = open(argv[1], O_RDONLY);
	if (cubfd == -1)
		return (printf("Error: failed to open map\n"), 1);
	app->map = init_map();
	app->endianness = check_endianness();
	app->mlx = mlx_init();
	if (app->mlx == NULL)
		exit(EXIT_FAILURE);

	app->root = mlx_new_window(app->mlx, app->win.width,
							   app->win.height, app->title);
	mlx_loop_hook(app->mlx, &loop_hook, app);
	mlx_expose_hook(app->root, &expose_win, app);
	mlx_hook(app->root, DestroyNotify, 0, (void *)&exit_win, app);
	switch_game_state(app, STATE_INITIAL);

	if (parse_cub(app, cubfd))
		return (free_map(app->map), 1);
	app->player = init_player(app->map);

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

	app->last_frame = get_time_us();
	app->framecount = 0;
	// app->last_frame_us = get_time_us();
	app->frametime = 5000;

	mlx_mouse_hide(app->mlx, app->root);

	mlx_loop(app->mlx);

	mlx_mouse_show(app->mlx, app->root);
	return (ok);
}

t_ret_code do_state_menu(void *param)
{
	t_info *const app = param;
	return (ok);
}

t_ret_code do_state_play(void *param)
{
	t_info *const app = param;
	return (ok);
}

t_ret_code do_state_win(void *param)
{
	t_info *const app = param;
	return (ok);
}

t_ret_code do_state_loose(void *param)
{
	t_info *const app = param;
	return (ok);
}

void do_initial_to_menu(void *param)
{
	t_info *const app = param;
}

void do_initial_to_end(void *param)
{
	t_info *const app = param;
}

void do_menu_to_play(void *param)
{
	t_info *const app = param;
}

void do_menu_to_end(void *param)
{
	t_info *const app = param;
}

void do_play_to_menu(void *param)
{
	t_info *const app = param;
}

void do_play_to_win(void *param)
{
	t_info *const app = param;
}

void do_play_to_loose(void *param)
{
	t_info *const app = param;
}

void do_play_to_end(void *param)
{
	t_info *const app = param;
}

void do_loose_to_menu(void *param)
{
	t_info *const app = param;
}

void do_loose_to_end(void *param)
{
	t_info *const app = param;
}

void do_win_to_menu(void *param)
{
	t_info *const app = param;
}

void do_win_to_end(void *param)
{
	t_info *const app = param;
}
