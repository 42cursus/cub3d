/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cub3d.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 19:54:08 by abelov            #+#    #+#             */
/*   Updated: 2025/03/25 18:39:37 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CUB3D_H
# define CUB3D_H
# include "libft.h"
# include "mlx.h"
# include "mlx_int.h"
# include <math.h>

# define NUM_5 0x35 /* (53) Number 5 on the main keyboard */
# define ESC 0xFF1B /* (53) Number 5 on the main keyboard */
# define UP 65362
# define DOWN 65364
# define RIGHT 65363
# define LEFT 65361
# define KEY_W 0x0077
# define KEY_A 0x0061
# define KEY_S 0x0073
# define KEY_D 0x0064

typedef struct s_vect
{
	double	x;
	double	y;
}	t_vect;

typedef	struct s_data
{
	char	*n_path;
	char	*s_path;
	char	*e_path;
	char	*w_path;
	int		f_col;
	int		c_col;
	char	**map;
	t_vect	starting_pos;
	char	starting_dir;
	int		height;
	int		width;
}	t_data;

typedef struct s_player
{
	t_vect	pos;
	t_vect	direction;
	double	angle;
	t_vect	rays[800];
}	t_player;

enum
{
	NONE = 0,
	FLOOR = 1,
	CEILING = 2,
	NORTH = 3,
	SOUTH = 4,
	EAST = 5,
	WEST = 6
};

typedef struct s_info
{
	struct s_fdf_win
	{
		int	width;
		int	height;
	}			win;
	t_xvar		*mlx;
	t_win_list	*root;
	double		zoom;
	char 		*title;
	t_img		*canvas;
	int			clip_x_origin;
	int			clip_y_origin;
	int			endianness;
	t_data		*map;
	t_player	*player;
}	t_info;

int		check_endianness(void);
void	on_expose(t_info *app);
int		exit_win(void *param);
int		cleanup(t_info *app);
void	replace_image(t_info *app);
int		expose_win(void *param);
int		mouse_win(unsigned int button, int x, int y, void *p);
int		key_win(int key, void *param);
void	mlx_keypress_hook(void *param);

t_data	*init_map(void);
void	free_map(t_data *map);
int		parse_cub(t_data *map, int fd);
void	print_t_map(t_data *map);
void	print_ascii_mmap(t_data *data, t_player *player);

t_player	*init_player(t_data *map);
void		move_player(t_player *player, char **map, t_vect dir);
void		rotate_player(t_player *player, int direction);

char	get_max_direction(t_vect vect);
t_vect	scale_vect(t_vect vect, double scalar);
t_vect	rotate_vect(t_vect vect, double angle);
void	rotate_vect_inplace(t_vect *vect, double angle);

#endif //CUB3D_H
