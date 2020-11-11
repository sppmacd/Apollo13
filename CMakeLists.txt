# EGE Link Test

cmake_minimum_required(VERSION 3.0)
project(Apollo13)

set(EGE_LIB_ROOT "${CMAKE_SOURCE_DIR}/../EGE")
set(CMAKE_INSTALL_PREFIX root)
include(${EGE_LIB_ROOT}/cmake/FindEGE.cmake)
ege_executable(Apollo13 src "ege-egeNetwork;ege-network;ege-controller;ege-scene;ege-geometry;ege-game;ege-gpo;ege-gui;ege-asyncLoop;ege-resources;ege-event;ege-gfx;ege-tilemap;ege-sfml;ege-loop;ege-debug;ege-util;ege-main")
ege_resources(res)

