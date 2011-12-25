#cmake_minimum_required(VERSION 2.6)

add_definitions( -DIS_CMAKE="true")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR}/src/)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set (QKSW_SRC 
  src/qkeysequencewidget.cpp)

set (QKSW_HDR 
  src/qkeysequencewidget.h
  src/qkeysequencewidget_p.h)

set (QKSW_QRC 
  ${CMAKE_CURRENT_SOURCE_DIR}/qkeysequencewidget.qrc)
qt4_add_resources(QKSW_QRC ${QKSW_QRC})

qt4_wrap_cpp(QKSW_SRC_MOC ${QKSW_HDR})

add_library(qkeysequencewidget ${QKSW_SRC} ${QKSW_SRC_MOC}  ${QKSW_QRC})
target_link_libraries(qkeysequencewidget ${QT_LIBRARIES})
