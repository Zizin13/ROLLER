#ifndef _ROLLER_ROLLER_H
#define _ROLLER_ROLLER_H
//-------------------------------------------------------------------------------------------------
#include "types.h"
#include "sound.h"
#include <stdio.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_gamepad.h>
//-------------------------------------------------------------------------------------------------

extern SDL_Gamepad *g_pController1;
extern SDL_Gamepad *g_pController2;

//-------------------------------------------------------------------------------------------------

// functions added by ROLLER
int InitSDL();
void ShutdownSDL();
void UpdateSDL();

bool MIDI_Init(const char *config_file);
void MIDIDigi_PlayBuffer(uint8 *midi_buffer, uint32 midi_length);
void MIDIDigi_ClearBuffer();
void MIDI_CloseMidiBuffer();
void MIDI_Shutdown();

void MIDIInitSong(tInitSong *data);
void MIDIStartSong();
void MIDIStopSong();

void MIDISetMasterVolume(int8 volume);
int MIDIGetMasterVolume();

int DIGISampleStart(tSampleData *data);
bool DIGISampleDone(int index);
int DIGISampleAvailable(int index);
void DIGIStopSample(int index);
void DIGISetMasterVolume(int volume);
int DIGIGetMasterVolume();

void PlayAudioSampleWait(int iIndex);
void PlayAudioDataWait(Uint8 *buffer, Uint32 length);
bool ROLLERfexists(const char *szFile);
FILE *ROLLERfopen(const char *szFile, const char *szMode); //tries to open file with both all caps and all lower case
int ROLLERopen(const char *szFile, int iOpenFlags); //tries to open file with both all caps and all lower case
uint32 ROLLERAddTimer(Uint32 uiFrequencyHz, SDL_TimerCallback callback, void *userdata);
uint32 SDLTickTimerCallback(void *userdata, SDL_TimerID timerID, Uint32 interval);
uint32 SDLS7TimerCallback(void *userdata, SDL_TimerID timerID, Uint32 interval);
int IsCDROMDevice(const char *szPath);
int GetAxisValue(SDL_Gamepad *pController, SDL_GamepadAxis axis);

//-------------------------------------------------------------------------------------------------
#endif
