// globals.h
#ifndef GLOBALS_H
#define GLOBALS_H

#include <atomic>

// 声明全局变量 stop_program
extern std::atomic<bool> stop_program;
extern std::atomic<bool> stop_joysProgram;
#endif
