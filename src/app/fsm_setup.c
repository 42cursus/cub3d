/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fsm10.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/14 15:16:29 by abelov            #+#    #+#             */
/*   Updated: 2026/02/23 21:55:37 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"
#include <X11/X.h>
#include "fsm.h"

/* transitions from end state aren't needed */
t_transition	*get_state_transitions(size_t *size)
{
	static t_transition	transitions[] = {
	{STATE_INITIAL, ok, STATE_MMENU},
	{STATE_INITIAL, fail, STATE_END},
	{STATE_INITIAL, extra, STATE_INTRO},
	{STATE_INTRO, ok, STATE_MMENU},
	{STATE_INTRO, fail, STATE_END},
	{STATE_MMENU, ok, STATE_LOAD},
	{STATE_MMENU, repeat, STATE_CREDITS},
	{STATE_MMENU, fail, STATE_END},
	{STATE_MMENU, extra, STATE_MULTILOAD},
	{STATE_CREDITS, ok, STATE_MMENU},
	{STATE_LOAD, ok, STATE_PLAY},
	{STATE_LOAD, fail, STATE_MMENU},
	{STATE_MULTILOAD, ok, STATE_MULTI},
	{STATE_MULTILOAD, fail, STATE_MMENU},
	{STATE_PLAY, ok, STATE_WIN},
	{STATE_PLAY, fail, STATE_LOSE},
	{STATE_PLAY, repeat, STATE_PMENU},
	{STATE_PLAY, extra, STATE_LOAD},
	{STATE_PMENU, ok, STATE_PLAY},
	{STATE_PMENU, repeat, STATE_MMENU},
	{STATE_PMENU, fail, STATE_END},
	{STATE_LOSE, ok, STATE_LOAD},
	{STATE_LOSE, repeat, STATE_MMENU},
	{STATE_LOSE, fail, STATE_END},
	{STATE_WIN, ok, STATE_LOAD},
	{STATE_WIN, fail, STATE_END},
	{STATE_WIN, repeat, STATE_MMENU},
	{STATE_WIN, extra, STATE_CREDITS},
	};
	static size_t		transitions_size = SDL_TABLESIZE(transitions);

	*size = transitions_size;
	return (transitions);
}

t_state_func *const	*get_state_table(void)
{
	static t_state_func *const	state_table[NUM_STATES] = {
	[STATE_INITIAL] = (void *)do_state_initial,
	[STATE_INTRO] = do_state_intro,
	[STATE_MMENU] = do_state_mmenu,
	[STATE_CREDITS] = do_state_credits,
	[STATE_LOAD] = do_state_load,
	[STATE_MULTILOAD] = do_state_multiload,
	[STATE_PLAY] = do_state_play,
	[STATE_MULTI] = do_state_multi,
	[STATE_PMENU] = do_state_pmenu,
	[STATE_LOSE] = do_state_lose,
	[STATE_WIN] = do_state_win,
	[NUM_STATES - 1] = NULL
	};

	return (state_table);
}

t_transition_func	**get_trans_table(void)
{
	static t_transition_func *const		t_table[NUM_STATES - 1][NUM_STATES] = {
		[STATE_INITIAL] = {
			[STATE_INTRO] = do_initial_to_intro,
			[STATE_MMENU] = do_initial_to_mmenu,
			[STATE_END] = do_initial_to_end
		},
		[STATE_INTRO] = {
			[STATE_MMENU] = do_intro_to_mmenu,
			[STATE_END] = do_intro_to_end
		},
		[STATE_MMENU] = {
			[STATE_INTRO] = do_mmenu_to_intro,
			[STATE_LOAD] = do_mmenu_to_load,
			[STATE_CREDITS] = do_mmenu_to_credits,
			[STATE_END] = do_mmenu_to_end,
			[STATE_MULTILOAD] = do_mmenu_to_multiload
		},
		[STATE_LOAD] = {
			[STATE_MMENU] = do_load_to_mmenu,
			[STATE_PLAY] = do_load_to_play,
			[STATE_END] = do_load_to_end
		},
		[STATE_MULTILOAD] = {
			[STATE_MMENU] = do_multiload_to_mmenu,
			[STATE_MULTI] = do_multiload_to_multi,
		},
		[STATE_PLAY] = {
			[STATE_PMENU] = do_play_to_pmenu,
			[STATE_LOSE] = do_play_to_lose,
			[STATE_WIN] = do_play_to_win,
			[STATE_END] = do_play_to_end,
			[STATE_LOAD] = do_play_to_load
		},
		[STATE_PMENU] = {
			[STATE_MMENU] = do_pmenu_to_mmenu,
			[STATE_PLAY] = do_pmenu_to_play,
			[STATE_END] = do_pmenu_to_end
		},
		[STATE_LOSE] = {
			[STATE_MMENU] = do_lose_to_mmenu,
			[STATE_END] = do_lose_to_end,
			[STATE_LOAD] = do_lose_to_load
		},
		[STATE_WIN] = {
			[STATE_LOAD] = do_win_to_load,
			[STATE_MMENU] = do_win_to_mmenu,
			[STATE_END] = do_win_to_end,
			[STATE_CREDITS] = do_win_to_credits
		},
		[STATE_CREDITS] = {
			[STATE_MMENU] = do_credits_to_mmenu,
			[STATE_END] = do_credits_to_end
		},
	};

	return ((t_transition_func **)t_table);
}

t_state	run_state(t_info *app, int argc, char **argv)
{
	t_transition_func	*transition_func;
	t_transition		transition;
	t_ret_code			rc;
	size_t				size;
	t_transition *const	transitions = get_state_transitions(&size);

	if (app->state == STATE_INITIAL)
		rc = do_state_initial(app, argc, argv);
	else
		rc = get_state_table()[app->state](app);
	transition.dst_state = app->state;
	while (size--)
	{
		transition = transitions[size];
		if (transition.src_state == app->state && transition.ret_code == rc)
			break ;
	}
	transition_func = get_trans_table()[
		(app->state * NUM_STATES) + transition.dst_state
	];
	if (transition_func)
		transition_func(app);
	return (transition.dst_state);
}
