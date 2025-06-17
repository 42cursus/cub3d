/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cub3d.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 19:54:08 by abelov            #+#    #+#             */
/*   Updated: 2025/06/07 18:05:05 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CUB3D_H
# define CUB3D_H
# include <math.h>
# include <sys/types.h>
# include <errno.h>
# include "libft.h"
# include "mlx.h"
# include "mlx_int.h"
# include "fsm.h"

# include <SDL2/SDL.h>
# include "SDL_mixer.h" // <SDL2/SDL_mixer.h>
# include <ft2build.h>
# include <freetype/freetype.h>
# ifdef ft_snprintf
#  undef ft_snprintf
# endif
# ifdef ft_memset
#  undef ft_memset
# endif
# ifdef ft_strrchr
#  undef ft_strrchr
# endif
# ifdef ft_memcpy
#  undef ft_memcpy
# endif
# ifdef ft_strstr
#  undef ft_strstr
# endif

# if !defined(WIN_WIDTH) || !defined(WIN_HEIGHT)
//# define WIN_WIDTH 720
//# define WIN_HEIGHT 480
//# define WIN_WIDTH 1280
//# define WIN_HEIGHT 960
#  define WIN_WIDTH 1920
#  define WIN_HEIGHT 1080
// # define WIN_WIDTH 1600
// # define WIN_HEIGHT 900
// # define WIN_WIDTH 1792
// # define WIN_HEIGHT 1008
# endif //WIN_WIDTH

# ifndef SKIP_INTRO
#  define SKIP_INTRO 0
# endif //SKIP_INTRO

//#define GO_TO_FULLSCREEN_ON_LOAD 0
# define GO_TO_FULLSCREEN_ON_LOAD 1
# define RAY_POOL_SIZE 5000
# define FIXED_SHIFT 32
# ifndef FRAMERATE
#  define FRAMERATE 120
# endif
// #define FR_SCALE (FRAMERATE / 50.0)
// #define FRAMETIME (1000000 / FRAMERATE)

# define BUFF_SIZE 128
# define MLX_WHITE 0x00ffffff
# define MLX_LIME 0x0000ff55
# define MLX_DARK_SLATE_GREY 0x2f4f4f
# define MLX_LIGHT_SLATE_GREY 0x00778899
# define MLX_LIGHT_SLATE_BLUE 0x008470ff
# define MLX_LIGHT_RED 0x00ff5555
# define MLX_RED 0x00ff0000
# define MLX_GREEN 0x0000ff00
# define MLX_BLUE 0x000000ff
# define MLX_DTURQUOISE 0x0000ddcc
# define MLX_PALETURQUOISE 0xafeeee
# define MLX_TANG_YELLOW 0x00ffcc00
# define MLX_PINK 0x00d6428e
# define MLX_PALE_GRAY 0xf8f8f8
# define XPM_TRANSPARENT 0xff000000

# define LARGE_MMAP_SCALE 16

# define TEX_DIR "./resources/textures"

enum e_dir
{
	LEFT = 0,
	RIGHT
};

enum
{
	NONE = 0,
	FLOOR = 1,
	CEILING = 2,
	NORTH = 3,
	SOUTH = 4,
	EAST = 5,
	WEST = 6,
	DOOR_N = 7,
	DOOR_S = 8,
	DOOR_E = 9,
	DOOR_W = 10,
	DOOR_N_OPEN = 11,
	DOOR_S_OPEN = 12,
	DOOR_E_OPEN = 13,
	DOOR_W_OPEN = 14,
	LVL_A = 15,
	LVL_B = 16,
	LVL_C = 17,
	MU,
};

typedef enum e_etype
{
	O_PROJ = 0,
	O_ENTITY = 1,
	O_ITEM = 2,
	O_EPROJ = 3,
	O_TRIGGER = 4,
	O_TELE = 5,
	O_DOOR = 6,
	O_LOGO,
	O_MAX
}	t_etype;

typedef enum e_subtype
{
	P_BEAM = 0,
	P_MISSILE,
	P_SUPER,
	P_PHANTOON,
	P_HOLTZ,
	E_ZOOMER,
	E_ATOMIC,
	E_REO,
	E_HOLTZ,
	E_PHANTOON,
	I_ETANK,
	I_SUPER,
	I_MISSILE,
	I_AMMO_M,
	I_AMMO_S,
	I_HEALTH,
	I_TROPHY,
	T_BOSS,
	SUBT_MAX
}	t_subtype;

enum e_idx
{
	idx_XK_a = 0,
	idx_XK_d,
	idx_XK_e,
	idx_XK_s,
	idx_XK_w,
	idx_XK_x,
	idx_XK_Left,
	idx_XK_Up,
	idx_XK_Right,
	idx_XK_Down,
};

typedef enum e_snd
{
	snd_door = 0,
	snd_gun,
	snd_hash,
	snd_intro,
	snd_portal,
	snd_boss_die,
	snd_rocket,
	snd_door_open,
	snd_door_close,
	snd_pickup_ammo,
	snd_pickup_health,
	snd_enemy_death,
	snd_enemy_attack1,
	snd_enemy_attack2,
	snd_enemy_shot,
	snd_music_boss,
	snd_player_damage,
	snd_win_music,
	SND_MAX
}	t_snd;

enum e_channel
{
	ch_any = -1,
	ch_music = 0,
	ch_door,
	ch_item,
	ch_tele,
	ch_weapons,
	ch_enemies,
	ch_player,
	ch_MAX = MIX_CHANNELS
};

typedef struct s_ivect
{
	int	x;
	int	y;
}	t_ivect;

typedef struct s_texture
{
	u_int		*data;
	union
	{
		struct
		{
			int			w;
			int			h;
		};
		t_ivect	xy;
	};
	u_char		nelem;
	size_t		sl;
}	t_tex;

typedef const struct s_texture	t_ctex;

typedef struct s_animation
{
	int			active;
	int			loop;
	size_t		timestart;
	t_tex		*tex;
	size_t		duration;
	int			frames;
}	t_anim;

typedef struct s_vect
{
	double	x;
	double	y;
}	t_vect;

typedef struct s_vecf
{
	union
	{
		struct
		{
			double			x;
			double			y;
			struct s_vect	(*addi)(const t_vect self, t_ivect to_add);
			struct s_vect	(*rot)(const t_vect self, double angle);
			struct s_vect	(*add)(const t_vect self, t_vect to_add);
		};
		t_vect	v;
	};
}	t_vecf;

typedef struct s_tstep
{
	double	step;
	double	tex_y;
}	t_tstep;

typedef struct s_cdata
{
	int	*src;
	int	*dst;
}	t_cdata;

typedef struct s_dmask
{
	int	diff;
	int	mask;
}	t_dmask;

typedef struct s_mcol
{
	u_int	colour;
	u_int	mask;
	double	frac;
}	t_mcol;

typedef struct s_m128i
{
	__m128i	tex_vec;
	__m128i	dst_vec;
	__m128i	mask;
	__m128i	blend;
	struct
	{
		int		colour;
		u_int	overlay;
	};
}	t_m128i;

typedef struct s_vec4
{
	__m128	r0;
	__m128	r1;
	__m128	r2;
	__m128	r3;
}	t_vec4;

typedef struct s_imask
{
	int		i;
	u_int	mask;
}	t_imask;

typedef struct s_lvect
{
	long	x;
	long	y;
}	t_lvect;

typedef struct s_lvars
{
	int	height;
	int	top;
}	t_lvars;

/**
 * Confused about unions with multiple structs:
 * 	https://www.reddit.com/r/C_Programming/comments/p4rpkm/
 * Union of structs with common first member:
 * 	https://stackoverflow.com/questions/20752149/
 * https://gcc.gnu.org/onlinedocs/gcc/Cast-to-Union.html
 */
typedef struct s_ivect3
{
	union
	{
		struct
		{
			int	x;
			int	y;
		};
		t_ivect	xy;
	};
	int	z;
}	t_ivect3;

typedef struct s_var3
{
	int	row;
	int	screen_y;
	int	max_rows;
}	t_var3;

typedef struct s_ivect			t_point;

typedef struct s_arc
{
	t_point	center;
	int		r;
	double	a_start;
	double	a_end;
}	t_arc;

typedef struct s_ring_segment
{
	t_arc	out;
	t_arc	in;
}	t_ring_segment;

typedef struct s_chvec
{
	char	x;
	char	y;
}	t_chvec;

typedef struct s_chvec3
{
	char	x;
	char	y;
	char	z;
}	t_chvec3;

typedef struct s_enemy
{
	int		type;
	t_vect	pos;
}	t_enpos;

typedef struct s_rock
{
	t_ivect	pos;
	double	speed;
	t_tex	*tex;
}	t_rock;

typedef struct s_object
{
	t_etype		type;
	t_subtype	subtype;
	int			dead;
	int			attacking;
	int			health;
	size_t		last_damaged;
	t_vect		pos;
	t_vect		end_pos;
	t_vect		norm;
	t_vect		dir;
	double		speed;
	t_vect		p2;
	t_tex		*texture;
	t_anim		anim;
	t_anim		anim2;
}	t_obj;

typedef struct s_ray
{
	t_vect			intcpt;
	t_ivect			maptile;
	int				face;
	int				damaged;
	t_tex			*tex;
	double			pos;
	double			distance;
	struct s_ray	*in_front;
}	t_ray;

typedef struct s_dda
{
	t_vect		dir;
	t_vect		side_dist;
	t_vect		delta_dist;
	t_ivect		step;
	t_ivect		norm;
	int			faces[2];
	t_tex		*textures[2];
	double		gradient;
	double		c;
}	t_dda;

typedef enum e_doors
{
	D_OPEN = 0,
	D_BLUE,
	D_PINK,
	D_GREEN,
	D_MAX,
}	t_edoor;

typedef struct s_info			t_info;

typedef enum e_menustate
{
	MAIN,
	LVL_SELECT,
	PAUSE,
	OPTIONS,
	WIN,
	LOSE,
}	t_emenus;

typedef struct s_menustate
{
	t_emenus	state;
	t_emenus	prev;
	int			selected;
	int			no_items;
}	t_menustate;

typedef enum e_shtex
{
	tex_DOOR = 0,
	TEX_MAX
}	t_etex;

typedef struct s_shtex
{
	t_tex	door_tex[7];
	t_tex	door_super_tex[7];
	t_tex	door_missile_tex[7];
	t_tex	door_boss_tex[7];
	t_tex	cannon_tex[2];
	t_tex	crawler_tex[6];
	t_tex	atomic_tex[6];
	t_tex	holtz_tex[6];
	t_tex	reo_tex[4];
	t_tex	proj_tex[10];
	t_tex	proj_green_tex[4];
	t_tex	explode_tex[17];
	t_tex	energy_tex[3];
	t_tex	etank_tex[2];
	t_tex	missile_tex[12];
	t_tex	super_tex[12];
	t_tex	health_pu[4];
	t_tex	missile_ammo[2];
	t_tex	super_ammo[2];
	t_tex	trophy_tex[2];
	t_tex	phantoon[10];
	t_tex	phantoon_proj[6];
	t_tex	logo_tex[14];
	t_tex	dmg_tex[8];
	t_tex	title;
	t_tex	scope;
	t_tex	alphabet;
	t_tex	tele;
	t_tex	credits;
	t_tex	boss_bar[2];
	t_tex	rocks[7];
	t_tex	empty;
	t_tex	playertile;
	t_tex	square;
	t_tex	textures[TEX_MAX];
}	t_shtex;

typedef enum e_textures
{
	T_FLOOR,
	T_CEILING,
	NUM_TEXTURES
}	t_enum_tex;

typedef struct s_lvl
{
	t_info		*app;
	t_tex		n_tex;
	t_tex		s_tex;
	t_tex		e_tex;
	t_tex		w_tex;
	t_tex		floor_tex;
	t_tex		ceil_tex;
	Mix_Chunk	*music;
	int			outside;
	t_img		*minimap_xs;
	t_img		*minimap_xl;
	t_img		*help;
	t_img		*overlay;
	t_vect		map_scale_factor;
	t_point		mmap_origin;
	int			f_col;
	int			c_col;
	char		**map;
	t_anim		**anims;
	t_list		*enemies;
	t_list		*items;
	t_list		*doors;
	t_list		*triggers;
	t_list		*projectiles;
	t_list		*logo;
	t_list		*enemy_pos;
	t_obj		*boss_obj;
	bool		boss_active;
	t_vect		starting_pos;
	t_vect		starting_dir;
	int			height;
	int			width;
	char		*sublvls[4];
	t_img		*texs[NUM_TEXTURES];
}	t_lvl;

typedef struct s_poolnode
{
	t_ray				pool[RAY_POOL_SIZE];
	size_t				stackp;
	struct s_poolnode	*next;
}	t_poolnode;

typedef struct s_timer
{
	size_t	total_ms;
	size_t	cur_lvl_start;
	size_t	stop_time;
	int		active;
}	t_timer;

typedef struct s_player
{
	t_vect	pos;
	int		health;
	int		max_health;
	int		ammo[3];
	int		max_ammo[3];
	int		equipped;
	int		dead;
	t_vect	dir;
	int		vert_offset;
	double	angle;
	t_ray	rays[WIN_WIDTH];
	double	angle_offsets[WIN_WIDTH];
	double	offsets[WIN_HEIGHT / 2];
	t_anim	hud;
	t_vect	tele_pos;
	t_vect	dmg_dir;
	size_t	dmg_time;
	int		total_pickups;
	int		pickups_collected;
}	t_player;

typedef struct s_dummy
{
	t_vect	pos;
	t_vect	dir;
	double	credits_offsets[WIN_HEIGHT];
	t_img	credits;
	double	speed;
	t_list	*rocks;
}	t_dummy;

/**
 * frequency: DSP frequency -- samples per second
 * format: Audio data format. MIX_DEFAULT_FORMAT == AUDIO_S16LSB
 * nchannels: Number of channels: 1 mono, 2 stereo
 * chunk_size: Audio buffer size in sample FRAMES
 * (total samples divided by channel count)
 *
 * For more: see `struct SDL_AudioSpec`
 */
typedef struct s_aud
{
	int			frequency;
	u_int16_t	format;
	int			nchannels;
	int			chunk_size;
	int			snd_volume;
	int			mus_volume;
	const char	*files[SND_MAX];
	Mix_Chunk	*chunks[SND_MAX];
}	t_aud;

enum e_type
{
	fnt_main = 0,
	fnt_SansMono,
	FNT_MAX
};

typedef struct s_typing
{
	FT_Library	ft;
	int			default_size;
	const char	*files[FNT_MAX];
	FT_Face		faces[FNT_MAX];
}	t_typing;

struct s_info
{
	t_xvar		*mlx;
	t_typing	typ;
	t_win_list	*win;
	char		*title;
	t_img		*canvas;
	t_img		*overlay;
	t_img		*pointer;
	t_img		*skybox;
	t_img		*bg;
	t_img		*stillshot;
	t_ivect		origin;
	t_aud		audio;
	t_shtex		*shtex;
	t_lvl		*lvl;
	char		**map_ids;
	int			no_maps;
	t_player	*player;
	bool		keys[16];
	bool		mouse[16];
	t_state		state;
	t_ret_code	rc;
	t_menustate	menu_state;
	int			current_level;
	int			current_sublevel;
	int			fov_deg;
	int			ads;
	int			old_fov;
	double		fov_rad_half;
	double		fov_opp_len;
	size_t		fr_rate;
	size_t		fr_delay;
	double		fr_scale;
	size_t		fr_last;
	size_t		fr_time;
	size_t		fr_count;
	t_list		*lvl_cache;
	int			filter;
	u_char		fullscreen;
	t_timer		timer;
	t_dummy		*dummy;
	int			sensitivity;
	char		hint_shown;
};

# if __BYTE_ORDER == __LITTLE_ENDIAN

typedef struct s_colour
{
	union
	{
		u_int	raw;

		struct
		{
			u_char	b;
			u_char	g;
			u_char	r;
			u_char	a;
		};
	};
}	t_colour;
# elif __BYTE_ORDER == __BIG_ENDIAN

typedef struct s_colour
{
	union
	{
		u_int	raw;
		struct
		{
			u_char	a;
			u_char	r;
			u_char	g;
			u_char	b;
		};
	};
}	t_colour;

# else
#  error "Unsupported byte order"
# endif

# define ANGLE_EPSILON 0.02 // angle blend width (radians)
# define CHAR_WIDTH 8
# define MMAP_TILE_W 8
# define MMAP_TILE_H 8

# define C3D_FORBIDDEN_CHAR -1

# define MAP_LEFT		0b00000001
# define MAP_TOP		0b00000010
# define MAP_RIGHT		0b00000100
# define MAP_BOTTOM		0b00001000
# define MAP_TOP_LEFT	0b00010000
# define MAP_TOP_RIGHT	0b00100000
# define MAP_BOT_LEFT	0b01000000
# define MAP_BOT_RIGHT	0b10000000

# define SMALL_MMAP_SCALE 8

typedef enum e_bit
{
	bit_left = 0,
	bit_top,
	bit_right,
	bit_bottom,
	bit_tleft,
	bit_tright,
	bit_bleft,
	bit_bright,
}	t_bitno;

void	apply_alpha(t_img *img, u_char alpha);
void	place_tile_on_image32(t_img *img, t_img *tile, t_point p);
void	place_tile_on_image32_alpha(t_img *image, t_img *tile, t_point p);
void	place_char_img(char c, t_img *img, t_info *app, t_ivect3 ps);
void	on_expose(t_info *app);
int		cleanup(t_info *app);
void	replace_frame(t_info *app);
int		expose_win(void *param);
int		mouse_release_play(unsigned int button, int x, int y, void *param);
int		mouse_press_play(unsigned int button, int x, int y, void *param);
int		mouse_move_play(int x, int y, void *param);

t_lvl	*init_map(void);
void	free_map(t_lvl *lvl);
int		collect_map(t_list	*file, t_lvl *data);
int		map_is_valid(t_lvl *data);
int		parse_cub(t_info *app, char *filename);
t_lvl	*get_cached_lvl(t_info *app, char *name);
void	free_split(char **split);
void	load_shtex(t_info *app);

t_player	*init_player(t_info *app);
void		refresh_player(t_info *app, t_player *player);
void		refresh_map(t_info *app, t_lvl *lvl);
void		move_entity(t_vect *pos, t_lvl *lvl, t_vect dir);
void		move_obj_bounce(t_info *app, t_obj *obj, t_lvl *data);
void		rotate_player(t_info *app, t_player *player, int direction,
			double sensitivity);
void	handle_open_door(t_info *app, t_ray *ray);
void	next_weapon(t_player *player);
void	prev_weapon(t_player *player);

void	spawn_projectile(t_info *app, t_player *player, t_lvl *lvl,
						 t_subtype subtype);
void	spawn_enemy_projectile(t_info *app, t_obj *enemy, t_vect dir,
			int subtype);
t_obj	*spawn_enemy(t_info *app, t_vect pos, t_vect dir, int subtype);
void	spawn_item(t_info *app, t_vect pos, t_subtype subtype);
void	spawn_door(t_info *app, t_vect pos, int subtype);
void	spawn_trigger(t_info *app, t_vect pos, t_subtype subtype);
void	spawn_teleporter(t_info *app, t_vect pos, int level);
void	spawn_logo_piece(t_info *app, t_vect pos, t_vect dir, t_tex *texture);
void	init_logo_pieces(t_info *app, t_vect pos);

void	developer_console(t_info *app, t_player *player);
void	subtract_health(t_info *app, t_player *player, int damage);
void	add_health(t_player *player, int health);
void	damage_enemy(t_info *app, t_obj *enemy, int damage);
void	add_ammo(t_player *player, int type);
void	toggle_boss_doors(t_info *app);
int		check_tile_open(char tile, t_lvl *lvl);

double	get_gradient_angle(double angle);
double	get_y_intercept(t_vect pos, double gradient);
t_vect	get_vertical_int(double x, double gradient, double c);
t_vect	get_horizontal_int(double y, double gradient, double c);
double	get_cam_distance(t_vect pos, double angle, t_vect intcpt);
void	add_in_front(t_ray *ray, int face, t_tex *texture);
t_vect	get_line_intersect(t_vect l1p1, t_vect l1p2, t_vect l2p1, t_vect l2p2);
t_ray	*check_obj_collision(t_obj *object, t_ray *ray, t_player *player);
void	order_obj_ray(t_ray *obj, t_ray *ray);
void	calc_object_collisions(t_lvl *lvl, t_player *player, t_ray *ray);

t_vect	vect(double x, double y);
char	get_max_direction(t_vect vect);
t_vect	scale_vect(t_vect vect, double scalar);
t_ivect	scale_ivect(t_ivect vect, int scalar);
t_vect	rotate_vect(t_vect vect, double angle);
t_vect	rotv(double x, double y, double angle);
void	rotate_vect_inplace(t_vect *vect, double angle);
t_vect	add_vect(t_vect v1, t_vect v2);
t_vect	addi_vect(t_vect v1, t_ivect v2);
t_ivect	add_ivect(t_ivect v1, t_ivect v2);
t_vect	subtract_vect(t_vect v1, t_vect v2);
double	vector_distance(t_vect v1, t_vect v2);
double	vector_magnitude(t_vect vect);
t_vect	normalise_vect(t_vect vect);
double	dot_product(t_vect v1, t_vect v2);
double	vector_angle(t_vect v1, t_vect v2);
double	get_hyp_len(double len1, double len2);
void	*fast_memcpy_test(int *dst, const int *src, size_t count);
void	memcpy_sse2(void *dst_void, const void *src_void, size_t size);

void	cast_all_rays_alt(t_info *app, t_lvl *lvl, t_player *player);
t_ray	*get_pooled_ray(int flag);
t_poolnode	*add_poolnode(t_poolnode *head);
void	clear_poolnodes(t_poolnode *head, t_poolnode **current);
void	reset_pool(t_poolnode *head, t_poolnode **current);
int		count_poolnodes(t_poolnode *head);
t_ray	ray_dda(t_info *app, t_lvl *lvl, t_player *player, double angle);
t_ray	ray_dda_refactor(t_info *app, t_lvl *lvl,
			t_player *player, double angle);
void	free_ray_children(t_ray *ray);

void	replace_image(t_info *app, t_img **img, char *tex_file);
void	replace_sky(t_info *app, char *tex_file);
int		dim_colour(u_int col, double fact);

t_img	*scale_image(t_info *app, t_img *img, int new_x, int new_y);
t_tex	scale_texture(t_tex *tex, int scale);
t_img	*img_dup(t_info *app, t_img *src);
void	pix_dup(t_img *src, t_img *dst);
void	fill_with_colour(t_img *img, int f_col, int c_col);
//void	my_put_pixel_32(t_img *img, int x, int y, unsigned int colour);
void	put_texture(t_info *app, t_tex *tex, int x, int y);
void	place_tex_to_image_scale(t_img *img, const t_tex *tex, t_ivect pos,
			double scalar);
void	place_str(char *str, t_info *app, t_ivect spos, int scalar);
void	place_str_centred(char *str, t_info *app, t_ivect pos, int scalar);
void	place_fps(t_info *app);
void	place_timer(t_info *app, size_t time, t_ivect pos, int scalar);
t_tex	img_to_tex(t_info *app, const char *filename);
t_tex	img_to_tex_row_major(t_info *app, const char *filename);
u_int	*img_to_tex_static_row_major(t_info *app, const char **xpm_data, int *w, int *h);
u_int	*img_to_tex_static_col_major(t_info *app, const char **xpm_data, int *w, int *h);
void	put_pixel_alpha(t_img *img, t_point p, int base_color, double alpha_frac);
void	put_pixel_alpha_add(t_img *img, t_ivect p, int base_color, double alpha_frac);
void	draw_rays(t_info *app, t_img *canvas);
void	draw_hud(t_info *app);
void	draw_circle_filled(t_img *img, t_point c, int r, int color);
void	draw_ring_segment(t_img *img, t_ring_segment seg, int color);
void	free_shtex(t_info *app);
void	free_shsnd(t_info *app);
void	free_fonts(t_info *app);
t_img	*build_minimap(t_info *app, int scale);
t_tex	get_tile(int idx);
size_t	get_time_ms(void);
size_t	get_time_us(void);
double	rand_range(double lower, double upper);

int key_press_intro(KeySym key, void *param);
int key_release_intro(KeySym key, void *param);

int key_press_mmenu(KeySym key, void *param);
int key_release_mmenu(KeySym key, void *param);

int key_press_play(KeySym key, void *param);
int key_release_play(KeySym key, void *param);

int key_press_pmenu(KeySym key, void *param);
int key_release_pmenu(KeySym key, void *param);

int key_press_lose(KeySym key, void *param);
int key_release_lose(KeySym key, void *param);

int key_press_win(KeySym key, void *param);
int key_release_win(KeySym key, void *param);

int key_press_credits(KeySym key, void *param);
int key_release_credits(KeySym key, void *param);

int	render_intro(void *param);
int	render_mmenu(void *param);
int	render_pmenu(void *param);
int	render_play(void *app);
int	render_load(void *app);
int	render_lose(void *param);
int	render_win(void *param);
int	render_credits(void *param);

void	draw_sky(t_info *app);
void	draw_nav(t_info *app);
void 	draw_sky_alt(t_info *app);
void	fill_ceiling(t_info *app, t_lvl *lvl, t_player *player);
void	fill_floor(t_info *app, t_player *player, int is_floor);

void	menu_select_current(t_info *app);
void	draw_menu_items(t_info *app);
void	change_menu_selection(t_info *app, int dir);
void	menu_change_option(t_info *app, int dir);

t_state run_state(t_info *app, int argc, char **argv);
void	set_fov(t_info *app, int fov);
void	set_fonts(t_info *app);
int		init_fonts(t_info *app);
void	set_audio(t_info *app);
int		init_audio(t_info *app);
void	set_framerate(t_info *app, size_t framerate);
void	set_sensitivity(t_info *app, int sensitivity);
void	set_music_volume(t_info *app, int volume);
void	set_sound_volume(t_info *app, int volume);
void	calculate_offsets(t_info *app, t_player *player);
void	calculate_credits_offset(t_info *app, t_dummy *dummy);

int	bilinear_filter(double x, double y, const t_tex *tex);
// int	linear_filter_credits(double x, int y, const t_texarr *tex);

void	start_obj_death(t_obj *obj, t_info *app);
t_list	*delete_object(t_list **obj_list, t_list *obj_node);
t_obj	*check_obj_proximity(t_vect pos, t_lvl *lvl);
int	point_oob_global(t_vect pos, t_lvl *lvl);
void	select_projectile_tex(t_obj *obj, t_player *player, t_info *app);
t_tex	*handle_animation(t_info *app, t_anim anim);
t_anim	**create_anim_arr(int x, int y);
void	init_anims(t_info *app, t_lvl *lvl);
void	reset_anims(t_info *app, t_lvl *lvl);
int	count_collectables(t_lvl *lvl);
int	handle_obj_projectile(t_info *app, t_obj *obj, t_list **current);
int	handle_enemy_projectile(t_info *app, t_obj *obj, t_list **current);
void	spawn_drops(t_info *app, t_obj *obj, int no);
void	phantoon_ai(t_info *app, t_obj *obj);
void	reo_ai(t_info *app, t_obj *enemy);
void	atomic_ai(t_info *app, t_obj *enemy);
void	holtz_ai(t_info *app, t_obj *enemy, t_player *player);
void	zoomer_ai(t_info *app, t_obj *enemy);
int	handle_obj_entity(t_info *app, t_obj *obj, t_list **current);
int	handle_trigger(t_info *app, t_obj *obj, t_list **current);
void	handle_tele(t_info *app, t_obj *tele);
int	handle_obj_item(t_info *app, t_obj *obj, t_list **current);
void	update_objects(t_info *app, t_player *player, t_lvl *lvl);

int		check_line_of_sight(t_info *app, t_obj *obj, t_player *player);
//u_int	interpolate_colour(t_colour col1, t_colour col2);
void	draw_credits(t_info *app, t_dummy *dummy);
t_tex	*get_open_door_tex(t_anim *anim, t_info *app);
t_tex	*get_close_door_tex(t_anim *anim, t_info *app);
void	toggle_fullscreen(t_info *app);
int		get_key_index(KeySym key);

void	draw_text_freetype(t_info *app, t_img *img, const char *text, t_point c);
void	spawn_rock(t_info *app, t_ivect pos, t_tex *tex, double speed);

#endif //CUB3D_H
