# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile.mk                                        :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: abelov <abelov@student.42london.com>       +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/03/08 17:02:06 by abelov            #+#    #+#              #
#    Updated: 2025/05/17 17:53:31 by fsmyth           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

FILES   = render.c \
			hud.c \
			floor.c \
			menus.c \
			render_frame.c \
			colour.c \
			credits.c \
			draw.c \
			draw_rays.c \
			sky.c \

CUB_SRCS    += $(FILES:%.c=$(dir $(lastword $(MAKEFILE_LIST)))%.c)
