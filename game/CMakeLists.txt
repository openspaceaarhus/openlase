
FIND_PACKAGE(ZeroMQPP)

FIND_PACKAGE(SDL2 REQUIRED)
INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIR})

INCLUDE_DIRECTORIES (${CMAKE_SOURCE_DIR}/include ${ZeroMQPP_INCLUDE_DIRS})
LINK_DIRECTORIES (${CMAKE_BINARY_DIR}/libol)

MACRO(ADDLASE lasename)
  ADD_EXECUTABLE(${lasename} laser.cpp ${ARGN})
  TARGET_LINK_LIBRARIES(${lasename} ${JACK_LIBRARIES} m openlase ${SDL2_LIBRARY} ${ZeroMQPP_LIBRARIES} zmq)
  TARGET_LINK_LIBRARIES(gameosaa )
ENDMACRO()


ADDLASE(gameosaa
  lase.cpp)

ADDLASE(calibrator 
  calibrator.cpp)

ADDLASE(svgotron
  svgotron.cpp)

ADDLASE(playClient 
playClient.cpp)

