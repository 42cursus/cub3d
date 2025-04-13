/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fsm.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/13 19:04:38 by abelov            #+#    #+#             */
/*   Updated: 2025/04/13 19:04:38 by abelov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FSM_H
# define FSM_H

typedef enum e_game_state
{
	STATE_INITIAL = 0,
	STATE_MENU,
	STATE_PLAY,
	STATE_LOOSE,
	STATE_WIN,
	STATE_END,
	NUM_STATES
}	t_state;

typedef enum e_ret_codes
{
	ok,
	fail,
	repeat
}	t_ret_code;

typedef struct s_transition
{
	t_state		src_state;
	t_ret_code	ret_code;
	t_state		dst_state;
}	t_transition;

typedef t_ret_code state_func_t(void *param);
typedef void t_transition_func(void *param);

/* transitions from end state aren't needed */
static t_transition state_transitions[] = {
	{STATE_INITIAL, ok,     STATE_MENU},
	{STATE_INITIAL, fail,   STATE_END},
	{STATE_MENU,    ok,     STATE_PLAY},
	{STATE_MENU,    fail,   STATE_END},
	{STATE_PLAY,    ok,     STATE_WIN},
	{STATE_PLAY,    fail,   STATE_LOOSE},
	{STATE_PLAY,    repeat, STATE_MENU},
	{STATE_LOOSE,   ok,     STATE_PLAY},
	{STATE_LOOSE,   fail,   STATE_END},
	{STATE_WIN,     ok,     STATE_END},
};

void do_initial_to_menu(void *param);
void do_initial_to_end(void *param);
void do_menu_to_play(void *param);
void do_menu_to_end(void *param);
void do_play_to_menu(void *param);
void do_play_to_win(void *param);
void do_play_to_loose(void *param);
void do_play_to_end(void *param);
void do_loose_to_menu(void *param);
void do_loose_to_end(void *param);
void do_win_to_menu(void *param);
void do_win_to_end(void *param);

static t_transition_func *const transition_table[NUM_STATES - 1][NUM_STATES] = {
	[STATE_INITIAL] = {NULL,	[STATE_MENU] = do_initial_to_menu, NULL, NULL, NULL, [STATE_END] = do_initial_to_end},
	[STATE_MENU] = {NULL, NULL, [STATE_PLAY] = do_menu_to_play, NULL, NULL, [STATE_END] = do_menu_to_end},
	[STATE_PLAY] = {NULL, [STATE_MENU] = do_play_to_menu, NULL, [STATE_LOOSE] = do_play_to_loose, [STATE_WIN] = do_play_to_win, [STATE_END] = do_play_to_end},
	[STATE_LOOSE] = {NULL, [STATE_MENU] = do_loose_to_menu, NULL, NULL, NULL, [STATE_END] = do_loose_to_end},
	[STATE_WIN] = {NULL, [STATE_MENU] = do_win_to_menu, NULL, NULL, NULL, [STATE_END] = do_win_to_end},
};

t_ret_code do_state_initial(void *param, int argc, char **argv);
t_ret_code do_state_menu(void *param);
t_ret_code do_state_play(void *param);
t_ret_code do_state_win(void *param);
t_ret_code do_state_loose(void *param);

static state_func_t *const state_table[NUM_STATES] = {
	[STATE_INITIAL] = (void *)do_state_initial,
	[STATE_MENU] = do_state_menu,
	[STATE_PLAY] = do_state_play,
	[STATE_LOOSE] = do_state_loose,
	[STATE_WIN] = do_state_win,
	NULL
};

#endif //FSM_H
