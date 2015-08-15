
FIND_PACKAGE(ZeroMQPP)

FIND_PACKAGE(SDL2 REQUIRED)
INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIR})

INCLUDE_DIRECTORIES (${CMAKE_SOURCE_DIR}/include ${ZeroMQPP_INCLUDE_DIRS})
LINK_DIRECTORIES (${CMAKE_BINARY_DIR}/libol)

MACRO(ADDLASE lasename)
  ADD_EXECUTABLE(${lasename} laser.cpp ${ARGN})
  TARGET_LINK_LIBRARIES(${lasename} ${JACK_LIBRARIES} m openlase ${SDL2_LIBRARY})
ENDMACRO()


ADDLASE(calibrator 
  calibrator.cpp)

ADDLASE(svgotron
  svgotron.cpp)

IF(0) 
  ADDLASE(playClient 
    playClient.cpp)

  ADDLASE(gameosaa
    lase.cpp)

  TARGET_LINK_LIBRARIES(playClient ${ZeroMQPP_LIBRARIES} zmq)
  TARGET_LINK_LIBRARIES(gameosaa ${ZeroMQPP_LIBRARIES} zmq)
ENDIF()

