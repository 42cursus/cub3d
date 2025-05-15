/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pool.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 19:36:01 by fsmyth            #+#    #+#             */
/*   Updated: 2025/04/28 20:18:55 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

t_poolnode	*add_poolnode(t_poolnode *head)
{
	t_poolnode	*current;
	t_poolnode	*new;

	new = malloc(sizeof(*new));
	new->stackp = 0;
	new->next = NULL;
	current = head;
	while (current->next != NULL)
		current = current->next;
	current->next = new;
	return (new);
}

void	clear_poolnodes(t_poolnode *head, t_poolnode **currp)
{
	t_poolnode	*current;
	t_poolnode	*temp;

	current = head->next;
	while (current != NULL)
	{
		temp = current->next;
		free(current);
		current = temp;
	}
	head->next = NULL;
	*currp = head;
}

void	reset_pool(t_poolnode *head, t_poolnode **currp)
{
	t_poolnode	*current;

	current = head;
	while (current != NULL)
	{
		current->stackp = 0;
		current = current->next;
	}
	*currp = head;
}

int	count_poolnodes(t_poolnode *head)
{
	t_poolnode	*current;
	int			count;

	count = 0;
	current = head;
	while (current != NULL)
	{
		count++;
		current = current->next;
	}
	return (count);
}
