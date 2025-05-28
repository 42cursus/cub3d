# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: abelov <abelov@student.42london.com>       +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/03/08 16:52:04 by abelov            #+#    #+#              #
#    Updated: 2025/05/16 18:41:44 by fsmyth           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME			:= cub3d

LIBFT_DIR		= ./lib/ft
LIBX_DIR		= ./lib/mlx
BUILD_DIR		= build
INC_DIR			= ./include

RMFLAGS			= -r

CC				:= clang
INCLUDE_FLAGS	:= -I. -I$(INC_DIR) -I/usr/include
OPTIMIZE_FLAGS	:= -Ofast
DEBUG_FLAGS		:= -g3 -gdwarf-3 \
					-ffast-math \
					-mprefer-vector-width=256 \
					-pg \
#					-D FRAMERATE=60 \
#					-fsanitize=address,undefined,float-divide-by-zero,float-cast-overflow

MANDATORY_FLAGS	:= -Wall -Wextra -Werror -Wimplicit -Wwrite-strings -mavx2
CFLAGS			= $(MANDATORY_FLAGS) $(DEBUG_FLAGS) $(OPTIMIZE_FLAGS) \
					$(INCLUDE_FLAGS)


LIBFT_LIB		=  $(LIBFT_DIR)/libft.a
LIBX			=  $(LIBX_DIR)/libmlx.a
LIBS			:= $(LIBFT) $(LIBX)
LINK_FLAGS		:= -L $(LIBFT_DIR) -L $(LIBX_DIR) -L/usr/lib/x86_64-linux-gnu \
					-lmlx -lft -lX11 -lXext -lm \
#					-fsanitize=address,undefined,float-divide-by-zero,float-cast-overflow

SRC_DIR			= src

SUB_DIRS		= parser utils app player render rays entities animations
CUB_SRCS		:=
TEXTURES		:=

include $(SUB_DIRS:%=$(SRC_DIR)/%/Makefile.mk)
include textures/Makefile.mk

SRCS			:= src/main.c
SRCS			+= $(CUB_SRCS)

OBJS			= $(SRCS:%.c=$(BUILD_DIR)/%.o)
TEX_OBJ			= $(TEXTURES:%.xpm=$(BUILD_DIR)/%.xpm.o)

ifeq ($(MAKELEVEL),0)
	# Only set --jobs if user didn't already pass a -j option manually
	ifeq ($(filter -j,$(MAKEFLAGS)),)
		MAKEFLAGS += --jobs=$(shell nproc) --no-print-directory #--quiet
	endif
	ifeq ($(filter "--output-sync=target",$(MAKEFLAGS)),)
		MAKEFLAGS += --output-sync=target
	endif
endif

## all
all: $(NAME)

## cub3d
$(NAME): $(LIBFT_LIB) $(LIBX) $(OBJS) $(TEX_OBJ)
		@$(CC) $(TEX_OBJ) $(OBJS) $(DEBUG_FLAGS) -o $@ $(LINK_FLAGS)
		@echo "CUB3D BUILD COMPLETE!"

$(BUILD_DIR)/%.xpm.o: %.xpm
		@if [ ! -d $(@D) ]; then mkdir -pv $(@D); fi
		@$(CC) -Dstatic= -x c -c $^ -o $@
		@#objcopy --globalize-symbol=$(*F) $@

$(BUILD_DIR)/%.o: %.c
		@if [ ! -d $(@D) ]; then mkdir -p $(@D); fi
		@$(CC) $(CFLAGS) $(INCLUDE) -c $^ -o $@

## libft
$(LIBFT_LIB):
		+$(MAKE) -C $(LIBFT_DIR)

$(LIBX_DIR)/Makefile.gen:
		+$(MAKE) -C $(LIBX_DIR)
		@echo "$(LIBX_DIR)/Makefile.gen BUILD COMPLETE!"

## mlx
$(LIBX): $(LIBX_DIR)/Makefile.gen
		+$(MAKE) -C $(LIBX_DIR) -f Makefile.gen all
		@if [ ! -d $(BUILD_DIR) ]; then mkdir -p $(BUILD_DIR); fi
		@ar x $(LIBX) mlx_xpm.o --output $(BUILD_DIR)
		@ar d $(LIBX) $(BUILD_DIR)/mlx_xpm.o
		@objcopy --globalize-symbol=strlcpy_is_not_posix $(BUILD_DIR)/mlx_xpm.o
		@objcopy --weaken-symbol=strlcpy_is_not_posix $(BUILD_DIR)/mlx_xpm.o
		@ar rcs $(LIBX) $(BUILD_DIR)/mlx_xpm.o
		@$(RM) -v $(BUILD_DIR)/mlx_xpm.o
		@echo "LIBX BUILD COMPLETE!"

## clean_libft
clean_libft:
		+$(MAKE) -C $(LIBFT_DIR) clean

## clean_libx
clean_libx: $(LIBX_DIR)/Makefile.gen
		+$(MAKE) -C $(LIBX_DIR) -f Makefile.gen clean

## clean
clean: clean_libft clean_libx
		@if [ -d $(BUILD_DIR) ]; then $(RM) $(RMFLAGS) $(BUILD_DIR); fi

## clean_libft
fclean_libft:
		+$(MAKE) -C $(LIBFT_DIR) fclean

## fclean
fclean: fclean_libft #clean_libx
		@if [ -f $(NAME) ]; then $(RM) $(RMFLAGS) $(NAME); fi
		@#if [ -f $(LIBX_DIR)/Makefile.gen ]; then $(RM) $(RMFLAGS) $(LIBX_DIR)/Makefile.gen; fi

re: fclean
		+$(MAKE) all

norm:
		@norminette --use-gitignore $(SRCS) || true
		+$(MAKE) -C $(LIBFT_DIR) norm

# Magic help adapted: from https://gitlab.com/depressiveRobot/make-help/blob/master/help.mk (MIT License)
help:
	@printf "Available targets:\n\n"
	@awk -F: '/^[a-zA-Z\-_0-9%\\ ]+:/ { \
			helpMessage = match(lastLine, /^## (.*)/); \
			if (helpMessage) { \
					helpCommand = $$1; \
					helpMessage = substr(lastLine, RSTART + 3, RLENGTH); \
					printf "  \x1b[32;01m%-35s\x1b[0m %s\n", helpCommand, helpMessage; \
			} \
	} \
	{ lastLine = $$0 }' $(MAKEFILE_LIST) | sort -u
	@printf "\n"

.PHONY: all clean fclean re bonus norm help
