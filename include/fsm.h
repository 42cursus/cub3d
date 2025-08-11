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

typedef t_ret_code	t_state_func(void *param);
typedef void		t_transition_func(void *param);

void		do_initial_to_mmenu(void *param);
void		do_initial_to_end(void *param);
void		do_initial_to_intro(void *param);
void		do_mmenu_to_load(void *param);
void		do_mmenu_to_credits(void *param);
void		do_mmenu_to_intro(void *param);
void		do_mmenu_to_end(void *param);
void		do_intro_to_end(void *param);
void		do_intro_to_mmenu(void *param);
void		do_credits_to_mmenu(void *param);
void		do_credits_to_end(void *param);
void		do_load_to_play(void *param);
void		do_load_to_mmenu(void *param);
void		do_load_to_end(void *param);
void		do_play_to_pmenu(void *param);
void		do_play_to_win(void *param);
void		do_play_to_lose(void *param);
void		do_play_to_end(void *param);
void		do_play_to_load(void *param);
void		do_pmenu_to_play(void *param);
void		do_pmenu_to_end(void *param);
void		do_pmenu_to_mmenu(void *param);
void		do_lose_to_mmenu(void *param);
void		do_lose_to_end(void *param);
void		do_win_to_mmenu(void *param);
void		do_win_to_end(void *param);
void		do_win_to_load(void *param);
void		do_win_to_credits(void *param);
void		do_lose_to_load(void *param);

t_ret_code	do_state_initial(void *param, int argc, char **argv);
t_ret_code	do_state_intro(void *param);
t_ret_code	do_state_mmenu(void *param);
t_ret_code	do_state_load(void *param);
t_ret_code	do_state_play(void *param);
t_ret_code	do_state_pmenu(void *param);
t_ret_code	do_state_win(void *param);
t_ret_code	do_state_lose(void *param);
t_ret_code	do_state_credits(void *param);

#endif //FSM_H
