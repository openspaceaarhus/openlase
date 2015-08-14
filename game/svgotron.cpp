#include "laser.h"

#include <libol.h>

#include <iostream>
#include <string>
#define GLM_FORCE_RADIANS
#include <glm/vec2.hpp>
#include <glm/glm.hpp>
#include <glm/ext.hpp> // << friends



#define NANOSVG_ALL_COLOR_KEYWORDS  // Include full list of color keywords.
#define NANOSVG_IMPLEMENTATION  // Expands implementation
#include "nanosvg.h"

using namespace std;
using namespace glm;


vec2 aabbMin { 424242	, 424242};
vec2 aabbMax {-424242	, -424242};


void includePt(vec2 p) {
  aabbMin.x = (p.x < aabbMin.x) ? p.x : aabbMin.x;
  aabbMin.y = (p.y < aabbMin.y) ? p.y : aabbMin.y;
  aabbMax.x = (p.x > aabbMax.x) ? p.x : aabbMax.x;
  aabbMax.y = (p.y > aabbMax.y) ? p.y : aabbMax.y;
}

int main(int argc, char *argv[]) {
  std::string filename{"hest.svg"};
  if (argc > 1) {
    filename = argv[1];
  }

  
  struct NSVGimage* image;
  image = nsvgParseFromFile(filename.c_str(), "px", 96);
  printf("size: %f x %f\n", image->width, image->height);
  // determine bounds
  for (auto shape = image->shapes; shape != NULL; shape = shape->next) {
    for (auto path = shape->paths; path != NULL; path = path->next) {
      for (auto i = 0; i < path->npts-1; i += 3) {
	float* p = &path->pts[i*2];
	includePt(vec2 {p[0],p[1]} );
	includePt(vec2 {p[2],p[3]} );
	includePt(vec2 {p[4],p[5]} );
      }
    }
  }
  
  cout << "AABB for svg: " << to_string(aabbMin) << " -> " << to_string(aabbMax) << endl;

  // map to [-1,-1], [1,1]
  
  for (auto shape = image->shapes; shape != NULL; shape = shape->next) {
    for (auto path = shape->paths; path != NULL; path = path->next) {
      for (auto i = 0; i < path->npts-1; i += 3) {
	float* p = &path->pts[i*2];
	//(vec2 {p[0],p[1]} );
	//(vec2 {p[2],p[3]} );
	//(vec2 {p[4],p[5]} );
      }
    }
  }

  
  laser::initol();
  float time = 0;
  float ftime;

  int frames = 0;

  while(1) {
    olLoadIdentity();


  for (auto shape = image->shapes; shape != NULL; shape = shape->next) {
    olBegin(OL_BEZIERSTRIP);
    auto paint = shape->stroke;
    uint32_t color = (paint.type == NSVG_PAINT_COLOR) ? paint.color : C_WHITE;

    for (auto path = shape->paths; path != NULL; path = path->next) {
      
      for (auto i = 0; i < path->npts-1; i += 3) {
	float* p = &path->pts[i*2];
	olVertex(p[0],p[1], color);
	olVertex(p[2],p[3], color);
	olVertex(p[4],p[5], color);
      }
    }
    olEnd();
  }

    
    ftime = olRenderFrame(60);
    frames++;
    time += ftime;

    printf("Frame time: %f, FPS:%f \n", ftime, frames/time);

  }

    // Delete
  nsvgDelete(image);

  
  return 0;
}

