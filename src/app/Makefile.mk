# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile.mk                                        :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: abelov <abelov@student.42london.com>       +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/03/08 17:02:06 by abelov            #+#    #+#              #
#    Updated: 2026/02/24 14:35:22 by fsmyth           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

FILES   = cleanup.c \
			fsm.c \
			fsm2.c \
			fsm3.c \
			fsm4.c \
			fsm5.c \
			fsm6.c \
			fsm7.c \
			fsm8.c \
			fsm9.c \
			fsm10.c \
			server.c \
			fsm_multi.c \
			hooks.c \
			hooks2.c \
			mouse_hooks.c \
			fullscreen.c \
			on_expose.c

CUB_SRCS    += $(FILES:%.c=$(dir $(lastword $(MAKEFILE_LIST)))%.c)
