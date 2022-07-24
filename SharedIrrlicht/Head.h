#pragma once

#ifndef __HEAD_H_INCLUDED__
#define __HEAD_H_INCLUDED__

#include <irrlicht.h>

using namespace irr;

/*using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;*/

//using namespace std;

template <class T1, class T2>
struct Pair {
	T1 v1;
	T2 v2;
};

#ifdef _DEBUG  
#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h> 
#define DEBUG_CLIENTBLOCK new( _CLIENT_BLOCK, __FILE__, __LINE__)  

#include "CGUIDebugBoard.h"
extern gui::CGUIDebugBoard* DebugBoard;
#else  
#define DEBUG_CLIENTBLOCK  
#endif // _DEBUG  

#ifdef _DEBUG  
#define new DEBUG_CLIENTBLOCK  
#endif // _DEBUG  

#endif // !__HEAD_H_INCLUDED__



//_CrtDumpMemoryLeaks(); //メモリリークを確認、ダンプしたい箇所に挿入してください。