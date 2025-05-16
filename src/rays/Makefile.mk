# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile.mk                                        :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: abelov <abelov@student.42london.com>       +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/03/08 17:02:06 by abelov            #+#    #+#              #
#    Updated: 2025/05/15 15:25:54 by fsmyth           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

FILES   = rays.c \
		  ray_dda.c \
		  ray_utils.c \
		  object_collision.c \
		  dda_init.c \
		  pool.c \

CUB_SRCS    += $(FILES:%.c=$(dir $(lastword $(MAKEFILE_LIST)))%.c)
