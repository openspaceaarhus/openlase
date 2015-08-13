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

void initol() {
  // from examples/simple.c
  OLRenderParams params;
  
  memset(&params, 0, sizeof params);
  params.rate = 48000;
  params.on_speed = 2.0/100.0;
  params.off_speed = 2.0/20.0;
  params.start_wait = 20;
  params.start_dwell = 3;
  params.curve_dwell = 0;
  params.corner_dwell = 8;
  params.curve_angle = cosf(30.0*(M_PI/180.0)); // 30 deg
  params.end_dwell = 3;
  params.end_wait = 7;
  params.snap = 1/100000.0;
  params.render_flags = RENDER_GRAYSCALE;
  
  if(olInit(3, 3000) < 0)
    throw "hest";
  
  olSetRenderParams(&params);
}

int main(int argc, char *argv[]) {

  initol();
  auto w =  1.0f;
  if (argc > 1) {
    w = std::atof(argv[1]);
  }

  cout << "w = " << w;

  float time = 0;
  float ftime;

  int frames = 0;

  while(1) {
    olLoadIdentity();

    auto step = 2.0*w / 7.0f;
    
    for(int i=0; i < 7; i++) {
      auto r = i & 1 ? 255 : 0;
      auto g = i>>1 & 1 ? 255 : 0;
      auto b = i>>2 & 1 ? 255 : 0;
      auto c = (r << 16) | (g<<8) | b;

      auto low = -w + i*step;
      auto high = -w + (i+1)*step;
      olRect(low,low, high,high,c);
      if (i==0) {
	olRect(low+.1,low+.1, high-.1,high-.1,C_RED);
      }
    }

    olRect(0		, -w, 1.0 * step , -w + step, C_RED);
    olRect(step		, -w, 2.0 * step , -w + step, C_GREEN);
    olRect(2.0 * step	, -w, 3.0 * step , -w + step, C_BLUE);

    ftime = olRenderFrame(60);
    frames++;
    time += ftime;

    printf("Frame time: %f, FPS:%f w=%f\n", ftime, frames/time, w);

  }
    
  return 0;
}

