#pragma once

#ifndef _MAIN_H
#define _MAIN_H

#include <fstream>
#include <iostream>
#include <sstream>
#include <list>
#include <windows.h>
#include <mmsystem.h>
#include <direct.h>
#include <stdio.h>
#include <string>

using namespace std;

class HTTPCommModule
{
public :
	string startHTTPComm(char* fpath);
};

#endif