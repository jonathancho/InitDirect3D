/******************************************************************************/
/*!
\file   Precompiled.h
\author Brian Blanchett
\par    email: brian.blanchett\@digipen.edu
\par    Copyright 2011, DigiPen Institute of Technology
*/
/******************************************************************************/
// no winsock1 will be included
#define _WINSOCKAPI_

//Prevent rarely used stuff from being included
#define WIN32_LEAN_AND_MEAN

#pragma warning(disable:4201) // nonstandard extension used : nameless struct/union
#pragma warning(disable:4505)
#include <Windows.h>

#include <mmsystem.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <ctime>
#include <cassert>

#include <fcntl.h> // _O_TEXT
#include <io.h> // _open_osfhandle

//Containers
//#include "IntrusiveList.h"
#include <string>
#include <list>
#include <vector>
#include <map>
#include <set>
#include <bitset>
#include <hash_map>
#include <sstream>

//Random Generator
//#include "RandomGenerator.h"

//algorithm
#include <algorithm>

//XInput
#include <Xinput.h>

//Direct3D
#include <dxerr.h>
#include <d3dx11.h>
//#include <d3d9.h>
//#include <d3dx9.h>

#define PI 3.141592654f
//#include "Vectors.h"
//#include "Matrix.h"
//#include "Matrix3.h"


//Macros
//#include "ErrorIf.h"

