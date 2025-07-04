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
			floor_avx2.c \
			floor_transposed.c \
			floor_transposed_avx2.c \
			floor_transposed_cols.c \
			menus.c \
			render_frame.c \
			colour.c \
			credits.c \
			credits_avx2.c \
			draw.c \
			draw_rays.c \
			sky.c \

CUB_SRCS    += $(FILES:%.c=$(dir $(lastword $(MAKEFILE_LIST)))%.c)
