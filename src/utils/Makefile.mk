# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile.mk                                        :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: abelov <abelov@student.42london.com>       +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/03/08 17:02:17 by abelov            #+#    #+#              #
#    Updated: 2025/08/08 16:26:59 by fsmyth           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

FILES   = time.c \
			vector.c \
			vector2.c \
			vector3.c \
			vector4.c \
			transpose_img_avx2.c \
			place_img_alpha_avx2_soa.c \
			place_img_alpha_avx2_soa2.c \
			misc.c \

CUB_SRCS    += $(FILES:%.c=$(dir $(lastword $(MAKEFILE_LIST)))%.c)
