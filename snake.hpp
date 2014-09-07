
typedef struct Point{
  int x,y;
} point;

class Display {
public:
  int x0,y0,x1,y1;
  Display();
  void refresh();
  void erase();
  int blockGetch();
  void center(int height, const char *message,...);
  ~Display();
};

class Snake {
public:
  Display *display;
  int length,steps,dx,dy,rate;
  point head;
  std::deque <point> parts;
  Snake(Display *display);
  void move(int dx,int dy);
  bool isDead();
  void step();
  void draw();
};

class Apple{
public:
  Display *display;
  point coords;
  Apple(Display *display);
  void draw();
};

class Game{
public:
  Display *display;
  int score;
  Snake *snake;
  Apple *apple;
  Game (Display *display);
  ~Game();
  point spawnCoords();
  bool isExit(int c);
  void scoreCheck();
  int run();
};
