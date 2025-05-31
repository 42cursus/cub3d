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
	STATE_INTRO,
	STATE_MMENU,
	STATE_LOAD,
	STATE_PLAY,
	STATE_CREDITS,
	STATE_PMENU,
	STATE_LOSE,
	STATE_WIN,
	STATE_END,
	NUM_STATES
}	t_state;

typedef enum e_ret_codes
{
	ok,
	fail,
	repeat,
	extra,
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
	{STATE_INITIAL, ok,     STATE_MMENU},
	{STATE_INITIAL, fail,   STATE_END},
	{STATE_INITIAL, extra,   STATE_INTRO},
	{STATE_INTRO, ok,     STATE_MMENU},
	{STATE_INTRO, fail,   STATE_END},
	{STATE_MMENU,   ok,     STATE_LOAD},
	{STATE_MMENU,   repeat, STATE_CREDITS},
	{STATE_MMENU,   fail,   STATE_END},
	{STATE_CREDITS,   ok,   STATE_MMENU},
	{STATE_LOAD,    ok,     STATE_PLAY},
	{STATE_LOAD,    fail,   STATE_MMENU},
	{STATE_PLAY,    ok,     STATE_WIN},
	{STATE_PLAY,    fail,   STATE_LOSE},
	{STATE_PLAY,    repeat, STATE_PMENU},
	{STATE_PLAY,    extra,  STATE_LOAD},
	{STATE_PMENU,   ok,     STATE_PLAY},
	{STATE_PMENU,   repeat, STATE_MMENU},
	{STATE_PMENU,   fail,   STATE_END},
	{STATE_LOSE,    ok,     STATE_LOAD},
	{STATE_LOSE,    repeat, STATE_MMENU},
	{STATE_LOSE,    fail,   STATE_END},
	{STATE_WIN,     ok,     STATE_LOAD},
	{STATE_WIN,     fail,   STATE_END},
	{STATE_WIN,     repeat, STATE_MMENU},
};

void do_initial_to_mmenu(void *param);
void do_initial_to_end(void *param);
void do_initial_to_intro(void *param);
void do_mmenu_to_load(void *param);
void do_mmenu_to_credits(void *param);
void do_mmenu_to_intro(void *param);
void do_mmenu_to_end(void *param);

void do_intro_to_end(void *param);
void do_intro_to_mmenu(void *param);
void do_credits_to_mmenu(void *param);
void do_credits_to_end(void *param);
void do_load_to_play(void *param);
void do_load_to_end(void *param);
void do_play_to_pmenu(void *param);
void do_play_to_win(void *param);
void do_play_to_lose(void *param);
void do_play_to_end(void *param);
void do_play_to_load(void *param);
void do_pmenu_to_play(void *param);
void do_pmenu_to_end(void *param);
void do_pmenu_to_mmenu(void *param);
void do_lose_to_mmenu(void *param);
void do_lose_to_end(void *param);
void do_win_to_mmenu(void *param);
void do_win_to_end(void *param);
void do_win_to_load(void *param);
void do_lose_to_load(void *param);

static t_transition_func *const transition_table[NUM_STATES - 1][NUM_STATES] = {
	[STATE_INITIAL] = {[STATE_INTRO] = do_initial_to_intro, [STATE_MMENU] = do_initial_to_mmenu, [STATE_END] = do_initial_to_end},
	[STATE_INTRO] = {[STATE_MMENU] = do_intro_to_mmenu, [STATE_END] = do_intro_to_end},
	[STATE_MMENU] = {[STATE_INTRO] = do_mmenu_to_intro, [STATE_LOAD] = do_mmenu_to_load, [STATE_CREDITS] = do_mmenu_to_credits, [STATE_END] = do_mmenu_to_end},
	[STATE_LOAD] = {[STATE_PLAY] = do_load_to_play, [STATE_END] =  do_load_to_end},
	[STATE_PLAY] = { [STATE_PMENU] = do_play_to_pmenu, [STATE_LOSE] = do_play_to_lose, [STATE_WIN] = do_play_to_win, [STATE_END] = do_play_to_end, [STATE_LOAD] = do_play_to_load},
	[STATE_PMENU] = {[STATE_MMENU] = do_pmenu_to_mmenu, [STATE_PLAY] = do_pmenu_to_play, [STATE_END] = do_pmenu_to_end},
	[STATE_LOSE] = {[STATE_MMENU] = do_lose_to_mmenu, [STATE_END] = do_lose_to_end, [STATE_LOAD] = do_lose_to_load},
	[STATE_WIN] = {[STATE_LOAD] = do_win_to_load, [STATE_MMENU] = do_win_to_mmenu, [STATE_END] = do_win_to_end},
	[STATE_CREDITS] = {[STATE_MMENU] = do_credits_to_mmenu, [STATE_END] = do_credits_to_end},
};

t_ret_code do_state_initial(void *param, int argc, char **argv);
t_ret_code do_state_intro(void *param);
t_ret_code do_state_mmenu(void *param);
t_ret_code do_state_load(void *param);
t_ret_code do_state_play(void *param);
t_ret_code do_state_pmenu(void *param);
t_ret_code do_state_win(void *param);
t_ret_code do_state_lose(void *param);
t_ret_code do_state_credits(void *param);

static state_func_t *const state_table[NUM_STATES] = {
	[STATE_INITIAL] = (void *)do_state_initial,
	[STATE_INTRO] = do_state_intro,
	[STATE_MMENU] = do_state_mmenu,
	[STATE_CREDITS] = do_state_credits,
	[STATE_LOAD] = do_state_load,
	[STATE_PLAY] = do_state_play,
	[STATE_PMENU] = do_state_pmenu,
	[STATE_LOSE] = do_state_lose,
	[STATE_WIN] = do_state_win,
	NULL
};

#endif //FSM_H
