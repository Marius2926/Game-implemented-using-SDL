
#include <SDL.h>
#include <stdio.h>
#include <SDL_ttf.h>
#define max(a,b) a<b ? b:a
#define min(a,b) a<b ? a:b
const int WidthSquaresNr = 20, HeightSquaresNr = 23;//squares

class Button {
	SDL_Rect o;
public:
	Button();
	Button(int, int, int,int );
	void draw_text(const char*,const char*,SDL_Renderer *&,int,SDL_Color);
	int event_handler(SDL_Event&);
	Button& operator=(Button&);
	~Button() {}
};

Button& Button::operator=(Button &b)
{
	this->o = b.o;
	return (*this);
}

Button::Button()
{
	o.x = 0;
	o.y = 0;
	o.w = 0;
	o.h = 0;
}

Button::Button(int x, int y,int w,int h)
{
	o.x = x;
	o.y = y;
	o.w = w;
	o.h = h;
}

void Button::draw_text(const char *way,const char* message,SDL_Renderer *&r,int size,SDL_Color bg)
{	
	SDL_Surface *Smessage = new SDL_Surface;
	TTF_Font *font = TTF_OpenFont(way, size);
	SDL_Color White = { 255,255,255,255 };
	Smessage = TTF_RenderText_Shaded(font, message, White,bg);
	SDL_Texture *Tmessage = SDL_CreateTextureFromSurface(r, Smessage);
	SDL_RenderCopy(r, Tmessage, NULL, &o);
}

int Button::event_handler(SDL_Event &e)
{
	if (e.type == SDL_QUIT)
		return -1;
	if (e.type == SDL_MOUSEBUTTONDOWN)
	{
		int x, y;
		SDL_GetMouseState(&x, &y);
		if (x >= o.x and x <= o.x + o.w and y >= o.y and y <= o.y + o.h)
			return 1;
		return 0;
	}
}

class Square {
	double x, y;
	double l;
	bool pressed = false;
public:
	void draw_circle(SDL_Renderer*&,int,int,int,int);
	Square() :x(0), y(0), l(0) {}
	void operator()(double, double, double);
	bool isPressed();
	double get_l() { return l; }
	friend void creating_cont(SDL_Renderer*&,Square **);
	friend bool square_det(int&, int&, Square**,SDL_Window*);
	~Square() {}
};

bool Square::isPressed() {
	return pressed;
}

void Square::draw_circle(SDL_Renderer *&p,int r,int g,int b,int a)
{	
	pressed = true;
	double x_center = x + l/2, y_center = y + l/2;
	double x1 = l/2, y1 = 0, P = 1.25 - l/2;
	SDL_SetRenderDrawColor(p, r, g, b, a);
	SDL_Point *v = new SDL_Point[200];
	int n = 0;
	v[n].x = x1 + x_center;
	v[n++].y = y1 + y_center;
	v[n].x = x1 + x_center;
	v[n++].y = -y1 + y_center;
	v[n].x = y1 + x_center;
	v[n++].y = x1 + y_center;
	v[n].x = -y1 + x_center;
	v[n++].y = x1 + y_center;
	while (x1 > y1)
	{	
		y1++;
		if (P <= 0)
			P = P + 2 * y1 + 1;
		else
		{
			x1--;
			P = P + 2 * y1 - 2 * x1 + 1;
		}
		if (x1 < y1)
			break;
		v[n].x = x1 + x_center;
		v[n++].y = y1 + y_center;
		v[n].x = -x1 + x_center;
		v[n++].y = y1 + y_center;
		v[n].x = x1 + x_center;
		v[n++].y = -y1 + y_center;
		v[n].x = -x1 + x_center;
		v[n++].y = -y1 + y_center;
		if (x1 != y1)
		{	
			v[n].x = y1 + x_center;
			v[n++].y = x1 + y_center;
			v[n].x = -y1 + x_center;
			v[n++].y = x1 + y_center;
			v[n].x = y1 + x_center;
			v[n++].y = -x1 + y_center;
			v[n].x = -y1 + x_center;
			v[n++].y = -x1 + y_center;
		}
	}
	v[n].x = x_center;
	v[n++].y = y_center;
	SDL_RenderDrawLines(p, v, n);
	delete[] v;
}

void Square::operator()(double x, double y, double l)
{
	this->x = x;
	this->y = y;
	this->l = l;
}

bool init(SDL_Window *&p,SDL_Renderer *&r)
{	
	if (SDL_Init(SDL_INIT_VIDEO) < 0 or TTF_Init())
	{
		printf("Error: %s", SDL_GetError());
		return false;
	}
	int width, height;
	SDL_DisplayMode Dm;
	SDL_GetCurrentDisplayMode(0, &Dm);
	width = Dm.w;
	height = Dm.h;
	p = SDL_CreateWindow("Gomoku", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height,SDL_WINDOW_SHOWN);
	if (p == NULL)
	{
		printf("Error: %s", SDL_GetError());
		return false;
	}
	r = SDL_CreateRenderer(p, -1, SDL_RENDERER_ACCELERATED);
	if (r == NULL)
	{
		printf("Error: %s", SDL_GetError());
		return false;
	}
	SDL_SetRenderDrawColor(r, 0xff, 0xff, 0xff, 0xff);
	return true;
}


void close(SDL_Window *&p,SDL_Renderer *&r)
{	
	SDL_DestroyRenderer(r);
	r = NULL;
	SDL_DestroyWindow(p);
	p = NULL;
	SDL_Quit();
}

void draw_map(SDL_Window *&p, SDL_Renderer *&r,Square **map)
{
	int hscreen, wscreen,i,j;
	double l, wmap, hmap, xmap, ymap;
	SDL_GetWindowSize(p, &wscreen, &hscreen);
	if (wscreen < hscreen)
		l = wscreen / WidthSquaresNr;
	else
		l = hscreen / HeightSquaresNr;
	wmap = l * WidthSquaresNr;
	hmap = l * HeightSquaresNr;
	xmap = ((double)wscreen - wmap) / 2;
	ymap = ((double)hscreen - hmap) / 2;
	for (i = 0; i < HeightSquaresNr; i++)
		for (j = 0; j < WidthSquaresNr; j++)
			map[i][j](j*l + xmap, i*l + ymap, l);
	SDL_SetRenderDrawColor(r, 26, 159, 155, 0xff);
	SDL_RenderClear(r);
	//generare contur
	creating_cont(r, map);
	SDL_RenderPresent(r);
}

void creating_cont(SDL_Renderer*& p, Square** map)
{
	int i;
	SDL_SetRenderDrawColor(p, 0, 0, 0, 255);
	for (i = 0; i < WidthSquaresNr; i++)
		SDL_RenderDrawLine(p, map[0][i].x, map[0][i].y, map[HeightSquaresNr - 1][i].x, map[HeightSquaresNr - 1][i].y + map[HeightSquaresNr-1][i].l);
	for (i = 0; i < HeightSquaresNr; i++)
		SDL_RenderDrawLine(p, map[i][0].x, map[i][0].y, map[i][WidthSquaresNr - 1].x + map[i][WidthSquaresNr - 1].l, map[i][WidthSquaresNr - 1].y);
	SDL_RenderDrawLine(p, map[0][WidthSquaresNr - 1].x + map[0][WidthSquaresNr - 1].l, map[0][WidthSquaresNr - 1].y, map[HeightSquaresNr - 1][WidthSquaresNr - 1].x + map[HeightSquaresNr - 1][WidthSquaresNr - 1].l, map[HeightSquaresNr - 1][WidthSquaresNr - 1].y + map[HeightSquaresNr - 1][WidthSquaresNr - 1].l);
	SDL_RenderDrawLine(p, map[HeightSquaresNr - 1][0].x, map[HeightSquaresNr - 1][0].y + map[HeightSquaresNr - 1][0].l, map[HeightSquaresNr - 1][WidthSquaresNr - 1].x + map[HeightSquaresNr - 1][WidthSquaresNr - 1].l, map[HeightSquaresNr - 1][WidthSquaresNr - 1].y + map[HeightSquaresNr - 1][WidthSquaresNr - 1].l);
}

bool square_det(int &x, int &y, Square** map,SDL_Window *p)
{
	if (x<map[0][0].x or y<map[0][0].y or x>(map[HeightSquaresNr - 1][WidthSquaresNr - 1].x + map[HeightSquaresNr - 1][WidthSquaresNr - 1].l) or y>(map[HeightSquaresNr - 1][WidthSquaresNr - 1].y + map[HeightSquaresNr - 1][WidthSquaresNr - 1].l))
		return false;
	int hscreen, wscreen, hfill,wfill;
	SDL_GetWindowSize(p, &wscreen, &hscreen);
	wfill =((double)wscreen - map[0][0].l*WidthSquaresNr) / 2;
	hfill =((double)hscreen - map[0][0].l*HeightSquaresNr) / 2;
	x = ((double)x-wfill) / map[0][0].l;
	y = ((double)y-hfill) / map[0][0].l;
	return true;
}

bool inside(int i, int j)
{
	if (i<0 or j<0 or i>HeightSquaresNr - 1 or j>WidthSquaresNr - 1)
		return false;
	return true;
}

bool check_vic(int **moves, int y, int x,int color)
{
	int i, j, m1=0,m2=0,m3=0,m4=0,m;
	if (color == -1)
	{	
		i = y - 1;
		j = x;
		while (inside(i, j) and moves[i][j] < 0)
		{
			m1--;
			i--;
		}
		i = y + 1;
		j = x;
		while (inside(i, j) and moves[i][j] < 0)
		{
			m1--;
			i++;
		}
		i = y;
		j = x - 1;
		while (inside(i, j) and moves[i][j] < 0)
		{
			m2--;
			j--;
		}
		i = y;
		j = x + 1;
		while (inside(i, j) and moves[i][j] < 0)
		{
			m2--;
			j++;
		}
		i = y - 1;
		j = x - 1;
		while (inside(i, j) and moves[i][j] < 0)
		{
			m3--;
			j--;
			i--;
		}
		i = y + 1;
		j = x + 1;
		while (inside(i, j) and moves[i][j] < 0)
		{
			m3--;
			j++;
			i++;
		}
		i = y + 1;
		j = x - 1;
		while (inside(i, j) and moves[i][j] < 0)
		{
			m4--;
			j--;
			i++;
		}
		i = y - 1;
		j = x + 1;
		while (inside(i, j) and moves[i][j] < 0)
		{
			m4--;
			j++;
			i--;
		}
		m = min(m1, m2);
		m = min(m, m3);
		m = min(m, m4);
		m--;
		moves[y][x] = -1;
		if (m == -5)
			return true;
		else return false;
	}
	else
	{
		i = y - 1;
		j = x;
		while (inside(i, j) and moves[i][j] > 0)
		{
			m1++;
			i--;
		}
		i = y + 1;
		j = x;
		while (inside(i, j) and moves[i][j] > 0)
		{
			m1++;
			i++;
		}
		i = y;
		j = x - 1;
		while (inside(i, j) and moves[i][j] > 0)
		{
			m2++;
			j--;
		}
		i = y;
		j = x + 1;
		while (inside(i, j) and moves[i][j] > 0)
		{
			m2++;
			j++;
		}
		i = y - 1;
		j = x - 1;
		while (inside(i, j) and moves[i][j] > 0)
		{
			m3++;
			j--;
			i--;
		}
		i = y + 1;
		j = x + 1;
		while (inside(i, j) and moves[i][j] > 0)
		{
			m3++;
			j++;
			i++;
		}
		i = y + 1;
		j = x - 1;
		while (inside(i, j) and moves[i][j] > 0)
		{
			m4++;
			j--;
			i++;
		}
		i = y - 1;
		j = x + 1;
		while (inside(i, j) and moves[i][j] > 0)
		{
			m4++;
			j++;
			i--;
		}
		m = max(m1, m2);
		m = max(m, m3);
		m = max(m, m4);
		m++;
		moves[y][x] = 1;
		if (m == 5)
			return true;
		else return false;
	}
}

void opening(SDL_Window *w, SDL_Renderer *&r, Button& swap, Button &swap2)
{	
	SDL_Color c = { 0,0,0,255 };
	SDL_SetRenderDrawColor(r, 51, 51, 153, 255);
	SDL_RenderClear(r);
	int hscreen, wscreen;
	SDL_GetWindowSize(w, &wscreen, &hscreen);
	Button Swap(wscreen / 2 - 200,hscreen/2 - 100, 200, 100), Swap2(wscreen / 2 + 100, hscreen/2 - 100 ,200, 100),intro(wscreen/2-300,100,wscreen/2,100);
	Swap.draw_text("fonts/OpenSans-Bold.ttf", "Swap", r, 24,c);
	Swap2.draw_text("fonts/OpenSans-Bold.ttf", "Swap2", r, 24,c);
	intro.draw_text("fonts/OpenSans-BoldItalic.ttf", "Opening Types", r, 38,c);
	swap = Swap;
	swap2 = Swap2;
}

void type1_choose(SDL_Window* w,SDL_Renderer *&r, Button &black, Button &white)
{	
	SDL_Color c = { 0,0,0,255 };
	int hscreen, wscreen;
	SDL_GetWindowSize(w, &wscreen, &hscreen);
	Button Black(wscreen/2-200, hscreen / 2 - 100, 150, 100);
	Button White(wscreen/2+50, hscreen / 2 - 100, 150, 100);
	Black.draw_text("fonts/OpenSans-Regular.ttf", "Black", r, 15,c);
	White.draw_text("fonts/OpenSans-Regular.ttf", "White", r, 15,c);
	black = Black;
	white = White;
}

void type2_choose(SDL_Window *w, SDL_Renderer *&r, Button &black, Button &white, Button &add2_stones)
{
	SDL_Color c = { 0,0,0,255 };
	int hscreen, wscreen;
	SDL_GetWindowSize(w, &wscreen, &hscreen);
	Button Black(wscreen / 2 - 400, hscreen / 2 - 100, 150, 100);
	Button White(wscreen / 2 - 200, hscreen / 2 - 100, 150, 100);
	Button Add2_stones(wscreen / 2, hscreen / 2 - 100, 400, 100);
	Black.draw_text("fonts/OpenSans-Regular.ttf", "Black", r, 15,c);
	White.draw_text("fonts/OpenSans-Regular.ttf", "White", r, 15,c);
	Add2_stones.draw_text("fonts/OpenSans-Regular.ttf", "Add 2 more stones(1 black and 1 white).", r, 15,c);
	black = Black;
	white = White;
	add2_stones = Add2_stones;
}

void show_info(int player,SDL_Renderer *&gRenderer,SDL_Window *gWindow,SDL_Color first,SDL_Color second,SDL_Color cmap,Square info,Button infotext)
{
	if (player == 1)
	{
		info.draw_circle(gRenderer, first.r, first.g, first.b, first.a);
		infotext.draw_text("fonts/OpenSans-Regular.ttf", "Player 1", gRenderer, 10,cmap);
		SDL_RenderPresent(gRenderer);
	}
	else
	{
		info.draw_circle(gRenderer, second.r, second.g, second.b, second.a);
		infotext.draw_text("fonts/OpenSans-Regular.ttf", "Player 2", gRenderer, 10,cmap);
		SDL_RenderPresent(gRenderer);
	}
}

bool check_draw(int **move)
{
	int i, j;
	bool r = true;
	for (i = 0; i < HeightSquaresNr; i++)
		for (j = 0; j < WidthSquaresNr; j++)
			if (move[i][j] == 0)
				r = false;
	return r;
}

void result(SDL_Renderer *&rend,int r)
{
	SDL_Surface *s=NULL;
	if (r < 0)
		s = SDL_LoadBMP("img/2.bmp");
	else
	{
		if (r == 0)
			s = SDL_LoadBMP("img/d.bmp");
		else
			s = SDL_LoadBMP("img/1.bmp");
	}
	SDL_Texture *t = SDL_CreateTextureFromSurface(rend, s);
	SDL_RenderCopy(rend, t, NULL, NULL);
	SDL_RenderPresent(rend);
	SDL_Delay(1000);
}

int main(int argc, char *argv[])
{	
	Button Swap, Swap2;
	SDL_Event e;
	SDL_Window *gWindow;
	SDL_Renderer* gRenderer;
	if (init(gWindow,gRenderer)==false)
		return 0;
	opening(gWindow, gRenderer, Swap, Swap2);
	SDL_RenderPresent(gRenderer);
	//harta de joc
	Square **map;
	map = new Square*[HeightSquaresNr];
	for (int i = 0; i < HeightSquaresNr; i++)
		map[i] = new Square[WidthSquaresNr];
	int **moves;
	moves = new int*[HeightSquaresNr];
	for (int i = 0; i < HeightSquaresNr; i++)
		moves[i] = new int[WidthSquaresNr];
	for (int i = 0; i < HeightSquaresNr; i++)
		for (int j = 0; j < WidthSquaresNr; j++)
			moves[i][j] = 0;
	int choice = 0,choosen=0;
	while (!choice)
	{
		while (SDL_PollEvent(&e))
		{	
			int aux = Swap.event_handler(e);
			if (aux == 1)
				choice = 1;
			else
				if (aux == -1)
					choice = -1;
			aux = Swap2.event_handler(e);
			if (aux == 1)
				choice = 2;
			else
				if (aux == -1)
					choice = -1;
		}
	}
	if (choice == -1)
	{
		close(gWindow, gRenderer);
		return 0;
	}
	draw_map(gWindow, gRenderer,map);
	SDL_RenderPresent(gRenderer);
	SDL_Color cmap = { 25,159,155,255 };
	Square info;
	int wscreen, hscreen;
	SDL_GetWindowSize(gWindow, &wscreen, &hscreen);
	info(wscreen - 275, 200, map[0][0].get_l());
	Button infotext(wscreen - 310, 50, 200, 100);
	bool quit = false;
	int number=0;
	SDL_Color first = { 0,0,0,0 }, second = { 0,0,0,0 };
	//opening
	SDL_Renderer *aux=NULL;
	SDL_Window *waux=NULL;
	SDL_Color *c = new SDL_Color[3];
	c[0] = { 0,0,0,255 };
	c[1] = { 0,0,0,255 };
	c[2] = { 255,255,255,255 };
	show_info(1,gRenderer,gWindow,c[0],c[1],cmap,info,infotext);
	while (!quit)
	{
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT or number >= 3)
				quit = true;
			else
				if (e.type == SDL_MOUSEBUTTONDOWN)
				{
					int x, y;
					SDL_GetMouseState(&x, &y);
					if (square_det(x, y, map, gWindow))
						if (!map[y][x].isPressed())
						{
							map[y][x].draw_circle(gRenderer, c[number].r, c[number].g, c[number].b, c[number].a);
							SDL_RenderPresent(gRenderer);
							if (c[number].r)
								moves[y][x] = 1;
							else
								moves[y][x] = -1;
							number++;
							show_info(1, gRenderer, gWindow, c[number], c[1], cmap, info, infotext);
						}
				}
		}
	}
	if (number < 3)
	{
		close(gWindow, gRenderer);
		return 0;
	}
	bool c2_retopt = false;
		if (choice == 1)
		{
				waux = SDL_CreateWindow("Choose your color (second player)", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 700, 300, SDL_WINDOW_UTILITY);
				aux = SDL_CreateRenderer(waux, -1, SDL_RENDERER_ACCELERATED);
				SDL_SetRenderDrawColor(aux, 255, 255, 255, 255);
				SDL_RenderClear(aux);
				Button white, black;
				type1_choose(waux, aux, black, white);
				SDL_RenderPresent(aux);
				quit = false;
				while (!quit)
				{
					while (SDL_PollEvent(&e))
					{
						if (black.event_handler(e) == 1)
						{
							quit = true;
							second = { 0,0,0,255 };
							first = { 255,255,255,255 };
						}
						else
							if (white.event_handler(e) == 1)
							{
								quit = true;
								first = { 0,0,0,255 };
								second = { 255,255,255,255 };
							}
					}
				}
				SDL_DestroyRenderer(aux);
				aux = NULL;
				SDL_DestroyWindow(waux);
				waux = NULL;
		}
		else
		{
			waux = SDL_CreateWindow("Choose your color or add 2 more stones and let the other player to choose colors", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 900, 300, SDL_WINDOW_UTILITY);
			aux = SDL_CreateRenderer(waux, -1, SDL_RENDERER_ACCELERATED);
			SDL_SetRenderDrawColor(aux, 255, 255, 255, 255);
			SDL_RenderClear(aux);
			Button white, black, add2_stones;
			type2_choose(waux, aux, black, white, add2_stones);
			SDL_RenderPresent(aux);
			quit = false;
			while (!quit)
			{
				while (SDL_PollEvent(&e))
				{
					if (black.event_handler(e) == 1)
					{
						quit = true;
						second = { 0,0,0,255 };
						first = { 255,255,255,255 };
					}
					else
					{
						if (white.event_handler(e) == 1)
						{
							quit = true;
							first = { 0,0,0,255 };
							second = { 255,255,255,255 };
						}
						else
							if (add2_stones.event_handler(e) == 1)
							{
								quit = true;
								first.r = 29;
							}
					}
				}
			}
			SDL_DestroyRenderer(aux);
			aux = NULL;
			SDL_DestroyWindow(waux);
			waux = NULL;
			if (first.r == 29)//jucatorul a ales sa adauge inca o bila neagra si una alba si sa lase celalalt jucator sa aleaga culorile
			{	
				c2_retopt = true;
				show_info(2, gRenderer, gWindow, c[0], c[1], cmap, info, infotext);
				number = 1;
				quit = false;
				while (!quit)
				{
					while (SDL_PollEvent(&e))
					{
						if (e.type == SDL_QUIT or number >= 3)
							quit = true;
						else
							if (e.type == SDL_MOUSEBUTTONDOWN)
							{
								int x, y;
								SDL_GetMouseState(&x, &y);
								if (square_det(x, y, map, gWindow))
									if (!map[y][x].isPressed())
									{
										map[y][x].draw_circle(gRenderer, c[number].r, c[number].g, c[number].b, c[number].a);
										SDL_RenderPresent(gRenderer);
										if (c[number].r)
											moves[y][x] = 1;
										else
											moves[y][x] = -1;
										number++;
										show_info(2, gRenderer, gWindow, c[1], c[number], cmap, info, infotext);
									}
							}
					}
				}
				if (number < 3)
				{
					close(gWindow, gRenderer);
					return 0;
				}
				waux = SDL_CreateWindow("Choose your color (first player)", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 700, 300, SDL_WINDOW_UTILITY);
				aux = SDL_CreateRenderer(waux, -1, SDL_RENDERER_ACCELERATED);
				SDL_SetRenderDrawColor(aux, 255, 255, 255, 255);
				SDL_RenderClear(aux);
				Button white, black;
				type1_choose(waux, aux, black, white);
				SDL_RenderPresent(aux);
				quit = false;
				while (!quit)
				{
					while (SDL_PollEvent(&e))
					{
						if (black.event_handler(e) == 1)
						{
							quit = true;
							first = { 0,0,0,255 };
							second = { 255,255,255,255 };
						}
						else
							if (white.event_handler(e) == 1)
							{
								quit = true;
								second = { 0,0,0,255 };
								first = { 255,255,255,255 };
							}
					}
				}
				SDL_DestroyRenderer(aux);
				aux = NULL;
				SDL_DestroyWindow(waux);
				waux = NULL;
			}
		}
	quit = false;
	int player;
	if (choice == 1)
	{
		if (second.r)
			player = 2;
		else
			player = 1;
	}
	else
	{
		if (c2_retopt or second.r==0)
			player = 1;
		else
			player = 2;
	}
	show_info(player, gRenderer, gWindow, first, second, cmap, info, infotext);
	//bucla principala a jocului
	int res=-2;
	SDL_Color st = { 0,100,175,255 };
	Button skip_turn(wscreen - 300, hscreen - 200, 100, 100);
	skip_turn.draw_text("fonts/OpenSans-Regular.ttf", "Skip turn" ,gRenderer, 15, st);
	Button sug_draw(wscreen - 175, hscreen - 200, 150, 100);
	sug_draw.draw_text("fonts/OpenSans-Regular.ttf", "Suggest Draw", gRenderer, 15, st);
	while (!quit)
	{	
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT)
				quit = true;
			else
			{	

				if (e.type == SDL_MOUSEBUTTONDOWN)
				{	
					int x, y;
					SDL_GetMouseState(&x, &y);
					if (square_det(x, y, map, gWindow))
					{	
						if (player == 1 and !map[y][x].isPressed())
						{	
							map[y][x].draw_circle(gRenderer, first.r, first.g, first.b, first.a);
							player = 2;
							SDL_RenderPresent(gRenderer);
							if (first.r)
							{
								if (check_vic(moves, y, x, 1))
								{	
									res = 1;
									quit = true;
								}
							}
							else
								if (check_vic(moves, y, x, -1))
								{
									res = 1;
									quit = true;
								}
							show_info(player, gRenderer, gWindow, first, second, cmap, info, infotext);
						}
						else
							if (player == 2 and !map[y][x].isPressed())
							{
								map[y][x].draw_circle(gRenderer, second.r, second.g, second.b, second.a);
								player = 1;
								SDL_RenderPresent(gRenderer);
								if (second.r)
								{
									if (check_vic(moves, y, x, 1))
									{
										res = -1;
										quit = true;
									}
								}
								else
									if (check_vic(moves, y, x, -1))
									{
										res = -1;
										quit = true;
									}
								show_info(player, gRenderer, gWindow, first, second, cmap, info, infotext);
							}
					}
					else
					{
						if (skip_turn.event_handler(e))
						{
							if (player == 1)
							{
								player = 2;
								show_info(player, gRenderer, gWindow, first, second, cmap, info, infotext);
							}
							else
							{
								player = 1;
								show_info(player, gRenderer, gWindow, first, second, cmap, info, infotext);
							}
						}
						else
							if (sug_draw.event_handler(e))
							{
								SDL_Color Black = { 0,0,0,255 };
								waux = SDL_CreateWindow("Suggest Draw", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 700, 300, SDL_WINDOW_UTILITY);
								aux = SDL_CreateRenderer(waux, -1, SDL_RENDERER_ACCELERATED);
								SDL_SetRenderDrawColor(aux, 255, 255, 255, 255);
								SDL_RenderClear(aux);
								Button ques(150, 50, 400, 100);
								ques.draw_text("fonts/OpenSans-Regular.ttf", "Do you accept the draw?", aux, 15, Black);
								Button Yes(250, 200, 100, 100), No(375, 200, 100, 100);
								Yes.draw_text("fonts/OpenSans-Regular.ttf", "Yes", aux, 15, Black);
								No.draw_text("fonts/OpenSans-Regular.ttf", "No", aux, 15, Black);
								SDL_RenderPresent(aux);
								bool quit2 = false;
								while (!quit2)
								{
									while (SDL_PollEvent(&e))
									{
										if (e.type == SDL_QUIT)
											quit2 = true; 
										else
											if (e.type == SDL_MOUSEBUTTONDOWN)
											{
												if (Yes.event_handler(e))
												{
													quit2 = true;
													quit = true;
													SDL_DestroyRenderer(aux);
													aux = NULL;
													SDL_DestroyWindow(waux);
													waux = NULL;
													res = 0;
												}
												else
													if (No.event_handler(e))
													{
														quit2 = true;
														SDL_DestroyRenderer(aux);
														aux = NULL;
														SDL_DestroyWindow(waux);
														waux = NULL;
													}
											}
									}
								}
							}
					}
					if (check_draw(moves))
					{
						quit = true;
						res = 0;
					}
				}
			}
		}
	}
	if(res!=-2)
		result(gRenderer, res);
	close(gWindow,gRenderer);
	return 0;
}