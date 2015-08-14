#include "laser.h"

#include <libol.h>

#include <iostream>
#include <string>

#define NANOSVG_ALL_COLOR_KEYWORDS  // Include full list of color keywords.
#define NANOSVG_IMPLEMENTATION  // Expands implementation
#include "nanosvg.h"

using namespace std;

int main(int argc, char *argv[]) {
  std::string filename{"hest.svg"};
  if (argc > 1) {
    filename = argv[1];
  }

  struct NSVGimage* image;
  image = nsvgParseFromFile(filename.c_str(), "px", 96);
  printf("size: %f x %f\n", image->width, image->height);
  // Use...
  for (auto shape = image->shapes; shape != NULL; shape = shape->next) {
    for (auto path = shape->paths; path != NULL; path = path->next) {
      for (auto i = 0; i < path->npts-1; i += 3) {
	float* p = &path->pts[i*2];
	//	drawCubicBez(p[0],p[1], p[2],p[3], p[4],p[5], p[6],p[7]);
	cout << p[0] << ", " << p[1] << ", " <<  p[2] << ", " << p[3] << ", " <<  p[4] << ", " << p[5] << ", " <<  p[6] << ", " <<  p[7] << ", " << endl;
      }
    }
  }
  // Delete
  nsvgDelete(image);
  
  return 0;
  
  laser::initol();
  float time = 0;
  float ftime;

  int frames = 0;

  while(1) {
    olLoadIdentity();


    ftime = olRenderFrame(60);
    frames++;
    time += ftime;

    printf("Frame time: %f, FPS:%f \n", ftime, frames/time);

  }
    
  return 0;
}

