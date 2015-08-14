#include "laser.h"
#include <libol.h>
#include <string.h>
#include <cmath>

namespace laser {

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
  
    if(olInit(3, 30000) < 0)
      throw "hest";
  
    olSetRenderParams(&params);
  }

}
