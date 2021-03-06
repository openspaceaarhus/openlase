#include <libol.h>
#include "laser.h"

#include <iostream>

using namespace std;

int main(int argc, char *argv[]) {

  laser::initol();
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

