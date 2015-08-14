#include "laser.h"

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

#include <libol.h>


using namespace std;
using namespace glm;

class GameObject {
public:
  virtual void tick(int dt) {};
  virtual void render() const {
    int color = (col.b) + (col.g<<8) + (col.r<<16);
    olBegin(OL_LINESTRIP);
    for(auto p : vertices)  {
      olVertex(p.x, p.y, color);
      cout << p.x << ", " << p.y << endl;
    }
    olEnd();
  }
  ivec3 col;
  vector<vec2> vertices;
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
    vertices.emplace_back(vec2 {cos(t-.02), sin(t-.02)});
    vertices.emplace_back(vec2 {cos(t), sin(t)});
    vertices.emplace_back(vec2 {cos(t+.02), sin(t+.02)});
    vertices.emplace_back(vec2 {cos(t-.02), sin(t-.02)});
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
    r1 = 0.2;
    w = 0.2;
    da = daDist(gen) * ((coin(gen)  > .5) ? 1.0 : -1.0);
  }
  Block(vec2 origo, double t0, float dt, float r1, float w) 
    : origo(origo), t0(t0), dt(dt), w(w), r1(r1), da(0.0001) {
  }

  virtual void tick(int ms) {
    vertices.clear();
    const float stepSize = .13f;
    auto d0 = vec2 {cos(t0), sin(t0)};
    auto d1 = vec2 {cos(t0 + dt), sin(t0 + dt)};
    vertices.emplace_back(origo + r1 * d0);
    vertices.emplace_back(origo + (r1 + w)	* d0);

    vertices.emplace_back(origo + (r1 + w)	* d0);

    vertices.emplace_back(origo + r1	* d0);

    t0 += ms * da;
    r1 += ms;
  }
  float r1;  // inner radius
private:
  vec2 origo; /// emerge point
  float t0; /// start angle
  float dt; /// delta angle
  float da; //angular speed
  float w; /// width of block

};


Player p1;


int main(int argc, char *argv[]) {

  vector<Block> blocks;

  auto b = Block {vec2{0,0} };
  blocks.push_back( b);


  laser::initol();
  float time = 0;
  float ftime;
  int frames = 0;

  int frame = 0;
  while(1) {
    frame++;

    olLoadIdentity();
    // // check input
    // while (SDL_PollEvent(&e)){
    //   auto key = e.key.keysym.sym;
    //   if (e.type == SDL_QUIT || SDLK_q == key) exit(0);
	    
    //   if (SDLK_RIGHT == key) {
    // 	p1.rightPressed();
    //   } else if (SDLK_LEFT == key) {
    // 	p1.leftPressed();
    //   }
    // }

    // display
    p1.render();
    for_each(begin(blocks), end(blocks), [] (const Block& b) { b.render();});
    // // limit FPS
    // while (dt < 30) {
    //   SDL_Delay(10);
    //   end_time = SDL_GetTicks();
    //   dt = end_time - start_time;
    // }


    if (blocks.front().r1 > 400) {
      /// far out man
      blocks.erase(blocks.begin());
    }
    if (blocks.back().r1 > 1.0) {
      // spawn new block
      blocks.emplace_back(Block {vec2{0,0} });
    }

    ftime = olRenderFrame(60);
    frames++;
    time += ftime;
    for_each(begin(blocks), end(blocks), [ftime] (Block& b) { b.tick(ftime);});
    p1.tick(ftime);

    printf("Frame time: %f, FPS:%f\n", ftime, frames/time);

  }
    
  return 0;
}

