#pragma once
#include <iostream>
#include <SDL.h>
#include "bass.h"


using namespace std;
class Audio
{
private:
	
public:
	Audio(BASS_3DVECTOR(float x, float y, float z))
	{
		
	}
	virtual ~Audio() 
	{
		BASS_Free();
	};
	static void LoadSample(char * filename);
};