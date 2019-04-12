#include "Audio.h"
#include <iostream>
#include <stdlib.h>  

using namespace std;

void Audio::LoadSample(char * filename)
{
	bool audio = true;
	if (!BASS_Init(1, 44100, BASS_SAMPLE_3D, 0, NULL))		//Initalise Audio
		cout << "Can't initialize device";

	HSAMPLE sample;
	if (sample = BASS_SampleLoad(FALSE, filename, 0, 0, 3, BASS_SAMPLE_OVER_POS))
		cout << "sample " << filename << " loaded!" << endl;
	else
	{
		cout << "Can't load sample";
		exit(0);
	}
	/*return sample;*/

	/*if (audio)
	{*/
	/*	cout << audio << endl;*/
		/*audio = false;*/
		HCHANNEL ch = BASS_SampleGetChannel(sample, FALSE);
		BASS_3DVECTOR* position = new BASS_3DVECTOR(5000.0f, 5000.0f, 5000.0f);
		BASS_Set3DPosition(position, NULL, NULL, NULL);
		BASS_Apply3D();
		//BASS_ChannelSetAttribute(ch, BASS_ATTRIB_FREQ, 0);
		//BASS_ChannelSetAttribute(ch, BASS_ATTRIB_VOL, 0.5);
		//BASS_ChannelSetAttribute(ch, BASS_ATTRIB_PAN, 0);

		if (!BASS_Set3DPosition(position, NULL, NULL, NULL))
		cout << "Could not set the 3d position of the listener" << endl;
		if (!BASS_ChannelPlay(ch, FALSE))													// Starts (or resumes) playback of a sample, stream, MOD music, or recording.
		if (!BASS_ChannelIsActive(ch))														// Checks if a sample, stream, or MOD music is active or stalled
		cout << "Can't play sample" << endl;
		/*else BASS_Start();*/
		Sleep(3000);
		/*audio = true;*/
	/*}
	else return;*/
		
}
