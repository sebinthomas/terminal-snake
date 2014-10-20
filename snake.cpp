#include <stdlib.h>
#include <thread>
#include <chrono>
#include <deque>
#include <cstring>
#include <stdarg.h>
#include <ncurses.h>
#include "snake.hpp"

Display::Display(){
  initscr();
  start_color();
  raw();
  noecho();
  keypad(stdscr, TRUE);
  timeout(0);
  curs_set(0);
  x1=3*(COLS)/4+COLS/8;
  y1=3*(LINES)/4+LINES/8;
  x0=COLS/8;
  y0=LINES/8;
}

Display::~Display() {
  endwin();
  fflush(stdout);
}

void Display::refresh() {
  ::refresh();
}

void Display::erase(){
  ::erase();
  init_pair(2, COLOR_BLACK, COLOR_WHITE);
  attron(COLOR_PAIR(2));
  mvvline(y0, x0, '#' ,y1-y0);
  mvvline(y0, x1, '#' ,y1-y0);
  mvhline(y0, x0, '#' ,x1-x0);
  mvhline(y1, x0, '#' ,x1-x0);
  mvaddch(y0, x0, '+');
  mvaddch(y1, x0, '+');
  mvaddch(y0, x1, '+');
  mvaddch(y1, x1, '+');
  attroff(COLOR_PAIR(2));
}

int Display::blockGetch() {
  refresh();
  timeout(-1);
  int c = getch();
  timeout(0);
  return c;
}

void Display::center(int height,const char *text,...){
  va_list ap;
  char message[60];
  va_start(ap,text);
  vsprintf(message, text, ap);
  va_end(ap);
  mvprintw(y0 + (y1-y0)/2 + height, x0 + (x1-x0)/2 - std::strlen(message)/2, "%s",message);
}

Snake::Snake(Display *display): display(display),length(12), steps(0), dx(1), dy(0), rate(5){
  head.x=display->x1/2;
  head.y=display->y1/2;
  parts.push_back(head);
}

void Snake::step() {
  //Each part of the snake's body is pushed and popped off a deque
  steps++;
  while(length-parts.size() > 0){
    parts.push_back(parts.back());
  }
  if(steps%rate == 0){
    head.x+=dx;
    head.y+=dy;
    parts.push_front(head);
    parts.pop_back();
   
  }
}

void Snake::move(int deltax,int deltay){
  if ( abs(deltax)==dx || abs(deltay)==dy){
    return ;
  }
  dx=deltax;
  dy=deltay;
}

bool Snake::isDead(){
  if (head.x < display->x0+1 || head.x > display->x1-1 || head.y < display->y0+1 || head.y > display->y1-1) {
    return true;
  }
for(int i=1; i<length; i++){
  if(head.x+ dx == parts[i].x && head.y+dy == parts[i].y ){
      return true;
    }
 }
  return false;
}

void Snake::draw() {
  init_pair(1, COLOR_GREEN, COLOR_GREEN);
  attron(COLOR_PAIR(1));
 for( int i=0; i< length; i++){
   mvaddch(parts[i].y, parts[i].x, '#');
 }
 attroff(COLOR_PAIR(1));
}

Apple::Apple(Display *display): display(display) {}

void Apple::draw() {
  init_pair(3, COLOR_RED, COLOR_BLACK);
  attron(COLOR_PAIR(3));
  mvaddch(coords.y, coords.x ,'@');
  attroff(COLOR_PAIR(3));
}

point Game::spawnCoords(){
  // Gives random coords for the apple
  point a;
  int rx=display->x1-display->x0-2, ry=display->y1-display->y0-2;
  a.x=rand()%rx + display->x0+1;
  a.y=rand()%ry + display->y0+1;
  return a;
}

Game::Game(Display *display): display(display), score(0){
  snake= new Snake(display);
  apple= new Apple(display);
  apple->coords=spawnCoords();
}

void Game::scoreCheck(){
  if(apple->coords.x == snake->head.x && apple->coords.y==snake->head.y ){
    snake->length +=2;
    score+=1;
    apple->coords=spawnCoords();
    snake->rate=(snake->rate <=2 )? 2: snake->rate -1;
  }
}

bool Game::isExit(int c) { return c == 'q' || c == ''; }

int Game::run(){
  const char *title="Terminal Snake",
    *instruction="Arrow keys to move. [q] to quit.",
    *start="Press any key to begin",
    *credits="By Sebin Thomas (http://sebin.in)",
    *scoreText="Score : ",
    *endText= "Game over";

  // init screen
  int cc;
  init_pair(4,COLOR_WHITE,COLOR_BLACK);
  attron(COLOR_PAIR(4));
  display->erase();
  attron(A_BOLD);
  display->center(-2, title);
  attroff(A_BOLD);
  display->center(0, instruction);
  display->center(2,start);
  display->center(4,credits);
  attroff(COLOR_PAIR(4));
  if (isExit(display->blockGetch())) return -1;

  //game loop
  while(!snake->isDead()) {
    cc= getch();
    if(isExit(cc)){
      return -1;
      }
    if(cc !=ERR){
      while(getch()!=ERR); // clear getch buffer
      switch(cc){
      case KEY_UP:
	snake->move(0,-1);
	break;
      case KEY_DOWN:
	snake->move(0,1);
	break;
      case KEY_LEFT:
	snake->move(-1,0);
	break;
      case KEY_RIGHT:
	snake->move(1,0);
	break;
      }
    }
    display->erase();
    snake->step();
    scoreCheck();
    snake->draw();
    apple->draw();
    
    // display score
    attron(COLOR_PAIR(4));
    mvprintw(display->y0-2,display->x1-std::strlen(scoreText)-4,"%s%d",scoreText,score);
    attroff(COLOR_PAIR(4));
    
    // Refreshes the screen and adds a delay
    display->refresh();
    std::this_thread::sleep_for(std::chrono::milliseconds{40});
  }
  // End Screen
  display->erase();
  attron(A_BOLD);
  display->center(-1,endText);
  attroff(A_BOLD);
  display->center(1,"%s %d",scoreText,score);
  display->center(3,"Press any key to quit");
  display->blockGetch();
  return 0;
}

Game::~Game(){
  delete(snake);
  delete(apple);
  delete(display);
}

int main(){
  Display* display= new Display();
  Game game(display);
  game.run();
  return 0;
}
