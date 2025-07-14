/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fsm10.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/14 15:16:29 by abelov            #+#    #+#             */
/*   Updated: 2025/07/14 15:16:30 by abelov           ###   ########.fr       */
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
	{STATE_CREDITS, ok, STATE_MMENU},
	{STATE_LOAD, ok, STATE_PLAY},
	{STATE_LOAD, fail, STATE_MMENU},
	{STATE_PLAY, ok, STATE_WIN},
	{STATE_PLAY, fail, STATE_LOSE}, {STATE_PLAY, repeat, STATE_PMENU},
	{STATE_PLAY, extra, STATE_LOAD}, {STATE_PMENU, ok, STATE_PLAY},
	{STATE_PMENU, repeat, STATE_MMENU}, {STATE_PMENU, fail, STATE_END},
	{STATE_LOSE, ok, STATE_LOAD}, {STATE_LOSE, repeat, STATE_MMENU},
	{STATE_LOSE, fail, STATE_END}, {STATE_WIN, ok, STATE_LOAD},
	{STATE_WIN, fail, STATE_END}, {STATE_WIN, repeat, STATE_MMENU},
	{STATE_WIN, extra, STATE_CREDITS},
	};
	static size_t		transitions_size = SDL_TABLESIZE(transitions);

	*size = transitions_size;
	return (transitions);
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
		rc = state_table[app->state](app);
	transition.dst_state = app->state;
	while (size--)
	{
		transition = transitions[size];
		if (transition.src_state == app->state && transition.ret_code == rc)
			break ;
	}
	transition_func = transition_table[app->state][transition.dst_state];
	if (transition_func)
		transition_func(app);
	return (transition.dst_state);
}
