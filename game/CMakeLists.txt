
FIND_PACKAGE(SDL2 REQUIRED)
INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIR})

INCLUDE_DIRECTORIES (${CMAKE_SOURCE_DIR}/include)
LINK_DIRECTORIES (${CMAKE_BINARY_DIR}/libol)

MACRO(ADDLASE lasename)
  ADD_EXECUTABLE(${lasename} laser.cpp ${ARGN})
  TARGET_LINK_LIBRARIES(${lasename} ${JACK_LIBRARIES} m openlase ${SDL2_LIBRARY})
ENDMACRO()


ADDLASE(gameosaa
  lase.cpp)

ADDLASE(calibrator 
  calibrator.cpp)

ADDLASE(svgotron
  svgotron.cpp)