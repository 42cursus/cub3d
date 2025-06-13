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

UNAME_S			= $(shell uname -s)
UNAME_M			= $(shell uname -m)
UNAME_R			= $(shell uname -r)
DOMAIN			= $(shell hostname -d)

LIBFT_DIR		= ./lib/ft
LIBX_DIR		= ./lib/mlx
BUILD_DIR		= build
INC_DIR			= ./include

RMFLAGS			= -r

CC				:= clang
INCLUDE_FLAGS	:= -I. -I$(INC_DIR) -I/usr/include -I/usr/include/SDL2 -I/usr/include/freetype2 -I/usr/include/libpng16
OPTIMIZE_FLAGS	:= -O3 -fstrict-aliasing -fno-strict-overflow -fomit-frame-pointer -march=native -fno-stack-protector #-fno-stack-protector-all
DEBUG_FLAGS		:= -g3 -gdwarf-3 \
					-ffast-math \
					-mprefer-vector-width=256 \
					# -fsanitize=address,undefined,float-divide-by-zero,float-cast-overflow \
#					-pg \
#					-D FRAMERATE=60 \

MANDATORY_FLAGS	:= -Wall -Wextra -Werror -Wimplicit -Wwrite-strings -mavx2 #-Wno-missing-braces
CFLAGS			= $(MANDATORY_FLAGS) $(DEBUG_FLAGS) $(OPTIMIZE_FLAGS) \
					$(INCLUDE_FLAGS)

SDL_MIX_LIB			:= -lSDL2_mixer

ifeq ($(UNAME_M),x86_64)
	ifeq ($(DOMAIN), 42london.com)
		SDL_MIX_LIB := -l:libSDL2_mixer-2.0.so.0.2.2
	endif
	ifeq ($(UNAME_R), 5.15.0-139-generic)
#		CFLAGS += -DWIN_WIDTH=1600 -DWIN_HEIGHT=900
	else
		CFLAGS += -DWIN_WIDTH=1920 -DWIN_HEIGHT=1080 #-DSKIP_INTRO=1
	endif
endif


LIBFT_LIB		=  $(LIBFT_DIR)/libft.a
LIBX			=  $(LIBX_DIR)/libmlx.a
LIBTEX			=  $(BUILD_DIR)/libtextures.a
LIBS			:= $(LIBFT) $(LIBX)
LINK_FLAGS		:= -L $(LIBFT_DIR) -L $(LIBX_DIR) -L $(BUILD_DIR) -L/usr/lib/x86_64-linux-gnu \
					-ltextures -lmlx -lft -lX11 -lXext -lm \
					$(SDL_MIX_LIB) -lSDL2 -lfreetype \
					# -fsanitize=address,undefined,float-divide-by-zero,float-cast-overflow

SRC_DIR			= src

SUB_DIRS		= parser utils app audio player render rays entities anim fonts
CUB_SRCS		:=
TEXTURES		:=

include $(SUB_DIRS:%=$(SRC_DIR)/%/Makefile.mk)
include resources/textures/Makefile.mk

SRCS			:= src/main.c
SRCS			+= $(CUB_SRCS)

OBJS			= $(SRCS:%.c=$(BUILD_DIR)/%.o)
TEX_OBJ			= $(TEXTURES:%.xpm=$(BUILD_DIR)/%.xpm.o)

ifeq ($(MAKELEVEL),0)
	# Only set --jobs if user didn't already pass a -j option manually
	ifeq ($(filter -j,$(MAKEFLAGS)),)
		MAKEFLAGS += --jobs=$(shell nproc) --no-print-directory --quiet
	endif
	ifeq ($(filter "--output-sync=target",$(MAKEFLAGS)),)
		MAKEFLAGS += --output-sync=target
	endif
endif

## all
all: $(NAME)

## cub3d
$(NAME): $(LIBFT_LIB) $(LIBX) $(OBJS) $(LIBTEX)
		@$(CC) $(TEX_OBJ) $(OBJS) $(DEBUG_FLAGS) -o $@ $(LINK_FLAGS)
		@echo "CUB3D BUILD COMPLETE!"

$(BUILD_DIR)/%.xpm.o: %.xpm
		@if [ ! -d $(@D) ]; then mkdir -p $(@D); fi
		@$(CC) -Dstatic= -x c -c $^ -o $@
		@#objcopy --globalize-symbol=$(*F) $@

$(BUILD_DIR)/%.o: %.c
		@if [ ! -d $(@D) ]; then mkdir -p $(@D); fi
		@$(CC) $(CFLAGS) $(INCLUDE) -c $^ -o $@

## libtextures
$(LIBTEX): $(TEX_OBJ)
		@$(AR) rcsP $@ $(TEX_OBJ)

## libft
$(LIBFT_LIB):
		+$(MAKE) -C $(LIBFT_DIR)

$(LIBX_DIR)/Makefile.gen:
		+$(MAKE) -C $(LIBX_DIR)
		@echo "$(LIBX_DIR)/Makefile.gen BUILD COMPLETE!"

## mlx
$(LIBX): $(LIBX_DIR)/Makefile.gen
		+$(MAKE) -C $(LIBX_DIR) -f Makefile.gen all
		@echo "LIBX BUILD COMPLETE!"

## clean_libft
clean_libft:
		+$(MAKE) -C $(LIBFT_DIR) clean

## clean_libx
clean_libx: $(LIBX_DIR)/Makefile.gen
		+$(MAKE) -C $(LIBX_DIR) -f Makefile.gen clean

## clean
clean: clean_libft #clean_libx
		@if [ -d $(BUILD_DIR) ]; then $(RM) $(RMFLAGS) $(BUILD_DIR); fi

## clean_libft
fclean_libft:
		+$(MAKE) -C $(LIBFT_DIR) fclean

## fclean
fclean: clean fclean_libft
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
