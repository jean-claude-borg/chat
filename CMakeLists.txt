cmake_minimum_required(VERSION 3.21)
project(chat)

set(CMAKE_CXX_STANDARD 23)

add_executable(chat server.cpp client.cpp includes/sock.h userCreation.c userCreation.h)
