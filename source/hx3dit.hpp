#ifndef HX_HX3DIT_H
#define HX_HX3DIT_H

#include <atomic>
#include <cmath>
#include <fcntl.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/times.h>
#include <sys/types.h>
#include <unordered_map>
#include <unistd.h>
#include <termios.h>
#include <thread>
#include <vector>

// ----------------------------------------------------------------------------  
#pragma region DEBUGGING
#ifdef _DEBUG
#   define HX_DBG_PRT_B(x) std::cout << "[DBG]: " << x 
#   define HX_DBG_PRT_I(x) std::cout << x 
#   define HX_DBG_PRT_E(x) std::cout << x << std::endl; 
#   define HX_DBG_PRT_N(x) std::cout << "[DBG]: " << x << std::endl
#   define HX_DBG_PRT_FE(arr, range)                                              \
    for (int i = 0; i < range; ++i) std::cout << "[DBG]: " << arr[i] << std::endl;
#   define HX_STR(x) std::to_string(x)
#else
#   define HX_DBG_PRT_B(x) 
#   define HX_DBG_PRT_I(x) 
#   define HX_DBG_PRT_E(x) 
#   define HX_DBG_PRT_N(x) 
#   define HX_DBG_PRT_FE(arr, range)                                     
#   define HX_STR(x) 
#endif // _DEBUG
#pragma endregion

namespace HX3
{   
#pragma region predeclarations
    class UserControls;
    namespace UI { class UiElement; }
    class Output;
#pragma endregion

// ---------------------------------------------------------------------------- 
    constexpr int AsciiZero = 48;
    constexpr int AsciiNine = 57;

// ---------------------------------------------------------------------------- 
    typedef uint8_t Byte;
    typedef uint8_t TerminalCell;
    typedef uint8_t KeyboardKey;

// ---------------------------------------------------------------------------- 
    template<typename T> struct Vec2 { T x; T y; };
    template<typename T> struct Vec3 { T x; T y; T z; };
    template<typename T> struct Vec4 { T x; T y; T z; T w; };

#ifdef _DEBUG
    constexpr int32_t InputTimeout = 2000000;
#else
    constexpr int32_t InputTimeout = 10000;
#endif // _DEBUG
}

#endif // !HX_HX3DIT_H
