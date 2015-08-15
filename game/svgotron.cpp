#include "laser.h"

#include <libol.h>

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#define GLM_FORCE_RADIANS
#include <glm/vec2.hpp>
#include <glm/glm.hpp>
#include <glm/ext.hpp> // << friends



#define NANOSVG_ALL_COLOR_KEYWORDS  // Include full list of color keywords.
#define NANOSVG_IMPLEMENTATION  // Expands implementation
#include "nanosvg.h"

using namespace std;
using namespace glm;

string prefix; /// the prefix appended to all files ie /where/my/svgs/resides
vec2 aabbMin { 424242	, 424242};
vec2 aabbMax {-424242	, -424242};

void resetAABB() {
  aabbMin.x =  aabbMin.y =  424242;
  aabbMax.x =  aabbMax.y = -424242;
}

struct NSVGimage* image;

struct Playlist {
  vector<int> times;
  vector<string> files;
  void clear() {
    times.clear();
    files.clear();
    idx = 0;
  }
  int currentTime() const {
    return times[idx];
  }
  int total_time;
  size_t idx;
};
Playlist playlist;

void includePt(vec2 p) {
  aabbMin.x = (p.x < aabbMin.x) ? p.x : aabbMin.x;
  aabbMin.y = (p.y < aabbMin.y) ? p.y : aabbMin.y;
  aabbMax.x = (p.x > aabbMax.x) ? p.x : aabbMax.x;
  aabbMax.y = (p.y > aabbMax.y) ? p.y : aabbMax.y;
}

void measureAABB() {
  aabbMin = vec2 { 424242	, 424242};
  aabbMax = vec2 {-424242	, -424242};

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
}

void scaleAndOffSet() {
  // map to [-1,-1], [1,1]
  auto width = aabbMax.x - aabbMin.x;
  auto height = aabbMax.y - aabbMin.y;
  auto scale = 1.0f / (width > height ? width : height);
  for (auto shape = image->shapes; shape != NULL; shape = shape->next) {
    for (auto path = shape->paths; path != NULL; path = path->next) {
      for (auto i = 0; i < path->npts-1; i += 3) {
	float* p = &path->pts[i*2];
	// first [0,1]
	p[0] = p[0] * scale - aabbMin.x * scale;
	p[2] = p[2] * scale - aabbMin.x * scale;
	p[4] = p[4] * scale - aabbMin.x * scale;
	p[1] = p[1] * scale - aabbMin.y * scale;
	p[3] = p[3] * scale - aabbMin.y * scale;
	p[5] = p[5] * scale - aabbMin.y * scale;
	// the expand to [1-,1]
	p[0] = p[0]*2.0 - 1.0;
	p[2] = p[2]*2.0 - 1.0;
	p[4] = p[4]*2.0 - 1.0;
	p[1] = p[1]*2.0 - 1.0;
	p[3] = p[3]*2.0 - 1.0;
	p[5] = p[5]*2.0 - 1.0;
	// invert y
	p[1] *= -1.0f;
	p[3] *= -1.0f;
	p[5] *= -1.0f;
      }
    }
  }
}

void loadFile(std::string filename) {
  cout << "loading: " << filename << endl;
  if (image)
    nsvgDelete(image);
  image = nsvgParseFromFile(filename.c_str(), "px", 96);
  assert(image);
  printf("size: %f x %f\n", image->width, image->height);
  measureAABB();
  scaleAndOffSet();
  // measureAABB();
}

void loadNextFile() {
   if (playlist.idx >= playlist.files.size())
     playlist.idx = 0;
   loadFile(playlist.files[playlist.idx++]);
}


void loadPlaylist(std::string filename) {
  playlist.clear();
  int time;
  string file;
  ifstream ifs(filename);
  while(ifs >> time >> file) {
    playlist.times.push_back(time);
    playlist.files.push_back(prefix + file);
    cout << "Adding : " << file << " to play for " <<  time << endl;
  }
  loadNextFile();
}

int main(int argc, char *argv[]) {
    std::string filename{"hest.svg"};
    image = nullptr;
    if (argc !=  3) {
      cout << "usage ./" << argv[0] << " prefix playlist" << endl;
      return EXIT_FAILURE;
    }
    prefix = argv[1];
    loadPlaylist(argv[2]);
    laser::initol();
    float time = 0; 			// how long the animation has been playing for
    float ftime;
    int frames = 0;
    while(1) {
      olLoadIdentity();
      for (auto shape = image->shapes; shape != NULL; shape = shape->next) {
	auto paint = shape->stroke;
	uint32_t color = (paint.type == NSVG_PAINT_COLOR) ? paint.color : C_WHITE;

	for (auto path = shape->paths; path != NULL; path = path->next) {
	  if (true || "how do you determine if this is a line og bezier curve?") {
	    // cout << "draw my like one of your french girls" << endl;
	    olBegin(OL_LINESTRIP);
	    for (auto i = 0; i < path->npts-1; i += 3) {
	      float* p = &path->pts[i*2];
	      olVertex(p[0],p[1], color);
	    }
	    olEnd();
	  } else {
	    olBegin(OL_BEZIERSTRIP);
	    for (auto i = 0; i < path->npts-1; i += 3) {
	      float* p = &path->pts[i*2];
	      olVertex(p[0],p[1], color);
	      olVertex(p[2],p[3], color);
	      olVertex(p[4],p[5], color);
	    }
	    olEnd();
	  }
	}
      }
      ftime = olRenderFrame(60);
      frames++;
      time += ftime;
      if (1000 * time > playlist.currentTime()) {
	loadNextFile();
	time = 0;
      }
      fprintf(stderr, "Frame time: %f, FPS:%f TotalTime %f\n", ftime, frames/time, time);
    }
    // Delete
    nsvgDelete(image);
    return 0;
  }

