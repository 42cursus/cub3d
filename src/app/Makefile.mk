# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile.mk                                        :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: abelov <abelov@student.42london.com>       +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/03/08 17:02:06 by abelov            #+#    #+#              #
#    Updated: 2025/08/08 16:50:32 by fsmyth           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

FILES   = cleanup.c \
			fsm.c \
			hooks.c \
			joypad.c \
			mouse_hooks.c \
			fullscreen.c \
			on_expose.c

CUB_SRCS    += $(FILES:%.c=$(dir $(lastword $(MAKEFILE_LIST)))%.c)
