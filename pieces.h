/***************************************************************************
 *  glTetrix - another "i'm learning so i'm gonna do tetris" game
 *
 *  Copyright (c) 2004  BELiAL
 *  carlo.casta@gmail.com
 ****************************************************************************/


#define BASE_WH			0.4
#define GAMEBOARD_BASE_W	3.0
#define GAMEBOARD_BASE_H	0.6

/* some shortcuts */
#define PIECE	pieces_t[currpiece.type].space[currpiece.rot_angle]
#define PIECE_Y currpiece.curr_y
#define PIECE_X currpiece.curr_x

enum {DEG_0 = 0, DEG_90, DEG_180, DEG_270};
enum {TX_QUAD = 0, TX_LINE, TX_T, TX_L, TX_LREV, TX_S, TX_Z};


static struct _gb {
	char board[20][10];
	GLfloat color[20][10][3];
} gameboard;

static struct _usr {
	char quit;
	char rotate;
	char moveleft;
	char moveright;
	char movedown;
} user;

static struct _fallingobj {
	short type;
	short curr_x;
	short curr_y;
	short rot_angle;
} currpiece;

typedef struct ps{
	/* quad, line, T, L, Lrev, S, Z */
	char name[5];
	/* 
	 *  internal representation.
	 *  [ ][ ][ ][ ]
	 *  [ ][ ][ ][ ]
	 *  [ ][ ][ ][ ]
	 *  [ ][ ][ ][ ]
	 */
	char space[4][4][4];
	GLfloat color[3];
} Piece;

static Piece pieces_t[7] = {
	{ {"quad"},
	{{{ ' ', ' ', ' ', ' ' },
	  { ' ', 'x', 'x', ' ' },
	  { ' ', 'x', 'x', ' ' },
	  { ' ', ' ', ' ', ' ' }},
	  
	 {{ ' ', ' ', ' ', ' ' },
	  { ' ', 'x', 'x', ' ' },
	  { ' ', 'x', 'x', ' ' },
	  { ' ', ' ', ' ', ' ' }}, 
	 
	 {{ ' ', ' ', ' ', ' ' },
	  { ' ', 'x', 'x', ' ' },
	  { ' ', 'x', 'x', ' ' },
	  { ' ', ' ', ' ', ' ' }},
	 
	 {{ ' ', ' ', ' ', ' ' },
	  { ' ', 'x', 'x', ' ' },
	  { ' ', 'x', 'x', ' ' },
	  { ' ', ' ', ' ', ' ' }}},
	  {0.8, 0.3, 0.8}
	},
	
	{ {"line"},
	{{{ ' ', 'x', ' ', ' ' },
	  { ' ', 'x', ' ', ' ' },
	  { ' ', 'x', ' ', ' ' },
	  { ' ', 'x', ' ', ' ' }},
	  
	 {{ ' ', ' ', ' ', ' ' },
	  { ' ', ' ', ' ', ' ' },
	  { 'x', 'x', 'x', 'x' },
	  { ' ', ' ', ' ', ' ' }},
	  
	 {{ ' ', 'x', ' ', ' ' },
	  { ' ', 'x', ' ', ' ' },
	  { ' ', 'x', ' ', ' ' },
	  { ' ', 'x', ' ', ' ' }},
	  
	 {{ ' ', ' ', ' ', ' ' },
	  { ' ', ' ', ' ', ' ' },
	  { 'x', 'x', 'x', 'x' },
	  { ' ', ' ', ' ', ' ' }}}, 
	  {0.3, 0.3, 0.8}
	},
	
	{ {"T"},
	{{{ ' ', ' ', ' ', ' ' },
	  { 'x', 'x', 'x', ' ' },
	  { ' ', 'x', ' ', ' ' },
	  { ' ', ' ', ' ', ' ' }},
	  
	 {{ ' ', ' ', ' ', ' ' },
	  { ' ', ' ', 'x', ' ' },
	  { ' ', 'x', 'x', ' ' },
	  { ' ', ' ', 'x', ' ' }}, 
	  
	 {{ ' ', ' ', ' ', ' ' },
	  { ' ', 'x', ' ', ' ' },
	  { 'x', 'x', 'x', ' ' },
	  { ' ', ' ', ' ', ' ' }},
	 
         {{ ' ', ' ', ' ', ' ' },
	  { ' ', 'x', ' ', ' ' },
	  { ' ', 'x', 'x', ' ' },
	  { ' ', 'x', ' ', ' ' }}}, 
	  {0.8, 0.8, 0.3}
	},
	
	{ {"L"},
	{{{ ' ', ' ', ' ', ' ' },
	  { ' ', 'x', ' ', ' ' },
	  { ' ', 'x', ' ', ' ' },
	  { ' ', 'x', 'x', ' ' }},
	  
	 {{ ' ', ' ', ' ', ' ' },
	  { ' ', 'x', 'x', 'x' },
	  { ' ', 'x', ' ', ' ' },
	  { ' ', ' ', ' ', ' ' }},
	  
	 {{ ' ', ' ', ' ', ' ' },
	  { ' ', 'x', 'x', ' ' },
	  { ' ', ' ', 'x', ' ' },
	  { ' ', ' ', 'x', ' ' }},
	  
	 {{ ' ', ' ', ' ', ' ' },
	  { ' ', ' ', ' ', 'x' },
	  { ' ', 'x', 'x', 'x' },
	  { ' ', ' ', ' ', ' ' }}}, 
	  {0.3, 0.8, 0.8}
	},
	
	{ {"Lrev"},
	{{{ ' ', ' ', ' ', ' ' },
	  { ' ', ' ', 'x', ' ' },
	  { ' ', ' ', 'x', ' ' },
	  { ' ', 'x', 'x', ' ' }},
	  
	 {{ ' ', ' ', ' ', ' ' },
	  { ' ', 'x', ' ', ' ' },
	  { ' ', 'x', 'x', 'x' },
	  { ' ', ' ', ' ', ' ' }},
	  
	 {{ ' ', ' ', ' ', ' ' },
	  { ' ', 'x', 'x', ' ' },
	  { ' ', 'x', ' ', ' ' },
	  { ' ', 'x', ' ', ' ' }},
	  
	 {{ ' ', ' ', ' ', ' ' },
	  { ' ', 'x', 'x', 'x' },
	  { ' ', ' ', ' ', 'x' },
	  { ' ', ' ', ' ', ' ' }}}, 
	  {0.9, 0.6, 0.0}	
	},
	
	{ {"S"},
	{{{ ' ', ' ', ' ', ' ' },
	  { ' ', 'x', ' ', ' ' },
	  { ' ', 'x', 'x', ' ' },
	  { ' ', ' ', 'x', ' ' }},
	  
	 {{ ' ', ' ', ' ', ' ' },
	  { ' ', ' ', 'x', 'x' },
	  { ' ', 'x', 'x', ' ' },
	  { ' ', ' ', ' ', ' ' }}, 
	  
	 {{ ' ', ' ', ' ', ' ' },
	  { ' ', 'x', ' ', ' ' },
	  { ' ', 'x', 'x', ' ' },
	  { ' ', ' ', 'x', ' ' }},
	  
	 {{ ' ', ' ', ' ', ' ' },
	  { ' ', ' ', 'x', 'x' },
	  { ' ', 'x', 'x', ' ' },
	  { ' ', ' ', ' ', ' ' }}},  
	  {0.3, 0.8, 0.3}
	},
	
	{ {"Z"},
	{{{ ' ', ' ', ' ', ' ' },
	  { ' ', ' ', 'x', ' ' },
	  { ' ', 'x', 'x', ' ' },
	  { ' ', 'x', ' ', ' ' }},

         {{ ' ', ' ', ' ', ' ' },
	  { 'x', 'x', ' ', ' ' },
	  { ' ', 'x', 'x', ' ' },
	  { ' ', ' ', ' ', ' ' }},
	  
	 {{ ' ', ' ', ' ', ' ' },
	  { ' ', ' ', 'x', ' ' },
	  { ' ', 'x', 'x', ' ' },
	  { ' ', 'x', ' ', ' ' }},

         {{ ' ', ' ', ' ', ' ' },
	  { 'x', 'x', ' ', ' ' },
	  { ' ', 'x', 'x', ' ' },
	  { ' ', ' ', ' ', ' ' }}},
	  {0.8, 0.4, 0.3}
	}
};

Uint32 now, lastmove, usermove, lastremove;
GLuint texture[3], base_font;
short next_piece, gameover, line_removal;
int score = 0;
int rlines = 0;
void load_textures(void);
void game_loop(void);
void zeroizeboard(void);
void newpiece(void);
int verify_move(void);
void build_font(void);
void free_font(void);
void glPrintf(GLint, GLint, char*, int);
int remove_lines(void);
void delete_lines(void);
