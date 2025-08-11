# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile.mk                                        :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: abelov <abelov@student.42london.com>       +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/03/08 17:02:06 by abelov            #+#    #+#              #
#    Updated: 2025/08/11 15:09:00 by fsmyth           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

FILES   = colour.c \
			credits.c \
			credits_avx2.c \
			draw.c \
			draw_rays.c \
			floor_transposed_avx2.c \
			floor_transposed_avx2_2.c \
			floor_transposed_avx2_3.c \
			hud.c \
			render.c \
			render_frame.c \
			render_frame2.c \
			render_frame_utils.c \
			img_to_tex.c \
			img_to_tex_static.c \
			sky.c

CUB_SRCS    += $(FILES:%.c=$(dir $(lastword $(MAKEFILE_LIST)))%.c)
