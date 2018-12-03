/***************************************************************************
 *  glTetrix - another "i'm learning so i'm gonna do tetris" game
 *
 *  Copyright (c) 2004 BELiAL
 *  carlo.casta@gmail.com
 ****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "pieces.h"

#define APPNAME		"glTetrix"
#define APPVER 		"0.1"

int main()
{
	unsigned short width, height, bpp, i, j;
	unsigned int screenflags;
	SDL_Event event;
	int start_frame = 0;
	int millis = (int)(1000 / 30);
	char *tmpstring = NULL;

	width = 800;
	height = 600;
	bpp = 16;

	fprintf(stderr, "%s - initializing SDL... ", APPNAME);
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
		fprintf(stderr, " initialization failed: %s\n", SDL_GetError());
		exit(-1);
	}
	fprintf(stderr, " SDL init ok.\n");

	fprintf(stderr, "%s - initializing OpenGL... ", APPNAME);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE,     5);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,   6);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,    5);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,  16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	screenflags = SDL_OPENGL /*| SDL_FULLSCREEN*/;
	if (SDL_SetVideoMode(width, height, bpp, screenflags) == 0) {
		fprintf(stderr, " setting video mode failed: %s\n", SDL_GetError());
		exit(-1);
	}
	fprintf(stderr, " OpenGL init ok.\n");

	/* ensure SDL_Quit is executed when program ends */
	atexit(SDL_Quit);

	/* kill the mouse cursor */
	SDL_ShowCursor(SDL_DISABLE);

	/* initialize openGL */
	glShadeModel(GL_FLAT);
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 10, 0, 10, -1, 1);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	load_textures();
	build_font();

	zeroizeboard();
	/* init random system */
	srand((unsigned int)time(NULL));
	next_piece = (short)(rand() % 7);
	newpiece();
	lastmove = SDL_GetTicks();
	lastremove = 0;

	user.quit = 0;
	/* basically the game is an infinite loop */
	while (!user.quit) {

		start_frame = SDL_GetTicks();

		/* check for any user input*/
		if (SDL_PollEvent(&event) != 0) {
			switch (event.type) {
				/* keyboard press */
				case SDL_KEYDOWN :
					switch (event.key.keysym.sym) {
						case SDLK_ESCAPE :
							user.quit = 1;
							break;
						case SDLK_SPACE :
							user.rotate = 1;
							break;
						case SDLK_DOWN :
							user.movedown = 1;
							break;
						case SDLK_RIGHT :
							user.moveright = 1;
							break;
						case SDLK_LEFT :
							user.moveleft = 1;
							break;
						case SDLK_d:
		/* dumps gameboard to screen */
		for (i = 0; i < 20; ++i) {
			for (j = 0; j < 10; ++j)
				fprintf(stderr, "|%c", gameboard.board[i][j]);
			fprintf(stderr, "|\n");
		}
		putchar('\n');
							break;
						default :
							break;
					}
					break;
				case SDL_KEYUP :
					switch (event.key.keysym.sym) {
						case SDLK_DOWN :
							user.movedown = 0;
							break;
						case SDLK_RIGHT :
							user.moveright = 0;
							break;
						case SDLK_LEFT :
							user.moveleft = 0;
							break;
						default:
							break;
					}
					break;

				/* i only process keypresses */
				default:
					break;
			}
		}

		game_loop();

		/* this is the display part */
		glClear(GL_COLOR_BUFFER_BIT);

		/* container */
		glLoadIdentity();
		glTranslatef(2.9, 0.5, 0.0);
		glBegin(GL_QUADS);
		 glColor3f(0.2, 0.2, 0.2);
		 glVertex2f(0.0, 0.0);
		 glVertex2f(4.2, 0.0);
		 glVertex2f(4.2, 8.2);
		 glVertex2f(0.0, 8.2);
		glEnd();

		/* gameboard */
		glLoadIdentity();
		glTranslatef(3.0, 0.6, 0.0);
		glBegin(GL_QUADS);
		 glColor3f(0.0, 0.0, 0.0);
		 glVertex2f(0.0, 0.0);
		 glVertex2f(4.0, 0.0);
		 glVertex2f(4.0, 8.0);
		 glVertex2f(0.0, 8.0);
		glEnd();

		/* preview window */
		glLoadIdentity();
		glTranslatef(7.5, 5.6, 0.0);
		glBegin(GL_QUADS);
		 glColor3f(0.0, 0.0, 0.0);
		 glVertex2f(0.0, 0.0);
		 glVertex2f(2.0, 0.0);
		 glVertex2f(2.0, 3.0);
		 glVertex2f(0.0, 3.0);
		glEnd();

		/* display board status */
		for (i = 0; i < 10; ++i) {
			for (j = 0; j < 20; ++j) {
				if (gameboard.board[j][i] == 'x' ||
				    gameboard.board[j][i] == 'r') {
					glLoadIdentity();
					glTranslatef(GAMEBOARD_BASE_W +
					              (BASE_WH * i),
					             GAMEBOARD_BASE_H +
					              (BASE_WH * j),
					             0.0);
					glEnable(GL_TEXTURE_2D);
					if (gameboard.board[j][i] == 'r')
						glBindTexture(GL_TEXTURE_2D, texture[2]);
					else
						glBindTexture(GL_TEXTURE_2D, texture[0]);
					glBegin(GL_QUADS);
					if (gameboard.board[j][i] == 'r')
						glColor3f(1.0, 1.0, 1.0);
					 else
					 	glColor3f(gameboard.color[j][i][0],
					           gameboard.color[j][i][1],
					           gameboard.color[j][i][2]);
					 glTexCoord2f(0.0, 0.0);
					 glVertex2f(0.0, 0.0);
					 glTexCoord2f(1.0, 0.0);
					 glVertex2f(0.4, 0.0);
					 glTexCoord2f(1.0, 1.0);
					 glVertex2f(0.4, 0.4);
					 glTexCoord2f(0.0, 1.0);
					 glVertex2f(0.0, 0.4);
					glEnd();
					glDisable(GL_TEXTURE_2D);
				}
			}
		}

		/* update "next" field
		/* TODO: 3D view with rotation  */
		for (i = 0; i < 4; ++i) {
			for (j = 0; j < 4; ++j) {
				glLoadIdentity();
				glTranslatef(7.7 + (i * 0.4),
				             5.8 + (j * 0.4), 0.0);
				if (pieces_t[next_piece].space[DEG_90][i][j] == 'x') {
					glEnable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D, texture[0]);
					glBegin(GL_QUADS);
					 glColor3f(pieces_t[next_piece].color[0],
					           pieces_t[next_piece].color[1],
					           pieces_t[next_piece].color[2]);
					 glTexCoord2f(0.0, 0.0);
					 glVertex2f(0.0, 0.0);
					 glTexCoord2f(1.0, 0.0);
					 glVertex2f(0.4, 0.0);
					 glTexCoord2f(1.0, 1.0);
					 glVertex2f(0.4, 0.4);
					 glTexCoord2f(0.0, 1.0);
					 glVertex2f(0.0, 0.4);
					glEnd();
					glDisable(GL_TEXTURE_2D);
				} else {
					glEnable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D, texture[0]);
					glBegin(GL_QUADS);
					 glColor3f(0.2, 0.2, 0.2);
					 glTexCoord2f(0.0, 0.0);
					 glVertex2f(0.0, 0.0);
					 glTexCoord2f(1.0, 0.0);
					 glVertex2f(0.4, 0.0);
					 glTexCoord2f(1.0, 1.0);
					 glVertex2f(0.4, 0.4);
					 glTexCoord2f(0.0, 1.0);
					 glVertex2f(0.0, 0.4);
					glEnd();
					glDisable(GL_TEXTURE_2D);
				}
			}
		}

		glLoadIdentity();
		glColor3f(0.0, 0.0, 1.0);
		glPrintf(490, 390, "N E X T", 0);

		glLoadIdentity();
		glColor3f(1.0, 1.0, 1.0);
		glPrintf(5, 120, "removed lines", 0);
		tmpstring = calloc(50, sizeof(char));
		sprintf(tmpstring, "%d", rlines);
		glPrintf(5, 100, tmpstring, 0);
		free(tmpstring);

		tmpstring = calloc(50, sizeof(char));
		sprintf(tmpstring, "SCORE: %d", score);
		glPrintf(5, 200, tmpstring, 0);
		free(tmpstring);

		/* notify user in case of game over :) */
		if (gameover == 1) {
			glLoadIdentity();
			glColor3f(1.0, 0.0, 0.0);
			glPrintf(192, 430, "G A M E   O V E R", 0);
		}

		SDL_GL_SwapBuffers();

		/* keep frame rate constant */
		if (SDL_GetTicks() < start_frame + millis)
			SDL_Delay((start_frame + millis) - SDL_GetTicks());
	}
	free_font();
	return (0);
}


void build_font(void)
{
	float cx;
	float cy;
	int loop;

	base_font = glGenLists(256);
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	for (loop=0; loop < 256; ++loop) {
		cx = (float)(loop%16)/16.0f;
		cy = (float)(loop/16)/16.0f;

		glNewList(base_font+loop,GL_COMPILE);
		glBegin(GL_QUADS);
		 glTexCoord2f(cx,1-cy-0.0625f);
		 glVertex2i(0,0);
		 glTexCoord2f(cx+0.0625f,1-cy-0.0625f);
		 glVertex2i(16,0);
		 glTexCoord2f(cx+0.0625f,1-cy);
		 glVertex2i(16,16);
		 glTexCoord2f(cx,1-cy);
		 glVertex2i(0,16);
		glEnd();
		glTranslated(13,0,0);
		glEndList();
	}
}

void free_font(void)
{
	glDeleteLists(base_font,256);
}

void glPrintf(GLint x, GLint y, char *string, int set)
{
	if (set > 1)
		set = 1;
	if (set < 1)
		set = 0;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, 640, 0, 480, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(x, y, 0);
	glListBase(base_font-32 + (128*set));
	glCallLists(strlen(string), GL_BYTE, string);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);

	return;
}

void load_textures(void)
{
	static const char texfile[] = "textures/quad.bmp";
	static const char texfile2[] = "textures/quad-rem.bmp";
	static const char fontfile[] = "textures/font.bmp";
	SDL_Surface *image, *image2, *font;
	Uint8 *rowhi, *rowlo;
	Uint8 *tmpbuf, tmpch;
	int i, j;

	image = SDL_LoadBMP(texfile);
	if (image == NULL) {
		fprintf(stderr, "Unable to load %s: %s\n", texfile, SDL_GetError());
		return;
	}

	image2 = SDL_LoadBMP(texfile2);
	if (image2 == NULL) {
		fprintf(stderr, "Unable to load %s: %s\n", texfile2, SDL_GetError());
		return;
	}

	font = SDL_LoadBMP(fontfile);
	if (font == NULL) {
		fprintf(stderr, "Unable to load font file %s: %s\n", fontfile, SDL_GetError());
		return;
	}

	tmpbuf = (Uint8 *)malloc(font->pitch);
	if ( tmpbuf == NULL ) {
		fprintf(stderr, "allocation failed!\n");
		return;
	}
	rowhi = (Uint8 *)font->pixels;
	rowlo = rowhi + (font->h * font->pitch) - font->pitch;
	for (i=0; i<font->h/2; ++i) {
		for (j=0; j<font->w; ++j) {
			tmpch = rowhi[j*3];
			rowhi[j*3] = rowhi[j*3+2];
			rowhi[j*3+2] = tmpch;
			tmpch = rowlo[j*3];
			rowlo[j*3] = rowlo[j*3+2];
			rowlo[j*3+2] = tmpch;
		}
		memcpy(tmpbuf, rowhi, font->pitch);
		memcpy(rowhi, rowlo, font->pitch);
		memcpy(rowlo, tmpbuf, font->pitch);
		rowhi += font->pitch;
		rowlo -= font->pitch;
	}
	free(tmpbuf);

	tmpbuf = (Uint8 *)malloc(image2->pitch);
	if ( tmpbuf == NULL ) {
		fprintf(stderr, "allocation failed!\n");
		return;
	}
	rowhi = (Uint8 *)image2->pixels;
	rowlo = rowhi + (image2->h * image2->pitch) - image2->pitch;
	for (i=0; i<image2->h/2; ++i) {
		for (j=0; j<image2->w; ++j) {
			tmpch = rowhi[j*3];
			rowhi[j*3] = rowhi[j*3+2];
			rowhi[j*3+2] = tmpch;
			tmpch = rowlo[j*3];
			rowlo[j*3] = rowlo[j*3+2];
			rowlo[j*3+2] = tmpch;
		}
		memcpy(tmpbuf, rowhi, image2->pitch);
		memcpy(rowhi, rowlo, image2->pitch);
		memcpy(rowlo, tmpbuf, image2->pitch);
		rowhi += image2->pitch;
		rowlo -= image2->pitch;
	}
	free(tmpbuf);

	glGenTextures(3, &texture[0]);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, image->w, image->h, 0, GL_RGB, GL_UNSIGNED_BYTE, image->pixels);

	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, font->w, font->h, 0, GL_RGB, GL_UNSIGNED_BYTE, font->pixels);

	glBindTexture(GL_TEXTURE_2D, texture[2]);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, image2->w, image2->h, 0, GL_RGB, GL_UNSIGNED_BYTE, image2->pixels);


	SDL_FreeSurface(image);
	SDL_FreeSurface(image2);
	SDL_FreeSurface(font);
}

void game_loop(void)
{
	now = SDL_GetTicks();
	short i, j, tmprl;


	/* Oh no... Game Over. :) */
	if (gameover == 1)
		return;

	/* if a line has to be removed, let the
	 * user actually SEE it.
	 * - this means _don't_ modify gameboard for
	 * at least 1half second -
	 */
	if (line_removal == 1) {
		if (now < lastremove + 500)
			return;

		/* ok, now update gameboard removing line(s) */
		line_removal = 0;
		delete_lines();
		newpiece();
	}

	/* only execute if it's time to lower piece or user
	 * pressed a (valid) key
         */
	if ((( (user.movedown == 1)  ||
	       (user.moveright == 1) ||
	       (user.moveleft == 1)  ||
	       (user.rotate == 1) )  && (now > usermove + 100)) ||
	       (now > lastmove + 1000)) {

		/* clear old position for falling object */
		for (i = 0; i < 4; ++i) {
			for (j = 0; j < 4; ++j) {
				if (PIECE[i][j] == ' ')
					continue;
				gameboard.board[PIECE_Y + j][PIECE_X + i] = ' ';
				memset(gameboard.color[PIECE_Y + j][PIECE_X + i], 0, 3 * sizeof(GLfloat));
			}
		}

		if (((user.movedown == 1) && (now > usermove + 100)) ||
		     (now > lastmove + 1000)) {
			lastmove = now;
			currpiece.curr_y -= 1;
			if (verify_move()) {
				/* so this piece reached end of the board */
				currpiece.curr_y += 1;
				for (i = 0; i < 4; ++i) {
					for (j = 0; j < 4; ++j) {
						/* don't care about white spaces */
						if (PIECE[i][j] == ' ')
							continue;
						gameboard.board[PIECE_Y + j][PIECE_X + i] = 'x';
						gameboard.color[PIECE_Y + j][PIECE_X + i][0] =
							pieces_t[currpiece.type].color[0];
						gameboard.color[PIECE_Y + j][PIECE_X + i][1] =
							pieces_t[currpiece.type].color[1];
						gameboard.color[PIECE_Y + j][PIECE_X + i][2] =
							pieces_t[currpiece.type].color[2];
					}
				}
				/*
		 		 * check for any filled line
		 		 */
				tmprl = remove_lines();
				rlines += tmprl;
				score += (tmprl > 0)? (100 * tmprl) : 0;
				if (line_removal == 1) {
					lastremove = SDL_GetTicks();
					return;
				}

				/* insert a new piece
				 * unless game's over
				 */
				if (gameover == 1)
					return;
				newpiece();

			}
		} else if (user.moveright == 1) {
			currpiece.curr_x += 1;
			if (verify_move())
				currpiece.curr_x -= 1;
		} else if (user.moveleft == 1) {
			currpiece.curr_x -= 1;
			if (verify_move())
				currpiece.curr_x += 1;
		} else if (user.rotate == 1) {
			user.rotate = 0;
			if (currpiece.rot_angle == DEG_270)
				currpiece.rot_angle = DEG_0;
			else
				currpiece.rot_angle++;
			if (verify_move()) {
				if (currpiece.rot_angle == DEG_0)
					currpiece.rot_angle = DEG_270;
				else
					currpiece.rot_angle--;
			}
		}

		/* update board */
		for (i = 0; i < 4; ++i) {
			for (j = 0; j < 4; ++j) {
				if (PIECE[i][j] == ' ')
					continue;
				gameboard.board[PIECE_Y + j][PIECE_X + i] = 'x';
				gameboard.color[PIECE_Y + j][PIECE_X + i][0] =
					pieces_t[currpiece.type].color[0];
				gameboard.color[PIECE_Y + j][PIECE_X + i][1] =
					pieces_t[currpiece.type].color[1];
				gameboard.color[PIECE_Y + j][PIECE_X + i][2] =
					pieces_t[currpiece.type].color[2];
			}
		}
		usermove = now;
	}
	return;
}


int verify_move(void)
{
	short i, j;

	for (i = 0; i < 4; ++i) {
		for (j = 0; j < 4; ++j) {
			/* don't care about white spaces */
			if (PIECE[i][j] == ' ')
				continue;
			if (gameboard.board[PIECE_Y + j][PIECE_X + i] == 'x' ||
			    (PIECE_Y + j < 0) ||
			    ((PIECE_X + i < 0)  || (PIECE_X + i >= 10)))
				/* Oops... collision detected! */
				return -1;
		}
	}
	return 0;
}


void zeroizeboard(void)
{
	short i, j;

	for (i = 0; i < 10; ++i) {
		for (j = 0; j < 20; ++j) {
			gameboard.board[j][i] = ' ';
			gameboard.color[j][i][0] = 0.0;
			gameboard.color[j][i][1] = 0.0;
			gameboard.color[j][i][2] = 0.0;
		}
	}

	return;
}

void newpiece(void)
{
	currpiece.type = next_piece;
	currpiece.curr_x = 3;
	currpiece.curr_y = 16;
	currpiece.rot_angle = DEG_0;

	if (verify_move())
		/* game over! */
		gameover = 1;

	next_piece = (short)(random() % 7);

	return;
}

int remove_lines(void)
{
	short i, j, reml = 1, rl;
	unsigned char lines[4] = {0, 0, 0, 0};

	for (i = 0; i < 20; ++i) {
		if (!strncmp(gameboard.board[i], "xxxxxxxxxx", 10)) {
			++reml;
			lines[0] = i;
			for (j = i+1; j < i+4; ++j) {
				if (!strncmp(gameboard.board[j], "xxxxxxxxxx", 10)) {
					lines[j-i] = j;
					++reml;
				}
			}
			break;
		}
	}

	j = 0;
	line_removal = (reml > 1)? 1 : 0;
	rl = reml;
	while (--reml) {
		memset(gameboard.board[(lines[j])], 'r', 10 * (sizeof(char)));
		++j;
	}

	return (rl - 1);
}

void delete_lines(void)
{
	short i, j;

	for (i = 19; i >= 0; --i) {
		if (gameboard.board[i][0] == 'r') {
			for (j = i; j < 19; ++j) {
				memcpy(gameboard.board[j], gameboard.board[j+1], 10 * (sizeof(char)));
				memcpy(gameboard.color[j], gameboard.color[j+1], 10 * 3 * (sizeof(GLfloat)));
			}
			memset(gameboard.board[19], ' ', 10 * (sizeof(char)));
			memset(gameboard.color[19], 0, 10 * 3 * (sizeof(GLfloat)));
		}
	}

	return;
}
