# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile.mk                                        :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: abelov <abelov@student.42london.com>       +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/03/08 17:02:17 by abelov            #+#    #+#              #
#    Updated: 2025/05/16 18:31:57 by fsmyth           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

FILES   = check_endianness.c \
		  vector.c \
		  time.c \

CUB_SRCS    += $(FILES:%.c=$(dir $(lastword $(MAKEFILE_LIST)))%.c)
