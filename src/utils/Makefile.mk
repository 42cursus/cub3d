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
			transpose_img_avx2.c \
			place_img_alpha_avx2_soa.c \
			misc.c \

CUB_SRCS    += $(FILES:%.c=$(dir $(lastword $(MAKEFILE_LIST)))%.c)
