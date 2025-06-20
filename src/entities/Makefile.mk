# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile.mk                                        :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: abelov <abelov@student.42london.com>       +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/03/08 17:02:06 by abelov            #+#    #+#              #
#    Updated: 2025/06/04 19:52:19 by fsmyth           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

FILES   = enemy_construction.c \
		  projectile.c \
		  enemy_projectile.c \
		  enemies.c \
		  triggers.c \
		  zoomer.c \
		  reo.c \
		  holtz.c \
		  atomic.c \
		  phantoon.c \
		  items.c \
		  obj_utils.c \
		  update_objects.c \
		  logo.c \
		  movement.c \

CUB_SRCS    += $(FILES:%.c=$(dir $(lastword $(MAKEFILE_LIST)))%.c)
