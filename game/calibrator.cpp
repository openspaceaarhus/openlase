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
  params.start_wait = 8;
  params.start_dwell = 3;
  params.curve_dwell = 0;
  params.corner_dwell = 8;
  params.curve_angle = cosf(30.0*(M_PI/180.0)); // 30 deg
  params.end_dwell = 3;
  params.end_wait = 7;
  params.snap = 1/100000.0;
  params.render_flags = RENDER_GRAYSCALE;
  
  if(olInit(3, 30000) < 0)
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

  float fps = 10.0;
  float time = 0;
  float ftime;
  int i;

  int frames = 0;

  int white = 0xFFFFFF;
  while(1) {
    olLoadIdentity();

    olBegin(OL_LINESTRIP);
    olVertex(0, 0, white);
    olVertex(w, w, white);
    olEnd();
    
    ftime = olRenderFrame(60);
    frames++;
    time += ftime;

    printf("Frame time: %f, FPS:%f w=%f\n", ftime, frames/time, w);
    w *= 1.001;

  }
    
  return 0;
}

