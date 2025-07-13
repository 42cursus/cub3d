/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cub3d.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 19:54:08 by abelov            #+#    #+#             */
/*   Updated: 2025/07/13 22:01:46 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CUB3D_H
# define CUB3D_H
# include "libft.h"
# include "mlx.h"
# include "mlx_int.h"
# include <math.h>

# define NUM_5 0x35
# define ESC 0xFF1B
# define UP 65362
# define DOWN 65364
# define RIGHT 65363
# define LEFT 65361
# define KEY_W 0x0077
# define KEY_A 0x0061
# define KEY_S 0x0073
# define KEY_D 0x0064

# define WIN_HEIGHT 900
# define WIN_WIDTH 1200

typedef struct s_vect
{
	double	x;
	double	y;
}	t_vect;

typedef struct s_ivect
{
	int	x;
	int	y;
}	t_ivect;

typedef struct s_cvect
{
	char	x;
	char	y;
}	t_cvect;

typedef struct s_ray
{
	t_vect	intcpt;
	int		face;
	double	distance;
}	t_ray;

typedef struct s_texarr
{
	unsigned int	**img;
	int				x;
	int				y;
}	t_texarr;

typedef struct s_data
{
	t_texarr		n_tex;
	t_texarr		s_tex;
	t_texarr		e_tex;
	t_texarr		w_tex;
	int				f_col;
	int				c_col;
	char			**map;
	t_vect			starting_pos;
	char			starting_dir;
	int				height;
	int				width;
}	t_data;

typedef struct s_player
{
	t_vect	pos;
	t_vect	direction;
	double	angle;
	t_ray	rays[WIN_WIDTH];
	double	angle_offsets[WIN_WIDTH];
}	t_player;

typedef struct s_imgdata
{
	void	*img;
	char	*addr;
	int		width;
	int		height;
	int		bpp;
	int		line_length;
	int		endian;
}	t_imgdata;

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
	char		*title;
	t_img		*canvas;
	int			clip_x_origin;
	int			clip_y_origin;
	int			endianness;
	t_data		*map;
	t_player	*player;
}	t_info;

int			check_endianness(void);
void		on_expose(t_info *app);
int			exit_win(void *param);
int			cleanup(t_info *app);
void		replace_image(t_info *app);
int			expose_win(void *param);
int			mouse_win(unsigned int button, int x, int y, void *p);
int			key_win(KeySym key, void *param);
void		mlx_keypress_hook(t_win_list *win,
				int (*hook)(KeySym, void *), void *param);

t_data		*init_map(void);
void		free_map(t_data *map);
int			parse_cub(t_info *app, int fd);
void		print_t_map(t_data *map);
int			str_cmp_whitespace(void *data, void *ref);
size_t		count_split_words(char **split);
void		**ft_lst_to_arr(t_list *list);
void		free_split(char **split);
size_t		find_longest_line(char **map);
int			is_map_line(char *line);
void		normalise_map(t_data *data);
int			map_is_valid(t_data *data);
int			collect_map(t_list	*file, t_data *data);
int			parse_colour(t_data *map, char *str, int identifier);
int			parse_texture(t_data *data, char *str, int identifier, t_info *app);
int			valid_identifier(char *str);

t_player	*init_player(t_data *map);
void		move_player(t_player *player, char **map, t_vect dir);
void		rotate_player(t_player *player, int direction);

char		get_max_direction(t_vect vect, t_vect *pos, t_vect *new_pos);
t_vect		scale_vect(t_vect vect, double scalar);
t_vect		rotate_vect(t_vect vect, double angle);
void		rotate_vect_inplace(t_vect *vect, double angle);

double		get_gradient_dir(t_vect dir);
double		get_gradient_angle(double angle);
t_vect		get_horizontal_int(double y, double gradient, double c);
t_vect		get_vertical_int(double x, double gradient, double c);
double		get_y_intercept(t_vect pos, double gradient);
t_ray		find_ray_collision(t_data *map, t_player *player, double angle);
void		cast_all_rays(t_data *map, t_player *player);
double		get_cam_distance(t_vect pos, double angle, t_vect intcpt);
int			determine_face(t_vect intersect);

t_ray		get_vert_intersect_west(t_player *player,
				t_data *map, double gradient, double c);
t_ray		get_vert_intersect_east(t_player *player,
				t_data *map, double gradient, double c);
t_ray		get_horiz_intersect_south(t_player *player,
				t_data *map, double gradient, double c);
t_ray		get_horiz_intersect_north(t_player *player,
				t_data *map, double gradient, double c);

void		fill_bg(t_imgdata *canvas, t_data *map);
u_int		**img_to_arr(char *filename, t_info *app, int *x, int *y);
void		draw_rays(t_info *app, t_imgdata *canvas);

#endif //CUB3D_H
