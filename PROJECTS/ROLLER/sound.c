#include "sound.h"
#include "frontend.h"
#include "moving.h"
#include "cdx.h"
#include "func2.h"
#include "func3.h"
#include "3d.h"
#include "loadtrak.h"
#include "car.h"
#include "roller.h"
#include "control.h"
#include "network.h"
#include "replay.h"
#include <memory.h>
#include <ctype.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <fcntl.h>
#ifdef IS_WINDOWS
#include <io.h>
#define open _open
#define close _close
#else
#include <inttypes.h>
#include <unistd.h>
#define O_BINARY 0 //linux does not differentiate between text and binary
#endif

//-------------------------------------------------------------------------------------------------

int samplespending = 0;     //000A4690
int writesample = 0;        //000A4694
int readsample = 0;         //000A4698
int lastsample = 0;         //000A469C
int musicon = -1;           //000A46A0
int soundon = -1;           //000A46A4
int allengines = -1;        //000A46A8
int cheat_samples = 0;      //000A46AC
int palette_brightness = 32;//000A46B0
int last_inp[2] = { 0, 0 }; //000A46BC
void *pal_selector = (void *)-1; //000A46B4
char SourcePath[64] = { 0 }; //000A46C4
char DestinationPath[64] = { 0 }; //000A4704
char languagename[32] = { 0 }; //000A4744
int writeptr = 0;           //000A476C
int readptr = 0;            //000A4770
int SoundCard = 0;          //000A4774
int SoundPort = 0;          //000A4778
int SoundIRQ = 0;           //000A477C
int SoundDMA = 0;           //000A4780
int EngineVolume = 32;      //000A4784
int SFXVolume = 56;         //000A4788
int SpeechVolume = 127;     //000A478C
int MusicVolume = 108;      //000A4790
int MusicCard = 0;          //000A4794
int MusicCD = 0;            //000A4798
int MusicPort = 0;          //000A479C
int SongPtr = 0;            //000A47A0
int SongHandle = 0;         //000A47A4
int CDSong[20] = { 10, 10, 10, 2, 3, 4, 5, 6, 7, 8, 0, 0, 0, 0, 0, 0, 0 }; //000A47A8
int GMSong[21] = { 0, 1, 2, 3, 4, 5, 6, 3, 4, 5, 6, 0, 0, 0, 0, 0, 0, 0 }; //000A47F8
tSampleData SampleData = { NULL, 0u, 0, 0, 2, 32767, 0, { 0, 0, 0, 18176, 0, 0, 0 }, 0, 0, 0, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1701995379 } }; //000A484C
tSampleData SampleFixed = { NULL, 0u, 0, 0, 2, 32767, 0, { 0, 0, 0, 256, 0, 0, 0, }, 0, 0, 0, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }; //000A48C4
tSampleData SamplePanned = { NULL, 0u, 0, 0, 2, 32767, 0, { 0, 0, 0, 768, 0, 0, 0, }, 0, 0, 0, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }; //000A493C
uint8 rud_gr[2] = { 0, 0 }; //000A4A08
uint8 rud_strat[2] = { 0, 0 }; //000A4A0A
int Joy1used = 0;           //000A4A0C
int Joy2used = 0;           //000A4A0C
int fraction = 0;           //000A4A14
int x1ok = 0;               //000A4A30
int y1ok = 0;               //000A4A34
int x2ok = 0;               //000A4A38
int y2ok = 0;               //000A4A3C
int bitaccept = 0;          //000A4A40
int holdmusic = 0;          //000A4A4C
int start_cd;               //0013FA9C 
uint8 unmangleinbuf[1024];  //00149EF0
uint8_t *musicbuffer;       //0014EBF8
int lastvolume[16];         //001603F8
int lastpitch[16];          //00160438
int lastpan[16];            //00160478
int net_time[16];           //001604B8
int joyvalue[8];            //001604F8
tJoyPos rud_Joy_pos;        //00160518
int rud_turn[2];            //00160538
int rud_swheel[2];          //00160540
int rud_steer[2];           //00160548
uint32 SampleLen[120];      //00160560
uint8 *SamplePtr[120];      //00160750
tSampleHandleCar SampleHandleCar[120]; //00160930
char Sample[120][15];       //00162730
char lang[16][32];          //00162E38
int TrackMap[32];           //00163038
char TextExt[64];           //001630CA
char SampleExt[64];         //0016310A
int HandleCar[32];          //00163B8C
int HandleSample[32];       //00163C0C
tCarSoundData enginedelay[16]; //00163C8C
int car_to_player[16];    //0016748C
int player_to_car[16];      //001674CC
int load_times[16];         //0016760C
tCopyData copy_multiple[512][16]; //0016764C
int unmangleinpoff;         //0016F64C
uint8 *unmangledst;         //0016F650
int unmangleoverflow;       //0016F654
FILE *unmanglefile;         //0016F658
int unmanglebufpos;         //0016F65C
volatile int s7;            //0016F660
int network_timeout;        //0016F668
void *MT32Data;             //0016F66C
void *FMDrums;              //0016F670
void *FMInstruments;        //0016F674
int network_sync_error;     //0016F678
int ticks_received;         //0016F680
int network_limit;          //0016F684
int MIDIHandle;             //0016F68C
int DIGIHandle;             //0016F690
volatile int frames;        //0016F694
char Song[20][15];          //0016F708
uint32 tickhandle;          //0016F834
DPMI_RMI RMI;               //0016F838
tColor *pal_addr;           //0016F86C
int user_inp;               //0016F87C
int nummusictracks;         //0016F8A8
int winchampsong;           //0016F8AC
int winsong;                //0016F8B0
int delaywritex;            //0016F8B4
int delayreadx;             //0016F8B8
int leaderboardsong;        //0016F8BC
int optionssong;            //0016F8C0
int titlesong;              //0016F8C4
int delaywrite;             //0016F8C8
int delayread;              //0016F8CC
int numsamples;             //0016F8D0
int cheatsample;            //0016F8D4
int languages;              //0016F8D8
int net_loading;            //0016F8DC
int already_quit;           //0016F8E0
int network_error;          //0017C97C

//-------------------------------------------------------------------------------------------------

void realmode(uint8 byRealModeInterrupt)
{
  /*union REGS regs; // [esp+0h] [ebp-34h] BYREF
  struct SREGS sregs; // [esp+1Ch] [ebp-18h] BYREF

  // 49 is DPMI interrupt
  // 768 Simulate Real Mode Interrupt
  // bx is being set to the real mode interrupt to simulate
  // all calls to this function set it to 0x10
  //   to simulate a BIOS video interrupt in real mode
  // __DS__ represents the current value of the DS (Data Segment)
  //   register in your program's segment context
  // By setting sregs.es = __DS__; and regs.x.edi = (unsigned int)&RMI;,
  //   we are telling the DPMI host that the RMI structure is located
  //   at offset &RMI in the segment __DS__
  memset(&sregs, 0, sizeof(sregs));
  regs.w.bx = byRealModeInterrupt;
  regs.w.ax = 768;
  regs.w.cx = 0;
  sregs.es = __DS__;
  regs.x.edi = (unsigned int)&RMI;
  int386x(49, &regs, &regs, &sregs);*/
}

//-------------------------------------------------------------------------------------------------

bool loadDOS(const char *szFilename, void **out_buffer)
{
  FILE *file = ROLLERfopen(szFilename, "rb");
  if (!file) {
    *out_buffer = NULL;
    return false;
  }

  // Get file size
  fseek(file, 0, SEEK_END);
  long iFileSize = ftell(file);
  rewind(file);

  if (iFileSize <= 0) {
    fclose(file);
    *out_buffer = NULL;
    return false;
  }

  // Allocate memory
  void *pBuffer = malloc(iFileSize);
  if (!pBuffer) {
    fclose(file);
    *out_buffer = NULL;
    return false;
  }

  // Read file into buffer
  int iReadBytes = (int)fread(pBuffer, 1, iFileSize, file);
  fclose(file);

  if (iReadBytes != iFileSize) {
    free(pBuffer);
    *out_buffer = NULL;
    return false;
  }

  *out_buffer = pBuffer;
  return true;
  /*
  union REGS regs;
  struct SREGS sregs;

  int handle = open_(szFilename, 0x200); // read-only
  if (handle == -1) {
    *out_segment = (void *)-1;
    return false;
  }

  long file_size = filelength_(handle);
  if (file_size <= 0) {
    close_(handle);
    *out_segment = (void *)-1;
    return false;
  }

  // Allocate a DOS memory block using int 31h AX=0100h
  unsigned short paragraphs = (file_size + 15) / 16; // round up to paragraphs
  memset(&sregs, 0, sizeof(sregs));
  regs.w.ax = 0x0100;
  regs.w.bx = paragraphs;
  int386x(0x31, &regs, &regs, &sregs);

  if (regs.x.cflag || regs.w.ax == 0) {
    close_(handle);
    *out_segment = (void *)-1;
    return false;
  }

  unsigned short seg = regs.w.ax;
  *out_segment = (void *)(uintptr_t)seg;

  // Convert segment:offset to linear address and read into it
  void *dst = (void *)((uintptr_t)seg << 4);
  read_(handle, dst, (unsigned int)file_size);
  close_(handle);

  return true;*/
}

//-------------------------------------------------------------------------------------------------

int claimDOS(int iSizeParagraphs, uint32 *uiSelectorOut)
{
  //int iRequestedParagraphs; // ecx
  //uint16 ax; // dx
  //union REGS regs; // [esp+0h] [ebp-34h] BYREF
  //struct SREGS sregs; // [esp+1Ch] [ebp-18h] BYREF
  //
  //// Calculate adjusted size in paragraphs
  //iRequestedParagraphs = ((iSizeParagraphs - (__CFSHL__(iSizeParagraphs >> 31, 4) + 16 * (iSizeParagraphs >> 31))) >> 4)
  //  + 1;
  //memset(&sregs, 0, sizeof(sregs));
  //regs.w.bx = iRequestedParagraphs;
  //regs.w.ax = 0x100;                            // AX = 0x100 (Allocate DOS Memory Block)
  //int386x(0x31, &regs, &regs, &sregs);
  //
  //// // Check if allocation failed (carry flag set)
  //if (regs.x.cflag) {
    *uiSelectorOut = -1;                        // Return error indicator
    return 0;                                   // Failed allocation
  //} else {
  //  ax = regs.w.ax;
  //  *uiSelectorOut = regs.w.dx;                 // Store real-mode segment
  //  return 16 * ax;                             // Return linear address (real-mode segment * 16)
  //}
}

//-------------------------------------------------------------------------------------------------

void releaseDOS(uint16 nSegment)
{
  //union REGS regs; // [esp+0h] [ebp-34h] BYREF
  //struct SREGS sregs; // [esp+1Ch] [ebp-18h] BYREF
  //
  //memset(&sregs, 0, sizeof(sregs));
  //regs.w.dx = nSegment;
  //regs.w.ax = 0x101;                            // EAX = 0x101 (Function: Free DOS Memory Block)
  //int386x(0x31, &regs, &regs, &sregs);
}

//-------------------------------------------------------------------------------------------------

bool setpal(const char *szFilename)
{
  void *pFileData = NULL;

  // Free old palette memory, if it was set
  if ((int64)pal_selector >= 0 && pal_addr) {
    free(pal_addr);
    pal_addr = NULL;
    pal_selector = (void *)-1;
  }
  /*if ( (int)pal_selector >= 0 )
  {
    v4 = (unsigned __int16)pal_selector;
    memset(&sregs, 0, sizeof(sregs));
    regs.w.dx = v4;
    regs.w.ax = 257;
    int386x(0x31, &regs, &regs, &sregs);
    pal_selector = (void *)-1;
  }*/

  // Load palette file into memory
  if (!loadDOS(szFilename, &pFileData)) {
    fprintf(stderr, "Failed to load palette file: %s\n", szFilename);
    return false;
  }

  // Assign palette globals
  pal_addr = pFileData;
  pal_selector = pFileData; // For compatibility

  uint8 *pRaw = (uint8 *)pFileData;

  // Copy RGB triplets into the structured palette array
  for (int i = 0; i < 256; ++i) {
    palette[i].byR = pRaw[i * 3 + 0];
    palette[i].byG = pRaw[i * 3 + 1];
    palette[i].byB = pRaw[i * 3 + 2];
  }

  // If cheat mode flag is set, apply grayscale filter
  if (cheat_mode & CHEAT_MODE_GRAYSCALE) {
    for (int i = 0; i < 256; ++i) {
      uint8 byAvg = (palette[i].byR + palette[i].byB + palette[i].byG) / 3;
      palette[i].byR = byAvg;
      palette[i].byB = byAvg;
      palette[i].byG = byAvg;
    }
  }

  palette_brightness = 0;
  return true;
}

//-------------------------------------------------------------------------------------------------

void blankpal()
{
  if (!pal_addr)
    return;

  memset(pal_addr, 0, sizeof(tColor) * 256);

  /*
  unsigned int uiLinearAddress; // ecx
  unsigned __int16 unDx; // si
  union REGS regs; // [esp+0h] [ebp-64h] BYREF
  REGS regs2; // [esp+1Ch] [ebp-48h] BYREF
  struct SREGS sregs; // [esp+38h] [ebp-2Ch] BYREF
  SREGS sregs2; // [esp+44h] [ebp-20h] BYREF

  memset(&sregs, 0, sizeof(sregs));
  regs.w.ax = 256;
  regs.w.bx = 48;

  // allocate 48 LDT selectors
  // * If successful, AX contains the first selector allocated.
  // * If failed, the carry flag is set and AX contains an error code.
  int386x(49, &regs, &regs, &sregs);

  // Multiplying by 16 (* 16) converts a segment
  // selector value to a linear address in real
  // mode (segment:offset addressing), because in
  // real mode, the physical address is
  // segment * 16 + offset.
  uiLinearAddress = 16 * regs.w.ax;
  unDx = regs.w.dx;
  memset((void *)uiLinearAddress, 0, 0x300u);
  RMI.ecx = 256;
  RMI.eax = 4114;
  RMI.ebx = 0;
  RMI.edx = 0;

  // Shifting the address right by 4 (>> 4) converts
  // the linear address back to a segment value
  // (since segment = address / 16).
  RMI.es = uiLinearAddress >> 4;
  realmode(0x10u);
  memset(&sregs2, 0, sizeof(sregs2));
  regs2.w.dx = unDx;
  regs2.w.ax = 257;
  int386x(49, &regs2, &regs2, &sregs2);
  */
  palette_brightness = 0;
}

//-------------------------------------------------------------------------------------------------

void resetpal()
{
  /*
  RMI.eax = 4114;
  RMI.ebx = 0;
  RMI.ecx = 256;
  RMI.es = (unsigned int)pal_addr >> 4;
  RMI.edx = 0;
  realmode(0x10u);
  */

  // Clean up heap-allocated palette memory
  if (pal_addr) {
    free(pal_addr);
    pal_addr = NULL;
  }
}

//-------------------------------------------------------------------------------------------------

void Initialise_SOS()
{
  //sosTIMERInitSystem(0xFF00, 0);                // 0xFF00 maybe a special case for a slow rate timer?
  if (MusicCD)
    SetAudioVolume(MusicVolume);
  if (MusicCard) {
    if (MusicCard == 40970)
      initgus();
    printf("Initialising MIDI device %4X...", MusicCard);
    fflush(stdout);
    memset(TrackMap, 255, 0x20 * sizeof(unsigned int));
    //sosMIDIInitSystem(0, 0);
    //MIDIInitDriver_variable_1 = 0;
    //MIDIHardware = MusicPort;
    //MIDIInitDriver_variable_2 = 0;
    //if (sosMIDIInitDriver(
    //  MusicCard,
    //  0,
    //  (unsigned int)&MIDIHardware,
    //  __DS__,
    //  (int)&MIDIInitDriver,
    //  (unsigned __int16)__DS__,
    //  (unsigned int)&MIDIHandle,
    //  (unsigned __int16)__DS__)) {
    //  MusicCard = 0;
    //  sosMIDIUnInitSystem();
    //  printf("Failed\n");
    //} else {
    //  printf("OK\n");
    //  devicespecificinit();
    //  sosMIDISetMasterVolume(MusicVolume);
    //}
  }
  if (SoundCard) {
    printf("Initialising DIGI device %4X...", SoundCard);
    fflush(stdout);
    //sosDIGIInitSystem(0, 0);
    printf("OK\n");
  }
  select8bitdriver();
  ticks = 0;
  frames = 0;

  // start 36Hz timer that calls tickhandler
  claim_ticktimer(36u);
}

//-------------------------------------------------------------------------------------------------

void updatejoy()
{
  int iX1Scaled; // eax
  int iX1Clamped2; // eax
  int iX1Clamped; // eax
  int iY1Scaled; // eax
  int iY1Clamped2; // eax
  int iY1Clamped; // eax
  int iX2Scaled; // eax
  int iX2Clamped2; // eax
  int iX2Clamped; // eax
  int iY2Scaled; // eax
  int iY2Clamped2; // eax
  int iY2Clamped; // eax

  memset(joyvalue, 0, sizeof(joyvalue));
  if (Joy1used || Joy2used)
    ReadJoys(&rud_Joy_pos);
  if (Joy1used) {
    keys[128] = rud_Joy_pos.iX1Status;
    keys[129] = rud_Joy_pos.iY1Status;
    iX1Scaled = ((2 * rud_Joy_pos.iX1Count - JAXmax - JAXmin) << 10) / (JAXmax - JAXmin);
    //apply 100-unit deadzone
    if (iX1Scaled >= 0) {
      iX1Clamped = iX1Scaled - 100;
      if (iX1Clamped < 0)
        iX1Clamped = 0;
      joyvalue[1] = iX1Clamped;
    } else {
      iX1Clamped2 = iX1Scaled + 100;
      if (iX1Clamped2 > 0)
        iX1Clamped2 = 0;
      joyvalue[0] = -iX1Clamped2;
    }
    iY1Scaled = ((2 * rud_Joy_pos.iY1Count - JAYmax - JAYmin) << 10) / (JAYmax - JAYmin);
    //apply 100-unit deadzone
    if (iY1Scaled >= 0) {
      iY1Clamped = iY1Scaled - 100;
      if (iY1Clamped < 0)
        iY1Clamped = 0;
      joyvalue[3] = iY1Clamped;
    } else {
      iY1Clamped2 = iY1Scaled + 100;
      if (iY1Clamped2 > 0)
        iY1Clamped2 = 0;
      joyvalue[2] = -iY1Clamped2;
    }
  }
  if (Joy2used) {
    keys[130] = rud_Joy_pos.iX2Status;
    keys[131] = rud_Joy_pos.iY2Status;
    iX2Scaled = ((2 * rud_Joy_pos.iX2Count - JBXmax - JBXmin) << 10) / (JBXmax - JBXmin);
    //apply 100-unit deadzone
    if (iX2Scaled >= 0) {
      iX2Clamped = iX2Scaled - 100;
      if (iX2Clamped < 0)
        iX2Clamped = 0;
      joyvalue[5] = iX2Clamped;
    } else {
      iX2Clamped2 = iX2Scaled + 100;
      if (iX2Clamped2 > 0)
        iX2Clamped2 = 0;
      joyvalue[4] = -iX2Clamped2;
    }
    iY2Scaled = ((2 * rud_Joy_pos.iY2Count - JBYmax - JBYmin) << 10) / (JBYmax - JBYmin);
    //apply 100-unit deadzone
    if (iY2Scaled >= 0) {
      iY2Clamped = iY2Scaled - 100;
      if (iY2Clamped < 0)
        iY2Clamped = 0;
      joyvalue[7] = iY2Clamped;
    } else {
      iY2Clamped2 = iY2Scaled + 100;
      if (iY2Clamped2 > 0)
        iY2Clamped2 = 0;
      joyvalue[6] = -iY2Clamped2;
    }
  }
}

//-------------------------------------------------------------------------------------------------

void readuserdata(int iPlayer)
{
  // Skip processing during countdown phase
  if (countdown >= 140) {
    user_inp = 0;
    return;
  }

  // Calculate key indices for this player (6 keys per player)
  int iKeyOffset = iPlayer * 6;
  tCarEngine *pEngine = (tCarEngine *)p_eng[iPlayer];

  // Early exit if no engine data
  if (!pEngine) {
    user_inp = 0;
    return;
  }

  // Initialize input state
  int iButtonFlags = 0;
  int iSteeringValue = last_inp[iPlayer];
  int iMaxSteering = pEngine->iSteeringSensitivity << 8;
  rud_steer[iPlayer] = iMaxSteering;

  // Process acceleration and brake inputs
  uint8 byAccelKey = userkey[iKeyOffset + 2];  // Acceleration key USERKEY_P1UP or USERKEY_P2UP
  uint8 byBrakeKey = userkey[iKeyOffset + 3];  // Brake key USERKEY_P1DOWN or USERKEY_P2DOWN
  int iAccelState = 0;

  if ((byAccelKey > 0x83 && joyvalue[byAccelKey] > 0xC8) ||
      (byAccelKey <= 0x83 && keys[byAccelKey])) {
    iAccelState = 1;  // Accelerate
  }
  if ((byBrakeKey > 0x83 && joyvalue[byBrakeKey] > 0xC8) ||
      (byBrakeKey <= 0x83 && keys[byBrakeKey])) {
    iAccelState = iAccelState == 1 ? -1 : -2;  // Brake or both
  }

  // Process steering input
  uint8 byLeftKey = userkey[iKeyOffset];      // Steer left USERKEY_P1LEFT or USERKEY_P2LEFT
  uint8 byRightKey = userkey[iKeyOffset + 1]; // Steer right USERKEY_P1RIGHT or USERKEYP2RIGHT

  if (byLeftKey > 0x83) {
    // Joystick steering processing
    int iJoyLeft = (80 * joyvalue[byLeftKey]) >> 8;
    int iJoyRight = (80 * joyvalue[byRightKey]) >> 8;

    if (iJoyLeft > 0x102) iJoyLeft = 0x102;
    if (iJoyRight > 0x102) iJoyRight = 0x102;

    int iLeftEffect = (p_joyk1[iPlayer] * ((iJoyLeft * iJoyLeft * iJoyLeft) >> 8) + p_joyk2[iPlayer] * iJoyLeft);
    int iRightEffect = (p_joyk1[iPlayer] * ((iJoyRight * iJoyRight * iJoyRight) >> 8) + p_joyk2[iPlayer] * iJoyRight);

    iSteeringValue = (iLeftEffect - iRightEffect) >> 16;

    // Clamp steering to valid range
    if (iSteeringValue < -iMaxSteering) iSteeringValue = -iMaxSteering;
    if (iSteeringValue > iMaxSteering) iSteeringValue = iMaxSteering;
  } else {
    // Keyboard steering processing
    if (keys[byLeftKey]) {
      // Process left steering
      int iTurnRate = rud_turn[iPlayer];

      if (iTurnRate >= 0) {
        // Apply turn decay
        iTurnRate += 32 * pEngine->iTurnDecayRate;
      } else {
        // Calculate new turn rate
        int iDecay = (pEngine->iMaxTurnRate << 8) - (32 * pEngine->iTurnDecayRate);
        int iAbsSteer = abs(iSteeringValue);
        iTurnRate = (iDecay * iAbsSteer) / iMaxSteering;
      }

      // Apply turn rate and clamp
      iSteeringValue += iTurnRate;
      if (iSteeringValue > iMaxSteering) iSteeringValue = iMaxSteering;
    } else if (keys[byRightKey]) {
      // Process right steering (mirror of left)
      int iTurnRate = rud_turn[iPlayer];

      if (iTurnRate <= 0) {
        iTurnRate -= 32 * pEngine->iTurnDecayRate;
      } else {
        int iDecay = (pEngine->iMaxTurnRate << 8) - (32 * pEngine->iTurnDecayRate);
        int iAbsSteer = abs(iSteeringValue);
        iTurnRate = -(iDecay * iAbsSteer) / iMaxSteering;
      }

      iSteeringValue += iTurnRate;
      if (iSteeringValue < -iMaxSteering) iSteeringValue = -iMaxSteering;
    } else {
      // Center steering when no input
      rud_turn[iPlayer] = -1;
      int iCentering = pEngine->iWheelCenteringRate << 8;

      if (iSteeringValue > 0) {
        iSteeringValue -= iCentering;
        if (iSteeringValue < 0) iSteeringValue = 0;
      } else if (iSteeringValue < 0) {
        iSteeringValue += iCentering;
        if (iSteeringValue > 0) iSteeringValue = 0;
      }
    }
  }

  // Update steering state
  rud_swheel[iPlayer] = iSteeringValue;

  // Build button flags
  if (iAccelState > 0) iButtonFlags |= BUTTON_FLAG_ACCEL;  // Accelerate flag
  if (iAccelState < 0) iButtonFlags |= BUTTON_FLAG_BRAKE;  // Brake flag

  // Process special buttons
  uint8 bySpecialKey = iPlayer ? userkey[USERKEY_P2CHEAT] : userkey[USERKEY_P2DOWNGEAR];  // P2downgear or P2cheat
  if ((bySpecialKey > 0x83 && joyvalue[bySpecialKey] > 0xC8) ||
      (bySpecialKey <= 0x83 && keys[bySpecialKey])) {
    iButtonFlags |= BUTTON_FLAG_SPECIAL;  // Special action flag
  }

  // Process gear shifting
  uint8 byGearUpKey = userkey[iKeyOffset + 4]; //USERKEY_P1UPGEAR or USERKEY_P2UPGEAR
  uint8 byGearDownKey = userkey[iKeyOffset + 5]; //USERKEY_P1DOWNGEAR or USERKEY_P2DOWNGEAR
  int iGearChange = 0;

  if ((byGearUpKey > 0x83 && joyvalue[byGearUpKey] > 0xC8) ||
      (byGearUpKey <= 0x83 && keys[byGearUpKey])) {
    iGearChange = 1;
    iButtonFlags |= BUTTON_FLAG_UPGEAR;  // Gear up flag
  }
  if ((byGearDownKey > 0x83 && joyvalue[byGearDownKey] > 0xC8) ||
      (byGearDownKey <= 0x83 && keys[byGearDownKey])) {
    iGearChange = -1;
    iButtonFlags |= BUTTON_FLAG_DOWNGEAR;  // Gear down flag
  }

  // Update gear state
  if (iGearChange != 0) {
    rud_gr[iPlayer] = -1;  // Gear changed
  }

  // Process strategy buttons (F1-F4)
  if (lastsample < 0) {
    int iStrategyFlags = 0;
    if (keys[63]) iStrategyFlags |= BUTTON_FLAG_F1;  // F1
    if (keys[64]) iStrategyFlags |= BUTTON_FLAG_F2;  // F2
    if (keys[65]) iStrategyFlags |= BUTTON_FLAG_F3;  // F3
    if (keys[66]) iStrategyFlags |= BUTTON_FLAG_F4;  // F4

    // Handle network messages for strategy buttons
    if (iStrategyFlags) {
      rud_strat[iPlayer] = -1;  // Mark strategy button pressed

      if (network_on) {
        int iTargetNode = network_mes_mode;

        // Validate target node for messaging
        if (iTargetNode >= 0) {
          if (human_control[iTargetNode]) {
            iTargetNode = car_to_player[iTargetNode];
          } else {
            iTargetNode = -2;  // Invalid target
          }
        }

        // Send message if valid target and player hasn't finished
        if (iTargetNode > -2 && !finished_car[player1_car]) {
          int iMessageType = -1;

          // Map strategy flags to message types
          if (iStrategyFlags & BUTTON_FLAG_F1) iMessageType = 0;      // F1 -> Message 0
          else if (iStrategyFlags & BUTTON_FLAG_F2) iMessageType = 1; // F2 -> Message 1
          else if (iStrategyFlags & BUTTON_FLAG_F3) iMessageType = 2; // F3 -> Message 2
          else if (iStrategyFlags & BUTTON_FLAG_F4) iMessageType = 3; // F4 -> Message 3

          if (iMessageType != -1) {
            send_mes(iMessageType, iTargetNode);
          }
        }
      }

      // Add strategy flags to button state
      iButtonFlags |= iStrategyFlags;
    } else {
      // Strategy buttons not pressed
      rud_strat[iPlayer] = 0;
    }
  }

  // Handle quit request
  if (I_Want_Out && !already_quit) {
    already_quit = -1;
    if (wConsoleNode == master) {
      iButtonFlags |= BUTTON_FLAG_MASTER_QUIT;  // Master quit flag
    } else {
      iButtonFlags |= BUTTON_FLAG_SLAVE_QUIT;  // Client quit flag
    }
  }

  // Store final input state
  user_inp = (iButtonFlags << 16) | (iSteeringValue & 0xFFFF);
}

//-------------------------------------------------------------------------------------------------

void tickhandler()
{
  if (network_on && syncleft) {
    do_sync_stuff();
    return;
  }

  // Initialize network state
  if (network_on) {
    net_loading = 0;
  } else {
    broadcast_mode = 0;
  }
  frames++;

  // Replay speed control
  if (replaytype == 2) {
    if (forwarding) {
      if (slowing) {
        replayspeed = (4 * replayspeed) / 5;
        if (replayspeed < 256) {
          ROldStatus();
          slowing = 0;
        }
      } else {
        replayspeed = (17 * replayspeed) / 16;
        if (replayspeed > REPLAY_SPEED_MAX) {
          replayspeed = REPLAY_SPEED_MAX;
        }
      }
    }

    if (rewinding) {
      if (slowing) {
        replayspeed = (4 * replayspeed) / 5;
        if (replayspeed > -256) {
          ROldStatus();
          slowing = 0;
        }
      } else {
        replayspeed = (17 * replayspeed) / 16;
        if (replayspeed < REPLAY_SPEED_MIN) {
          replayspeed = REPLAY_SPEED_MIN;
        }
      }
    }

    // Calculate tick advancement
    int iTotal = replayspeed + fraction;
    int iTickAdvance = iTotal / 256;
    fraction = iTotal % 256;

    // Update and clamp tick position
    ticks += iTickAdvance;
    if (ticks < 0) ticks = 0;
    if (ticks >= replayframes) ticks = replayframes - 1;

    // Handle boundary conditions
    if (ticks == 0 && fraction < 0) fraction = 0;
    if (ticks == replayframes - 1 && fraction > 0) fraction = 0;
  }
  // Handle normal game mode
  else {
    ticks++;
    if (!paused && !frontend_on) {
      updatejoy();
    }
  }

  // Network message processing
  if (tick_on) {
    if (network_on && (replaytype == 2 || game_type > 2)) {
      CheckNewNodes();
      SendAMessage();
      BroadcastNews();
    } else if (replaytype != 2 && game_type < 3) {
      if (frontend_on) {
        if (network_on) {
          CheckNewNodes();
          SendAMessage();
          BroadcastNews();
        }
      } else {
        if (network_on && winner_mode) {
          CheckNewNodes();
          SendAMessage();
          BroadcastNews();
        }
        // Single player mode
        if (!network_on || winner_mode) {
          if (start_race && !paused) {

            // Process player 1 input
            readuserdata(0);
            last_inp[0] = user_inp;
            copy_multiple[writeptr][player1_car].uiFullData = user_inp;

            // Process player 2 input if present
            if (player_type == 2) {
              readuserdata(1);
              last_inp[1] = user_inp;
              copy_multiple[writeptr][player2_car].uiFullData = user_inp;
            }

            // Update write pointer
            writeptr = (writeptr + 1) % REPLAY_BUFFER_SIZE;

            // Process sound if enabled
            if (soundon && !paused) {
              if (delayread < delaywrite) {
                delayreadx = delayread % DELAY_BUFFER_SIZE;

                if (player_type == 2) {
                  // Process both players
                  tEngineSoundData *pSound = &enginedelay[ViewType[0]].engineSoundData[delayreadx];
                  if (pSound->iEngineVol >= 0)
                    loopsample(ViewType[0], SOUND_SAMPLE_ENGINE, pSound->iEngineVol, pSound->iEnginePitch, pSound->iPan);
                  if (pSound->iEngine2Vol >= 0)
                    loopsample(ViewType[0], SOUND_SAMPLE_ENGINE2, pSound->iEngine2Vol, pSound->iEngine2Pitch, pSound->iPan);
                  if (pSound->iSkid1Vol >= 0)
                    loopsample(ViewType[0], SOUND_SAMPLE_SKID1, pSound->iSkid1Vol, pSound->iSkid1Pitch, pSound->iPan);
                  pSound = &enginedelay[ViewType[1]].engineSoundData[delayreadx];
                  if (pSound->iEngineVol >= 0)
                    loopsample(ViewType[1], SOUND_SAMPLE_ENGINE, pSound->iEngineVol, pSound->iEnginePitch, pSound->iPan);
                  if (pSound->iEngine2Vol >= 0)
                    loopsample(ViewType[1], SOUND_SAMPLE_ENGINE2, pSound->iEngine2Vol, pSound->iEngine2Pitch, pSound->iPan);
                  if (pSound->iSkid1Vol >= 0)
                    loopsample(ViewType[1], SOUND_SAMPLE_SKID1, pSound->iSkid1Vol, pSound->iSkid1Pitch, pSound->iPan);
                } else if (allengines) {
                  // Process all cars
                  for (int i = 0; i < numcars; i++) {
                    tEngineSoundData *pSound = &enginedelay[i].engineSoundData[delayreadx];
                    if (pSound->iEngineVol >= 0)
                      loopsample(i, SOUND_SAMPLE_ENGINE, pSound->iEngineVol, pSound->iEnginePitch, pSound->iPan);
                    if (pSound->iEngine2Vol >= 0)
                      loopsample(i, SOUND_SAMPLE_ENGINE2, pSound->iEngine2Vol, pSound->iEngine2Pitch, pSound->iPan);
                    if (pSound->iSkid1Vol >= 0)
                      loopsample(i, SOUND_SAMPLE_SKID1, pSound->iSkid1Vol, pSound->iSkid1Pitch, pSound->iPan);
                  }
                } else {
                  // Process only player 1
                  tEngineSoundData *pSound = &enginedelay[ViewType[0]].engineSoundData[delayreadx];
                  if (pSound->iEngineVol >= 0)
                    loopsample(ViewType[0], SOUND_SAMPLE_ENGINE, pSound->iEngineVol, pSound->iEnginePitch, pSound->iPan);
                  if (pSound->iEngine2Vol >= 0)
                    loopsample(ViewType[0], SOUND_SAMPLE_ENGINE2, pSound->iEngine2Vol, pSound->iEngine2Pitch, pSound->iPan);
                  if (pSound->iSkid1Vol >= 0)
                    loopsample(ViewType[0], SOUND_SAMPLE_SKID1, pSound->iSkid1Vol, pSound->iSkid1Pitch, pSound->iPan);
                }

                // Advance buffer
                if (delaywrite - delayread >= 6) {
                  delayread++;
                }
              }
            }
          }
        } else {
          // Handle multiplayer mode
          ticks_received = 0;

          // Handle master node
          if (wConsoleNode == master) {
            // Check player timeouts
            for (int i = 0; i < network_on; i++) {
              if (paused) {
                net_time[i] = frames;
              }
              if (i != master && net_players[i] &&
                  (frames > net_time[i] + network_limit) &&
                  !send_finished) {
                network_error = 222;
              }
            }

            // Handle network errors
            if (network_error && network_error != 666) {
              send_net_error();
            }

            // Full player count reached
            if (network_on - 1 == start_multiple) {
              // Copy previous frame data
              memcpy(copy_multiple[writeptr],
                     copy_multiple[(writeptr - 1 + REPLAY_BUFFER_SIZE) % REPLAY_BUFFER_SIZE],
                     sizeof(tCopyData) * MAX_CARS);

              // Reset connection flags
              for (int i = 0; i < numcars; i++) {
                uint16 unFlags = copy_multiple[writeptr][i].data.unFlags;

                if (copy_multiple[writeptr][i].uiFullData & 0x4000000) {
                  unFlags ^= FLAG_DISCONNECT;
                }
                if (copy_multiple[writeptr][i].uiFullData & 0x8000000) {
                  unFlags = (unFlags ^ FLAG_MASTER_CHANGE) | FLAG_DISCONNECT;
                }
                copy_multiple[writeptr][i].data.unFlags = unFlags;
              }

              // Process player input
              readuserdata(0);
              copy_multiple[writeptr][player1_car].uiFullData = user_inp;
              last_inp[0] = user_inp;

              receive_all_singles();
            }

            // Countdown handling
            if (countdown >= 0) {
              active_nodes = 0;
              for (int i = 0; i < network_on; i++) {
                if (player_ready[i]) active_nodes++;
                if (player_ready[i] == 666 && net_players[i]) {
                  copy_multiple[writeptr][player_to_car[i]].data.unFlags |= FLAG_DISCONNECT;
                  net_players[i] = 0;
                }
              }
            }

            // Send data when all nodes are ready
            if (active_nodes == network_on) {
              net_loading = 0;
              if (network_on - 1 != start_multiple || !paused) {
                if (start_race && !paused && send_finished && numcars > 0) {
                  for (int i = 0; i < numcars; i++) {
                    copy_multiple[writeptr][i].data.unFlags |= FLAG_FINISHED;
                  }
                }
                send_multiple();
                
                // Handle master transition
                if (network_on - 1 == start_multiple) {
                  for (int i = 0; i < numcars; i++) {
                    if (copy_multiple[writeptr][i].data.unFlags & FLAG_DISCONNECT) {
                      net_players[car_to_player[i]] = 0;
                      network_timeout = frames;
                    }
                  }

                  if (copy_multiple[writeptr][player1_car].data.unFlags & FLAG_MASTER_CHANGE) {
                    // Reset network state
                    read_check = -1;
                    write_check = -1;
                    memset(player_checks, -1, sizeof(player_checks));

                    // Find new master
                    net_players[master] = 0;
                    while (!net_players[master] && master < MAX_PLAYERS) {
                      master++;
                    }

                    // Reset timers
                    for (int i = 0; i < network_on; i++) {
                      net_time[i] = frames;
                    }

                    // Update flags
                    uint16 unFlags = copy_multiple[writeptr][player1_car].data.unFlags;
                    unFlags ^= FLAG_DISCONNECT;
                    copy_multiple[(writeptr + 1) % REPLAY_BUFFER_SIZE][player1_car].data.unFlags = unFlags;

                    network_timeout = frames;
                  }

                  writeptr = (writeptr + 1) % REPLAY_BUFFER_SIZE;
                  ticks_received++;
                }
              }
            }
          } else if (wConsoleNode > master && net_players[wConsoleNode]) {
            if (paused) {
              network_timeout = frames;
            }
            if (frames > network_timeout + network_limit && human_finishers < players) {
              network_error = 123;
            }

            readuserdata(0);
            send_single(user_inp);
            last_inp[0] = user_inp;

            receive_multiple();

            // Handle master change
            if (copy_multiple[(writeptr - 1 + REPLAY_BUFFER_SIZE) % REPLAY_BUFFER_SIZE][player_to_car[master]].uiFullData & 0x8000000) {
              read_check = -1;
              write_check = -1;
              memset(player_checks, -1, sizeof(player_checks));

              net_players[master] = 0;
              while (!net_players[master] && master < MAX_PLAYERS) {
                master++;
              }

              active_nodes = network_on;
              start_multiple = network_on - 1;

              for (int i = 0; i < network_on; i++) {
                net_time[i] = frames;
              }
              network_timeout = frames;
            }

            // Check for disconnections
            for (int i = 0; i < numcars; i++) {
              uint32 uiData = copy_multiple[(writeptr - 1 + REPLAY_BUFFER_SIZE) % REPLAY_BUFFER_SIZE][i].uiFullData;

              if (uiData & 0x10000000) {
                network_error = 0;  // Reset error on valid data
              }
              if (uiData & 0x4000000) {
                net_players[car_to_player[i]] = 0;
                network_timeout = frames;
                memset(player_checks, -1, sizeof(player_checks));
                read_check = 0;
                write_check = 0;
              }
            }
          } else {
            // Handle disconnected state
            read_check = -1;
            write_check = -1;

            readuserdata(0);
            last_inp[0] = user_inp;
            copy_multiple[writeptr][player1_car].uiFullData = user_inp;

            // Set disconnect flag
            uint16 unFlags = copy_multiple[writeptr][player1_car].data.unFlags;
            unFlags ;
            copy_multiple[writeptr][player1_car].data.unFlags |= FLAG_DISCONNECT;

            writeptr = (writeptr + 1) % REPLAY_BUFFER_SIZE;
          }

          // Process sound
          if (soundon && !paused) {
            if (delayread >= delaywrite) return;

            delayreadx = delayread % DELAY_BUFFER_SIZE;

            if (allengines) {
              for (int i = 0; i < numcars; i++) {
                tEngineSoundData *pSound = &enginedelay[i].engineSoundData[delayreadx];
                if (pSound->iEngineVol >= 0)
                  loopsample(i, SOUND_SAMPLE_ENGINE, pSound->iEngineVol, pSound->iEnginePitch, pSound->iPan);
                if (pSound->iEngine2Vol >= 0)
                  loopsample(i, SOUND_SAMPLE_ENGINE2, pSound->iEngine2Vol, pSound->iEngine2Pitch, pSound->iPan);
                if (pSound->iSkid1Vol >= 0)
                  loopsample(i, SOUND_SAMPLE_SKID1, pSound->iSkid1Vol, pSound->iSkid1Pitch, pSound->iPan);
              }
            } else {
              tEngineSoundData *pSound = &enginedelay[ViewType[0]].engineSoundData[delayreadx];
              if (pSound->iEngineVol >= 0)
                loopsample(ViewType[0], SOUND_SAMPLE_ENGINE, pSound->iEngineVol, pSound->iEnginePitch, pSound->iPan);
              if (pSound->iEngine2Vol >= 0)
                loopsample(ViewType[0], SOUND_SAMPLE_ENGINE2, pSound->iEngine2Vol, pSound->iEngine2Pitch, pSound->iPan);
              if (pSound->iSkid1Vol >= 0)
                loopsample(ViewType[0], SOUND_SAMPLE_SKID1, pSound->iSkid1Vol, pSound->iSkid1Pitch, pSound->iPan);
            }

            // Advance by ticks received
            delayread += ticks_received;
          }
        }
      }
    }
  }

  // Replay sound processing
  if (replaytype == 2 && replayspeed == REPLAY_NORMAL_SPEED) {
    if (delayread < delaywrite) {
      int delayreadx = delayread % DELAY_BUFFER_SIZE;

      for (int i = 0; i < numcars; i++) {
        tEngineSoundData *pSound = &enginedelay[i].engineSoundData[delayreadx];
        if (pSound->iEngineVol >= 0)
          loopsample(i, SOUND_SAMPLE_ENGINE, pSound->iEngineVol, pSound->iEnginePitch, pSound->iPan);
        if (pSound->iEngine2Vol >= 0)
          loopsample(i, SOUND_SAMPLE_ENGINE2, pSound->iEngine2Vol, pSound->iEngine2Pitch, pSound->iPan);
        if (pSound->iSkid1Vol >= 0)
          loopsample(i, SOUND_SAMPLE_SKID1, pSound->iSkid1Vol, pSound->iSkid1Pitch, pSound->iPan);
      }

      if (delaywrite - delayread >= 6) {
        delayread++;
      }
    }
  }
}

//-------------------------------------------------------------------------------------------------

void claim_ticktimer(unsigned int uiRateHz)
{
  /***
  * ADDED BY ROLLER
  ***/
  tickhandle = ROLLERAddTimer(uiRateHz, SDLTickTimerCallback, NULL); //may as well re-use tickhandle, it is also a uint32
  /***
  * END ROLLER CODE
  ***/

  /*
  if (sosTIMERRegisterEvent(
    uiRateHz,
    a2,
    (int)tickhandler,
    __CS__,
    (unsigned int)&tickhandle,
    (unsigned __int16)__DS__)) {
    printf("Unable to set timer\n");
    doexit();
  }*/
}

//-------------------------------------------------------------------------------------------------

void release_ticktimer()
{
  /***
  * ADDED BY ROLLER
  ***/
  SDL_RemoveTimer(tickhandle); //may as well re-use tickhandle, it is also a uint32
  /***
  * END ROLLER CODE
  ***/

  //sosTIMERRemoveEvent(tickhandle);
}

//-------------------------------------------------------------------------------------------------

void Uninitialise_SOS()
{
  /***
  * ADDED BY ROLLER
  ***/
  SDL_RemoveTimer(tickhandle); //may as well re-use tickhandle, it is also a uint32
  /***
  * END ROLLER CODE
  ***/

  //sosTIMERRemoveEvent(tickhandle);
  //sosTIMERRemoveEvent(TimerEventHandle);
  //sosTIMERUnInitSystem(0);
  //if (SoundCard) {
  //  sosDIGIUnInitDriver(DIGIHandle, 1, 1);
  //  sosDIGIUnInitSystem();
  //}
  if (MusicCard) {
    devicespecificuninit();
    //sosMIDIUnInitDriver(MIDIHandle, 1);
    //sosMIDIUnInitSystem();
  }
}

//-------------------------------------------------------------------------------------------------

void loadsamples()
{
  int iWinnerSample = winner_mode;

  if (iWinnerSample != 0) {
    if (champ_mode != 0) {
      // Championship mode
      loadasample(SOUND_SAMPLE_ENGINE); // 0 - Engine sound
      loadasample(SOUND_SAMPLE_ENGINE2); // 1 - Engine sound 2
      loadasample(SOUND_SAMPLE_SKID1); // 3 - Skid sound
      loadasample(SOUND_SAMPLE_LANDSKID); // 9 - Landskid sound
      loadasample(SOUND_SAMPLE_LIGHTLAN); // 10 - Light land sound
      loadasample(SOUND_SAMPLE_BANK); // 11 - Bank sound
      loadasample(SOUND_SAMPLE_WALL1); // 12 - Wall sound
      loadasample(SOUND_SAMPLE_EXPLO); // 6 - Explosion sound

      int iChampIdx = champorder[0];
      int iWins = total_wins[iChampIdx];
      if (iWins > 0) {
        loadasample(iWins + (SOUND_SAMPLE_1RACE - 1)); // 88 - Sound for win count 
      }

      int iKills = total_kills[iChampIdx];
      if (iKills > 0 && iKills < 17) {
        loadasample(iKills + (SOUND_SAMPLE_ONE - 1)); // 96 - Sound for kill count 
      } else if (iKills >= 17) {
        loadasample(SOUND_SAMPLE_FATLOTS); // 113 - Sound for 17+ kills
      }

      loadasample(SOUND_SAMPLE_CONGRAT); // 118 - Sound for championship win 
    } else {
      // Winner mode but not championship
      loadasample(SOUND_SAMPLE_ENGINE); // 0 - Engine sound
      loadasample(SOUND_SAMPLE_ENGINE2); // 1 - Engine sound 2
      loadasample(SOUND_SAMPLE_SKID1); // 3 - Skid sound
      loadasample(SOUND_SAMPLE_LANDSKID); // 9 - Landskid sound
      loadasample(SOUND_SAMPLE_LIGHTLAN); // 10 - Light land sound
      loadasample(SOUND_SAMPLE_BANK); // 11 - Bank sound
      loadasample(SOUND_SAMPLE_WALL1); // 12 - Wall sound
      loadasample(SOUND_SAMPLE_WON); // 114
      loadasample(SOUND_SAMPLE_STAT); // 115
      loadasample(SOUND_SAMPLE_NEWLAP); // 116
      loadasample(SOUND_SAMPLE_NEWFAST); // 117

      int iKills = total_kills[result_order[0]];
      if (iKills > 0 && iKills < 17) {
        loadasample(iKills + (SOUND_SAMPLE_ONE - 1)); // Sound for kill count // 96
      } else if (iKills >= 17) {
        loadasample(SOUND_SAMPLE_FATLOTS); // 113 // Sound for 17+ kills
      }

      loadasample(SOUND_SAMPLE_CONGRAT); // 118 - Sound for winner mode
    }
  } else {
      // Normal mode
    if (numsamples > 0) {
      int iSampleIdx = 0;
      int iDesignCmp = -71;

      while (iWinnerSample < numsamples) {
        if (winner_mode != 0) break;

        if (iWinnerSample < 71 || iWinnerSample > 78) {
          if (SamplePtr[iSampleIdx] != NULL) {
            goto next;
          }
        } else if (iWinnerSample >= 89) {
          goto next;
        }

        int iViewIdx = ViewType[0];
        int iDesign = Car[iViewIdx].byCarDesignIdx;
        if (iDesign != iDesignCmp) {
          goto next;
        }

        if (SamplePtr[iSampleIdx] != NULL) {
          goto next;
        }

        loadasample(iWinnerSample);

      next:
        iDesignCmp++;
        iWinnerSample++;
        iSampleIdx++;
      }
    }
  }

  // Reset sample state
  samplespending = 0;
  writesample = 0;
  readsample = 0;
  lastsample = -10000;
}

//-------------------------------------------------------------------------------------------------

void loadfatalsample()
{
  if (!SamplePtr[SOUND_SAMPLE_FATAL]) // 88 - Fatal sample
    loadasample(SOUND_SAMPLE_FATAL);
  if (!SamplePtr[SOUND_SAMPLE_BUTTON]) // 83 - Button sample
    loadasample(SOUND_SAMPLE_BUTTON);
  if (!SamplePtr[SOUND_SAMPLE_START]) // 87 - Start sample
    loadasample(SOUND_SAMPLE_START);
  if (!SamplePtr[SOUND_SAMPLE_CARIN]) // 84 - Car in sample
    loadasample(SOUND_SAMPLE_CARIN);
  if (!SamplePtr[SOUND_SAMPLE_CAROUT]) // 85 - Car out sample
    loadasample(SOUND_SAMPLE_CAROUT);
  if (!SamplePtr[SOUND_SAMPLE_TRACK]) // 86 - Track sample
    loadasample(SOUND_SAMPLE_TRACK);
  samplespending = 0;
  writesample = 0;
  readsample = 0;
  lastsample = -10000;
}

//-------------------------------------------------------------------------------------------------

void freefatalsample()
{
  fre((void**)&SamplePtr[SOUND_SAMPLE_FATAL]); // 88 - Fatal sample
  fre((void**)&SamplePtr[SOUND_SAMPLE_BUTTON]); // 83 - Button sample
  fre((void**)&SamplePtr[SOUND_SAMPLE_START]); // 87 - Start sample
  fre((void**)&SamplePtr[SOUND_SAMPLE_CARIN]); // 84 - Car in sample
  fre((void**)&SamplePtr[SOUND_SAMPLE_CAROUT]); // 85 - Car out sample
  fre((void**)&SamplePtr[SOUND_SAMPLE_TRACK]); // 86 - Track sample
}

//-------------------------------------------------------------------------------------------------

void releasesamples()
{
  if (SoundCard) {
    for (int i = 0; i < 120; ++i) {
      fre((void**)&SamplePtr[i]);
      SamplePtr[i] = NULL;
    }
  }
}

//-------------------------------------------------------------------------------------------------

void play()
{
  if (musicon) {
    if (SongPtr) {
      MIDIStartSong();

      //InitSong.nSection = __DS__;
      //InitSong.pData = (void *)SongPtr;
      //InitSong.nUnk2 = 0;
      //InitSong.iUnk1 = 0;
      //return sosMIDIStartSong(*(int *)&SongHandle);
    }
  }
}

//-------------------------------------------------------------------------------------------------

void stop()
{
  if (MusicCard) {
    if (SongPtr) {
      MIDIStopSong();
      //sosMIDIStopSong(SongHandle);
      //sosMIDIResetSong(SongHandle);
    }
  }
}

//-------------------------------------------------------------------------------------------------

void devicespecificinit()
{
  int i; // esi
  unsigned int uiSize;

  switch (MusicCard) {
    case 40961:
      printf("Resetting SCC-1...");
      fflush(stdout);
      //sosMIDISendMIDIData(MIDIHandle, 11, (int)&SCreset, __DS__);
      printf("OK\n");
      break;
    case 40962:
    case 40969:
      printf("Loading Instrument bank file...");
      fflush(stdout);
      loadfile("melodic.bnk", &FMInstruments, &uiSize, 0);
      if (FMInstruments) {
        printf("OK\n");
        //sosMIDISetInsData(MIDIHandle, 1, (int)FMInstruments, __DS__);
      } else {
        printf("Failed\n");
        MusicCard = 0;
      }
      printf("Loading Drums bank file...");
      fflush(stdout);
      loadfile("drum.bnk", &FMDrums, &uiSize, 0);
      if (FMDrums) {
        printf("OK\n");
        //sosMIDISetInsData(MIDIHandle, 1, (int)FMDrums, __DS__);
      } else {
        printf("Failed\n");
        MusicCard = 0;
      }
      if (!MusicCard) {
        //sosMIDIUnInitDriver(MIDIHandle, 1);
        //sosMIDIUnInitSystem();
      }
      break;
    case 40964:
      printf("Resetting LAPC-1...");
      //sosMIDISendMIDIData(MIDIHandle, 11, (int)&MT32reset, __DS__);
      fflush(stdout);
      loadfile("mt32map.mtx", &MT32Data, &uiSize, 0);
      if (MT32Data) {
        fflush(stdout);
        for (i = 0; i != 1104; i += 138) {
          //sosMIDISendMIDIData(MIDIHandle, 138, (int)MT32Data + i, __DS__);
          printf(".");
          fflush(stdout);
        }
        printf("OK\n");
      } else {
        printf("Failed\n");
      }
      break;
    default:
      return;
  }
}

//-------------------------------------------------------------------------------------------------

int initgus()
{
  printf("DownLoading  GRAVIS patches...");
  fflush(stdout);
  system("..\\LOADPATS  -Q -IFATALPAT.INI >NUL");
  return printf("Done\n");
}

//-------------------------------------------------------------------------------------------------

void devicespecificuninit()
{
  switch (MusicCard) {
    case 40961:
      //sosMIDISendMIDIData(MIDIHandle, 11, (int)&SCreset, __DS__);
      break;
    case 40962:
    case 40969:
      fre(&FMInstruments);
      fre(&FMDrums);
      break;
    case 40964:
      //sosMIDISendMIDIData(MIDIHandle, 11, (int)&MT32reset, __DS__);
      fre(&MT32Data);
      break;
  }
}

//-------------------------------------------------------------------------------------------------

void readsoundconfig(void)
{
  FILE *fp = ROLLERfopen("../config.ini", "rb");
  char *pBuffer = NULL;
  long iSize;

  if (!fp) return;

  // Determine file size
  fseek(fp, 0, SEEK_END);
  iSize = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  pBuffer = (char *)getbuffer(iSize + 1);
  if (!pBuffer) {
    fclose(fp);
    return;
  }

  fread(pBuffer, 1, iSize, fp);
  pBuffer[iSize] = '\0'; // Null-terminate for string parsing

  // Parse Language
  char *szVar = FindConfigVar(pBuffer, "Language");
  if (szVar) sscanf(szVar, "%s", languagename);

  // Match language index
  language = 0;
  for (int i = 0; i < languages; i++) {
    if (strcmp(languagename, lang[i]) == 0) {
      language = i;
      break;
    }
  }

  // SourcePath
  szVar = FindConfigVar(pBuffer, "SourcePath");
  if (szVar) sscanf(szVar, "%s", SourcePath);

  // DestinationPath
  szVar = FindConfigVar(pBuffer, "DestinationPath");
  if (szVar) sscanf(szVar, "%s", DestinationPath);

  // Sound settings
  szVar = FindConfigVar(pBuffer, "SoundCard");
  uint16 unTemp = 0;
  if (szVar) {
    sscanf(szVar, "%hi", &unTemp);
    SoundCard = unTemp;
  }

  szVar = FindConfigVar(pBuffer, "SoundPort");
  if (szVar) {
    sscanf(szVar, "%hi", &unTemp);
    SoundPort = unTemp;
  }

  szVar = FindConfigVar(pBuffer, "SoundIRQ");
  if (szVar) {
    sscanf(szVar, "%hi", &unTemp);
    SoundIRQ = unTemp;
  }

  szVar = FindConfigVar(pBuffer, "SoundDMA");
  if (szVar) {
    sscanf(szVar, "%hi", &unTemp);
    SoundDMA = unTemp;
  }

  // Music settings
  szVar = FindConfigVar(pBuffer, "MusicCard");
  if (szVar) {
    sscanf(szVar, "%hi", &unTemp);
    MusicCard = unTemp;
  }

  szVar = FindConfigVar(pBuffer, "MusicPort");
  if (szVar) {
    sscanf(szVar, "%hi", &unTemp);
    MusicPort = unTemp;
  }

  // Cleanup
  fre((void**)&pBuffer);
  fclose(fp);

  // Special MusicCard case
  if (MusicCard == 0xCD) {
    MusicCard = 0;
    MusicCD = -1;
  }

  // Set flags
  if (MusicCard == 0 && MusicCD == 0)
    musicon = 0;

  if (SoundCard == 0)
    soundon = 0;
}

//-------------------------------------------------------------------------------------------------

char *FindConfigVar(const char *szConfigText, const char *szVarName)
{
  char *szMatch = strstr(szConfigText, szVarName);
  if (!szMatch)
    return NULL;

  if (szMatch != szConfigText) {
    // check that the match is at the start of a line or preceded by whitespace.
    char cPrev = szMatch[-1];
    if (isalpha((uint8)cPrev) || isdigit((uint8)cPrev) || cPrev == '_')
      return NULL;
  }

  // move 'edx' to point past the matched variable name
  char *szEnd = szMatch;
  uint32 uiLen = (uint32)strlen(szVarName) + 1;
  szEnd += uiLen;

  // skip any trailing spaces
  while (isspace((uint8)(*szEnd + 1)))
    szEnd++;

  // make sure it's followed by '='
  if (szEnd[-1] != '=')
    return NULL;

  return szEnd;
}

//-------------------------------------------------------------------------------------------------

void loadfile(const char *szFile, void **pBuf, unsigned int *uiSize, int iIsSound)
{
  int iFile; // eax
  void *pBuf2; // eax
  uint8 *pUint8Buf; // edx

  *pBuf = 0;
  *uiSize = 0;
  iFile = ROLLERopen(szFile, O_RDONLY | O_BINARY); //0x200 is O_BINARY in WATCOM/h/fcntl.h
  if (iFile == -1) {
    *uiSize = 0;
    *pBuf = 0;
  } else {
    close(iFile);
    *uiSize = getcompactedfilelength(szFile);
    if (!iIsSound || iIsSound == 1 && soundon) {
      pBuf2 = trybuffer(*uiSize);
      *pBuf = pBuf2;
      if (pBuf2) {
        pUint8Buf = (uint8 *)pBuf2;
        initmangle(szFile);
        loadcompactedfilepart(pUint8Buf, *uiSize);
        uninitmangle();
      }
    }
  }
}

//-------------------------------------------------------------------------------------------------

void ReadJoys(tJoyPos *pJoy)
{
  // Process events to update controller state
  SDL_PumpEvents();

  // Check joystick 1 (controller 1)
  pJoy->iX1Status = g_pController1 ? 1 : 0;
  pJoy->iY1Status = g_pController1 ? 1 : 0;
  pJoy->iX1Count = GetAxisValue(g_pController1, SDL_GAMEPAD_AXIS_LEFTX);
  pJoy->iY1Count = GetAxisValue(g_pController1, SDL_GAMEPAD_AXIS_LEFTY);

  // Check joystick 2 (controller 2)
  pJoy->iX2Status = g_pController2 ? 1 : 0;
  pJoy->iY2Status = g_pController2 ? 1 : 0;
  pJoy->iX2Count = GetAxisValue(g_pController2, SDL_GAMEPAD_AXIS_LEFTX);
  pJoy->iY2Count = GetAxisValue(g_pController2, SDL_GAMEPAD_AXIS_LEFTY);

  // Update presence flags based on controller status
  x1ok = (g_pController1 != NULL);
  y1ok = (g_pController1 != NULL);
  x2ok = (g_pController2 != NULL);
  y2ok = (g_pController2 != NULL);

  // Update global acceptance mask
  bitaccept = (x1ok << 0) | (y1ok << 1) | (x2ok << 2) | (y2ok << 3);

  /*unsigned __int8 byCurrentVal; // al
  int i; // ebx
  int v4; // eax
  int iX2Count; // esi
  int iY1Ok; // ecx
  int iY2Ok; // edi
  int iCurrentVal; // eax
  int iX1Ok2; // ebx
  int iX2Ok; // [esp+4h] [ebp-2Ch]
  int iY2Count; // [esp+8h] [ebp-28h]
  int iX1Ok; // [esp+Ch] [ebp-24h]
  int iX1Count; // [esp+10h] [ebp-20h]
  int iY1count; // [esp+14h] [ebp-1Ch]

  // Read initial joystick state
  byCurrentVal = __inbyte(0x201u);
  i = 0;
  iX1Count = 0;
  iY1count = 0;
  v4 = (int)(unsigned __int8)~byCurrentVal >> 4;
  iY2Count = 0;
  pJoyPos->iX1Status = v4 & 1;
  pJoyPos->iY1Status = (v4 & 3) >> 1;
  iX2Count = 0;
  pJoyPos->iX2Status = (v4 >> 2) & 1;
  pJoyPos->iY2Status = ((v4 >> 2) & 3) >> 1;
  _disable();
  __outbyte(0x201u, 0xFFu);

  // Store current axis presence flags
  iX1Ok = x1ok;
  iY1Ok = y1ok;
  iX2Ok = x2ok;
  iY2Ok = y2ok;

  // Main measurement loop
  do {
    iCurrentVal = 0;
    LOBYTE(iCurrentVal) = __inbyte(0x201u);

    // Measure X1 axis
    if (x1ok) {
      iX1Ok = iCurrentVal & 1;
      if ((iCurrentVal & 1) != 0)             // Axis still active
        ++iX1Count;
    }

    // Measure Y1 axis
    if (y1ok) {
      iY1Ok = iCurrentVal & 2;
      if ((iCurrentVal & 2) != 0)             // Axis still active
        ++iY1count;
    }

    // Measure X2 axis
    if (x2ok) {
      iX2Ok = iCurrentVal & 4;
      if ((iCurrentVal & 4) != 0)             // Axis still active
        ++iX2Count;
    }

    // Measure Y2 axis
    if (y2ok) {
      iY2Ok = iCurrentVal & 8;
      if ((iCurrentVal & 8) != 0)             // Axis still active
        ++iY2Count;
    }
    ++i;
  } while ((iCurrentVal & bitaccept) != 0 && i < 10000);
  // Exit conditions:
  // 1. All axes have discharged
  // 2. Reached maximum loop count (10,000)

  // re-enable interrupts
  iX1Ok2 = iX1Ok;
  _enable();

  // Process results for X1 axis
  if (iX1Ok2) {
    pJoyPos->iX1Count = 0;
    x1ok = 0;                                   // mark axis as unavailable
    bitaccept = y2ok | y1ok | x2ok;             // Update global acceptance mask
  } else {
    pJoyPos->iX1Count = iX1Count;
  }

  // Process results for Y1 axis
  if (iY1Ok) {
    pJoyPos->iY1Count = 0;
    y1ok = 0;                                   // mark axis as unavailable
    bitaccept = y2ok | x1ok | x2ok;             // Update global acceptance mask
  } else {
    pJoyPos->iY1Count = iY1count;
  }

  // Process results for X2 axis
  if (iX2Ok) {
    pJoyPos->iX2Count = 0;
    x2ok = 0;                                   // Mark axis as unavailable
    bitaccept = y2ok | y1ok | x1ok;             // Update global acceptance mask
  } else {
    pJoyPos->iX2Count = iX2Count;
  }

  // Process results for Y2 axis
  if (iY2Ok) {
    pJoyPos->iY2Count = 0;
    y2ok = 0;                                   // Mark axis as unavailable
    bitaccept = x2ok | y1ok | x1ok;             // Update global acceptance mask
  } else {
    pJoyPos->iY2Count = iY2Count;
  }*/
}

//-------------------------------------------------------------------------------------------------

int check_joystickpresence()
{
  return 0; /*
  int v0; // ebx
  int v1; // esi
  int v2; // ecx
  int v3; // edi
  int v4; // eax
  int result; // eax
  int v6; // [esp+0h] [ebp-18h]

  v0 = 0;
  x1ok = 1;
  y1ok = 2;
  x2ok = 4;
  y2ok = 8;
  bitaccept = 15;
  JAXmin = 10000;
  JAXmax = -10000;
  JAYmin = 10000;
  JAYmax = -10000;
  JBXmin = 10000;
  JBXmax = -10000;
  JBYmin = 10000;
  JBYmax = -10000;
  __outbyte(0x201u, 0xFFu);
  v6 = x1ok;
  v1 = y1ok;
  v2 = x2ok;
  v3 = y2ok;
  do {
    v4 = 0;
    LOBYTE(v4) = __inbyte(0x201u);
    if (x1ok)
      v6 = v4 & 1;
    if (y1ok)
      v1 = v4 & 2;
    if (x2ok)
      v2 = v4 & 4;
    if (y2ok)
      v3 = v4 & 8;
    ++v0;
  } while ((v4 & bitaccept) != 0 && v0 < 10000);
  if (v6)
    x1ok = 0;
  if (v2)
    x2ok = 0;
  if (v1)
    y1ok = 0;
  if (v3)
    y2ok = 0;
  result = y2ok;
  bitaccept = y2ok | y1ok | x1ok | x2ok;
  return result;*/
}

//-------------------------------------------------------------------------------------------------

int initsounds()
{
  return 0; /*
  int v0; // ebx
  int result; // eax
  int i; // ecx
  int v3; // edx

  v0 = numcars;
  result = 8;
  HandleSample = -1;
  HandleCar = -1;
  HandleSample_variable_1 = -1;
  HandleCar_variable_1 = -1;
  do {
    result += 20;
    *(int *)((char *)&HandleCar_variable_2 + result) = -1;
    *(int *)((char *)&SamplePending_variable_3 + result) = -1;
    *(int *)((char *)&HandleCar_variable_3 + result) = -1;
    *(int *)((char *)&SamplePending_variable_4 + result) = -1;
    *(int *)((char *)&HandleCar_variable_4 + result) = -1;
    *(int *)((char *)&SamplePending_variable_5 + result) = -1;
    *(int *)((char *)&HandleCar_variable_5 + result) = -1;
    *(int *)((char *)&SamplePending_variable_6 + result) = -1;
    *(int *)((char *)&HandleCar_variable_6 + result) = -1;
    *(int *)((char *)&SamplePending_variable_7 + result) = -1;
  } while (result != 128);
  for (i = 0; i != 7680; i += 64) {
    v3 = 0;
    if (v0 > 0) {
      result = i;
      do {
        result += 4;
        ++v3;
        *(int *)((char *)&SamplePtr_variable_15 + result) = -1;
      } while (v3 < v0);
    }
  }
  if (v0 > 0) {
    result = 0;
    do {
      result += 4;
      *(int *)((char *)&SampleExt_variable_1 + result) = 0;
    } while (result < 4 * v0);
  }
  numcars = v0;
  return result;*/
}

//-------------------------------------------------------------------------------------------------

int stopallsamples()
{
  return 0; /*
  int i; // edx
  int v1; // ebx
  int v2; // ebp
  int v3; // eax
  int result; // eax
  int j; // esi
  int v6; // ecx
  int v7; // ebx
  int v8; // ecx

  _disable();
  for (i = 0; i != 896; i += 28) {
    v1 = 0;
    if (numcars > 0) {
      v2 = numcars;
      v3 = i;
      do {
        *(int *)((char *)&enginedelay_variable_1 + v3) = -1;
        *(int *)((char *)&enginedelay_variable_3 + v3) = -1;
        *(int *)((char *)&enginedelay_variable_5 + v3) = -1;
        ++v1;
        v3 += 896;
      } while (v1 < v2);
    }
  }
  result = numcars;
  for (j = 0; j < numcars; ++j) {
    v6 = 0;
    if (numsamples > 0) {
      v7 = j;
      do {
        if (SampleHandleCar[v7] != -1)
          sosDIGIStopSample(DIGIHandle);
        v7 += 16;
        result = numsamples;
        ++v6;
        SamplePtr_variable_14[v7] = -1;
      } while (v6 < result);
    }
  }
  v8 = 2;
  HandleSample = -1;
  HandleCar = -1;
  HandleSample_variable_1 = -1;
  HandleCar_variable_1 = -1;
  do {
    v8 += 5;
    HandleCar_variable_2[v8] = -1;
    SamplePending_variable_3[v8] = -1;
    HandleCar_variable_3[v8] = -1;
    SamplePending_variable_4[v8] = -1;
    HandleCar_variable_4[v8] = -1;
    SamplePending_variable_5[v8] = -1;
    HandleCar_variable_5[v8] = -1;
    SamplePending_variable_6[v8] = -1;
    HandleCar_variable_6[v8] = -1;
    SamplePending_variable_7[v8] = -1;
  } while (v8 != 32);
  lastsample = -1000;
  _enable();
  return result;*/
}

//-------------------------------------------------------------------------------------------------

int pannedsample(int result, int a2, int a3)
{
  return 0; /*
  int v3; // esi
  int v4; // edi
  int v6; // ebp
  unsigned int v7; // ebx
  int v8; // eax
  unsigned int v9; // ebx
  unsigned int v10; // ecx

  v3 = result;
  v4 = a2;
  if (soundon && SamplePtr[result]) {
    v6 = result << 6;
    v7 = SampleHandleCar[16 * result];
    if (v7 != -1) {
      _disable();
      a2 = SampleHandleCar[16 * result];
      if (!sosDIGISampleDone(DIGIHandle, v7)) {
        a2 = v7;
        sosDIGIStopSample(DIGIHandle);
        *(int *)((char *)SampleHandleCar + v6) = -1;
        HandleSample[v7] = -1;
      }
      _enable();
    }
    SamplePanned_variable_1 = __DS__;
    SamplePanned_variable_4 = v3;
    SamplePanned_variable_3 = v4;
    v8 = SamplePtr[v3];
    SamplePanned_variable_5 = a3;
    SamplePanned = v8;
    SamplePanned_variable_2 = SampleLen[v3];
    _disable();
    v9 = sosDIGIStartSample(DIGIHandle, a2, &SamplePanned, __DS__);
    result = v3 << 6;
    SampleHandleCar[16 * v3] = v9;
    _enable();
    if (v9 != -1) {
      if (v9 >= 0x20)
        _assert(0, aS0S32, &aDpsoundC[2], 2071);
      v10 = HandleSample[v9];
      if (v10 != -1) {
        if (v10 >= 0x78)
          _assert(0, &aIHandlesampleS[3], &aDpsoundC[2], 2075);
        if ((unsigned int)HandleCar[v9] >= 0x10)
          _assert(0, aHandlecarS0Han, &aDpsoundC[2], 2076);
        SampleHandleCar[16 * HandleSample[v9] + HandleCar[v9]] = -1;
      }
      result = 0;
      HandleSample[v9] = v3;
      HandleCar[v9] = 0;
    }
  }
  return result;*/
}

//-------------------------------------------------------------------------------------------------

int speechonly(int result, int a2, int a3, int a4)
{
  return 0; /*
  if (readsample == writesample && -a3 > lastsample)
    return speechsample(result, a2, a3, a4);
  return result;*/
}

//-------------------------------------------------------------------------------------------------

int speechsample(int result, int a2, int a3, int a4)
{
  return 0; /*
  int v4; // edi
  int v5; // esi

  v4 = writesample;
  v5 = result;
  if (!disable_messages) {
    if (result == 30) {
      result = ++game_overs;
      if (player_type != 2 || result == 2)
        disable_messages = -1;
    }
    if (!winner_mode || v5 >= 89) {
      result = 16 * writesample;
      *(int *)((char *)&speechinfo + result) = v5;
      *(int *)((char *)&speechinfo_variable_1 + result) = a2;
      *(int *)((char *)&speechinfo_variable_2 + result) = a3;
      ++v4;
      *(int *)((char *)&speechinfo_variable_3 + result) = a4;
      if (v4 == 16)
        v4 = 0;
    }
  }
  writesample = v4;
  return result;*/
}

//-------------------------------------------------------------------------------------------------

int analysespeechsamples()
{
  return 0; /*
  int result; // eax
  int v1; // ecx
  int v2; // esi
  int v3; // edx
  char *v4; // eax
  char *v5; // eax
  int v6; // eax
  int v7; // edx
  int v8; // edi
  int v9; // ecx
  int v10; // eax

  result = readsample;
  --lastsample;
  if (readsample != writesample) {
    result = -speechinfo_variable_2[4 * readsample];
    if (result > lastsample) {
      v1 = (unsigned __int8)speechinfo_variable_3[4 * readsample];
      v2 = speechinfo_variable_3[4 * readsample] >> 8;
      v3 = player_type == 2 && v1 != (__int16)player1_car;
      switch (speechinfo[4 * readsample]) {
        case 14:
          start_zoom(language_buffer_variable_30, v3);
          if (Car_variable_43[308 * v1])
            goto LABEL_13;
          goto LABEL_61;
        case 15:
          v6 = Car_variable_31[308 * v1];
          if (NoOfLaps == v6)
            goto LABEL_15;
          sprintf(&buffer, "%i %s", NoOfLaps + 1 - v6, language_buffer_variable_58);
          v5 = &buffer;
          goto LABEL_60;
        case 16:
        case 17:
        case 18:
        LABEL_15:
          v5 = (char *)&language_buffer_variable_35;
          goto LABEL_60;
        case 21:
          if (Car_variable_23[308 * Victim] <= 0)
            v4 = language_buffer_variable_31;
          else
            v4 = language_buffer_variable_30;
          start_zoom(v4, v3);
          sprintf(&buffer, "%s %s", language_buffer_variable_32, &driver_names[9 * Victim]);
          subzoom(&buffer);
          goto LABEL_61;
        case 22:
          v5 = language_buffer_variable_40;
          goto LABEL_60;
        case 23:
        case 24:
        case 25:
          v5 = (char *)&language_buffer + 64 * (unsigned __int8)Car_variable_32[308 * v1] + 384;
          goto LABEL_60;
        case 26:
          v5 = (char *)&language_buffer_variable_41;
          goto LABEL_60;
        case 30:
          v5 = (char *)&language_buffer_variable_33;
          goto LABEL_60;
        case 37:
        case 38:
          start_zoom(language_buffer_variable_43, v3);
          make_time(&buffer, v3, *(float *)&Car_variable_53[77 * v1]);
          subzoom(&buffer);
          goto LABEL_61;
        case 39:
          start_zoom(language_buffer_variable_44, v3);
          make_time(&buffer, v3, *(float *)&Car_variable_53[77 * v1]);
          subzoom(&buffer);
          goto LABEL_61;
        case 40:
          start_zoom(language_buffer_variable_45, v3);
          make_time(&buffer, v3, *(float *)&Car_variable_53[77 * v1]);
          subzoom(&buffer);
          goto LABEL_61;
        case 43:
          v5 = (char *)&language_buffer_variable_46;
          goto LABEL_60;
        case 44:
          v5 = (char *)&language_buffer_variable_47;
          goto LABEL_60;
        case 45:
          start_zoom(language_buffer_variable_31, v3);
          if (Car_variable_43[308 * v1]) {
          LABEL_13:
            sprintf(
              &buffer,
              "%s %s",
              language_buffer_variable_34,
              &driver_names[9 * (unsigned __int8)Car_variable_41[308 * v1]]);
            subzoom(&buffer);
          }
          goto LABEL_61;
        case 46:
          v5 = (char *)&language_buffer_variable_42;
          goto LABEL_60;
        case 51:
          v5 = (char *)&language_buffer_variable_95;
          goto LABEL_60;
        case 52:
          v5 = (char *)&language_buffer_variable_96;
          goto LABEL_60;
        case 53:
          v5 = (char *)&language_buffer_variable_39;
          goto LABEL_60;
        case 54:
          v5 = (char *)&language_buffer_variable_38;
          goto LABEL_60;
        case 55:
          v5 = (char *)&language_buffer_variable_37;
          goto LABEL_60;
        case 56:
          v5 = (char *)&language_buffer_variable_36;
          goto LABEL_60;
        case 57:
        case 58:
        case 59:
        case 60:
          small_zoom(language_buffer_variable_78);
          subzoom((_BYTE *)&language_buffer + 64 * speechinfo[4 * readsample] + 1408);
          goto LABEL_61;
        case 65:
        case 66:
          small_zoom(language_buffer_variable_77);
          subzoom((_BYTE *)&language_buffer + 64 * v2 + 1152);
          goto LABEL_61;
        case 71:
        case 72:
        case 73:
        case 74:
        case 75:
        case 76:
        case 77:
        case 78:
          small_zoom((_BYTE *)&language_buffer + 64 * speechinfo[4 * readsample] + 768);
          switch (v2) {
            case '=':
            case '>':
            case '?':
            case '@':
              subzoom((_BYTE *)&language_buffer + 64 * v2 + 1152);
              break;
            case 'C':
              subzoom(language_buffer_variable_93);
              break;
            case 'D':
              subzoom(language_buffer_variable_94);
              break;
            case 'E':
              subzoom(language_buffer_variable_92);
              break;
            case 'F':
              subzoom(language_buffer_variable_91);
              break;
            default:
              goto LABEL_61;
          }
          goto LABEL_61;
        case 89:
        case 90:
        case 91:
        case 92:
        case 93:
        case 94:
        case 95:
        case 96:
          v7 = total_wins[champorder[0]];
          if (v7 == 1)
            sprintf(&buffer, "%s", config_buffer_variable_85);
          else
            sprintf(&buffer, "%s %i %s", config_buffer_variable_83, v7, config_buffer_variable_84);
          v5 = &buffer;
          goto LABEL_59;
        case 97:
        case 98:
        case 99:
        case 100:
        case 101:
        case 102:
        case 103:
        case 104:
        case 105:
        case 106:
        case 107:
        case 108:
        case 109:
        case 110:
        case 111:
        case 112:
        case 113:
          if (champ_mode) {
            v8 = total_kills[champorder[0]];
            if (v8 == 1)
              sprintf(&buffer, "%s", config_buffer_variable_80);
            else
              sprintf(&buffer, "%s %i %s", config_buffer_variable_81, v8, config_buffer_variable_82);
            v5 = &buffer;
          } else {
            v9 = result_kills[result_order[0]];
            if (v9 != 1) {
              sprintf(&buffer, "%s %i %s", config_buffer_variable_81, v9, config_buffer_variable_82);
              v3 = 0;
              v5 = &buffer;
              goto LABEL_60;
            }
            v5 = config_buffer_variable_80;
          }
        LABEL_59:
          v3 = 0;
        LABEL_60:
          start_zoom(v5, v3);
        LABEL_61:
          if (soundon) {
            v10 = speechinfo[4 * readsample];
            if (SamplePtr[v10])
              dospeechsample(v10, speechinfo_variable_1[4 * readsample]);
          }
          result = 36 * SampleLen[speechinfo[4 * readsample++]] / 11025;
          lastsample = result;
          if (readsample == 16)
            readsample = 0;
          break;
        case 115:
          if (result_kills[result_order[0]] <= 0 && result_order[0] != FastestLap)
            goto LABEL_61;
          v5 = (char *)&config_buffer_variable_74;
          goto LABEL_59;
        case 116:
          v5 = (char *)&config_buffer_variable_75;
          goto LABEL_59;
        case 117:
          v5 = (char *)&config_buffer_variable_76;
          goto LABEL_59;
        default:
          goto LABEL_61;
      }
    }
  }
  return result;*/
}

//-------------------------------------------------------------------------------------------------

void dospeechsample(int iSampleIdx, int iVolume)
{
  int iUseVolume;
  int iHandle, iSampleHandle;

  // Check sample pointer is valid
  if (SamplePtr[iSampleIdx] == 0) {
    SDL_Log("dospeechsample: Sample pointer is NULL for sample index %d", iSampleIdx);
    return;
  }

  // Clamp volume to 0x7FFF
  if (iVolume > 0x7FFF)
    iVolume = 0x7FFF;

  // Scale volume by speech volume setting (div 127)
  iUseVolume = (SpeechVolume * iVolume) / 127;

  // Abort if sound is off or paused
  if (!soundon || paused)
    return;

  iSampleHandle = SampleHandleCar[iSampleIdx].handles[0];

  // If a sample is already assigned
  if (iSampleHandle != -1) {
    if (!DIGISampleDone(iSampleHandle)) {
      // Stop previous sample if it's still playing
      DIGIStopSample(iSampleHandle);
      SampleHandleCar[iSampleIdx].handles[0] = -1;
      HandleSample[iSampleHandle] = -1;
    }
  }

  // Setup sample parameters
  //SampleFixed.unSegment = __DS__;
  SampleFixed.iSampleIndex = iSampleIdx;
  SampleFixed.iVolume = iUseVolume;
  SampleFixed.pSample = SamplePtr[iSampleIdx];
  SampleFixed.iLength = SampleLen[iSampleIdx];

  // Start sample
  iHandle = DIGISampleStart(&SampleFixed);
  SampleHandleCar[iSampleIdx].handles[0] = iHandle;

  if (iHandle == -1)
    return;

  int iPrevHandle = HandleSample[iHandle];

  // If that handle was previously used for another sample
  if (iPrevHandle != -1)
    SampleHandleCar[iPrevHandle].handles[HandleCar[iHandle]] = -1;

  HandleSample[iHandle] = iSampleIdx;
  HandleCar[iHandle] = 0;
}

//-------------------------------------------------------------------------------------------------

int frontendspeechhandle;
uint8 *frontendspeechptr;
uint32 frontendlen;

void loadfrontendsample(char *fileName)
{
  if (!SoundCard)
    return;

  // Clear any existing frontend speech sample
  if (frontendspeechhandle != -1) {
    DIGIStopSample(frontendspeechhandle);
    frontendspeechhandle = -1;
  }

  // Free any existing frontend speech pointer
  if (frontendspeechptr)
    fre((void**)&frontendspeechptr);

  char szFilenameBuf[32];
  // construct sample filename
  char *szSrc = fileName;
  char *szDst = szFilenameBuf;
  while (*szSrc) {
    *szDst++ = *szSrc++;
    if (!*szSrc) break;
    *szDst++ = *szSrc++;
  }
  *szDst = '\0';

  // convert sample name to real path or cheat version
  convertname(szFilenameBuf);
  SDL_Log("Loading track sample: %s\n", szFilenameBuf);
  // load file into memory
  loadfile(szFilenameBuf, (void**)&frontendspeechptr, &frontendlen, 1);

  if (cheatsample && frontendspeechptr) {
    uint8 *pData = frontendspeechptr;
    int iLen = frontendlen;
    uint32 uiSeed = 0x1C73;
    uint32 uiStep = 0x896;
    int i;

    for (i = 0; i < iLen; ++i) {
      uiSeed += uiStep;
      pData[i] ^= (uint8)uiSeed;
    }
  }

  // if using 50Hz timer
  if (frontendspeechptr && (cheat_mode & CHEAT_MODE_50HZ_TIMER)) {
    if (cheat_mode & CHEAT_MODE_100HZ_TIMER) { //100Hz timer
      // halve the sample length, keep only even-indexed bytes
      int iLen = frontendlen;
      uint8 *pData = frontendspeechptr;
      for (int i = 0; i < iLen; i += 2) {
        pData[i / 2] = pData[i];
      }
      frontendlen = iLen / 2;
    } else {
      // remove every third byte
      int iLen = frontendlen;
      uint8 *pData = frontendspeechptr;
      int iNewLen = 0;
      for (int i = 0; i < iLen; ++i) {
        if (i % 3 != 2) {
          pData[iNewLen++] = pData[i];
        }
      }
      frontendlen = iNewLen;
    }
  }
}

//-------------------------------------------------------------------------------------------------

int frontendsample(int iVol)
{
  if (iVol > 0x7FFF)
    iVol = 0x7FFF;

  // Scale volume using SpeechVolume
  int iScaledVol = (SpeechVolume * iVol) / 127;

  SampleFixed.iSampleIndex = 0;
  SampleFixed.iVolume = iScaledVol;
  SampleFixed.pSample = frontendspeechptr;
  SampleFixed.iLength = frontendlen;

  frontendspeechhandle = DIGISampleStart(&SampleFixed);

  return frontendspeechhandle;
}

//-------------------------------------------------------------------------------------------------

void remove_frontendspeech()
{
  // Clear any existing frontend speech sample
  if (frontendspeechhandle != -1) {
    DIGIStopSample(frontendspeechhandle);
    frontendspeechhandle = -1;
  }

// Free any existing frontend speech pointer
  if (frontendspeechptr)
    fre((void**)&frontendspeechptr);
}

//-------------------------------------------------------------------------------------------------

int sfxplaying(int a1)
{
  return 0; /*
  int v1; // ecx
  unsigned int v2; // ebx
  int result; // eax

  v1 = a1 << 6;
  v2 = SampleHandleCar[16 * a1];
  if (v2 == -1)
    return 0;
  if (!sosDIGISampleDone(DIGIHandle, v2))
    return -1;
  result = 0;
  *(int *)((char *)SampleHandleCar + v1) = -1;
  HandleSample[v2] = -1;
  return result;*/
}

//-------------------------------------------------------------------------------------------------

int cheatsampleok(int a1)
{
  if (a1 == player1_car || a1 == player2_car)
    return -1;
  else
    return 0;
}

//-------------------------------------------------------------------------------------------------

void sfxsample(int iSample, int iVol)
{
  if (SamplePtr[iSample] == 0) {
    SDL_Log("sfxsample: Sample pointer is NULL for sample index %d", iSample);
    return;
  }

  // Clamp volume to maximum 0x7FFF
  if (iVol > 0x7FFF)
    iVol = 0x7FFF;

  // Scale volume using SFXVolume
  int iScaledVol = (SFXVolume * iVol) / 127;

  // Check if sound is disabled or paused
  if (!soundon || paused)
    return;

  // Stop any currently playing sample for this index
  int iOldHandle = SampleHandleCar[iSample].handles[0];
  if (iOldHandle != -1) {
    // cli(); Disable interrupts
    if (!DIGISampleDone(iOldHandle)) {
      DIGIStopSample(iOldHandle);
      SampleHandleCar[iSample].handles[0] = -1;
      HandleSample[iOldHandle] = -1;
    }
    // sti(); Enable interrupts
  }

  // Prepare sample data
  //SampleFixed.unSegment = ds;
  SampleFixed.iSampleIndex = iSample;
  SampleFixed.iVolume = iScaledVol;
  SampleFixed.pSample = SamplePtr[iSample];
  SampleFixed.iLength = SampleLen[iSample];

  // cli(); Disable interrupts
  // Calculate offset in SampleHandleCar array (64 bytes per element)
  //int iHandleOffset = iSample << 6;
  int iNewHandle = DIGISampleStart(&SampleFixed);
  // Store new handle
  SampleHandleCar[iSample].handles[0] = iNewHandle;
  // sti(); Enable interrupts

  // Clear previous sample association if handle was reused
  if (iNewHandle == -1)
    return;
  if (HandleSample[iNewHandle] != -1) {
    int iPrevCar = HandleCar[iNewHandle];
    SampleHandleCar[HandleSample[iNewHandle]].handles[iPrevCar] = -1;
  }

  // Update handle tracking arrays
  HandleSample[iNewHandle] = iSample;
  HandleCar[iNewHandle] = 0; // Always use first slot in handles array
}


//-------------------------------------------------------------------------------------------------

void sample2(int iCarIndex, int iSampleIndex, int iVolume, int iPitch, int iPan, int iByteOffset)
{
  if (!soundon || paused || !SamplePtr[iSampleIndex]) {
    return;
  }

  // Only proceed if no sample is currently playing in this slot
  if (SampleHandleCar[iSampleIndex].handles[iCarIndex] != -1) {
    // Prepare sample data structure
    //SampleData.unSegment = 0;
    SampleData.iSampleIndex = iSampleIndex;
    SampleData.iPitch = iPitch;
    SampleData.pSample = SamplePtr[iSampleIndex];
    SampleData.iLength = SampleLen[iSampleIndex];
    SampleData.iPan = iPan;
    SampleData.iByteOffset = iByteOffset;
    SampleData.iVolume = iVolume;

    // Start sample playback
    int iNewHandle = DIGISampleStart(&SampleData);

    // Store new hhandle
    SampleHandleCar[iSampleIndex].handles[iCarIndex] = iNewHandle;

    if (iNewHandle != -1) {
      // Clear any previous assignment of this handle
      if (HandleSample[iNewHandle] != -1) {
        int iPrevCar = HandleCar[iNewHandle];
        SampleHandleCar[HandleSample[iNewHandle]].handles[iPrevCar] = -1;
      }

      // Update handle tracking arrays
      HandleSample[iNewHandle] = iSampleIndex;
      HandleCar[iNewHandle] = iCarIndex;
    }
  }
}

//-------------------------------------------------------------------------------------------------

int sfxpend(int a1, int a2, int a3)
{
  return 0; /*
  int v5; // edi
  int result; // eax
  int v7; // edi
  int v8; // esi
  int i; // edx
  int v10; // ecx

  v5 = a3;
  if (a3 > 0x7FFF)
    v5 = 0x7FFF;
  if (replaytype != 2) {
    newrepsample[a2] = a1;
    repvolume[a2] = (unsigned __int16)(v5 - (__CFSHL__(v5 >> 31, 8) + ((unsigned __int16)(v5 >> 31) << 8))) >> 8;
  }
  result = v5 * SFXVolume / 127;
  v7 = result;
  if (soundon) {
    result = 4 * a2;
    if (Pending[a2] != 5) {
      v8 = 0;
      result = 160 * a2;
      for (i = 0; i < Pending[a2]; result += 8) {
        if (a1 == *(int *)((char *)&SamplePending + result))
          v8 = -1;
        ++i;
      }
      if (!v8) {
        result = 160 * a2 + 8 * Pending[a2];
        *(int *)((char *)&SamplePending + result) = a1;
        v10 = Pending[a2] + 1;
        *(int *)((char *)&SamplePending_variable_1 + result) = v7;
        Pending[a2] = v10;
      }
    }
  }
  return result;*/
}

//-------------------------------------------------------------------------------------------------

int enginesounds2(int result, int a2)
{
  return 0; /*
  int v2; // ecx
  int v3; // ecx

  if (soundon) {
    delaywritex = delaywrite & 0x1F;
    v2 = 224 * result;
    enginedelay_variable_1[7 * (delaywrite & 0x1F) + v2] = -1;
    enginedelay_variable_3[7 * delaywritex + v2] = -1;
    enginedelay_variable_5[7 * delaywritex + v2] = -1;
    v3 = 224 * a2;
    enginedelay_variable_1[7 * delaywritex + v3] = -1;
    enginedelay_variable_3[7 * delaywritex + v3] = -1;
    enginedelay_variable_5[7 * delaywritex + v3] = -1;
    enginesound(0.0, 0.0, 2000.0, 0x8000);
    enginesound(0.0, 0.0, 2000.0, 0x8000);
    return delaywrite++;
  }
  return result;*/
}

//-------------------------------------------------------------------------------------------------

int enginesounds(int result)
{
  return 0; /*
  int v1; // edi
  unsigned int v2; // eax
  int v3; // ebp
  float *v4; // eax
  int v5; // edx
  float *v6; // edx
  int v7; // ebx
  double v8; // st7
  double v9; // st6
  int v10; // ebp
  double v11; // st7
  int v12; // ecx
  float *v13; // ebx
  int v14; // eax
  int v15; // esi
  int v16; // esi
  __int64 v17; // rax
  int v18; // eax
  int v19; // esi
  double v20; // st7
  double v21; // st6
  int v22; // edx
  int v23; // eax
  int v24; // eax
  double v25; // st7
  int v26; // [esp+0h] [ebp-9Ch]
  int v27; // [esp+4h] [ebp-98h] BYREF
  int v28; // [esp+8h] [ebp-94h] BYREF
  int v29; // [esp+10h] [ebp-8Ch] BYREF
  float v30; // [esp+14h] [ebp-88h]
  float v31; // [esp+18h] [ebp-84h]
  float v32; // [esp+20h] [ebp-7Ch]
  float v33; // [esp+24h] [ebp-78h]
  float v34; // [esp+28h] [ebp-74h]
  float v35; // [esp+2Ch] [ebp-70h]
  float v36; // [esp+30h] [ebp-6Ch]
  float v37; // [esp+34h] [ebp-68h]
  float v38; // [esp+38h] [ebp-64h]
  float v39; // [esp+3Ch] [ebp-60h]
  float v40; // [esp+40h] [ebp-5Ch]
  int v41; // [esp+44h] [ebp-58h]
  float v42; // [esp+48h] [ebp-54h]
  int v43; // [esp+4Ch] [ebp-50h]
  float v44; // [esp+50h] [ebp-4Ch]
  float v45; // [esp+54h] [ebp-48h]
  float v46; // [esp+58h] [ebp-44h]
  float v47; // [esp+5Ch] [ebp-40h]
  float v48; // [esp+60h] [ebp-3Ch]
  float v49; // [esp+64h] [ebp-38h]
  float v50; // [esp+68h] [ebp-34h]
  int v51; // [esp+6Ch] [ebp-30h]
  int v52; // [esp+70h] [ebp-2Ch]
  int v53; // [esp+74h] [ebp-28h]
  float v54; // [esp+78h] [ebp-24h]
  float v55; // [esp+7Ch] [ebp-20h]
  float v56; // [esp+80h] [ebp-1Ch]

  v1 = result;
  if (soundon) {
    delaywritex = delaywrite & 0x1F;
    if (numcars > 0) {
      v2 = 0;
      v3 = 896 * numcars;
      do {
        enginedelay_variable_1[7 * delaywritex + v2 / 4] = -1;
        enginedelay_variable_3[7 * delaywritex + v2 / 4] = -1;
        v2 += 896;
        SamplePending_variable_2[7 * delaywritex + v2 / 4] = -1;
      } while ((int)v2 < v3);
    }
    if (DriveView[0] == 3 || DriveView[0] == 6) {
      if (DriveView[0] == 6) {
        v42 = -localdata_variable_16;
        *(float *)&v43 = -localdata_variable_17;
        v10 = 2;
        *(float *)&v41 = sound_c_variable_37 - localdata_variable_18;
      } else {
        v10 = TowerBase[5 * NearTow];
        v42 = *(float *)&TowerGx;
        v43 = TowerGy;
        v41 = TowerGz;
      }
      v39 = 0.0;
      v38 = 0.0;
      v40 = 0.0;
    } else {
      enginesound(0.0, 0.0, 2000.0, 0x8000);
      v4 = &Car[77 * v1];
      v5 = *((__int16 *)v4 + 6);
      if (v5 == -1) {
        v42 = *v4;
        v43 = *((int *)v4 + 1);
        v41 = *((int *)v4 + 2);
        v39 = v4[10];
        v38 = v4[11];
        v40 = v4[12];
      } else {
        v6 = (float *)((char *)&localdata + 128 * v5);
        v42 = v6[1] * v4[1] + *v6 * *v4 + v6[2] * v4[2] - v6[9];
        *(float *)&v43 = v6[4] * v4[1] + v6[3] * *v4 + v6[5] * v4[2] - v6[10];
        *(float *)&v41 = v6[7] * v4[1] + v6[6] * *v4 + v6[8] * v4[2] - v6[11];
        v7 = *((_DWORD *)v4 + 16);
        v8 = v4[6] * tcos[v7];
        v9 = v4[6] * tsin[v7];
        v39 = v6[1] * v9 + *v6 * v8;
        v38 = v6[3] * v8 + v6[4] * v9;
        v40 = v8 * v6[6] + v9 * v6[7];
      }
      v10 = *((__int16 *)v4 + 6);
      if (v10 == -1)
        v10 = *((__int16 *)v4 + 7);
    }
    if (DriveView[0] != 3 && DriveView[0] != 6) {
      if (Car_variable_17[77 * v1] == 3) {
        getworldangles(&v28, &v29, &v27);
      } else {
        v28 = Car_variable_7[154 * v1];
        v29 = Car_variable_6[154 * v1];
        v27 = Car_variable_5[154 * v1];
      }
      v32 = tcos[v28] * tcos[v29];
      v37 = tcos[v28] * tsin[v29] * tsin[v27] - tsin[v28] * tcos[v27];
      v35 = tsin[v28] * tcos[v29];
      v36 = tsin[v28] * tsin[v29] * tsin[v27] + tcos[v28] * tcos[v27];
      v11 = -tsin[v27] * tcos[v29];
      v34 = tsin[v29];
      v33 = v11;
    }
    v12 = 0;
    if (numcars > 0) {
      v13 = Car;
      do {
        v14 = -1;
        if (Car_variable_23[308 * v12] < 0)
          v14 = 0;
        if (DriveView[0] == 3 || DriveView[0] == 6) {
          if (v12 == v1 || allengines)
            goto LABEL_34;
        } else if (v12 != v1 && allengines) {
          goto LABEL_34;
        }
        v14 = 0;
      LABEL_34:
        if (v14) {
          v15 = Car_variable_3[154 * v12];
          if (v15 == -1)
            v15 = Car_variable_4[154 * v12];
          v16 = v15 - v10;
          if (v16 < 0)
            v16 += TRAK_LEN;
          if (v16 > TRAK_LEN / 2)
            v16 -= TRAK_LEN;
          v17 = v16;
          if ((int)abs32(v16) >= 40) {
            enginesound(0.0, 0.0, 1048576.0, 0x8000);
          } else {
            v18 = *((__int16 *)v13 + 6);
            if (v18 == -1) {
              v47 = *v13;
              v55 = v13[1];
              v49 = v13[2];
              v56 = v13[10];
              v46 = v13[11];
              *(float *)&v17 = v13[12];
              v48 = *(float *)&v17;
            } else {
              LODWORD(v17) = (char *)&localdata + 128 * v18;
              v47 = *(float *)(v17 + 4) * v13[1]
                + *(float *)v17 * *v13
                + *(float *)(v17 + 8) * v13[2]
                - *(float *)(v17 + 36);
              v55 = *(float *)(v17 + 16) * v13[1]
                + *(float *)(v17 + 12) * *v13
                + *(float *)(v17 + 20) * v13[2]
                - *(float *)(v17 + 40);
              v49 = *(float *)(v17 + 28) * v13[1]
                + *(float *)(v17 + 24) * *v13
                + *(float *)(v17 + 32) * v13[2]
                - *(float *)(v17 + 44);
              v19 = *((_DWORD *)v13 + 16);
              v20 = v13[6] * tcos[v19];
              v21 = v13[6] * tsin[v19];
              v56 = *(float *)(v17 + 4) * v21 + *(float *)v17 * v20;
              v46 = *(float *)(v17 + 12) * v20 + *(float *)(v17 + 16) * v21;
              v48 = v20 * *(float *)(v17 + 24) + v21 * *(float *)(v17 + 28);
            }
            v54 = v47 - v42;
            *(float *)&v52 = v55 - *(float *)&v43;
            *(float *)&v53 = v49 - *(float *)&v41;
            v30 = v54 * v54 + *(float *)&v52 * *(float *)&v52;
            v50 = *(float *)&v53 * *(float *)&v53 + v30;
            v30 = sqrt(v30);
            v50 = sqrt(v50);
            v22 = getangle(v17, SHIDWORD(v17), v54, v52);
            v23 = getangle(v22, v22, v30, v53);
            v44 = (v39 * tcos[v22] + v38 * tsin[v22]) * tcos[v23] + v40 * tsin[v23];
            v45 = -(v56 * tcos[v22] + v46 * tsin[v22]) * tcos[v23] + v48 * tsin[v23];
            if (DriveView[0] == 3 || DriveView[0] == 6) {
              v51 = 0x8000;
            } else {
              v31 = v54 * v32 + *(float *)&v52 * v35 + *(float *)&v53 * v34;
              *(float *)&v26 = v54 * v37 + *(float *)&v52 * v36 + *(float *)&v53 * v33;
              v24 = getangle(v23, v22, v31, v26);
              v25 = (1.0 - tsin[v24]) * sound_c_variable_38;
              _CHP(v24, v22);
              v51 = (int)v25;
              if ((int)v25 >= (int)cstart_branch_1)
                v51 = 0xFFFF;
            }
            enginesound(v44, v45, v50, v51);
          }
        }
        ++v12;
        v13 += 77;
      } while (v12 < numcars);
    }
    return delaywrite++;
  }
  return result;*/
}

//-------------------------------------------------------------------------------------------------

void loopsample(int iCarIdx, int iSampleIdx, int iVolume, int iPitch, int iPan)
{
  if (!soundon || !SamplePtr[iSampleIdx]) {
    return;
  }

  // Calculate handle array position
  int iHandle = SampleHandleCar[iSampleIdx].handles[iCarIdx];

  if (iHandle != -1) {
    if (iVolume == 0) {
      // Stop playing sample
      //sosDIGIStopSample(DIGIHandle, iHandle);
      DIGIStopSample(iHandle);
      HandleSample[iHandle] = -1;
      SampleHandleCar[iSampleIdx].handles[iCarIdx] = -1;
      return;
    } else {
      // Clamp pitch value between 0x800 and 0x80000
      if (iPitch < 0x800) iPitch = 0x800;
      if (iPitch > 0x80000) iPitch = 0x80000;

      // Convert parameters to fixed-point format
      int iVolFixed = iVolume >> 10;        // Convert to 10.6 fixed-point
      int iPitchFixed = iPitch >> 10;       // Convert to 10.6 fixed-point
      int iPanFixed = iPan >> 12;           // Convert to 4.12 fixed-point

      // Update volume if changed
      if (iVolFixed != lastvolume[iCarIdx]) {
        //sosDIGISetSampleVolume(DIGIHandle, handle, iVolFixed << 10);
        lastvolume[iCarIdx] = iVolFixed;
      }

      // Update pitch if changed
      if (iPitchFixed != lastpitch[iCarIdx]) {
        //sosDIGISetPitch(DIGIHandle, handle, iPitchFixed << 10);
        lastpitch[iCarIdx] = iPitchFixed;
      }

      // Update pan if changed
      if (iPanFixed != lastpan[iCarIdx]) {
        //sosDIGISetPanLocation(DIGIHandle, handle, iPanFixed << 12);
        lastpan[iCarIdx] = iPanFixed;
      }
    }
  } else if (iVolume != 0) {
    // Start new sample playback - calculate byte offset for audio system
    int iByteOffset = (iSampleIdx << 6) + (iCarIdx << 2);
    sample2(iCarIdx, iSampleIdx, iVolume, iPitch, iPan, iByteOffset);
  }
}

//-------------------------------------------------------------------------------------------------

void enginesound(int a1, float a2, float a3, float a4, int a5)
{/*
  int v8; // ebp
  int v9; // edx
  __int16 v10; // fps
  double v11; // st7
  _BOOL1 v12; // c0
  char v13; // c2
  _BOOL1 v14; // c3
  double v15; // st7
  int v16; // ebx
  int v17; // edx
  int v18; // eax
  int v19; // eax
  __int16 v20; // fps
  _BOOL1 v21; // c0
  char v22; // c2
  _BOOL1 v23; // c3
  int v24; // edx
  double v25; // st7
  double v26; // st7
  int v27; // eax
  int v28; // eax
  double v29; // st7
  int v30; // eax
  int v31; // edx
  int v32; // edx
  int v33; // edx
  double v34; // st7
  float v35; // eax
  int v36; // edx
  double v37; // st7
  __int64 v38; // rax
  int v39; // ebx
  double v41; // st7
  double v42; // st7
  int v43; // edx
  double v44; // st7
  int v45; // eax
  double v46; // st7
  int v47; // ecx
  int v48; // ebp
  int v49; // esi
  int v50; // edx
  int v51; // ecx
  int v52; // eax
  double v53; // st7
  float v54; // [esp+Ch] [ebp-4Ch]
  float v55; // [esp+10h] [ebp-48h]
  int v56; // [esp+14h] [ebp-44h]
  int v57; // [esp+18h] [ebp-40h]
  float v58; // [esp+20h] [ebp-38h]
  int v59; // [esp+24h] [ebp-34h]
  int v60; // [esp+24h] [ebp-34h]
  int v61; // [esp+24h] [ebp-34h]
  int v62; // [esp+28h] [ebp-30h]
  int v63; // [esp+28h] [ebp-30h]
  int v64; // [esp+28h] [ebp-30h]
  int v65; // [esp+28h] [ebp-30h]
  int v66; // [esp+2Ch] [ebp-2Ch]
  float v67; // [esp+30h] [ebp-28h]
  float v68; // [esp+34h] [ebp-24h]
  float v69; // [esp+38h] [ebp-20h]
  int v70; // [esp+3Ch] [ebp-1Ch]

  v8 = a1;
  if (soundon) {
    v55 = (a2 + sound_c_variable_39) / (sound_c_variable_39 - a3);
    v9 = 77 * a1;
    v67 = sound_c_variable_40 / (a4 * a4 + sound_c_variable_40);
    v11 = Car_variable_28[77 * a1];
    v12 = v11 < sound_c_variable_41;
    v13 = 0;
    v14 = v11 == sound_c_variable_41;
    LOWORD(a1) = v10;
    if (v11 >= sound_c_variable_41) {
      v59 = 258 * EngineVolume;
    } else {
      v15 = Car_variable_28[v9] * sound_c_variable_42 * sound_c_variable_43 * (double)EngineVolume;
      _CHP(a1, v9 * 4);
      v59 = (int)v15;
    }
    v16 = Car_variable_72[77 * v8];
    v17 = 8 * ((v16 * v59) >> 31);
    v18 = (v16 * v59 - (__CFSHL__((v16 * v59) >> 31, 3) + v17)) >> 3;
    v60 = v18;
    if (v18 > 0x7FFF)
      v60 = 0x7FFF;
    _CHP(v18, v17);
    v61 = (int)((double)v60 * v67);
    if (v61 < 256)
      v61 = 0;
    v21 = a4 < (double)sound_c_variable_44;
    v22 = 0;
    v23 = a4 == sound_c_variable_44;
    LOWORD(v19) = v20;
    if (a4 >= (double)sound_c_variable_44)
      v61 = 0;
    v24 = 77 * v8;
    v25 = Car_variable_28[77 * v8] * sound_c_variable_45 + sound_c_variable_46;
    _CHP(v19, 308 * v8);
    v26 = Car_variable_73[v24] * sound_c_variable_47 / (Car_variable_73[v24] + sound_c_variable_48) + (double)(int)v25;
    _CHP(v27, 308 * v8);
    v58 = (Car_variable_9[77 * v8] + sound_c_variable_49) * sound_c_variable_50;
    if (v58 > 1.0)
      v58 = 1.0;
    v28 = Car_variable_74[77 * v8];
    v29 = tsin[v28] * (1.0 - v58) * sound_c_variable_51 + (double)(6 * Car_variable_36[77 * v8] + (int)v26);
    _CHP(v28, v24 * 4);
    _CHP(v30, v24 * 4);
    v56 = 0;
    v31 = Car_variable_3[154 * v8];
    v57 = Car_variable_50[77 * v8];
    v66 = (int)(v29 * v55);
    if (v31 != -1 && (TrakColour_variable_1[12 * v31 + 2 * v57] & 0x2000) != 0) {
      v56 = -1;
      v68 = localdata_variable_4[32 * v31] + Car[77 * v8];
      v32 = v31 - 1;
      if (!v32)
        v32 = TRAK_LEN - 1;
      while ((TrakColour_variable_1[12 * v32 + 2 * v57] & 0x2000) != 0 && v68 < (double)sound_c_variable_52) {
        v68 = sound_c_variable_61 * localdata_variable_4[32 * v32--] + v68;
        if (!v32)
          v32 = TRAK_LEN - 1;
      }
      v33 = Car_variable_3[154 * v8] + 1;
      v69 = localdata_variable_4[32 * Car_variable_3[154 * v8]] - Car[77 * v8];
      if (v33 == TRAK_LEN)
        v33 ^= TRAK_LEN;
      while ((TrakColour_variable_1[12 * v33 + 2 * v57] & 0x2000) != 0 && v69 < (double)sound_c_variable_52) {
        v34 = sound_c_variable_61 * localdata_variable_4[32 * v33++] + v69;
        v69 = v34;
        if (v33 == TRAK_LEN)
          v33 ^= TRAK_LEN;
      }
      if (v69 >= (double)v68)
        v35 = v68;
      else
        v35 = v69;
      v54 = v35;
      if (v35 > (double)sound_c_variable_52)
        v54 = 6400.0;
    }
    v36 = 224 * v8;
    enginedelay_variable_6[224 * v8 + 7 * delaywritex] = a5;
    enginedelay[224 * v8 + 7 * delaywritex] = v66;
    enginedelay_variable_1[7 * delaywritex + v36] = v61;
    enginedelay_variable_2[7 * delaywritex + v36] = v66;
    if (v56) {
      v37 = v54 * sound_c_variable_53 * sound_c_variable_54 * (double)v61;
      _CHP(v66, v36 * 4);
      enginedelay_variable_3[7 * delaywritex + v36] = (int)v37;
    } else {
      enginedelay_variable_3[7 * delaywritex + v36] = 0;
    }
    if (Car_variable_17[77 * v8] != 3
      || (HIDWORD(v38) = Car_variable_7[154 * v8], v39 = Car_variable_15[77 * v8], HIDWORD(v38) == v39)) {
      v65 = 0;
    } else {
      LODWORD(v38) = HIDWORD(v38) - v39 + (HIDWORD(v38) - v39 < 0 ? 0x4000 : 0);
      if ((int)v38 > 0x2000)
        LODWORD(v38) = v38 - 0x4000;
      v38 = (int)v38;
      LODWORD(v38) = abs32(v38);
      if ((int)v38 >= 200) {
        v41 = (double)((int)v38 - 400) * sound_c_variable_55 * sound_c_variable_56 * sound_c_variable_57;
        _CHP(v38 - 400, HIDWORD(v38));
        v62 = (int)v41;
      } else {
        v62 = 0;
      }
      v42 = (double)v62 * Car_variable_8[77 * v8] * sound_c_variable_58;
      _CHP(308 * v8, HIDWORD(v38));
      v63 = (int)v42;
      if ((int)v42 > 0x7FFF)
        v63 = 0x7FFF;
      v43 = SFXVolume * v63 % 127;
      v64 = SFXVolume * v63 / 127;
      _CHP(v64, v43);
      v65 = (int)((double)v64 * v67);
      if (v65 < 256)
        v65 = 0;
      v44 = v55 * sound_c_variable_59;
      _CHP(308 * v8, v43);
      v46 = (*(float *)((char *)Car_variable_8 + v45) + sound_c_variable_52) * sound_c_variable_60 * (double)(int)v44;
      _CHP(v45, v43);
      v66 = (int)v46;
    }
    enginedelay_variable_4[224 * v8 + 7 * delaywritex] = v66;
    enginedelay_variable_5[224 * v8 + 7 * delaywritex] = v65;
    v47 = 5 * v8;
    v48 = v8;
    v49 = 0;
    v50 = Pending[v48];
    v51 = 8 * v47;
    if (v50 > 0) {
      do {
        v52 = SamplePending_variable_1[v51];
        v70 = v52;
        if (v52 > 0x8000)
          v70 = 0x8000;
        v53 = (double)v70 * v67;
        _CHP(v52, v50);
        if ((int)v53 >= 256) {
          v50 = (int)v53;
          pannedsample(SamplePending[v51], (int)v53, a5);
        }
        ++v49;
        v51 += 2;
      } while (v49 < Pending[v48]);
    }
    Pending[v48] = 0;
  }*/
}

//-------------------------------------------------------------------------------------------------

void startmusic(int iSong)
{
  int musicOrTitle = 0;
  if (MusicCD && track_playing) {
    StopTrack();
  } else if (MusicCard && SongPtr) {
    stop();
    // sosMIDIUnInitSong(SongHandle);
    SongPtr = 0;
  }

  if (musicon)
    MIDISetMasterVolume(MusicVolume);

  if (MusicCD)
    SetAudioVolume(MusicVolume);

  int iMusic; // Index of the music track to play
  if (iSong >= 0) {
    musicOrTitle = -1;
    iMusic = (nummusictracks + iSong - 1) % nummusictracks;
  } else {
    iMusic = -iSong;
  }

  // int audioInfo;
  int cdSongId; // Track ID for CD audio
  if (MusicCD && musicon) {
    ResetDrive();
    //GetAudioInfo(audioInfo, iMusic, musicOrTitle, MusicCD);
    if (cd_cheat) {
      cdSongId = cd_cheat;
    } else {
      if (musicOrTitle) {
        PlayTrack4(CDSong[iMusic]);
        start_cd = frames;
        return;
      }
      cdSongId = CDSong[iMusic];
    }
    PlayTrack(cdSongId);
    start_cd = frames;
    return;
  }

  if (musicon) {
    uint32_t musiclength;
    loadfile((const char *)&Song[GMSong[iMusic]], (void *)&musicbuffer, &musiclength, 0);
    SongPtr = (int)&musicbuffer;
    if (&musicbuffer) {
      // Init song in the MIDI system
      tInitSong InitSong = {
        .pData = (void *)musicbuffer,
        .iLength = musiclength,
      };
      MIDIInitSong(&InitSong);
      free(musicbuffer);

      // Play the song in the MIDI system
      MIDIStartSong();
    }
  }
  /*
  int v1; // edx
  int v2; // ebx
  int v3; // edx
  int v4; // eax
  int v5; // eax

  v1 = result;
  v2 = 0;
  if (MusicCD && track_playing) {
    result = StopTrack();
  } else if (MusicCard && SongPtr) {
    stop(result, result, 0, MusicCD);
    sosMIDIUnInitSong(SongHandle);
    result = 0;
    SongPtr = 0;
  }
  if (musicon)
    result = sosMIDISetMasterVolume((unsigned __int8)MusicVolume);
  if (MusicCD)
    result = SetAudioVolume(MusicVolume);
  if (v1 >= 0) {
    v2 = -1;
    result = (nummusictracks + v1 - 1) / nummusictracks;
    v3 = (nummusictracks + v1 - 1) % nummusictracks;
  } else {
    v3 = -v1;
  }
  if (MusicCD && musicon) {
    ResetDrive();
    GetAudioInfo();
    if (cd_cheat) {
      v4 = cd_cheat;
    } else {
      if (v2) {
        PlayTrack4(CDSong[v3], v3);
      LABEL_22:
        result = frames;
        start_cd = frames;
        return result;
      }
      v4 = CDSong[v3];
    }
    PlayTrack(v4, v3, v2);
    goto LABEL_22;
  }
  if (musicon) {
    initmangle((char *)&Song + 15 * GMSong[v3]);
    v5 = loadcompactedfilepart(&musicbuffer, 1000000000);
    result = uninitmangle(v5);
    SongPtr = (int)&musicbuffer;
    if (&musicbuffer) {
      InitSong_variable_1 = __DS__;
      InitSong_variable_3 = 0;
      InitSong_variable_2 = 0;
      InitSong = (int)&musicbuffer;
      result = sosMIDIInitSong((char)&SongHandle, (unsigned __int16)__DS__);
      if (musicon) {
        if (SongPtr) {
          InitSong_variable_1 = __DS__;
          InitSong = SongPtr;
          InitSong_variable_2 = 0;
          InitSong_variable_3 = 0;
          return sosMIDIStartSong(SongHandle);
        }
      }
    }
  }
  return result;*/
}

//-------------------------------------------------------------------------------------------------

void stopmusic()
{
  if (MusicCD && track_playing) {
    StopTrack();
  } else if (MusicCard && SongPtr) {
    if (MusicCard) {
      if (SongPtr) {
        //sosMIDIStopSong(SongHandle);
        //sosMIDIResetSong(SongHandle);
        MIDIStopSong();
      }
    }
    //sosMIDIUnInitSong(*(unsigned int *)&SongHandle);
    SongPtr = 0;
  }
}

//-------------------------------------------------------------------------------------------------

void load_language_map()
{
  FILE *pFile;
  int i;

  // open file
  pFile = ROLLERfopen("LANGUAGE.INI", "r");
  if (!pFile) return;

  // read number of languages into languages
  readline(pFile, "i", &languages);

  // check if languages is valid
  if (languages <= 0) {
    fclose(pFile);
    return;
  }

  // Set up pointers to global buffers
  char *szLangPtr = lang[0];
  char *szTextExtPtr = TextExt;
  char *szSampleExtPtr = SampleExt;

  // loop through each language and read settings
  for (i = 0; i < languages; i++) {
    readline(pFile, "ccc", szLangPtr, szTextExtPtr, szSampleExtPtr);

    // Move pointers forward to next slots
    szLangPtr += 32;        // Each lang entry is 32 bytes
    szTextExtPtr += 4;       // Each extension is 4 bytes
    szSampleExtPtr += 4;
  }

  // close the file
  fclose(pFile);
}

//-------------------------------------------------------------------------------------------------

void initmusic()
{
  FILE *pFile = ROLLERfopen("SOUND.INI", "r");
  if (!pFile) return;

  readline(pFile, "i", &numsamples);
  int iSongCount = 0;
  readline(pFile, "i", &iSongCount);
  readline(pFile, "i", &nummusictracks);

  // read sample filenames
  for (int i = 0; i < numsamples; i++) {
    char szBuf[20];
    readline(pFile, "c", szBuf);

    // add extension ".RAW"
    char *szEnd = &szBuf[strlen(szBuf)];
    if (szEnd) strcpy(szEnd, ".RAW");

    strncpy(Sample[i], szBuf, 15);
    Sample[i][14] = '\0';
  }

  // read song filenames
  for (int i = 0; i < iSongCount; i++) {
    char szBuf[20];
    readline(pFile, "c", szBuf);

    // add extension ".HMP"
    char *szEnd = &szBuf[strlen(szBuf)];
    if (szEnd) strcpy(szEnd, ".HMP");

    strncpy(Song[i], szBuf, 15);
    Song[i][14] = '\0';
  }

  // read GMSong/CDSong values for each track (+5 entries)
  for (int i = 0; i < nummusictracks + 5; i++) {
    readline(pFile, "ii", &GMSong[i], &CDSong[i]);
  }

  // set special track indices (stored as negative values)
  titlesong = -nummusictracks;
  optionssong = -(nummusictracks + 1);
  leaderboardsong = -(nummusictracks + 2);
  winsong = -(nummusictracks + 3);
  winchampsong = -(nummusictracks + 4);

  fclose(pFile);
}

//-------------------------------------------------------------------------------------------------

void fade_palette(int iTargetBrightness)
{
  int iOriginalTickOn = tick_on;
  int iOriginalTicks = ticks;
  uint32 uiTimerHandle = 0;

  if (iTargetBrightness == 0)
    disable_keyboard();

  if (iTargetBrightness == 32 && soundon) {
    //sosDIGISetMasterVolume(DIGIHandle, 0x7FFF);
    DIGISetMasterVolume(0x7FFF); // Set max volume for sound effects
  }

  int iCurrentBrightness = palette_brightness;

  if (iTargetBrightness == iCurrentBrightness)
    return;

  tick_on = 0;
  s7 = 0;

  if (current_mode != 0) {
    uiTimerHandle = ROLLERAddTimer(70, SDLS7TimerCallback, NULL); //added by ROLLER
  }

  if (iTargetBrightness > iCurrentBrightness) {
      // FADE IN LOOP
    for (int iStep = iCurrentBrightness; iStep <= iTargetBrightness; iStep++) {
      for (int i = 0; i < 256; i++) {
        pal_addr[i].byR = (palette[i].byR * iStep) >> 5;
        pal_addr[i].byB = (palette[i].byB * iStep) >> 5;
        pal_addr[i].byG = (palette[i].byG * iStep) >> 5;
      }

      if (current_mode != 0) {
        //could also do this instead of SDL timer
        //uint32 uiStartTime = SDL_GetTicks();
        //while (SDL_GetTicks() - uiStartTime < 70) {
        //  SDL_Delay(1);
        //}
        int iPrev = s7;
        while (s7 == iPrev); // Wait for timer tick
      } else {
        //wait for retrace
        //aka VSYNC for DOS
      }

      //set dac palette
      UpdateSDL();
    }
  } else {
      // FADE OUT LOOP
    int iVolumeStep = (iTargetBrightness << 15) - iTargetBrightness;

    for (int iStep = iCurrentBrightness; iStep >= iTargetBrightness; iStep--) {
      if (iTargetBrightness == 0 && !holdmusic) {
        if (musicon) {
          //sosMIDISetMasterVolume(((MusicVolume * iStep) >> 5) & 0xFF);
          MIDISetMasterVolume(((MusicVolume * iStep) >> 5) & 0xFF);
        }

        if (soundon) {
          //sosDIGISetMasterVolume(DIGIHandle, (iVolumeStep >> 5));
          DIGISetMasterVolume(iVolumeStep >> 5);
        }

        if (MusicCD) {
          SetAudioVolume(((MusicVolume * iStep) >> 5) & 0xFF);
        }
      }

      for (int i = 0; i < 256; i++) {
        pal_addr[i].byR = (palette[i].byR * iStep) >> 5;
        pal_addr[i].byB = (palette[i].byB * iStep) >> 5;
        pal_addr[i].byG = (palette[i].byG * iStep) >> 5;
      }

      if (current_mode != 0) {
        int iPrevS7 = s7;
        while (s7 == iPrevS7); // Wait for timer tick
      } else {
        //wait for retrace
        //aka VSYNC for DOS
      }

      //set dac palette
      UpdateSDL();

      iVolumeStep -= 0x7FFF;
    }
  }

  if (current_mode != 0) {
    SDL_RemoveTimer(uiTimerHandle); //added by ROLLER
  }

  memcpy(pal_addr, palette, 768);
  palette_brightness = iTargetBrightness;
  tick_on = iOriginalTickOn;
  ticks = iOriginalTicks;

  if (iTargetBrightness == 0)
    enable_keyboard();

  if (iTargetBrightness == 0 && !holdmusic) {
    if (MusicCD && track_playing)
      StopTrack();
    else if (MusicCard && SongPtr) {
      stop();
      //sosMIDIUnInitSong(SongHandle);
      SongPtr = 0;
    }
  }
}

//-------------------------------------------------------------------------------------------------

void set_palette(int iBrightness)
{
  if (iBrightness == palette_brightness)
    return;

  if (iBrightness < 0) iBrightness = 0;
  if (iBrightness > 31) iBrightness = 31;

  for (int i = 0; i < 256; ++i) {
    pal_addr[i].byR = (palette[i].byR * iBrightness) / 31;
    pal_addr[i].byG = (palette[i].byG * iBrightness) / 31;
    pal_addr[i].byB = (palette[i].byB * iBrightness) / 31;
  }

  // Update the SDL palette
  //SDL_Color sdl_colors[256];
  //
  //for (int i = 0; i < 256; ++i) {
  //  sdl_colors[i].r = pal_addr[i].r;
  //  sdl_colors[i].g = pal_addr[i].g;
  //  sdl_colors[i].b = pal_addr[i].b;
  //  sdl_colors[i].a = 255;
  //}
  //SDL_SetPaletteColors(sdl_palette, sdl_colors, 0, 256);

  palette_brightness = iBrightness;
}

//-------------------------------------------------------------------------------------------------

int check_joystick_usage()
{
  return 0; /*
  int result; // eax
  char v1; // dl
  char v2; // cl

  Joy1used = 0;
  Joy2used = 0;
  for (result = 0; result < 12; ++result) {
    v1 = userkey[result];
    if (v1 == (char)0x80 || v1 == -127 || (unsigned __int8)v1 >= 0x84u && (unsigned __int8)v1 <= 0x87u)
      Joy1used = -1;
    v2 = userkey[result];
    if (v2 == -126 || v2 == -125 || (unsigned __int8)v2 >= 0x88u && (unsigned __int8)v2 <= 0x8Bu)
      Joy2used = -1;
  }
  return result;*/
}

//-------------------------------------------------------------------------------------------------

void convertname(char *szFilename)
{
  char szTemp[32];
  char *p = szTemp;
  char *s = szFilename;

  cheatsample = 0;

  // copy filename to temp buffer (word-wise copy, handling 2 bytes at a time)
  while (1) {
    *p++ = *s;
    if (*s++ == '\0') break;
    *p++ = *s;
    if (*s++ == '\0') break;
  }

  // check for cheat sample conversion
  if (cheat_samples && language == 0) {
    char *ext = strstr(szFilename, ".RAW");
    if (ext) {
      ext[1] = 'K';   // Change to ".KC"
      ext[2] = 'C';
      ext[3] = '\0';

      FILE *fp = ROLLERfopen(szFilename, "rb");
      if (fp) {
        cheatsample = -1;
        fclose(fp);
        return;
      }
    }
  }

  // language-specific sample override
  if (language != 0) {
    char *szExt = strstr(szFilename, ".RAW");
    if (szExt) {
      const char *szLangExt = (const char *)SampleExt + language * 4;
      strcpy(szExt + 1, szLangExt);

      FILE *fp = ROLLERfopen(szFilename, "rb");
      if (fp) {
        fclose(fp);
        return;
      }
    }
  }

  // restore original filename from temp if all else fails
  s = szTemp;
  p = szFilename;
  while (1) {
    *p++ = *s;
    if (*s++ == '\0') break;
    *p++ = *s;
    if (*s++ == '\0') break;
  }
}

//-------------------------------------------------------------------------------------------------

void decode(uint8 *pData, int iLength, uint32 uiStep, uint32 uiOffset)
{
  int i; // edx
  uint8 byOriginal; // bh
  uint8 byXorValue; // bl
  uint32 uiNextOffset; // [esp+0h] [ebp-Ch]

  for (i = 0; i < iLength; *(pData - 1) = byXorValue ^ byOriginal) {
    ++i;
    byOriginal = *pData;
    uiNextOffset = uiOffset + uiStep;
    ++pData;
    byXorValue = uiOffset + uiStep;
    uiStep = uiOffset;
    uiOffset = uiNextOffset;
  }
}

//-------------------------------------------------------------------------------------------------

void loadasample(int iIndex)
{
  char szFilenameBuf[32];

  // construct sample filename
  char *szSrc = Sample[iIndex];
  char *szDst = szFilenameBuf;
  while (*szSrc) {
    *szDst++ = *szSrc++;
    if (!*szSrc) break;
    *szDst++ = *szSrc++;
  }
  *szDst = '\0';

  // convert sample name to real path or cheat version
  convertname(szFilenameBuf);
  SDL_Log("loadasample: %s\n", szFilenameBuf);
  // load file into memory
  loadfile(szFilenameBuf, (void**)&SamplePtr[iIndex], &SampleLen[iIndex], 1);

  // check if cheat sample flag is set and process if needed
  if (cheatsample && SamplePtr[iIndex]) {
    uint8 *pData = SamplePtr[iIndex];
    int iLen = SampleLen[iIndex];
    uint32 uiSeed = 0x1C73;
    uint32 uiStep = 0x896;
    int i;

    for (i = 0; i < iLen; ++i) {
      uiSeed += uiStep;
      pData[i] ^= (uint8)uiSeed;
    }
  }

  // if the index is between 79 and 82 apply a second xor pass
  if (iIndex >= 79 && iIndex <= 82 && SamplePtr[iIndex]) {
    uint8 *pData = SamplePtr[iIndex];
    int iLen = SampleLen[iIndex];
    uint32 uiSeed = 0x4D;
    uint32 uiStep = 0x57;

    for (int i = 0; i < iLen; ++i) {
      uiSeed += uiStep;
      pData[i] ^= (uint8)uiSeed;
    }
  }

  // if using 50Hz timer
  if (SamplePtr[iIndex] && (cheat_mode & CHEAT_MODE_50HZ_TIMER)) {
    if (cheat_mode & CHEAT_MODE_100HZ_TIMER) { //100Hz timer
      // halve the sample length, keep only even-indexed bytes
      int iLen = SampleLen[iIndex];
      uint8 *pData = SamplePtr[iIndex];
      for (int i = 0; i < iLen; i += 2) {
        pData[i / 2] = pData[i];
      }
      SampleLen[iIndex] = iLen / 2;
    } else {
      // remove every third byte
      int iLen = SampleLen[iIndex];
      uint8 *pData = SamplePtr[iIndex];
      int iNewLen = 0;
      for (int i = 0; i < iLen; ++i) {
        if (i % 3 != 2) {
          pData[iNewLen++] = pData[i];
        }
      }
      SampleLen[iIndex] = iNewLen;
    }
  }
}

//-------------------------------------------------------------------------------------------------

void select8bitdriver()
{
  /*
  int v0; // edx

  if (SoundCard) {
    if (drivertype == 1) {
      sosTIMERRemoveEvent(TimerEventHandle, v0);
      sosDIGIUnInitDriver(DIGIHandle, 1, 1);
    }
    resetsamplearray();
    if (drivertype) {
      drivertype = 0;
      memset(&InitDriver, 0, 0x4Cu);
      InitDriver = 4096;
      InitDriver_variable_1 = -1;
      InitDriver_variable_2 = 11025;
      Hardware = SoundPort;
      Hardware_variable_1 = SoundIRQ;
      Hardware_variable_2 = SoundDMA;
      InitDriver_variable_5 = 0;
      if (sosDIGIInitDriver(
        SoundCard,
        0,
        (unsigned int)&Hardware,
        __DS__,
        (unsigned int)&InitDriver,
        (unsigned __int16)__DS__,
        (unsigned int)&DIGIHandle,
        (unsigned __int16)__DS__)) {
        sosDIGIUnInitSystem();
        SoundCard = 0;
      } else {
        sosTIMERRegisterEvent(
          0x12Cu,
          0x7FFF,
          InitDriver_variable_3,
          InitDriver_variable_4,
          (unsigned int)&TimerEventHandle,
          (unsigned __int16)__DS__);
        sosDIGISetMasterVolume(DIGIHandle, 0x7FFFu);
      }
    }
  }*/
}

//-------------------------------------------------------------------------------------------------

int resetsamplearray()
{
  return 0; /*
  int v0; // ebx
  int i; // ecx
  int v2; // edx
  int v3; // eax
  int v4; // eax
  int v5; // esi
  int j; // ecx
  int result; // eax

  v0 = numcars;
  for (i = 0; i != 896; i += 28) {
    v2 = 0;
    if (v0 > 0) {
      v3 = i;
      do {
        *(int *)((char *)enginedelay_variable_1 + v3) = -1;
        *(int *)((char *)enginedelay_variable_3 + v3) = -1;
        *(int *)((char *)enginedelay_variable_5 + v3) = -1;
        ++v2;
        v3 += 896;
      } while (v2 < v0);
    }
  }
  v4 = 2;
  HandleSample[0] = -1;
  HandleCar[0] = -1;
  HandleSample_variable_1 = -1;
  HandleCar_variable_1 = -1;
  do {
    v4 += 5;
    HandleCar_variable_2[v4] = -1;
    SamplePending_variable_3[v4] = -1;
    HandleCar_variable_3[v4] = -1;
    SamplePending_variable_4[v4] = -1;
    HandleCar_variable_4[v4] = -1;
    SamplePending_variable_5[v4] = -1;
    HandleCar_variable_5[v4] = -1;
    SamplePending_variable_6[v4] = -1;
    HandleCar_variable_6[v4] = -1;
    SamplePending_variable_7[v4] = -1;
  } while (v4 != 32);
  v5 = 64;
  for (j = 0; j < 120; ++j) {
    result = j << 6;
    do {
      result += 4;
      *(int *)((char *)&SamplePtr_variable_15 + result) = -1;
    } while (result != v5);
    v5 += 64;
  }
  numcars = v0;
  return result;*/
}

//-------------------------------------------------------------------------------------------------

void reinitmusic()
{
  int iSong; // eax

  if (musicon) {
    if (frontend_on) {
      iSong = optionssong;
    } else if (replaytype == 2) {
      iSong = titlesong;
    } else {
      iSong = TrackLoad;
    }
    startmusic(iSong);
  } else if (MusicCD && track_playing) {
    StopTrack();
  } else if (MusicCard) {
    if (SongPtr) {
      stop();
      //sosMIDIUnInitSong(*(unsigned int *)&SongHandle);
      SongPtr = 0;
    }
  }
}

//-------------------------------------------------------------------------------------------------

void waitsampledone(int iSampleIdx)
{
  if (!soundon)
    return;

  // Check sample pointer is valid
  if (SamplePtr[iSampleIdx] == 0)
    return;

  for (int i = 0; i < 16; i++) {
    int iSampleHandle = SampleHandleCar[iSampleIdx].handles[i];
    while (!DIGISampleDone(iSampleHandle)) {
      UpdateSDL(); //added by ROLLER
    }
  }

  return;
}

//-------------------------------------------------------------------------------------------------

int getcompactedfilelength(const char *szFile)
{
  FILE *pFile; // esi
  int iLength; // [esp+0h] [ebp-14h] BYREF

  pFile = ROLLERfopen(szFile, "rb");
  fread(&iLength, 1u, 4u, pFile);
  fclose(pFile);
  return iLength;
}

//-------------------------------------------------------------------------------------------------

int initmangle(const char *szFile)
{
  unmangleinpoff = 4;
  unmanglebufpos = 4;
  unmangleoverflow = 0;
  unmanglefile = ROLLERfopen(szFile, "rb");
  fseek(unmanglefile, unmanglebufpos, 0);
  return (int)fread(unmangleinbuf, 1u, 0x400u, unmanglefile);
}

//-------------------------------------------------------------------------------------------------

int uninitmangle()
{
  return fclose(unmanglefile);
}

//-------------------------------------------------------------------------------------------------

int loadcompactedfile(const char *szFile, uint8 *pBuf)
{
  initmangle(szFile);
  uint32 uiSize = getcompactedfilelength(szFile);
  loadcompactedfilepart(pBuf, uiSize);
  return fclose(unmanglefile);
}

//-------------------------------------------------------------------------------------------------

void readmangled(uint8 *pBufRet, int iLength)
{
  int iLengthToRead; // ebp
  uint8 *pBufUnit8; // ebx
  int iOverflow; // ecx
  int iDelta; // esi

  iLengthToRead = iLength;
  pBufUnit8 = pBufRet + 40000;
  if (unmangleoverflow) {
    memcpy(pBufUnit8, pBufRet, unmangleoverflow);
    iOverflow = unmangleoverflow;
    unmangleoverflow = 0;
    pBufUnit8 += iOverflow;
    iLengthToRead = iLength - iOverflow;
  }
  if (iLengthToRead > 0) {
    loadcompactedfilepart(pBufUnit8, iLengthToRead);
    iDelta = (int)(unmangledst - pBufUnit8);
    if (unmangledst - pBufUnit8 > iLengthToRead) {
      unmangleoverflow = iDelta - iLengthToRead;
      memcpy(pBufRet, &pBufUnit8[iLengthToRead], iDelta - iLengthToRead);
    }
    memcpy(pBufRet + 20000, &pBufUnit8[iLengthToRead - 20000], 0x4E20u);
  }
}

//-------------------------------------------------------------------------------------------------

void loadcompactedfilepart(uint8 *pDest, uint32 uiDestLength)
{
  uint32 uiOutputPos = 0;

  while (uiOutputPos < uiDestLength) {
    uint8 *pByte = unmangleGet(unmangleinpoff, 1);
    uint8 b = *pByte;
    unmangleinpoff++;

    if (b <= 0x3F) {
        // Literal copy
      uint32 uiCount = b;
      if (unmangleinpoff + uiCount > (uint32)unmangleinpoff + 0x400 || uiOutputPos + uiCount > uiDestLength)
        break;
      uint8 *pData = unmangleGet(unmangleinpoff, uiCount);
      memcpy(&pDest[uiOutputPos], pData, uiCount);
      unmangleinpoff += uiCount;
      uiOutputPos += uiCount;
    } else if (b <= 0x4F) {
        // Linear extrapolation (bytes)
      uint32 uiCount = (b & 0x0F) + 3;
      if (uiOutputPos < 2 || uiOutputPos + uiCount > uiDestLength)
        break;
      int iDelta = pDest[uiOutputPos - 1] - pDest[uiOutputPos - 2];
      for (uint32 i = 0; i < uiCount; ++i) {
        pDest[uiOutputPos] = (uint8)((pDest[uiOutputPos - 1] + iDelta) & 0xFF);
        uiOutputPos++;
      }
    } else if (b <= 0x5F) {
        // Linear extrapolation (words)
      uint32 uiCount = (b & 0x0F) + 2;
      if (uiOutputPos < 4 || uiOutputPos + uiCount * 2 > uiDestLength)
        break;
      int16 nPrev = (int16)(pDest[uiOutputPos - 2] | (pDest[uiOutputPos - 1] << 8));
      int16 nPrev2 = (int16)(pDest[uiOutputPos - 4] | (pDest[uiOutputPos - 3] << 8));
      int16 nDelta = nPrev - nPrev2;
      for (uint32 i = 0; i < uiCount; ++i) {
        int16 nNewVal = nPrev + nDelta;
        pDest[uiOutputPos++] = nNewVal & 0xFF;
        pDest[uiOutputPos++] = (nNewVal >> 8) & 0xFF;
        nPrev = nNewVal;
      }
    } else if (b <= 0x6F) {
        // Repeat last byte
      uint32 uiCount = (b & 0x0F) + 3;
      if (uiOutputPos == 0 || uiOutputPos + uiCount > uiDestLength)
        break;
      uint8 val = pDest[uiOutputPos - 1];
      memset(&pDest[uiOutputPos], val, uiCount);
      uiOutputPos += uiCount;
    } else if (b <= 0x7F) {
        // Repeat last word
      uint32 uiCount = (b & 0x0F) + 2;
      if (uiOutputPos < 2 || uiOutputPos + uiCount * 2 > uiDestLength)
        break;
      for (uint32 i = 0; i < uiCount; ++i) {
        pDest[uiOutputPos] = pDest[uiOutputPos - 2];
        pDest[uiOutputPos + 1] = pDest[uiOutputPos - 1];
        uiOutputPos += 2;
      }
    } else if (b <= 0xBF) {
        // Copy 3 bytes from offset
      int iOffset = (b & 0x3F) + 3;
      if ((int)uiOutputPos - iOffset < 0 || uiOutputPos + 3 > uiDestLength)
        break;
      for (int i = 0; i < 3; ++i)
        pDest[uiOutputPos++] = pDest[uiOutputPos - iOffset];
    } else if (b <= 0xDF) {
        // Copy from offset and length from next byte
      uint8 *pLen = unmangleGet(unmangleinpoff, 1);
      uint8 lenByte = *pLen;
      unmangleinpoff++;

      int iOffset = (((b & 0x03) << 8) | lenByte) + 3;
      int length = ((b >> 2) & 0x07) + 4;
      if ((int)uiOutputPos - iOffset < 0 || uiOutputPos + length > uiDestLength)
        break;

      for (int i = 0; i < length; ++i)
        pDest[uiOutputPos++] = pDest[uiOutputPos - iOffset];
    } else {
        // Copy from offset and length from next 2 bytes
      uint8 *pBytes = unmangleGet(unmangleinpoff, 2);
      uint8 offsetLo = pBytes[0];
      uint8 lengthByte = pBytes[1];
      unmangleinpoff += 2;

      int iOffset = (((b & 0x1F) << 8) | offsetLo) + 3;
      int iLength = lengthByte + 5;
      if ((int)uiOutputPos - iOffset < 0 || uiOutputPos + iLength > uiDestLength)
        break;

      for (int i = 0; i < iLength; ++i)
        pDest[uiOutputPos++] = pDest[uiOutputPos - iOffset];
    }
  }
}

//-------------------------------------------------------------------------------------------------

uint8 *unmangleGet(unsigned int uiPos, unsigned int uiLookahead)
{
  // if what we want to read is not currently in the
  // buffer load it into the buffer from the file
  if (uiPos < (uint32)unmanglebufpos || uiPos + uiLookahead >(uint32)unmanglebufpos + 1024) {
    unmanglebufpos = uiPos;
    fseek(unmanglefile, uiPos, 0);
    fread(unmangleinbuf, 1u, 0x400u, unmanglefile);
  }
  return &unmangleinbuf[uiPos - (uint32)unmanglebufpos];// return new pos
}

//-------------------------------------------------------------------------------------------------
