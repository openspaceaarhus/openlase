#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#define GLM_FORCE_RADIANS
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/glm.hpp>
#include <glm/ext.hpp> // << friends

#include <random>
#include <sstream>

#include <SDL2/SDL.h>

#include <libol.h>


using namespace std;
using namespace glm;

class GameObject {
public:
  virtual void tick(int dt) {};
  virtual void render(SDL_Renderer* renderer) const {
    auto pts = vector<SDL_Point>();
    for(auto p : vertices) 
      pts.emplace_back(SDL_Point { static_cast<int>(p.x), static_cast<int>(p.y)});
    SDL_SetRenderDrawColor(renderer, col.r, col.g, col.b, col.a);
    SDL_RenderDrawLines(renderer, pts.data(), pts.size());
    int color = (col.b) + (col.g<<8) + (col.r<<16);

    olBegin(OL_LINESTRIP);
    for(auto p : vertices) 
      olVertex(p.x, p.y, color);
    olEnd();
  }
  vector<vec2> vertices;
  SDL_Color col { 255, 255, 255, 255 };
  void applyDelta(vec2 delta) {
    for_each(begin(vertices), end(vertices), [delta] (vec2& o) { o += delta; });
  }
};

class Player : public GameObject  {
public:
  Player() {
    col.g = col.b = 0;
    t = 0.0f;
  }

  virtual void tick(int ms) {
    vertices.clear();
    vertices.emplace_back(vec2 {400,400} + vec2 {cos(t-.02), sin(t-.02)} * 320.0f);
    vertices.emplace_back(vec2 {400,400} + vec2 {cos(t), sin(t)} * 310.0f);
    vertices.emplace_back(vec2 {400,400} + vec2 {cos(t+.02), sin(t+.02)} * 320.0f);
    vertices.emplace_back(vec2 {400,400} + vec2 {cos(t-.02), sin(t-.02)} * 320.0f);
  }

  void rightPressed() {
    t += .05;
  }

  void leftPressed() {
    t -= .05;
  }

  float t = 0;
};

class Block : public GameObject {
public:
  Block(vec2 origo)
    : origo(origo) {
    // an interesting defaultish block
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> tdist { 0, 6.28 };
    static std::uniform_real_distribution<> dtdist { 0.3, 2.0 };
    static std::uniform_real_distribution<> daDist { 0.0, 0.001 };
    static std::uniform_real_distribution<> coin { 0, 1.0 };

    t0 = tdist(gen);
    dt = dtdist(gen);
    r1 = 20;
    w = 20;
    da = daDist(gen) * ((coin(gen)  > .5) ? 1.0 : -1.0);
  }
  Block(vec2 origo, double t0, float dt, float r1, float w) 
    : origo(origo), t0(t0), dt(dt), w(w), r1(r1), da(0.0001) {
  }

  virtual void tick(int ms) {
    vertices.clear();
    const float stepSize = 1.13f;
    auto d0 = vec2 {cos(t0), sin(t0)};
    auto d1 = vec2 {cos(t0 + dt), sin(t0 + dt)};
    vertices.emplace_back(origo + r1* d0);
    vertices.emplace_back(origo + (r1 + w)	* d0);

    for(float t = 0.0f; t < dt; t += stepSize) {
      auto dir = vec2 {cos(t0 + t), sin(t0 + t)};
      vertices.emplace_back(origo + (r1 + w) * dir);
    }

    vertices.emplace_back(origo + (r1 + w)	* d1);
    vertices.emplace_back(origo + r1	* d1);
    for(float t = dt; t > 0; t -= stepSize) {
      auto dir = vec2 {cos(t0 + t), sin(t0 + t)};
      vertices.emplace_back(origo + r1  * dir);
    }

    vertices.emplace_back(origo + r1	* d0);

    t0 += ms * da;
    r1 += ms/100.0f;
  }
  float r1;  // inner radius
private:
  vec2 origo; /// emerge point
  float t0; /// start angle
  float dt; /// delta angle
  float w; /// width of block
  float da; //angular speed

};


Player p1;
SDL_Renderer *renderer   = nullptr;
SDL_Window   *win        = nullptr;
void startDisplay(int w, int h) { 
  if (0 != SDL_Init(SDL_INIT_EVERYTHING) ) exit(42);
  SDL_CreateWindowAndRenderer(w, h, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL, &win, &renderer);
  if(nullptr == win || nullptr == renderer) exit(42);
}

int initol() {
  // from examples/simple.c
  OLRenderParams params;
  
  memset(&params, 0, sizeof params);
  params.rate = 48000;
  params.on_speed = 2.0/100.0;
  params.off_speed = 2.0/20.0;
  params.start_wait = 8;
  params.start_dwell = 3;
  params.curve_dwell = 0;
  params.corner_dwell = 8;
  params.curve_angle = cosf(30.0*(M_PI/180.0)); // 30 deg
  params.end_dwell = 3;
  params.end_wait = 7;
  params.snap = 1/100000.0;
  params.render_flags = RENDER_GRAYSCALE;
  
  if(olInit(3, 130000) < 0)
    return 1;
  
  olSetRenderParams(&params);
  return 0;
}

int main(int argc, char *argv[]) {

  vector<Block> blocks;

  auto b = Block {vec2{400,400} };
  blocks.push_back( b);


  startDisplay(800, 800);
  auto retval = initol();
  if (retval < 0) {
    cerr << "Something happend" << endl;
    return retval;
  }

  SDL_Event e;
  float fps = 10.0;
  auto start_time = SDL_GetTicks();
  float time = 0;
  float ftime;
  int i;

  int frames = 0;

  int frame = 0;
  while(1) {
    frame++;

    olLoadIdentity();

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // check input
    while (SDL_PollEvent(&e)){
      auto key = e.key.keysym.sym;
      if (e.type == SDL_QUIT || SDLK_q == key) exit(0);
	    
      if (SDLK_RIGHT == key) {
	p1.rightPressed();
      } else if (SDLK_LEFT == key) {
	p1.leftPressed();
      }
    }

    // display
    p1.render(renderer);
    for_each(begin(blocks), end(blocks), [] (const Block& b) { b.render(renderer);});
	
    auto end_time = SDL_GetTicks();
    auto dt = end_time - start_time;
    // // limit FPS
    // while (dt < 30) {
    //   SDL_Delay(10);
    //   end_time = SDL_GetTicks();
    //   dt = end_time - start_time;
    // }

    for_each(begin(blocks), end(blocks), [dt] (Block& b) { b.tick(dt);});
    p1.tick(dt);

    if (blocks.front().r1 > 400) {
      /// far out man
      blocks.erase(blocks.begin());
    }
    if (blocks.back().r1 > 53) {
      // spawn new block
      blocks.emplace_back(Block {vec2{400,400} });
    }

    fps = fps * .9 + .1 * (1000.0 / dt);
    start_time = end_time;


    ftime = olRenderFrame(60);
    frames++;
    time += ftime;
    printf("Frame time: %f, FPS:%f\n", ftime, frames/time);


    char buf[512];
    sprintf(buf, "%s (%3d ms, %.2f fps)",
	    argv[0], dt, fps);

    SDL_SetWindowTitle(win, buf);
    SDL_RenderPresent(renderer);

  }
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(win);
  SDL_Quit();
    
  return 0;
}

