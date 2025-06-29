#include "3d.h"
#include "cdx.h"
#include "control.h"
#include "drawtrk3.h"
#include "loadtrak.h"
#include "moving.h"
#include "frontend.h"
#include "func2.h"
#include "func3.h"
#include "replay.h"
#include "svgacpy.h"
#include "sound.h"
#include "network.h"
#include "roller.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#ifdef IS_WINDOWS
#include <direct.h>
#define chdir _chdir
#else
#include <unistd.h>
#endif
//-------------------------------------------------------------------------------------------------

int exiting = 0;            //000A3170
int champ_mode = 0;         //000A3178
int cd_error = 0;           //000A317C
int game_svga = 0;          //000A31B0
int game_size = 64;         //000A31B4
int game_view[2] = { 0, 0 }; //000A31B8
int svga_possible = -1;     //000A31C0
int hibuffers;              //000A32E0
uint32 mem_used;            //000A32E8
int gosound = 3;            //000A3334
int current_mode = 666;     //000A333C
int names_on = 1;           //000A3340
tCarEngine *p_eng[2] = { NULL, NULL }; //000A3498
int SVGA_ON = 0;            //000A34AC
int TrackLoad = 1;          //000A34B0
int paused = 0;             //000A34C4
int network_on = 0;         //000A3510
int mirror = 0;             //000A3524
void *screen = NULL; //= 0xA0000; //000A3538
uint8 *scrbuf = NULL;        //000A353C
void *mirbuf = NULL;        //000A3540
void *texture_vga = NULL;   //000A3544
void *building_vga = NULL;  //000A3548
void *horizon_vga = NULL;   //000A354C
void *cartex_vga[16] = { NULL }; //000A3550
void *cargen_vga = NULL;    //000A3590
void *rev_vga[16] = { NULL }; //000A3594
int firstrun = -1;          //000A35D4
int language = 0;           //000A4768
tData localdata[500];       //000BEA10
tGroundPt GroundPt[500];    //000CE410
uint8 shade_palette[4096];  //000FFDD0
tColor palette[256];        //00100DD0
float tsin[16384];          //001010F0
float ptan[16384];          //001110F0
float tcos[16384];          //00121128
char buffer[128];           //00131228
uint8 blank_line[640];      //001312A8
int p_joyk1[2];             //0013E048
int p_joyk2[2];             //0013E050
tMemBlock mem_blocks[128];  //0013E058
volatile int ticks;         //0013E92C
float tatn[1025];           //0013E95C
uint32 textures_off;        //0013F960
int tex_count;              //0013F964
int scr_size;               //0013F988
int ybase;                  //0013F98C
int xbase;                  //0013F990
int winx;                   //0013F994
int winy;                   //0013F998
float ext_y;                //0013F99C
float ext_z;                //0013F9A0
float viewx;                //0013F9A4
float viewy;                //0013F9A8
float viewz;                //0013F9AC
char keys[140];             //0013F9B4
int clear_borders;          //0013FA44
float DDX;                  //0013FA48
float DDY;                  //0013FA4C
float DDZ;                  //0013FA50
float ext_x;                //0013FA54
int test_f1;                //0013FA58
int test_f2;                //0013FA5C
int test_f3;                //0013FA60
int print_data;             //0013FA68
int demo_control;           //0013FA6C
int tick_on;                //0013FA70
int oldmode;                //0013FA74
int start_race;             //0013FA80
int NoOfLaps;               //0013FA84
int human_finishers;        //0013FA88
int countdown;              //0013FA90
int max_mem;                //0013FAA4
int scrmode;                //0013FAB4
int intro;                  //0013FAC0
int fatal_ini_loaded;       //0013FADC
int machine_speed;          //0013FAE0
int netCD;                  //0013FAE4
int localCD;                //0013FAE8
int I_Want_Out;             //0013FAF0
int winner_mode;            //0013FB08
int network_mes_mode;       //0013FB0C
int network_slot;           //0013FB14
int w95;                    //0013FB30
int send_finished;          //0013FB40
int winh;                   //0013FB68
int winw;                   //0013FB6C
int VIEWDIST;               //0013FB70
int YMAX;                   //0013FB74
int XMAX;                   //0013FB78
int time_shown;             //0013FB7C
int player2_car;            //0013FB7E
int player1_car;            //0013FB80

//-------------------------------------------------------------------------------------------------

void copypic(uint8 *pSrc, uint8 *pDest)
{
  /*  int j; // edx
  uint8 *v4; // esi
  uint8 *v5; // edi
  uint8 *v6; // ecx
  uint8 *v7; // eax
  int i; // esi
  int v9; // edx
  int v10; // edi
  uint8 v11; // bl

  if ( SVGA_ON )
  {
    if ( SVGA_ON == 1 )
    {
      copyscreenmodex(pSrc, pDest);
    }
    else if ( scrmode == 257 )
    {
      svgacopy(pSrc, winx, winy + 40, (__int16)winw, (__int16)winh);
    }
    else
    {
      svgacopy(pSrc, winx, winy, (__int16)winw, (__int16)winh);
    }
  }
  else if ( winw != XMAX || winx || mirror )
  {
    if ( mirror )
    {
      v6 = pSrc;
      v7 = &pDest[XMAX * winy - 1 + winx + winw];
      for ( i = 0; i < winh; v7 += XMAX + winw )
      {
        v9 = 0;
        if ( winw > 0 )
        {
          do
          {
            v10 = winw;
            --v7;
            v11 = *v6++;
            ++v9;
            v7[1] = v11;
          }
          while ( v9 < v10 );
        }
        ++i;
      }
    }
    else
    {
      for ( j = 0; j < winh; ++j )
      {
        v4 = &pSrc[winw * j];
        v5 = &pDest[XMAX * (j + winy) + winx];
        qmemcpy(v5, v4, winw);
      }
    }
  }
  else
  {
    qmemcpy(&pDest[winw * winy], pSrc, winh * winw);
  }*/
}

//-------------------------------------------------------------------------------------------------

void init_screen()
{
  int iVesaMode; // ebx
  int i; // esi
  int16 nY; // bx
  int vesaModes[9]; // [esp+0h] [ebp-24h] BYREF

  vesaModes[0] = 0x100;
  vesaModes[1] = 0x101;
  vesaModes[2] = -1;
  if (!svga_possible && SVGA_ON) {
    setmodex();
    SVGA_ON = 1;
    current_mode = -1;
    XMAX = 640;
    YMAX = 400;
    scr_size = 128;
  }
  if (SVGA_ON && current_mode != -1) {
    current_mode = -1;
    iVesaMode = VESAmode(vesaModes);
    scrmode = iVesaMode;
    if (iVesaMode == -1) {
      if (firstrun) {
        if (!language) {
          printf("\n\nThis program has not detected a VESA video driver which it needs to run\n");
          printf("at it's optimum level. Please contact your video board manufacturer for\n");
          printf("more information. The program will now continue using lower resolution\n");
          printf("graphics.\n");
          printf("Press any key to continue...");
          fflush(stdout);
          while (fatkbhit())
            fatgetch();
          fatgetch();
        }
        firstrun = 0;
      }
      svga_possible = 0;
      setmodex();
      SVGA_ON = 1;
      XMAX = 640;
      YMAX = 400;
      scr_size = 128;
      current_mode = -1;
    } else {
      current_mode = -1;
      SVGA_ON = -1;
      XMAX = 640;
      YMAX = 400;
      scr_size = 128;
      if (iVesaMode == 0x101) {
        memset(blank_line, 0, sizeof(blank_line));
        for (i = 0; i < 40; ++i) {
          nY = i;
          svgacopy(blank_line, 0, nY, 640, 1);
        }
        memset(blank_line, 112, sizeof(blank_line));
      }
    }
  }
  if (!SVGA_ON && current_mode) {
    current_mode = SVGA_ON;
    XMAX = 320;
    YMAX = 200;
    scr_size = 64;
    
    // BIOS interrupt for video services
    // ah = 0
    // al = 13h
    // Set video mode 13h
    // (320x200 pixels, 256 colors)
    //__asm { int     10h; -VIDEO - SET VIDEO MODE }
  }
  xbase = 159;
  ybase = 115;
  winw = XMAX;
  winx = 0;
  winh = YMAX;
  winy = 0;
  if (palette_brightness > 0)
    resetpal();
}

//-------------------------------------------------------------------------------------------------

void init()
{
  test_f1 = 0;
  test_f2 = 0;
  switch_sets = 0;
  switch_same = 0;
  switch_types = 0;
  my_number = -1;
  if (SVGA_ON)
    scr_size = 128;
  else
    scr_size = 64;
  tex_count = 1;

  for (int i = 0; i < 25; ++i) {
    strcpy(RecordNames[i], "-----");
    RecordLaps[i] = 128.0f;
    RecordCars[i] = -1;
    RecordKills[i] = 0;
  }

  NoOfLaps = 0;
  player1_car = 0;
  player_invul[0] = 0;
  player_invul[1] = 0;
  player2_car = 1;

  // Fill blank_line with 640 bytes of 0x70
  memset(blank_line, 0x70, 640);
  //_STOSB(blank_line, 0x70707070, 1, 0x280u);

  DDY = 160.0;
  DDZ = 270.0;
  clear_borders = 0;
  VIEWDIST = 270;
  DDX = 0.0;

  //load tsin, tcos, and ptan
  for (int i = 0; i < 16384; ++i) {
    double dAngle = i * TWO_PI * ONE_OVER_TRIG_LOOKUP_AY_COUNT;
    tsin[i] = (float)sin(dAngle);
    tcos[i] = (float)cos(dAngle);
    ptan[i] = (float)tan(dAngle);
  }

  ext_x = 0;
  ext_y = 0;
  ext_z = 0;

  //load tatn
  for (int i = 0; i < 1025; ++i) {
    double dAngle = i * TWO_PI * ONE_OVER_TATN_LOOKUP_AY_COUNT;
    tatn[i] = (float)atan(dAngle);
  }

  scrbuf = getbuffer(256000u);
  LoadRecords();
}

//-------------------------------------------------------------------------------------------------

void *getbuffer(uint32 uiSize)
{
  int iMemBlocksIdx; // esi
  void *pBuf; // eax
  int iRegsDi; // [esp+28h] [ebp-24h] BYREF
  void *pPtr; // [esp+2Ch] [ebp-20h] BYREF

  iMemBlocksIdx = 0;
  if (mem_blocks[0].pBuf) {
    while (mem_blocks[++iMemBlocksIdx].pBuf)
      ;
  }
  pBuf = malloc2(uiSize, &pPtr, &iRegsDi);
  if (pBuf) {
    mem_blocks[iMemBlocksIdx].pBuf = pBuf;
    mem_blocks[iMemBlocksIdx].uiSize = uiSize;
    mem_blocks[iMemBlocksIdx].pAlsoBuf = pPtr;
    mem_blocks[iMemBlocksIdx].iRegsDi = iRegsDi;
    hibuffers++;
    mem_used += uiSize;
  }
  return pBuf;
}

//-------------------------------------------------------------------------------------------------

void *trybuffer(uint32 uiSize)
{
  int iMemBlocksIdx; // esi
  void *pBuf; // eax
  void *pPtr; // [esp+28h] [ebp-20h] BYREF
  int iRegsDi; // [esp+2Ch] [ebp-1Ch] BYREF
  int iMemUsed; // ecx

  iMemBlocksIdx = 0;
  if (mem_blocks[0].pBuf) {
    while (mem_blocks[++iMemBlocksIdx].pBuf)
      ;
  }
  pBuf = malloc2(uiSize, &pPtr, &iRegsDi);
  if (pBuf) {
    mem_blocks[iMemBlocksIdx].pBuf = pBuf;
    mem_blocks[iMemBlocksIdx].uiSize = uiSize;
    mem_blocks[iMemBlocksIdx].pAlsoBuf = pPtr;
    iMemUsed = mem_used;
    mem_blocks[iMemBlocksIdx].iRegsDi = iRegsDi;
    mem_used = uiSize + iMemUsed;
    ++hibuffers;
  }

  return pBuf;
}

//-------------------------------------------------------------------------------------------------

void fre(void **ppData)
{
  void *pBuf; // ebx
  int iMemBlocksIdx; // edx
  int i; // eax
  signed int uiSize; // eax

  if (*ppData) {
    pBuf = mem_blocks[0].pBuf;
    iMemBlocksIdx = 0;
    for (i = 0; pBuf != *ppData; ++iMemBlocksIdx) {
      if (i >= 128)
        break;
      pBuf = mem_blocks[++i].pBuf;
    }
    if (iMemBlocksIdx >= 128)                 // mem_blocks is 128 tMemBlocks
    {
      assert(0);
      printf("Failed to find allocated block\n");
      doexit();
    } else {
      mem_blocks[iMemBlocksIdx].pBuf = 0;
    }
    uiSize = mem_blocks[iMemBlocksIdx].uiSize;
    if (uiSize >= 0) {
      mem_used -= uiSize;
      free2(mem_blocks[iMemBlocksIdx].pBuf); //.pAlsoBuf?
    } else {
      // is this necessary?
    }
    *ppData = 0;
  }
}

//-------------------------------------------------------------------------------------------------

void doexit()
{
  exiting = -1;
  if (network_on) {
    tick_on = -1;
    frontend_on = -1;
    broadcast_mode = -666;
    while (broadcast_mode == -666) {
      UpdateSDL();
    }
  }
  close_network();
  SaveRecords();
  fre(&mirbuf);
  for (int i = 0; i < 16; ++i) {
    fre(&rev_vga[i]);
    fre(&cartex_vga[i]);
    fre(&front_vga[i]);
  }
  fre(&font_vga);
  fre(&title_vga);
  fre(&cargen_vga);
  fre(&texture_vga);
  fre(&building_vga);
  fre(&scrbuf);
  release_key_int();
  Uninitialise_SOS();
  releasesamples();
  if (MusicCD)
    cdxdone();
  remove("../REPLAYS/REPLAY.TMP");
  if ((cheat_mode & CHEAT_MODE_WIDESCREEN) != 0)
    textures_off |= 0x2000u;
  else
    textures_off &= ~0x2000u;
  if (false_starts)
    textures_off |= 0x40;
  else
    textures_off &= 0xBF;
  if (!intro)
    save_fatal_config();

  //added by ROLLER
  ShutdownSDL();

  //clear keyboard buffer
  //while (kbhit(iSuccess))
  //  iSuccess = getch();
  chdir("..");

  exit(0);
}

//-------------------------------------------------------------------------------------------------

void firework_screen()
{
  /*
     int v0; // esi
  tCarSpray *v1; // ebx
  unsigned __int8 v2; // al
  int v3; // ecx
  double v4; // st7
  double v5; // st7
  int i; // ecx
  double v7; // st7
  double v8; // st7
  int v9; // eax
  int v10; // eax
  int v11; // eax
  char v12; // dl
  int v13; // [esp+0h] [ebp-30h]
  int v14; // [esp+4h] [ebp-2Ch]
  int v15; // [esp+8h] [ebp-28h]
  int v16; // [esp+Ch] [ebp-24h]
  char v17; // [esp+10h] [ebp-20h]

  v0 = 0;
  memset(scrbuf, 112, winh * winw);
  do
  {
    v1 = &CarSpray[v0];
    v2 = LOBYTE(CarSpray[v0].array[10]);
    if ( v2 )
    {
      if ( v2 <= 1u )
      {
        v3 = 0;
        v17 = 0;
        do
        {
          if ( LOBYTE(v1->array[10]) )
          {
            v4 = v1->array[0];
            _CHP();
            v15 = (int)v4;
            v5 = v1->array[1];
            _CHP();
            v14 = (int)v5;
            if ( v15 >= 0 && v15 < winw && v14 >= 0 && v14 < winh )
              scrbuf[winw * v14 + v15] = LOBYTE(v1->array[9]) - v17;
          }
          v1 = (tCarSpray *)((char *)v1 + 44);
          ++v3;
          v17 += 3;
        }
        while ( v3 < 5 );
      }
      else if ( v2 == 2 )
      {
        for ( i = 0; i < 32; ++i )
        {
          if ( SLODWORD(v1->array[7]) > 0 )
          {
            v7 = v1->array[0];
            _CHP();
            v16 = (int)v7;
            v8 = v1->array[1];
            _CHP();
            v13 = (int)v8;
            v10 = rand(v9);
            v11 = (16 * v10 - (__CFSHL__((16 * v10) >> 31, 15) + ((16 * v10) >> 31 << 15))) >> 15;
            v12 = v11 - 4;
            if ( v11 - 4 < 0 )
              v12 = 0;
            if ( v16 >= 0 && v16 < winw && v13 >= 0 && v13 < winh )
              scrbuf[v16 + winw * v13] = LOBYTE(v1->array[9]) - v12;
          }
          v1 = (tCarSpray *)((char *)v1 + 44);
        }
      }
    }
    ++v0;
  }
  while ( v0 != 18 );
  game_copypic(scrbuf, (int)screen, (char *)ViewType[0]);
  */
}

//-------------------------------------------------------------------------------------------------

void updatescreen()
{
  /*
  int v0; // esi
  int v1; // edi
  int v2; // ebp
  int v3; // esi
  int v4; // ecx
  char *v5; // esi
  int v6; // ebx
  _BYTE *k; // ecx
  int v8; // eax
  _BYTE *m; // ecx
  char v10; // dl
  int v11; // eax
  int v12; // eax
  int v13; // eax
  int v15; // esi
  int v16; // edi
  int v17; // ebp
  int v18; // ecx
  char *v19; // esi
  int v20; // ebx
  _BYTE *i; // ecx
  int v22; // eax
  _BYTE *j; // ecx
  char v24; // dl
  int v25; // eax
  int v26; // ebx
  int v27; // edx
  int v28; // ecx
  int v29; // [esp+0h] [ebp-24h]
  int v30; // [esp+4h] [ebp-20h]

  mirror = 0;
  shown_panel = 0;
  screenready = -1;
  if (SVGA_ON)
    small_poly = 200;
  else
    small_poly = 100;
  time_shown = 0;
  if (!Play_View)
    goto LABEL_30;
  if ((unsigned __int8)Play_View <= 1u) {
    time_shown = -1;
    v15 = scr_size;
    xbase = 319;
    scr_size = (scr_size - (__CFSHL__(scr_size >> 31, 2) + 4 * (scr_size >> 31))) >> 2;
    winw = (320 * scr_size) >> 5;
    v16 = (320 * scr_size) >> 5;
    winx = 0;
    winy = 0;
    winh = (200 * scr_size) >> 6;
    v17 = (200 * scr_size) >> 6;
    v30 = 0;
    if ((ViewType & 1) != 0) {
      if (Car_variable_23[308 * ViewType - 308] < 0)
        goto LABEL_20;
    } else if (Car_variable_23[308 * ViewType + 308] < 0) {
      goto LABEL_20;
    }
    v30 = -1;
    draw_road(0);
  LABEL_20:
    time_shown = 0;
    shown_panel = 0;
    winw = (320 * v15 + 32) >> 6;
    xbase = 159;
    winh = (200 * v15 + 32) >> 6;
    winx = (XMAX - ((320 * v15 + 32) >> 6)) / 2;
    winy = (YMAX - ((200 * v15 + 32) >> 6)) / 2;
    scr_size = v15;
    v12 = draw_road(0);
    if (clear_borders) {
      clear_borders = 0;
      ((void (*)(void))clear_border)();
      clear_border(0, winy, winx, winh);
      clear_border(winx + winw, winy, winx, winh);
      v12 = clear_border(0, winy + winh, XMAX, YMAX - (winy + winh));
    }
    if (v30) {
      v18 = (winw - v16 - 1) / 2
        + scrbuf
        + winw * ((11 * scr_size - (__CFSHL__((11 * scr_size) >> 31, 6) + ((11 * scr_size) >> 31 << 6))) >> 6);
      v19 = (char *)mirbuf;
      memset(v18, 119, v16 + 2);
      v20 = 0;
      for (i = (_BYTE *)(winw + v18); v20 < v17; i = &j[v25 - v16 - 1]) {
        *i = 119;
        v22 = 0;
        for (j = i + 1; v22 < v16; *(j - 1) = v24) {
          ++j;
          v24 = *v19++;
          ++v22;
        }
        v25 = winw;
        *j = 119;
        ++v20;
      }
      v12 = memset(i, 119, v16 + 2);
    }
    if (screenready)
      goto LABEL_14;
    return init_animate_ads(v12);
  }
  if (Play_View == 2) {
    v0 = scr_size;
    xbase = 159;
    scr_size /= 2;
    winw = (320 * scr_size) >> 6;
    v1 = (320 * scr_size) >> 6;
    time_shown = -1;
    winx = 0;
    winy = 0;
    winh = (200 * scr_size) >> 7;
    v2 = (200 * scr_size) >> 7;
    draw_road(0);
    xbase = 159;
    winw = (320 * v0 + 32) >> 6;
    winh = (200 * v0 + 32) >> 6;
    time_shown = 0;
    shown_panel = 0;
    winx = (XMAX - ((320 * v0 + 32) >> 6)) / 2;
    winy = (YMAX - ((200 * v0 + 32) >> 6)) / 2;
    scr_size = v0;
    draw_road(0);
    v3 = (winw - v1 - 1) / 2;
    v29 = (11 * scr_size - (__CFSHL__((11 * scr_size) >> 31, 6) + ((11 * scr_size) >> 31 << 6))) >> 6;
    if (clear_borders) {
      clear_borders = 0;
      clear_border(0, 0, XMAX, winy);
      clear_border(0, winy, winx, winh);
      clear_border(winx + winw, winy, winx, winh);
      clear_border(0, winh + winy, XMAX, YMAX - (winh + winy));
    }
    v4 = v29 * winw + v3 + scrbuf;
    v5 = (char *)(v1 + mirbuf - 1);
    memset(v4, 112, v1 + 2);
    v6 = 0;
    for (k = (_BYTE *)(winw + v4); v6 < v2; k = &m[v11 - v1 - 1]) {
      *k = 112;
      v8 = 0;
      for (m = k + 1; v8 < v1; *(m - 1) = v10) {
        ++m;
        v10 = *v5--;
        ++v8;
      }
      v11 = winw;
      ++v6;
      *m = 112;
      v5 += 2 * v1;
    }
    v12 = memset(k, 112, v1 + 2);
    if (screenready) {
    LABEL_14:
      v13 = game_copypic(scrbuf, screen, ViewType);
      return init_animate_ads(v13);
    }
    return init_animate_ads(v12);
  }
LABEL_30:
  if (player_type == 2) {
    if (SVGA_ON)
      scr_size = 64;
    else
      scr_size = 32;
    winw = XMAX;
    xbase = 319;
    winx = 0;
    winy = 0;
    winh = YMAX / 2 - 2;
    if (clear_borders)
      memset(scrbuf + winh * winw, 0, 4 * winw);
    draw_road(0);
    shown_panel = 0;
    draw_road(1);
    time_shown = -1;
    if (SVGA_ON)
      scr_size = 128;
    else
      scr_size = 64;
    draw_type = 0;
    winw = XMAX;
    winh = YMAX;
    v12 = game_copypic(scrbuf, screen, ViewType);
    draw_type = 2;
    return init_animate_ads(v12);
  }
  if ((cheat_mode & 0x40) == 0 || paused) {
    if ((cheat_mode & 0x40) != 0) {
      if (SVGA_ON)
        scr_size = 128;
      else
        scr_size = 64;
    }
    winh = (200 * scr_size + 32) >> 6;
    winw = (320 * scr_size + 32) >> 6;
    winx = (XMAX - ((320 * scr_size + 32) >> 6)) / 2;
    xbase = 159;
    winy = (YMAX - ((200 * scr_size + 32) >> 6)) / 2;
    if (clear_borders) {
      clear_borders = 0;
      clear_border(0, 0, XMAX, winy);
      clear_border(0, winy, winx, winh);
      clear_border(winx + winw, winy, winx, winh);
      v28 = YMAX;
      v27 = winh + winy;
      v26 = XMAX;
      goto LABEL_44;
    }
  } else {
    if (SVGA_ON)
      scr_size = 64;
    else
      scr_size = 32;
    winh = YMAX / 2;
    winw = XMAX;
    winx = 0;
    xbase = 319;
    winy = (YMAX - (__CFSHL__(YMAX >> 31, 2) + 4 * (YMAX >> 31))) >> 2;
    if (clear_borders) {
      clear_borders = 0;
      clear_border(0, 0, XMAX, (YMAX - (__CFSHL__(YMAX >> 31, 2) + 4 * (YMAX >> 31))) >> 2);
      v26 = XMAX;
      v27 = winh + winy;
      v28 = YMAX;
    LABEL_44:
      clear_border(0, v27, v26, v28 - v27);
    }
  }
  v12 = draw_road(0);
  if ((cheat_mode & 0x40) == 0)
    return init_animate_ads(v12);
  if (SVGA_ON)
    scr_size = 128;
  else
    scr_size = 64;
  winw = XMAX;
  winh = YMAX;
  return init_animate_ads(YMAX);
  */
}

//-------------------------------------------------------------------------------------------------

void draw_road(uint8 *a1, char *a2, float *a3, int a4, int a5)
{/*
  double v6; // st7
  double v7; // st7

  if ((textures_off & 0x80000) != 0 || game_track == 13) {
    if (gfx_size == 1)
      min_sub_size = 64;
    else
      min_sub_size = 128;
  } else if (gfx_size == 1) {
    min_sub_size = 4;
  } else {
    min_sub_size = 8;
  }
  if (SVGA_ON)
    v6 = (double)scr_size * threed_c_variable_20;
  else
    v6 = (double)scr_size * threed_c_variable_21;
  subscale = v6;
  if ((textures_off & 0x80000) != 0 || game_track == 13) {
    if (SVGA_ON)
      v7 = (double)scr_size * threed_c_variable_22;
    else
      v7 = (double)scr_size * threed_c_variable_23;
    subscale = v7;
  }
  screen_pointer = a1;
  calculateview((int)a3, (int)a2, a5);
  DrawHorizon((int)a1);
  CalcVisibleTrack((int)a2, (unsigned int)a3);
  DrawCars((int)a2, (int)a3);
  CalcVisibleBuildings();
  DrawTrack3((int *)a1, a5, (int)a2, a3, (float *)a1);
  if (a4) {
    if (screenready)
      game_copypic(a1, (int)screen, a2);
  }*/
}

//-------------------------------------------------------------------------------------------------

int main(int argc, const char **argv, const char **envp)
{
  uint32 uiCheatMode; // edi
  uint32 uiTexturesOff2; // ecx
  char *szDirectory; // eax
  int iMemBlocksIdx2; // eax
  uint32 uiTexturesOff; // ebx
  int iMemBlocksIdx; // edx
  uint32 v10; // edx
  unsigned int v11; // edx
  uint32 i; // ebx
  //unsigned int v13; // eax
  //int v16; // eax
  uint32 uiCheatMode2; // edi
  //void *ebx52; // ebx
  //uint8 *v17; // ecx
  //int v18; // eax
  //int v19; // eax
  //int v20; // eax
  //int v21; // eax
  //int v22; // eax
  //int v23; // eax
  //int v24; // eax
  //int result; // eax

  //Added by ROLLER
  InitSDL();

  //gssCommsSetCommandBase(0x686C6361u);
  oldmode = readmode();
  blankpal();
  SVGA_ON = 0;
  init_screen();
  blankpal();
  SVGA_ON = -1;
  init_screen();
  blankpal();
  test_w95();
  uiCheatMode = 0;
  //harderr((int)criticalhandler, __CS__);
  network_slot = 0;
  if (argc == 2)
    network_slot = atoi(argv[1]) & 0xFFFFFF;
  uiTexturesOff2 = 1;
  player1_car = 0;
  player2_car = 1;
  name_copy(player_names[0], "HUMAN");
  name_copy(player_names[player2_car], "PLAYER 2");
  textures_off = 0;
  frontend_on = -1;
  claim_key_int();
  szDirectory = (char *)*argv;
  max_mem = 0;
  setdirectory(szDirectory);
  iMemBlocksIdx2 = 0;
  do {
    uiTexturesOff = 0;
    iMemBlocksIdx = (int16)iMemBlocksIdx2++;
    mem_blocks[iMemBlocksIdx].pBuf = 0;
  } while ((int16)iMemBlocksIdx2 < 128);
  cheat_mode = 0;
  load_language_map();
  load_fatal_config();
  if ((textures_off & 0x2000) != 0) {
    cheat_mode |= CHEAT_MODE_WIDESCREEN;
    textures_off ^= 0x2000u;
  }
  if ((textures_off & 0x4000) != 0) {
    false_starts = -1;

    // BYTE1 is the second byte
    // textures_off ^= 0x00004000
    uiTexturesOff2 = textures_off;
    uiTexturesOff2 ^= 0x00004000;
    textures_off = uiTexturesOff2;
  } else {
    false_starts = 0;
  }
  findintrofiles();
  initmusic();
  tick_on = 0;
  remove("../REPLAYS/REPLAY.TMP");
  readsoundconfig();
  loadcheatnames();
  cdxinit();
  GetFirstCDDrive();
  ResetDrive();
  StopTrack();
  StopTrack();
  print_data = 0;
  TrackSelect = 0;

  // this initializes the timer system and calls
  // claim_ticktimer which sets up a callback to
  // increment ticks
  Initialise_SOS();
  check_machine_speed();
  if (fatal_ini_loaded) {
    if (view_limit) {
      if (machine_speed >= 2800)
        view_limit = 32;
      else
        view_limit = 24;
    }
  } else {
    if (machine_speed < 9000)
      textures_off |= 0x80000u;
    if (machine_speed < 5000)
      textures_off |= 0x40000u;
    if (machine_speed < 4600) {
      // textures_off |= 0x00000800
      uiTexturesOff = textures_off;
      uiTexturesOff |= 0x00000800;
      textures_off = uiTexturesOff;
    }
    if (machine_speed < 4300)
      textures_off |= 8u;
    if (machine_speed < 4000)
      view_limit = 32;
    if (machine_speed < 3750) {
      v10 = textures_off;
      v10 |= 0x00000020;
      textures_off = v10;
    }
    if (machine_speed < 3500) {
      uiTexturesOff2 = textures_off;
      uiTexturesOff2 |= 0x00000010;
      textures_off = uiTexturesOff2;
    }
    if (machine_speed < 3250)
      textures_off |= 0x200u;
    if (machine_speed < 3000)
      game_size = 48;
    if (machine_speed < 3800) {
      uiTexturesOff = 0;
      allengines = 0;
    }
    if (machine_speed < 2900)
      replay_record = 0;
    if (machine_speed < 2800)
      view_limit = 24;
  }
  InitCarStructs();
  init();
  v11 = 0;
  print_data = 0;
  tick_on = 0;
  copy_screens();
  i = 0;
  title_screens();
  time_to_start = 0;
  replaytype = 2;
  start_race = 0;
  countdown = 144;
  gosound = 3;
  delayread = 0;
  delaywrite = 6;
  writeptr = 0;
  readptr = 0;
  winner_mode = 0;
  intro = -1;
  play_game(TrackLoad, 0, 0);
  intro = 0;
  VIEWDIST = 270;
  do {
    start_race = 0;
    time_to_start = 0;
    if (restart_net) {
      restart_net_game();
    } else {
      //while (!time_to_start)
      //  select_screen(0);
    }
    restart_net = 0;
    countdown = 144;
    uiCheatMode2 = 3;
    gosound = 3;
    delayread = 0;
    delaywrite = 6;
    writeptr = 0;
    readptr = 0;
    if (!quit_game) {
      //if (game_type < 3) {
      //  title_screens(v16, v11);
      //  ebx52 = (void *)replaytype;
      //  v11 = -1;
      //  if (replaytype == 2)
      //    v11 = 0;
      //  if (intro)
      //    v11 = 0;
      //  net_quit = 0;
      //  prev_track = TrackLoad;
      //  play_game(TrackLoad, v11, replaytype);
      //  if (network_buggered) {
      //    network_fucked(v16, v11, ebx52);
      //    v11 = 0;
      //  }
      //  if (net_quit || network_buggered) {
      //    v11 = 0;
      //    if (network_champ_on && (network_buggered != 666 || !restart_net)) {
      //      game_type = 0;
      //      network_champ_on = 0;
      //    }
      //    if (net_quit)
      //      close_network();
      //  }
      //  if (cd_error)
      //    no_cd(v16, v11, (int)ebx52, (_BYTE *)0x90);
      //  i = 270;
      //  v17 = (uint8 *)quit_game;
      //  VIEWDIST = 270;
      //  if (!quit_game) {
      //    if (v11) {
      //      v16 = game_type;
      //      if (game_type) {
      //        if ((unsigned int)game_type > 1) {
      //          if (game_type == 2) {
      //            StoreResult();
      //            for (i = 0; (__int16)i < numcars; ++i) {
      //              if (human_control[(__int16)i]) {
      //                v11 = sizeof(tCar) * (__int16)i;
      //                if (*((char *)&Car[0].byUnk31 + v11) > 1)
      //                  TimeTrials((__int16)i, v11, i, 0);
      //              }
      //            }
      //            ShowLapRecords((__int16)i, v11, i, 0);
      //          }
      //          goto LABEL_103;
      //        }
      //        finish_race();
      //        StoreResult();
      //        v21 = carorder[0];
      //        if (human_control[carorder[0]] || (cheat_mode & 0x20) != 0) {
      //          v11 = carorder[0] & 1;
      //          v21 = winner_screen(Car[carorder[0]].byCarDesignIdx, v11, (_WORD *)0x10E);
      //          if (v21)
      //            v21 = winner_race();
      //        }
      //        ResultRoundUp(v21, v11, 270, 0);
      //        RaceResult(v22, v11, 270, 0);
      //        ChampionshipStandings(v23, v11, 270, 0);
      //        if (competitors[0] > 8) {
      //          uiCheatMode2 = cheat_mode;
      //          if ((cheat_mode & 0x4000) == 0)
      //            TeamStandings(v24, v11, 270, 0);
      //        }
      //        ShowLapRecords(v24, v11, 270, 0);
      //        ++Race;
      //        v16 = prev_track + 1;
      //        TrackLoad = prev_track + 1;
      //        if (Race == 8 || (v11 = cheat_mode, (cheat_mode & 0x10) != 0)) {
      //          v16 = ChampionshipOver(v16, v11, 270, 0);
      //          goto LABEL_103;
      //        }
      //        i = cheat_mode;
      //        if ((cheat_mode & 0x80u) == 0)
      //          goto LABEL_103;
      //      LABEL_83:
      //        v16 = RollCredits((_WORD *)i, v17, uiCheatMode2);
      //        goto LABEL_103;
      //      }
      //      if (!gave_up) {
      //        finish_race();
      //        StoreResult();
      //        v18 = carorder[0];
      //        if (human_control[carorder[0]] || (cheat_mode & 0x20) != 0) {
      //          v11 = carorder[0] & 1;
      //          v18 = winner_screen(Car[carorder[0]].byCarDesignIdx, v11, (_WORD *)0x10E);
      //          if (v18)
      //            v18 = winner_race();
      //        }
      //        ResultRoundUp(v18, v11, 270, 0);
      //        RaceResult(v19, v11, 270, 0);
      //        ShowLapRecords(v20, v11, 270, 0);
      //      }
      //      v17 = (_BYTE *)cheat_mode;
      //      if ((cheat_mode & 0x10) != 0)
      //        v16 = ChampionshipOver(v16, v11, 270, (_BYTE *)cheat_mode);
      //      uiCheatMode2 = cheat_mode;
      //      if ((cheat_mode & 0x80) != 0)
      //        goto LABEL_83;
      //    LABEL_103:
      //      if (player_type == 1)
      //        player_type = 0;
      //    }
      //    intro = 0;
      //  }
      //} else if (game_type == 3) {
      //  ChampionshipStandings(v16, v11, i, (_BYTE *)0x90);
      //  if (competitors[0] > 8) {
      //    i = cheat_mode;
      //    if ((cheat_mode & 0x4000) == 0)
      //      TeamStandings(v16, v11, cheat_mode, (_BYTE *)0x90);
      //  }
      //  dontrestart = -1;
      //} else {
      //  ShowLapRecords(v16, v11, i, (_BYTE *)0x90);
      //  dontrestart = -1;
      //}
    }

    //added by ROLLER
    UpdateSDL();

  } while (!quit_game);
  //__asm { int     10h; -VIDEO - SET VIDEO MODE }
  doexit();

  return 0;
}

//-------------------------------------------------------------------------------------------------

int play_game_init()
{
  return 0;/*
  uint32 uiTexturesOff; // edx
  uint32 uiCheatMode; // ebx
  int v2; // ebx
  unsigned int v3; // eax
  int v4; // ebp
  int v5; // edx
  int v6; // ebp
  int i; // eax
  int v8; // edx
  int v9; // eax
  int v10; // eax
  int v11; // eax
  int v12; // eax
  int v13; // eax
  int v14; // ebp
  char *v15; // eax
  int v16; // eax
  int v17; // ecx
  int v18; // edx
  int v19; // ecx
  int v20; // edx
  tData *pLocalDataAy; // edx
  int iTrackLen; // ecx
  double dTotalTrackDistance; // st7
  double dTotalTrackDistance2; // st6
  int v25; // eax
  int result; // eax
  int v27; // ebp
  int v28; // edx

  DeathView = -1;
  DeathView_variable_1[0] = -1;
  check_set = 0;
  I_Would_Like_To_Quit = 0;
  Quit_Count = 0;
  dostopsamps = 0;
  my_number = -1;
  network_error = 0;
  network_sync_error = 0;
  network_mistake = 0;
  view1_cnt = 0;
  view0_cnt = 0;
  network_buggered = 0;
  trying_to_exit = 0;
  gave_up = 0;
  define_mode = 0;
  next_resync = -1;
  sync_errors = 0;
  game_frame = -1;
  already_quit = 0;
  if (network_on)
    cd_error = 0;
  send_finished = 0;
  fudge_wait = -1;
  memset(player_checks, -1, sizeof(player_checks));
  memset(&player_syncs, 0, 0x20u);
  read_check = 0;
  write_check = 0;
  if (!svga_possible || no_mem)
    game_svga = 0;
  SVGA_ON = game_svga;
  if (no_mem) {
    fre(&scrbuf);
    scrbuf = (uint8 *)getbuffer(0xFA00u);
  }
  if (gfx_size == 1)
    min_sub_size = 4;
  else
    min_sub_size = 8;
  cdchecked = 0;
  paused = 0;
  tick_on = 0;
  start_race = 0;
  game_overs = 0;
  if (replaytype != 2) {
    if (game_type == 1 && player_type == 1) {
      network_champ_on = random_seed;
      if (!random_seed)
        network_champ_on = 666;
    } else if (!winner_mode) {
      network_champ_on = 0;
    }
  }
  replay_player = player_type;
  frame_number = 0;
  if (replaytype == 2) {
    replay_cheat = cheat_mode;
    if ((cheat_mode & 0x40) != 0) {
      uiTexturesOff = textures_off;
      BYTE1(uiTexturesOff) = BYTE1(textures_off) | 0x20;
      textures_off = uiTexturesOff;
      uiCheatMode = cheat_mode;
      LOBYTE(uiCheatMode) = cheat_mode ^ 0x40;
      cheat_mode = uiCheatMode;
    }
    player_type = 0;
    if (numcars > 0) {
      v2 = 4 * numcars;
      v3 = 0;
      do {
        v3 += 4;
        result_best_variable_1[v3 / 4] = TrackArrow_variable_1[v3 / 4];
      } while ((int)v3 < v2);
    }
  }
  memset(copy_multiple, 0, 0x8000u);
  draw_type = player_type;
  local_players = (player_type == 2) + 1;
  if (network_on) {
    v4 = (player1_car & 1) != 0 ? player1_car - 1 : player1_car + 1;
    team_mate = v4;
    v5 = non_competitors[v4];
    network_mes_mode = v4;
    if (v5) {
      if (players <= 1) {
        v6 = -1;
      } else {
        v6 = 0;
        for (i = 0; !human_control[i] || v6 == player1_car; ++i)
          ++v6;
      }
      network_mes_mode = v6;
      team_mate = -1;
    }
  }
  if (player_type != 2) {
    player2_car = -1;
    ViewType_variable_1 = -1;
  }
  FirstTick = -1;
  dead_humans = 0;
  ticks = 0;
  tick_on = -1;
  load_language_file((int)aIngameEng, 0);
  load_language_file((int)aConfigEng, 1);
  if (frontendspeechptr) {
    printf(&aDfrontEndSpeec[1]);
    doexit(1, 1, (void *)0x8000);
  }
  if ((cheat_mode & 2) != 0) {
    game_level = level;
    game_dam = damage_level;
    damage_level = 3;
    level = 0;
  }
  disable_messages = 0;
  Fatality = -1;
  Destroyed = 0;
  ahead_sect = -2;
  ahead_time = 0;
  game_count[0] = -2;
  game_count_variable_1 = -2;
  game_scale[0] = 32768.0;
  game_scale[1] = 32768.0;
  Joy1used = 0;
  Joy2used = 0;
  check_joystick_usage();
  memset(repsample, 1, sizeof(repsample));
  qmemcpy(newrepsample, repsample, 0x10u);
  autoswitch = -1;
  game_req = 0;
  mirbuf = (int)getbuffer(0x7D00u);
  screenready = 0;
  racing = -1;
  if (replaytype != 2)
    replayedit = 0;
  if (!winner_mode) {
    SelectedView[0] = game_view;
    SelectedView_variable_1 = game_view_variable_1;
  }
  race_started = 0;
  rud_turn[0] = 0;
  finishers = 0;
  human_finishers = 0;
  setreplaytrack();
  v8 = 0;
  loadtrack((_DWORD *)game_track, 0);
  LoadGenericCarTextures(v9, 0, 0, 0);
  InitCars();
  if (game_type >= 2) {
    v8 = 5;
    NoOfLaps = 5;
    if (network_on)
      countdown = 144;
    else
      countdown = -72;
  } else {
    v10 = cur_laps[level];
    NoOfLaps = v10;
    if (competitors[0] == 2) {
      v8 = v10 % 2;
      NoOfLaps = v10 / 2;
    }
    if (infinite_laps[0]) {
      v8 = 0;
      NoOfLaps = 0;
    }
  }
  startreplay();
  LoadPanel();
  initclouds(v11);
  if (!w95 || MusicCD) {
    if (!winner_mode && !loading_replay) {
      if (replaytype == 2)
        v12 = titlesong;
      else
        v12 = TrackLoad;
      startmusic(v12);
    }
    holdmusic = -1;
  }
  loadsamples();
  initcollisions();
  initsounds();
  fade_palette(0, v8, 0, 0);
  init_screen(v13, v8, 0);
  if (intro || replaytype == 2) {
    v14 = network_mes_mode;
    if (SVGA_ON)
      scr_size = 128;
    else
      scr_size = 64;
  } else {
    v14 = network_mes_mode;
    scr_size = game_size;
  }
  network_mes_mode = v14;
  clear_borders = -1;
  if ((((unsigned int)&loc_1FFFD + 3) & cheat_mode) != 0)
    v15 = aCheatpalPal;
  else
    v15 = &aNdPalettePal[3];
  setpal((int)v15, -1, 0, 0);
  FindShades();
  InitRemaps();
  if (player_type == 2) {
    if (!DuoViews[SelectedView[0]])
      viewplus(0);
    if (!DuoViews[SelectedView_variable_1])
      viewplus(1);
  } else if (SelectedView[0] == 7) {
    v16 = (ViewType[0] & 1) != 0 ? ViewType[0] - 1 : ViewType[0] + 1;
    if ((Car[v16].byUnk23 & 0x80u) != 0)
      SelectedView[0] = 0;
  }
  select_view(0);
  if (Play_View == 1)
    doteaminit();
  else
    initcarview(player1_car, 0);
  if (player_type == 2) {
    select_view(1);
    initcarview(player2_car, 1);
  }
  initnearcars();
  ticks = 0;
  p_eng[0] = (int)&CarEngines.engines[Car[player1_car].byCarDesignIdx];
  if (player_type == 2)
    p_eng_variable_1 = (int)&CarEngines.engines[Car[player2_car].byCarDesignIdx];
  v17 = *(_DWORD *)(p_eng[0] + 72) + 2 * *(_DWORD *)(p_eng[0] + 76);
  v18 = *(_DWORD *)(p_eng[0] + 92);
  p_joyk1[0] = ((v18 * (*(_DWORD *)(p_eng[0] + 72) - *(_DWORD *)(p_eng[0] + 76))) << 8) / v17;
  p_joyk2[0] = ((3 * v18 * *(_DWORD *)(p_eng[0] + 76)) << 16) / v17;
  if (player_type == 2) {
    v19 = *(_DWORD *)(p_eng_variable_1 + 72) + 2 * *(_DWORD *)(p_eng_variable_1 + 76);
    v20 = *(_DWORD *)(p_eng_variable_1 + 92);
    p_joyk1_variable_1 = ((v20 * (*(_DWORD *)(p_eng_variable_1 + 72) - *(_DWORD *)(p_eng_variable_1 + 76))) << 8) / v19;
    p_joyk2_variable_1 = ((3 * v20 * *(_DWORD *)(p_eng_variable_1 + 76)) << 16) / v19;
  }
  start_race = -1;
  set_palette(0);
  fadedin = 0;
  totaltrackdistance = 0;
  if (TRAK_LEN > 0) {
    pLocalDataAy = localdata;
    iTrackLen = TRAK_LEN;
    dTotalTrackDistance = (double)totaltrackdistance;
    do {
      dTotalTrackDistance2 = threed_c_variable_31 * pLocalDataAy->fUnk13 + dTotalTrackDistance;
      ++pLocalDataAy;
      _CHP();
      dTotalTrackDistance = dTotalTrackDistance2;
    } while (v25 < iTrackLen);
    totaltrackdistance = (int)dTotalTrackDistance2;
  }
  result = totaltrackdistance / TRAK_LEN;
  v27 = network_mes_mode;
  averagesectionlen = totaltrackdistance / TRAK_LEN;
  if (winner_mode || game_type == 2) {
    if (numcars > 0) {
      result = 0;
      v28 = 308 * numcars;
      do {
        result += 308;
        CarBox_variable_25[result] = 0;
        *(int *)((char *)&CarBox_variable_24 + result) = 0;
      } while (result < v28);
    }
    race_started = -1;
  }
  network_mes_mode = v27;
  return result;*/
}

//-------------------------------------------------------------------------------------------------

void play_game_uninit(int a1, int a2, int a3, int a4)
{/*
  int v4; // edx
  int v5; // eax
  int v6; // eax
  int v7; // ebx
  unsigned int v8; // eax

  _disable();
  if (!winner_mode && replaytype != 2 && network_on) {
    name_copy((int)player_names, &player_names[9 * wConsoleNode]);
    manual_control[0] = manual_control[wConsoleNode];
    player_invul[0] = player_invul[wConsoleNode];
    a1 = Players_Cars[wConsoleNode];
    Players_Cars[0] = a1;
    wConsoleNode = 0;
  }
  frontend_on = -1;
  _enable();
  v4 = replaytype;
  if (replaytype == 2) {
    a1 = replay_cheat;
    cheat_mode = replay_cheat;
  }
  if ((cheat_mode & 2) != 0) {
    level = game_level;
    a1 = game_dam;
    damage_level = game_dam;
  }
  if (replaytype != 2 && !winner_mode) {
    game_size = scr_size;
    game_svga = SVGA_ON;
    v5 = DeathView;
    if (DeathView < 0)
      v5 = SelectedView[0];
    game_view = v5;
    a3 = DeathView_variable_1[0];
    if (DeathView_variable_1[0] < 0)
      game_view_variable_1 = SelectedView_variable_1;
    else
      game_view_variable_1 = DeathView_variable_1[0];
    a1 = player1_car;
    a4 = player_type;
    result_p1 = player1_car;
    if (player_type == 2) {
      a1 = player2_car;
      result_p2 = player2_car;
    }
  }
  if (replaytype == 2) {
    if ((textures_off & 0x2000) != 0) {
      textures_off ^= 0x2000u;
      cheat_mode |= 0x40u;
    }
    a1 = replay_player;
    player_type = replay_player;
  }
  stopreplay(a1, replaytype, a3, a4);
  tick_on = 0;
  if (network_error == 666) {
    network_buggered = 666;
  } else if (network_sync_error) {
    network_buggered = 456;
  } else {
    network_buggered = network_error;
  }
  network_error = 0;
  network_sync_error = 0;
  fade_palette(0, v4, 0, a4);
  if (!loading_replay)
    stopmusic(v6, v4);
  stopallsamples();
  releasesamples();
  free_game_memory();
  if (v4 == 2) {
    network_buggered = 0;
    if (numcars > 0) {
      v7 = 4 * numcars;
      v8 = 0;
      do {
        v8 += 4;
        TrackArrow_variable_1[v8 / 4] = result_best_variable_1[v8 / 4];
      } while ((int)v8 < v7);
    }
  } else {
    SaveRecords();
    save_fatal_config();
  }
  if (no_mem) {
    fre(&scrbuf);
    scrbuf = (uint8 *)getbuffer(0x3E800u);
  }
  tick_on = -1;*/
}

//-------------------------------------------------------------------------------------------------

int winner_race()
{
  /*
  int v0; // ecx
  int v1; // eax
  int v2; // edx
  int v3; // edx
  int v4; // edx
  int v5; // ebx
  int result; // eax
  int v7; // ecx
  int v8; // ebx

  v0 = numcars;
  v1 = 0;
  if (numcars > 0) {
    v2 = 0;
    do {
      finished_car_variable_1[++v2] = v1++;
      TrackArrow_variable_1[v2] = -1;
    } while (v1 < v0);
  }
  v3 = carorder[0];
  winner_mode = -1;
  winner_done = 0;
  ViewType = carorder[0];
  if (prev_track >= 17)
    SelectedView[0] = 1;
  else
    SelectedView[0] = 4;
  grid[0] = carorder[0];
  champ_zoom = 0;
  non_competitors[carorder[0]] = 0;
  human_control[v3] = 0;
  grid[v3] = 0;
  start_race = 0;
  countdown = -36;
  gosound = 3;
  delayread = 0;
  delaywrite = 6;
  writeptr = 0;
  readptr = 0;
  v4 = racers;
  v5 = player_type;
  numcars = v0;
  player_type = 0;
  replaytype = 0;
  racers = 1;
  play_game(prev_track);
  result = 270;
  v7 = numcars;
  winner_mode = 0;
  racers = v4;
  player_type = v5;
  VIEWDIST = 270;
  if (numcars > 0) {
    v8 = 4 * numcars;
    result = 0;
    do {
      result += 4;
      *(int *)((char *)TrackArrow_variable_1 + result) = *(int *)((char *)result_best_variable_1 + result);
    } while (result < v8);
  }
  numcars = v7;
  return result;*/
  return 0;
}

//-------------------------------------------------------------------------------------------------

int champion_race()
{
  /*
  int v0; // ebx
  unsigned int v1; // eax
  int v2; // eax
  int v3; // ecx
  int v4; // ebx
  int v5; // edx
  int v6; // esi
  int v7; // edx
  int v8; // ecx
  int v9; // edi
  int i; // eax
  int v11; // ebx
  int v12; // edx
  int v13; // edx
  unsigned int v14; // eax
  int v15; // ebx
  int result; // eax
  int v17; // [esp+0h] [ebp-20h]
  int v18; // [esp+4h] [ebp-1Ch]

  v17 = TrackLoad;
  champ_size = scr_size;
  if (game_type != 1 && numcars > 0) {
    v0 = 4 * numcars;
    v1 = 0;
    do {
      v1 += 4;
      teamorder_variable_1[v1 / 4] = result_lap_variable_1[v1 / 4];
    } while ((int)v1 < v0);
  }
  v18 = champ_track[champorder[0] / 2];
  v2 = 0;
  if (racers > 0) {
    v3 = racers;
    v4 = 0;
    do {
      ++v4;
      v5 = champorder[v3 - 1 - v2++];
      finished_car_variable_1[v4] = v5;
      non_competitors[v5] = 0;
      human_control[v5] = 0;
    } while (v2 < v3);
  }
  v6 = racers;
  v7 = 0;
  if (racers < numcars) {
    v8 = 4 * racers;
    v9 = 4 * numcars;
    do {
      for (i = v7; !result_competing[i]; ++i)
        ++v7;
      ++v6;
      grid[v8 / 4u] = v7;
      v8 += 4;
      human_control[i] = 0;
      non_competitors[i] = -1;
      ++v7;
    } while (v8 < v9);
  }
  v11 = racers;
  winner_mode = -1;
  winner_done = 0;
  champ_mode = -1;
  champ_zoom = 0;
  SelectedView[0] = 8;
  v12 = teamorder_variable_1[racers];
  champ_car = racers - 1;
  ViewType = v12;
  champ_go[v12] = -1;
  start_race = 0;
  countdown = -36;
  gosound = 3;
  delayread = 0;
  delaywrite = 6;
  writeptr = 0;
  readptr = 0;
  v13 = player_type;
  replaytype = 0;
  player_type = 0;
  play_game(v18);
  VIEWDIST = 270;
  winner_mode = 0;
  champ_mode = 0;
  racers = v11;
  player_type = v13;
  TrackLoad = v17;
  if (numcars > 0) {
    v14 = 0;
    v15 = 4 * numcars;
    do {
      v14 += 4;
      TrackArrow_variable_1[v14 / 4] = result_best_variable_1[v14 / 4];
    } while ((int)v14 < v15);
  }
  result = champ_size;
  scr_size = champ_size;
  return result;*/
  return 0;
}

//-------------------------------------------------------------------------------------------------

void play_game(int a1, int a2, int a3)
{
#if _DEBUG
  startmusic(titlesong); // Add by ROLLER to start music in debug mode
#endif
  /*
  int v3; // eax
  int v4; // ecx
  int v5; // edx
  int v6; // eax
  __int16 i; // bx
  int v8; // eax
  int v9; // ebx
  int v10; // eax

  game_track = a1;
  lagdone = 0;
  I_Want_Out = 0;
  play_game_init();
  reset_net_wait();
  max_mem = mem_used_low + mem_used;
  enable_keyboard();
  pend_view_init = ViewType[0];
  _disable();
  network_limit = 4320;
  v3 = 0;
  do {
    v4 = (__int16)v3;
    v5 = frames;
    ++v3;
    net_time[v4] = frames;
  } while ((__int16)v3 < 16);
  v6 = frames;
  network_timeout = frames;
  network_error = 0;
  network_sync_error = 0;
  frontend_on = 0;
  _enable();
  while (racing || lastsample > 0) {
    if (dostopsamps) {
      stopallsamples();
      dostopsamps = 0;
    }
    if (network_on) {
      if (frame_number >= 250)
        countdown = -75;
      if (countdown == 144 && replaytype != 2 && fadedin)
        send_ready();
      if (network_limit == 4320 && countdown < 140) {
        v5 = 360;
        _disable();
        network_limit = 360;
        for (i = 0; ; ++i) {
          v8 = i;
          if (i >= network_on)
            break;
          v5 = frames;
          net_time[v8] = frames;
        }
        network_timeout = frames;
        _enable();
      }
    }
    if (player_type == 1 && (network_error || network_sync_error || cd_error))
      racing = 0;
    if (fadedin && !lagdone && (countdown < 140 || replaytype == 2)) {
      initsoundlag(0);
      lagdone = -1;
    }
    if (champ_mode < 16)
      control();
    else
      firework_display();
    if (!replayspeed && intro && !game_req)
      racing = replayspeed;
    if (track_playing) {
      v5 = 36 * track_duration / 75;
      if (frames - start_cd > v5) {
        StopTrack();
        RepeatTrack();
        start_cd = frames;
      }
    }
    if (network_on) {
      if (net_quit) {
        v5 = intro;
        if (!intro)
          racing = 0;
      }
    }
    v4 = player_type;
    if (player_type == 2) {
      if ((Car[player1_car].byUnk23 & 0x80u) != 0
        && (Car[player2_car].byUnk23 & 0x80u) != 0
        && lastsample < -72
        && readsample == writesample
        && player_type != replaytype) {
        v4 = 0;
        racing = 0;
      }
    } else if (!network_on
           && (Car[player1_car].byUnk23 & 0x80u) != 0
           && lastsample < -72
           && readsample == writesample
           && replaytype != 2) {
      racing = writesample ^ readsample;
    }
    if (winner_mode) {
      if (winner_done && lastsample < -72 && readsample == writesample) {
        v4 = 0;
        racing = 0;
      } else {
        v9 = champ_mode;
        racing = -1;
        if (champ_mode) {
          if (champ_mode >= 16 && champ_zoom > 7 && champ_count < 0 && !winner_done) {
            v9 = 18;
            v4 = player1_car;
            v5 = 0x8000;
            speechsample(114, 0x8000, 18, player1_car);
            winner_done = -1;
          }
          if (!winner_done && champ_mode == 2) {
            v9 = 18;
            v4 = player1_car;
            v5 = 0x8000;
            speechsample(114, 0x8000, 18, player1_car);
            ++champ_mode;
          }
          if (champ_mode == 3 && lastsample < -72) {
            v5 = writesample;
            if (readsample == writesample) {
              holdmusic = -1;
              fade_palette(writesample ^ readsample, writesample, v9, v4);
              v4 = 16;
              ++champ_mode;
              firework_screen();
              fade_palette(32, 720, 792, 16);
              champ_count = 720;
              champ_zoom = 3;
              champ_mode = 16;
              memset(CarSpray, 0, 0x318u);
              if (SVGA_ON)
                scr_size = 128;
              else
                scr_size = 64;
              v5 = 0;
              winw = XMAX;
              winx = 0;
              winy = 0;
              winh = YMAX;
            }
          }
        } else if ((char)Car[ViewType[0]].byUnk31 > 1 && !winner_done) {
          v4 = player1_car;
          v5 = 0x8000;
          speechsample(114, 0x8000, 18, player1_car);
          winner_done = -1;
        }
      }
    }
    if (champ_mode < 16)
      updatescreen();
    else
      firework_screen();
    if (print_data)
      printf(&threed_c_variable_32);
    a3 = 0;
    print_data = 0;
    if (pause_request && !intro) {
      if (!pausewindow || !paused) {
        if (network_on && replaytype != 2) {
          if (wConsoleNode == master) {
            if (!finished_car[player1_car]) {
              paused = paused == 0;
              if (paused)
                stopallsamples();
              if (paused)
                pauser = wConsoleNode;
              transmitpausetoslaves();
            }
          } else if (!finished_car[player1_car]) {
            send_pause();
          }
        } else {
          if (game_req) {
            clear_borders = -1;
            scr_size = req_size;
            remove_uncalibrated();
            check_joystick_usage();
          } else {
            req_size = scr_size;
            if (SVGA_ON)
              scr_size = 128;
            else
              scr_size = 64;
          }
          control_edit = -1;
          req_edit = 0;
          a3 = 0;
          game_req = game_req == 0;
          pausewindow = 0;
          paused = paused == 0;
          if (paused)
            stopallsamples();
        }
      }
      v5 = 0;
      pause_request = 0;
    }
    if (network_on && slave_pause && wConsoleNode == master) {
      paused = paused == 0;
      if (paused)
        stopallsamples();
      transmitpausetoslaves();
      slave_pause = 0;
    }
    if (!filingmenu)
      game_keys();
    if (replaytype == 2) {
      if (SVGA_ON)
        scr_size = 128;
      else
        scr_size = 64;
    }
    if (!racing && !winner_done && winner_mode) {
      a3 = 18;
      v4 = player1_car;
      v5 = 0x8000;
      speechsample(114, 0x8000, 18, player1_car);
      winner_done = -1;
      racing = -1;
    }
    v6 = keys_variable_3 || keys_variable_4;
    shifting = v6;
    if (v6 && keys_variable_8 || keys_variable_2 && controlicon == 8) {
      if (shifting && keys_variable_8)
        controlicon = 9;
      v5 = 0;
      a3 = rewinding;
      slowing = 0;
      if (!rewinding)
        v6 = Rrewindstart(v6, 0);
    } else if (rewinding) {
      slowing = -1;
    }
    if (shifting && keys_variable_9 || keys_variable_2 && controlicon == 10) {
      if (shifting && keys_variable_9)
        controlicon = 9;
      v6 = 0;
      v5 = forwarding;
      slowing = 0;
      if (!forwarding)
        v6 = Rforwardstart(0, 0);
    } else if (forwarding) {
      slowing = -1;
    }
    if (screenready) {
      v4 = fadedin;
      if (!fadedin) {
        fade_palette(32, v5, a3, 0);
        v5 = w95;
        fadedin = -1;
        holdmusic = 0;
        if (w95) {
          if (!MusicCD && !winner_mode && !loading_replay) {
            if (replaytype == 2)
              v10 = titlesong;
            else
              v10 = game_track;
            v6 = startmusic(v10);
          }
        }
      }
    }
    if (!intro && replaytype != 2) {
      v6 = player_type;
      if (player_type && player_type != 2) {
        if (!cdchecked) {
          if (active_nodes == network_on)
            cdchecked = -1;
          v6 = wConsoleNode;
          if (wConsoleNode == master && !netCD) {
            v6 = active_nodes;
            if (active_nodes == network_on) {
              v6 = send_nocd_error();
              cd_error = -1;
              cdchecked = -1;
            }
          }
        }
      } else {
        v6 = localCD;
        if (!localCD) {
          racing = 0;
          cd_error = -1;
        }
      }
    }
  }
  play_game_uninit(v6, v5, a3, v4);*/
}

//-------------------------------------------------------------------------------------------------

void game_keys(int64 a1)
{/*
  unsigned int v1; // eax
  int v2; // ecx
  int v3; // eax
  int v4; // eax
  int v5; // eax
  _BOOL1 v6; // zf
  bool v7; // eax
  uint32 v8; // eax
  uint32 v9; // eax
  uint32 v10; // ecx
  uint32 v11; // eax
  bool v12; // eax

  if (define_mode)
    LABEL_328:
  JUMPOUT(0x1381B);
  while (1) {
    do {
      while (1) {
        do {
          while (1) {
            while (1) {
              while (1) {
              LABEL_2:
                if (!fatkbhit())
                  goto LABEL_328;
                v1 = fatgetch();
                v2 = intro;
                if (intro || winner_mode) {
                  if (v1) {
                    v1 = -1;
                    racing = 0;
                  } else {
                    fatgetch();
                    v1 = -1;
                  }
                }
                if (trying_to_exit) {
                  if (v1) {
                    if (v1 == 121 || v1 == 89) {
                      racing = 0;
                      quit_game = -1;
                      scr_size = req_size;
                    }
                    trying_to_exit = 0;
                  } else {
                    trying_to_exit = 0;
                    fatgetch();
                  }
                  v1 = -1;
                }
                if (v1)
                  break;
                v3 = fatgetch();
                if (v3 != 75 && v3 != 77 && v3 != 72 && v3 != 80) {
                  LOBYTE(v2) = keys_variable_3;
                  if (keys_variable_3 || (BYTE1(v2) = keys_variable_4) != 0) {
                    v3 += 25;
                  } else if (keys_variable_5) {
                    v3 += 45;
                  }
                }
                v4 = v3 - 59;
                switch (v4) {
                  case 0:
                    if (network_on)
                      v4 = mesminus();
                    if (replaytype == 2)
                      carminus(v4, 0, 2);
                    break;
                  case 1:
                    if (network_on)
                      v4 = mesplus();
                    if (replaytype == 2)
                      carplus(v4, 0, 2, v2);
                    break;
                  case 2:
                    if (view0_cnt < 0) {
                      LODWORD(a1) = 18;
                      view0_cnt = 18;
                      viewminus(0);
                    }
                    break;
                  case 3:
                    if (view0_cnt < 0) {
                      view0_cnt = 18;
                      viewplus(0);
                    }
                    break;
                  case 6:
                    if (player_type == 2 && view1_cnt < 0) {
                      LODWORD(a1) = 18;
                      view1_cnt = 18;
                      viewminus(1);
                    }
                    break;
                  case 7:
                    if (player_type == 2 && view1_cnt < 0) {
                      view1_cnt = 18;
                      viewplus(1);
                    }
                    break;
                  case 8:
                    if (++names_on > 2)
                      names_on = 0;
                    break;
                  case 9:
                    if (I_Would_Like_To_Quit && Quit_Count <= 0) {
                      I_Want_Out = -1;
                      stopallsamples();
                    }
                    break;
                  case 11:
                    showversion = showversion == 0;
                    break;
                  case 13:
                    HIDWORD(a1) = game_req;
                    if (game_req) {
                      LODWORD(a1) = pausewindow;
                      if (!pausewindow && req_edit > 0)
                        --req_edit;
                      if (pausewindow == 1 && !calibrate_mode && calibrate_select < pausewindow)
                        calibrate_select += pausewindow;
                      HIDWORD(a1) = pausewindow;
                      if (pausewindow == 2) {
                        LODWORD(a1) = control_select;
                        if (control_select < 2)
                          ++control_select;
                      }
                      if (pausewindow == 3) {
                        HIDWORD(a1) = graphic_mode;
                        if (graphic_mode < 16)
                          LODWORD(a1) = ++graphic_mode;
                      }
                      if (pausewindow == 4) {
                        if (sound_edit <= 1) {
                          if (!sound_edit)
                            sound_edit = 7;
                        } else {
                          --sound_edit;
                        }
                      }
                    } else if (replaytype == 2 && game_req != replaypanel) {
                      switch (game_req + controlicon) {
                        case 7:
                        case 8:
                        case 15:
                        case 16:
                        case 17:
                          controlicon -= 6;
                          break;
                        case 9:
                        case 10:
                        case 11:
                        case 12:
                        case 13:
                        case 14:
                          controlicon -= 5;
                          break;
                        default:
                          continue;
                      }
                    }
                    break;
                  case 16:
                    if (game_req) {
                      LODWORD(a1) = pausewindow;
                      if (pausewindow == 4) {
                        switch (sound_edit) {
                          case 1:
                            EngineVolume -= pausewindow;
                            if (EngineVolume < 0)
                              EngineVolume = 0;
                            continue;
                          case 2:
                            HIDWORD(a1) = SFXVolume - pausewindow;
                            SFXVolume = HIDWORD(a1);
                            if (a1 < 0)
                              SFXVolume = 0;
                            continue;
                          case 3:
                            SpeechVolume -= pausewindow;
                            if (SpeechVolume < 0)
                              SpeechVolume = 0;
                            continue;
                          case 4:
                            MusicVolume -= pausewindow;
                            if (MusicVolume < 0)
                              MusicVolume = 0;
                            if (MusicCard)
                              sosMIDISetMasterVolume(MusicVolume);
                            if (MusicCD)
                              goto LABEL_82;
                            continue;
                          default:
                            continue;
                        }
                      }
                    } else if (replaytype == 2 && game_req != replaypanel) {
                      LODWORD(a1) = controlicon;
                      if (controlicon != 1 && controlicon != 7 && controlicon != 12)
                        HIDWORD(a1) = --controlicon;
                    }
                    break;
                  case 18:
                    v5 = game_req;
                    if (game_req) {
                      if (pausewindow == 4) {
                        switch (sound_edit) {
                          case 1:
                            EngineVolume += 4;
                            if (EngineVolume >= 128)
                              EngineVolume = 127;
                            break;
                          case 2:
                            HIDWORD(a1) = pausewindow + SFXVolume;
                            SFXVolume = HIDWORD(a1);
                            if (SHIDWORD(a1) >= 128)
                              SFXVolume = 127;
                            break;
                          case 3:
                            LODWORD(a1) = pausewindow + SpeechVolume;
                            SpeechVolume = a1;
                            if ((int)a1 >= 128)
                              SpeechVolume = 127;
                            break;
                          case 4:
                            HIDWORD(a1) = pausewindow + MusicVolume;
                            MusicVolume = HIDWORD(a1);
                            if (SHIDWORD(a1) >= 128)
                              MusicVolume = 127;
                            if (MusicCard)
                              sosMIDISetMasterVolume(MusicVolume);
                            if (MusicCD)
                              LABEL_82:
                            SetAudioVolume(MusicVolume);
                            break;
                          default:
                            continue;
                        }
                      }
                    } else if (replaytype == 2 && game_req != replaypanel) {
                      LODWORD(a1) = controlicon;
                      if (controlicon != 6 && controlicon != 11 && controlicon != 25) {
                        if (controlicon != 17 || game_req != replayedit)
                          v5 = 1;
                        if (v5)
                          ++controlicon;
                      }
                    }
                    break;
                  case 21:
                    HIDWORD(a1) = game_req;
                    if (game_req) {
                      if (!pausewindow && req_edit < 6)
                        ++req_edit;
                      HIDWORD(a1) = pausewindow;
                      if (pausewindow == 1) {
                        LODWORD(a1) = calibrate_select;
                        if (calibrate_select > 0 && !calibrate_mode)
                          calibrate_select -= pausewindow;
                      }
                      if (pausewindow == 2) {
                        LODWORD(a1) = control_select;
                        if (control_select > 0)
                          --control_select;
                      }
                      if (pausewindow == 3) {
                        HIDWORD(a1) = graphic_mode;
                        if (graphic_mode > 0)
                          LODWORD(a1) = --graphic_mode;
                      }
                      if (pausewindow == 4 && sound_edit > 0) {
                        if (sound_edit >= 7)
                          sound_edit = 0;
                        else
                          LODWORD(a1) = ++sound_edit;
                      }
                    } else if (replaytype == 2 && game_req != replaypanel) {
                      switch (controlicon) {
                        case 1:
                        case 2:
                        case 3:
                        case 9:
                        case 10:
                        case 11:
                          controlicon += 6;
                          break;
                        case 4:
                        case 5:
                        case 6:
                        case 7:
                        case 8:
                          controlicon += 5;
                          break;
                        default:
                          continue;
                      }
                    }
                    break;
                  case 25:
                    controlicon = 9;
                    Rreverseplay(v4, 0, 2, v2);
                    break;
                  case 26:
                    controlicon = 9;
                    Rplay(a1);
                    break;
                  case 27:
                    controlicon = 9;
                    Rspeedminus(a1);
                    break;
                  case 28:
                    controlicon = 9;
                    Rspeedplus(v4, 0, a1);
                    break;
                  case 29:
                    controlicon = 9;
                    Rframeminus(v4, 0);
                    break;
                  case 30:
                    controlicon = 9;
                    Rframeplus(v4, 0);
                    break;
                  case 33:
                    controlicon = 9;
                    Rstart(v4, 0, 2, v2);
                    break;
                  case 34:
                    controlicon = 9;
                    Rend(v4, 0, 2, v2);
                    break;
                  case 45:
                    controlicon = 9;
                    rtoggleedit(v4, 0, 2, v2);
                    break;
                  case 46:
                    controlicon = 9;
                    rstartblock(v4, 0, 2, v2);
                    break;
                  case 47:
                    controlicon = 9;
                    rselectblock();
                    break;
                  case 48:
                    controlicon = 9;
                    rdeleteblock(a1);
                    break;
                  case 49:
                    controlicon = 9;
                    rstoreview(v4, 0, 2, v2);
                    break;
                  case 50:
                    controlicon = 9;
                    rremoveview(v4, 0, 2, v2);
                    break;
                  case 51:
                    controlicon = 9;
                    rpreviouscut(v4, 0, 2, v2);
                    break;
                  case 52:
                    controlicon = 9;
                    rnextcut(v4, 0, 2, v2);
                    break;
                  case 53:
                    controlicon = 9;
                    rstartassemble(a1);
                    break;
                  default:
                    continue;
                }
              }
              if (v1 < 0x2D)
                break;
              if (v1 <= 0x2D) {
              LABEL_313:
                if (replaytype != 2) {
                  if (game_req) {
                    if (pausewindow == 3 && graphic_mode == 2) {
                      if (SVGA_ON) {
                        req_size -= 16;
                        if (req_size < 64)
                          req_size = 64;
                      } else {
                        req_size -= 8;
                        if (req_size < 32)
                          req_size = 32;
                      }
                    }
                  } else if (SVGA_ON) {
                    LODWORD(a1) = scr_size - 16;
                    scr_size = a1;
                    if ((int)a1 >= 64)
                      goto LABEL_318;
                    scr_size = 64;
                  } else {
                    scr_size -= 8;
                    if (scr_size >= 32)
                      LABEL_318:
                    clear_borders = -1;
                    else
                      scr_size = 32;
                  }
                }
              } else if (v1 < 0x4C) {
                if (v1 >= 0x3D) {
                  if (v1 <= 0x3D) {
                  LABEL_291:
                    if (replaytype != 2) {
                      if (game_req) {
                        if (pausewindow == 3 && graphic_mode == 2) {
                          if (SVGA_ON) {
                            LODWORD(a1) = req_size + 16;
                            req_size = a1;
                            if ((int)a1 > 128)
                              req_size = 128;
                            if (req_size == 128)
                              replaypanel = -1;
                          } else {
                            LODWORD(a1) = req_size + 8;
                            req_size = a1;
                            if ((int)a1 > 64)
                              req_size = 64;
                            if (req_size == 64)
                              replaypanel = -1;
                          }
                        }
                      } else if (SVGA_ON) {
                        scr_size += 16;
                        if (scr_size > 128)
                          scr_size = 128;
                        if (scr_size == 128)
                          replaypanel = -1;
                      } else {
                        scr_size += 8;
                        if (scr_size > 64)
                          scr_size = 64;
                        if (scr_size == 64)
                          replaypanel = -1;
                      }
                    }
                  } else if (v1 == 68) {
                    if (keys_variable_5) {
                      HIDWORD(a1) = replaytype;
                      controlicon = 9;
                      if (replaytype == 2)
                        filingmenu = 3;
                    }
                  }
                }
              } else if (v1 <= 0x4C) {
                if (keys_variable_5) {
                  a1 = (unsigned int)replaytype | 0x900000000LL;
                  controlicon = 9;
                  if (replaytype == 2)
                    filingmenu = 1;
                }
              } else if (v1 >= 0x53) {
                if (v1 <= 0x53) {
                  if (keys_variable_5) {
                    controlicon = 9;
                    if (replaytype == 2)
                      filingmenu = 2;
                  }
                } else if (v1 == 95) {
                  goto LABEL_313;
                }
              }
            }
            if (v1 < 0x1B)
              break;
            if (v1 <= 0x1B) {
              if (game_req && pausewindow) {
                if (pausewindow == 1 || pausewindow == 2)
                  remove_uncalibrated();
                pausewindow = 0;
              } else if (filingmenu) {
                filingmenu = 0;
                lastfile = 0;
              } else if (!network_on || replaytype == 2) {
              LABEL_194:
                pause_request = -1;
              } else if (active_nodes == network_on) {
                if (I_Would_Like_To_Quit == -1) {
                  if (Quit_Count <= 0)
                    I_Would_Like_To_Quit = 0;
                } else {
                  LODWORD(a1) = 18;
                  I_Would_Like_To_Quit = -1;
                  Quit_Count = 18;
                }
              }
            } else if (v1 >= 0x20) {
              if (v1 <= 0x20) {
                if (replaytype == 2) {
                  HIDWORD(a1) = 9;
                  replaypanel = replaypanel == 0;
                  controlicon = 9;
                  rotpoint = currentreplayframe;
                }
              } else if (v1 == 43) {
                goto LABEL_291;
              }
            }
          }
        } while (v1 != 13);
        LODWORD(a1) = game_req;
        if (!game_req)
          break;
        switch (pausewindow) {
          case 0:
            switch (req_edit) {
              case 0:
                goto LABEL_194;
              case 1:
                paused = 0;
                racing = 0;
                gave_up = -1;
                scr_size = req_size;
                goto LABEL_2;
              case 2:
                sound_edit = 0;
                pausewindow = 4;
                goto LABEL_2;
              case 3:
                LODWORD(a1) = 1;
                calibrate_select = 0;
                calibrate_mode = 0;
                pausewindow = 1;
                goto LABEL_2;
              case 4:
                pausewindow = 2;
                Joy1used = 0;
                Joy2used = 0;
                controlrelease = -1;
                define_mode = 0;
                control_select = 0;
                control_edit = -1;
                goto LABEL_2;
              case 5:
                HIDWORD(a1) = 3;
                graphic_mode = 0;
                pausewindow = 3;
                goto LABEL_2;
              case 6:
                trying_to_exit = -1;
                goto LABEL_2;
              default:
                goto LABEL_2;
            }
          case 1:
            if (calibrate_select) {
              if (calibrate_select == 1) {
                v6 = calibrate_mode != 0;
                calibrate_mode = calibrate_mode == 0;
                if (v6)
                  remove_uncalibrated();
                else
                  check_joystickpresence();
              }
            } else {
              pausewindow = 0;
              remove_uncalibrated();
              check_joystick_usage();
              calibrate_mode = 0;
            }
            break;
          case 2:
            if (control_select) {
              if ((unsigned int)control_select <= 1) {
                define_mode = -1;
                control_edit = 0;
                disable_keyboard();
                controlrelease = -1;
                qmemcpy(oldkeys, userkey, 0xCu);
                qmemcpy(&oldkeys[12], &userkey[12], 2u);
                HIDWORD(a1) = &userkey[14];
                LODWORD(a1) = oldkeys;
              } else if (control_select == 2) {
                define_mode = -1;
                control_edit = 6;
                disable_keyboard();
                controlrelease = -1;
                qmemcpy(oldkeys, userkey, 0xCu);
                qmemcpy(&oldkeys[12], &userkey[12], 2u);
                HIDWORD(a1) = &userkey[14];
                LODWORD(a1) = oldkeys;
              }
            } else {
            LABEL_212:
              pausewindow = 0;
            }
            break;
          case 3:
            switch (graphic_mode) {
              case 0:
                goto LABEL_212;
              case 1:
                if (svga_possible && !no_mem || SVGA_ON) {
                  v7 = SVGA_ON == 0;
                  SVGA_ON = v7;
                  init_screen(v7, 0, 2);
                  req_size = scr_size;
                }
                goto LABEL_2;
              case 2:
                if (replaytype != 2) {
                  if (SVGA_ON) {
                    req_size += 16;
                    if (req_size > 128)
                      req_size = 64;
                    if (req_size == 128)
                      replaypanel = -1;
                  } else {
                    req_size += 8;
                    if (req_size > 64)
                      req_size = 32;
                    if (req_size == 64)
                      replaypanel = -1;
                  }
                }
                goto LABEL_2;
              case 3:
                if (view_limit) {
                  view_limit = 0;
                } else if (machine_speed >= 2800) {
                  view_limit = 32;
                } else {
                  view_limit = 24;
                }
                goto LABEL_2;
              case 4:
                if ((textures_off & 0x20) != 0) {
                  v8 = textures_off;
                  LOBYTE(v8) = textures_off ^ 0x20;
                  textures_off = ((unsigned int)&loc_3FFFC + 4) | v8;
                } else if ((((unsigned int)&loc_3FFFC + 4) & textures_off) != 0) {
                  textures_off ^= (unsigned int)&loc_3FFFC + 4;
                } else {
                  HIDWORD(a1) = textures_off | 0x20;
                  textures_off |= 0x20u;
                }
                goto LABEL_2;
              case 5:
                LODWORD(a1) = textures_off ^ 8;
                textures_off ^= 8u;
                goto LABEL_2;
              case 6:
                HIDWORD(a1) = textures_off ^ 0x100;
                textures_off ^= 0x100u;
                goto LABEL_2;
              case 7:
                textures_off ^= 2u;
                goto LABEL_2;
              case 8:
                v9 = textures_off;
                LOBYTE(v9) = textures_off ^ 0x80;
                textures_off = v9;
                goto LABEL_2;
              case 9:
                LODWORD(a1) = textures_off ^ 1;
                textures_off ^= 1u;
                goto LABEL_2;
              case 10:
                HIDWORD(a1) = textures_off ^ 4;
                textures_off ^= 4u;
                goto LABEL_2;
              case 11:
                v10 = textures_off;
                LOBYTE(v10) = textures_off ^ 0x40;
                textures_off = v10;
                goto LABEL_2;
              case 12:
                v11 = textures_off;
                LOBYTE(v11) = textures_off ^ 0x10;
                textures_off = v11;
                goto LABEL_2;
              case 13:
                LODWORD(a1) = textures_off ^ 0x800;
                textures_off ^= 0x800u;
                goto LABEL_2;
              case 14:
                HIDWORD(a1) = textures_off ^ 0x200;
                textures_off ^= 0x200u;
                goto LABEL_2;
              case 15:
                if (++names_on > 2)
                  names_on = 0;
                goto LABEL_2;
              case 16:
                textures_off ^= 0x80000u;
                goto LABEL_2;
              default:
                goto LABEL_2;
            }
          case 4:
            switch (sound_edit) {
              case 0:
                goto LABEL_212;
              case 5:
                allengines = allengines == 0;
                goto LABEL_2;
              case 6:
                LODWORD(a1) = SoundCard;
                if (SoundCard) {
                  v6 = soundon != 0;
                  soundon = soundon == 0;
                  if (!v6)
                    loadsamples();
                } else {
                  soundon = 0;
                }
                goto LABEL_2;
              case 7:
                if (MusicCard || (v2 = MusicCD) != 0) {
                  v12 = musicon == 0;
                  musicon = v12;
                  reinitmusic(v12, 0, 2, v2);
                } else {
                  musicon = 0;
                }
                goto LABEL_2;
              default:
                goto LABEL_2;
            }
          default:
            goto LABEL_2;
        }
      }
    } while (replaytype != 2 || game_req == replaypanel && controlicon != 9 || !ricon_variable_2[2 * controlicon]);
    if ((unsigned int)controlicon < 0xE) {
    LABEL_273:
      ((void (*)(void))ricon_variable_2[2 * controlicon])();
    } else if ((unsigned int)controlicon <= 0xE) {
      viewminus(0);
    } else {
      if (controlicon != 15)
        goto LABEL_273;
      viewplus(0);
    }
  }*/
}

//-------------------------------------------------------------------------------------------------

int mesminus()
{
  /*
  int v0; // ecx
  int result; // eax
  int v2; // edx

  v0 = network_mes_mode;
  result = network_mes_mode;
  v2 = network_mes_mode;
  do {
    if (result < 0)
      result = numcars;
    if (--result == team_mate || result < 0 && players > 2 || human_control[result] && result != (__int16)player1_car) {
      v2 = -666;
      v0 = result;
    }
  } while (v2 == v0);
  network_mes_mode = v0;
  return result;*/
  return 0;
}

//-------------------------------------------------------------------------------------------------

int mesplus()
{
/*
  int v0; // ecx
  int result; // eax
  int v2; // edx

  v0 = network_mes_mode;
  result = network_mes_mode;
  v2 = network_mes_mode;
  do {
    if (++result >= numcars)
      result = -1;
    if (result == team_mate || result < 0 && players > 2 || human_control[result] && result != (__int16)player1_car) {
      v2 = -666;
      v0 = result;
    }
  } while (v2 == v0);
  network_mes_mode = v0;
  return result;*/
  return 0;
}

//-------------------------------------------------------------------------------------------------

int carminus(int a1, int a2, int a3)
{
  /*
  __int64 v4; // [esp-4h] [ebp-8h]

  LODWORD(v4) = a2;
  do
  {
    if ( --ViewType[0] < 0 )
      ViewType[0] = numcars - 1;
  }
  while ( (Car[ViewType[0]].byUnk23 & 0x80u) != 0 );
  if ( Play_View == 1 )
    doteaminit();
  else
    initcarview(ViewType[0], 0);
  return sfxsample(v4);
  */
  return 0;
}

//-------------------------------------------------------------------------------------------------

int carplus(int a1, unsigned int a2, int a3, unsigned int a4)
{
  return 0;
  /*
  do {
    if (++ViewType[0] >= numcars)
      ViewType[0] = 0;
  } while ((Car[ViewType[0]].byUnk23 & 0x80u) != 0);
  if (Play_View == 1)
    doteaminit();
  else
    initcarview(ViewType[0], 0);
  return sfxsample(__SPAIR64__(a4, a2));*/
}

//-------------------------------------------------------------------------------------------------

int viewminus(int a1)
{
  (void)(a1);
  return 0;
  /*
  int result; // eax
  int v3; // edx
  int v4; // ebx
  int v5; // ebx
  int v6; // ebx
  int v7; // eax
  int v8; // edi
  int v9; // ebx
  int v10; // esi

  result = a1;
  v3 = a1;
  if (replaytype != 2) {
    if ((Car_variable_33[308 * ViewType[result]] & 4) != 0)
      return result * 2;
    if (player_type == 2) {
      v6 = v3 * 4;
      do {
        v10 = SelectedView[v3] - 1;
        SelectedView[v3] = v10;
        if (v10 < 0)
          SelectedView[v3] = 8;
      } while (!DuoViews[SelectedView[v3]]);
    } else {
      do {
        v8 = SelectedView[v3] - 1;
        SelectedView[v3] = v8;
        if (v8 == 7) {
          v9 = (ViewType[result] & 1) != 0 ? ViewType[result] - 1 : ViewType[result] + 1;
          if (Car_variable_23[308 * v9] < 0)
            --SelectedView[v3];
        }
        if (SelectedView[v3] < 0)
          SelectedView[v3] = 8;
        v6 = SelectedView[v3];
      } while (!AllowedViews[v6]);
    }
    v7 = select_view(a1);
    if (Play_View != 1)
      goto LABEL_12;
  LABEL_31:
    doteaminit(v7);
    return sfxsample(83, 0x8000, v6);
  }
  do {
    v4 = SelectedView[v3] - 1;
    SelectedView[v3] = v4;
    if (v4 == 7) {
      v5 = (ViewType[result] & 1) != 0 ? ViewType[result] - 1 : ViewType[result] + 1;
      if (Car_variable_23[308 * v5] < 0)
        --SelectedView[v3];
    }
    if (SelectedView[v3] < 0)
      SelectedView[v3] = 8;
    v6 = SelectedView[v3];
  } while (!ReplayViews[v6]);
  v7 = select_view(a1);
  if (Play_View == 1)
    goto LABEL_31;
LABEL_12:
  initcarview(ViewType[a1], a1);
  return sfxsample(83, 0x8000, v6);*/
}

//-------------------------------------------------------------------------------------------------

int viewplus(int a1)
{
  (void)(a1);
  return 0;
  /*
  int result; // eax
  int v3; // edx
  int v4; // ebx
  int v5; // ebx
  int v6; // ebx
  int v7; // eax
  int v8; // ebx
  int v9; // ebx
  int v10; // esi

  result = a1;
  v3 = a1;
  if (replaytype != 2) {
    if ((Car_variable_33[308 * ViewType[result]] & 4) != 0)
      return result * 2;
    if (player_type == 2) {
      v6 = v3 * 4;
      do {
        v10 = SelectedView[v3] + 1;
        SelectedView[v3] = v10;
        if (v10 >= 9)
          SelectedView[v3] = 0;
      } while (!DuoViews[SelectedView[v3]]);
    } else {
      do {
        v8 = SelectedView[v3] + 1;
        SelectedView[v3] = v8;
        if (v8 == 7) {
          v9 = (ViewType[result] & 1) != 0 ? ViewType[result] - 1 : ViewType[result] + 1;
          if (Car_variable_23[308 * v9] < 0)
            ++SelectedView[v3];
        }
        if (SelectedView[v3] >= 9)
          SelectedView[v3] = 0;
        v6 = SelectedView[v3];
      } while (!AllowedViews[v6]);
    }
    v7 = select_view(a1);
    if (Play_View != 1)
      goto LABEL_12;
  LABEL_31:
    doteaminit(v7);
    return sfxsample(83, 0x8000, v6);
  }
  do {
    v4 = SelectedView[v3] + 1;
    SelectedView[v3] = v4;
    if (v4 == 7) {
      v5 = (ViewType[result] & 1) != 0 ? ViewType[result] - 1 : ViewType[result] + 1;
      if (Car_variable_23[308 * v5] < 0)
        ++SelectedView[v3];
    }
    if (SelectedView[v3] >= 9)
      SelectedView[v3] = 0;
    v6 = SelectedView[v3];
  } while (!ReplayViews[v6]);
  v7 = select_view(a1);
  if (Play_View == 1)
    goto LABEL_31;
LABEL_12:
  initcarview(ViewType[a1], a1);
  return sfxsample(83, 0x8000, v6);
  */
}

//-------------------------------------------------------------------------------------------------

void game_copypic(uint8 *a1, uint8 *a2, char *a3)
{
  (void)(a1); (void)(a2); (void)(a3);
  /*
  int v6; // eax
  char v7; // dh
  int j; // edx
  char v9; // bl
  int k; // eax
  int i; // eax
  int v12; // eax
  int v13; // ecx
  char *v14; // eax
  int v15; // ecx
  int v16; // ecx
  int v17; // eax
  int v18; // eax
  int v19; // edx
  int v20; // ecx
  int v21; // eax
  char *v22; // edx
  int v23; // ebx
  char *v24; // edx
  int v25; // ebx
  int v26; // edi
  int v27; // edi
  int v28; // edi
  char *v29; // edx
  int v30; // ebx
  int *v31; // [esp-4h] [ebp-14h]

  screen_pointer = (int)a1;
  if (network_on) {
    if (message_received < 0) {
      if (message_sent >= 0) {
        if (message_sent == 4) {
          small_zoom(&language_buffer_variable_103);
        } else {
          small_zoom(&language_buffer_variable_102);
          for (i = 0; i < 24; currentdir_variable_8[i] = *((_BYTE *)&head_y + 14 * message_sent + i + 3))
            ++i;
          subzoom(&buffer);
        }
        message_sent = -1;
      }
    } else {
      v6 = 0;
      if (language_buffer_variable_101[0]) {
        do {
          v7 = language_buffer_variable_101[++v6];
          currentdir_variable_8[v6] = language_buffer_variable_100[v6];
        } while (v7);
      }
      for (j = 0; j < 9; ++j) {
        ++v6;
        v9 = player_names[9 * message_received + j];
        currentdir_variable_8[v6] = v9;
      }
      small_zoom(&buffer);
      for (k = 0; k < 24; currentdir_variable_8[k] = p_data_variable_8[k]) {
        k += 8;
        currentdir_variable_1[k] = p_data_variable_1[k];
        currentdir_variable_2[k] = p_data_variable_2[k];
        currentdir_variable_3[k] = p_data_variable_3[k];
        currentdir_variable_4[k] = p_data_variable_4[k];
        currentdir_variable_5[k] = p_data_variable_5[k];
        currentdir_variable_6[k] = p_data_variable_6[k];
        currentdir_variable_7[k] = p_data_variable_7[k];
      }
      subzoom(&buffer);
      message_received = -1;
    }
  }
  if (Car_variable_40[308 * ViewType[0]]) {
    if (game_count[0] == -2)
      start_zoom(language_buffer_variable_25, 0);
    if (game_count[0] > 0 && Car_variable_9[77 * ViewType[0]] < (double)threed_c_variable_40)
      game_count[0] = 72;
  }
  if (player_type == 2 && Car_variable_40[308 * ViewType_variable_1]) {
    if (game_count_variable_1 == -2)
      start_zoom(language_buffer_variable_25, 1);
    if (game_count_variable_1 > 0 && Car_variable_9[77 * ViewType_variable_1] < (double)threed_c_variable_40)
      game_count_variable_1 = 72;
  }
  if (!time_shown) {
    if (shown_panel || winner_mode) {
    LABEL_58:
      if (winner_mode) {
        if (champ_mode) {
          if (game_count[0] == -2 && !winner_done) {
            switch (champ_zoom) {
              case 0:
                if (racers - 1 == champ_car)
                  v14 = language_buffer_variable_21;
                else
                  v14 = (char *)&language_buffer + 64 * champ_car + 384;
                start_zoom(v14, 0);
                goto LABEL_97;
              case 1:
                start_zoom(&driver_names[9 * champorder[champ_car]], winner_done);
                goto LABEL_97;
              case 3:
                speechsample(118, 0x8000, 18, (__int16)player1_car);
                start_zoom(config_buffer_variable_77, 0);
                goto LABEL_97;
              case 4:
                start_zoom(config_buffer_variable_78, 0);
                goto LABEL_97;
              case 5:
                start_zoom(config_buffer_variable_79, 0);
                goto LABEL_97;
              case 6:
                if (total_wins[champorder[0]] > 0)
                  speechsample(total_wins[champorder[0]] + 88, 0x8000, 18, (__int16)player1_car);
                goto LABEL_97;
              case 7:
                v15 = total_kills[champorder[0]];
                if (v15 > 0) {
                  if (v15 >= 17) {
                    v17 = 113;
                    v16 = (__int16)player1_car;
                  } else {
                    v16 = (__int16)player1_car;
                    v17 = total_kills[champorder[0]] + 96;
                  }
                  speechsample(v17, 0x8000, 18, v16);
                }
                ++champ_zoom;
                break;
              default:
                break;
            }
          }
        } else if (game_count[0] == -2 && !winner_done) {
          switch (champ_zoom) {
            case 0:
              speechsample(114, 0x8000, 18, (__int16)player1_car);
              start_zoom(config_buffer_variable_72, 0);
              goto LABEL_97;
            case 1:
              start_zoom(config_buffer_variable_73, 0);
              goto LABEL_97;
            case 2:
              if (result_kills[result_order[0]] > 0 || result_order[0] == FastestLap)
                speechsample(115, 0x8000, 18, (__int16)player1_car);
              goto LABEL_97;
            case 3:
              if (result_order[0] == FastestLap) {
                if (BestTime == RecordLaps[game_track])
                  v18 = 116;
                else
                  v18 = 117;
                speechsample(v18, 0x8000, 18, (__int16)player1_car);
              }
              goto LABEL_97;
            case 4:
              v19 = result_kills[result_order[0]];
              if (v19 > 0) {
                if (v19 >= 17) {
                  v21 = 113;
                  v20 = (__int16)player1_car;
                } else {
                  v20 = (__int16)player1_car;
                  v21 = result_kills[result_order[0]] + 96;
                }
                speechsample(v21, 0x8000, 18, v20);
              }
            LABEL_97:
              ++champ_zoom;
              break;
            default:
              break;
          }
        }
      }
      if (intro && game_count[0] == -2) {
        start_zoom(language_buffer_variable_28, 0);
        subzoom(language_buffer_variable_29);
      }
      if (network_on && paused) {
        sprintf(&buffer, "%s", language_buffer_variable_98);
        start_zoom(&buffer, 0);
        sprintf(&buffer, "%s", &player_names[9 * pauser]);
        subzoom(&buffer);
        game_scale[0] = 1115684864;
        game_count[0] = 2;
      }
      if (a3 == (char *)(__int16)player1_car || winner_mode || intro) {
        if (zoom_size[0]) {
          v31 = (int *)&font3_offsets;
          v22 = (char *)&font3_ascii;
          v23 = rev_vga_variable_3;
        } else {
          v31 = &font6_offsets;
          if (winh < 200) {
            v22 = &ascii_conv3;
            v23 = rev_vga;
          } else {
            v22 = font6_ascii;
            v23 = rev_vga_variable_1;
          }
        }
        ZoomString(zoom_mes, (int)v22, v23, 0, (int)v31);
        if (sub_on[0]) {
          if (winh < 200) {
            v24 = &ascii_conv3;
            v25 = rev_vga;
          } else {
            v24 = font6_ascii;
            v25 = rev_vga_variable_1;
          }
          ZoomSub((int)&zoom_sub, (int)v24, v25, 0, (int)&font6_offsets);
        }
      }
      if (a3 == (char *)player2_car) {
        if (zoom_size_variable_1) {
          ZoomString(zoom_mes_variable_1, (int)&font3_ascii, rev_vga_variable_3, 1, (int)&font3_offsets);
        } else if (winh < 200) {
          ZoomString(zoom_mes_variable_1, (int)&ascii_conv3, rev_vga, 1, (int)&font6_offsets);
        } else {
          ZoomString(zoom_mes_variable_1, (int)font6_ascii, rev_vga_variable_1, 1, (int)&font6_offsets);
        }
        if (sub_on_variable_1) {
          if (winh < 200)
            ZoomSub((int)&zoom_sub_variable_1, (int)&ascii_conv3, rev_vga, 1, (int)&font6_offsets);
          else
            ZoomSub((int)&zoom_sub_variable_1, (int)font6_ascii, rev_vga_variable_1, 1, (int)&font6_offsets);
        }
      }
      goto LABEL_127;
    }
    shown_panel = -1;
    if (replaytype != 2) {
      if (game_type != 2) {
        if (countdown <= -72)
          goto LABEL_55;
        if (countdown < 0 && gosound >= 1) {
          gosound = 0;
          speechsample(4, 0x8000, 0, (__int16)player1_car);
        }
        if ((unsigned int)countdown < 0x48) {
          if (gosound >= 2) {
            gosound = 1;
            speechsample(49, 0x8000, 0, (__int16)player1_car);
          }
          if (countdown < 18) {
            if (strcmp(language_buffer_variable_26, zoom_mes)) {
              start_zoom(language_buffer_variable_26, 0);
              if (player_type == 2)
                start_zoom(language_buffer_variable_26, 1);
            }
          }
        }
        if (countdown < 72 || countdown >= 144 || !fadedin || gosound < 3)
          goto LABEL_55;
        v12 = 50;
        gosound = 2;
        v13 = (__int16)player1_car;
        goto LABEL_54;
      }
      if (gosound >= 1 && active_nodes == network_on) {
        v12 = 4;
        v13 = (__int16)player1_car;
        gosound = 0;
      LABEL_54:
        speechsample(v12, 0x8000, 0, v13);
      }
    }
  LABEL_55:
    draw_smoke((int)a1, (int)a3);
    if (replaytype != 2 && (textures_off & 0x20) == 0)
      test_panel((int)a1, a3);
    goto LABEL_58;
  }
LABEL_127:
  if (replaytype == 2 && !intro)
    displayreplay();
  if (game_req) {
    if (draw_type != 2) {
      display_paused();
      if (trying_to_exit) {
        if ((frames & 0xFu) < 8)
          prt_centrecol(171);
      }
    }
  }
  if (!winner_mode && replaytype != 2) {
    if (network_on && active_nodes < network_on && (frames & 0xFu) < 8) {
      if (winh >= 200) {
        prt_centrecol(207);
      } else {
        v26 = scr_size;
        scr_size = 64;
        mini_prt_centre(rev_vga, aWaitingForPlay, winw / 2, winh / 2);
        scr_size = v26;
      }
    }
    if (network_on
      && finished_car[(__int16)player1_car]
      && (frames & 0xFu) < 8
      && !I_Would_Like_To_Quit
      && lastsample < -180) {
      v27 = scr_size;
      scr_size = 64;
      mini_prt_centre(rev_vga, aPleaseWait, winw / 2, winh / 2);
      mini_prt_centre(rev_vga, aForRaceToEnd, winw / 2, winh / 2 + 16);
      scr_size = v27;
    }
    if (I_Would_Like_To_Quit && (frames & 0xFu) < 8) {
      v28 = scr_size;
      scr_size = 64;
      if (game_type == 1)
        v29 = aF10ToQuitChamp;
      else
        v29 = &aIngf10ToQuitGa[3];
      mini_prt_centre(rev_vga, v29, winw / 2, winh / 2);
      mini_prt_centre(rev_vga, aEscToCancel, winw / 2, winh / 2 + 14);
      scr_size = v28;
    }
  }
  if (draw_type != 2) {
    curr_time = ticks;
    v30 = ++frame_count;
    if (ticks - start_time >= 36) {
      frame_rate = v30;
      frame_count = 0;
      start_time = curr_time;
    }
  }
  if (draw_type != 2)
    copypic(a1, a2);*/
}

//-------------------------------------------------------------------------------------------------

void test_w95()
{
  /*
  REGS regs; // [esp+0h] [ebp-34h] BYREF
  struct SREGS sregs; // [esp+1Ch] [ebp-18h] BYREF

  // AX=1600h(5632) is a Microsoft-defined multiplex interrupt function used to
  // query the windows environment. This returns:
  // * AL = 0xFF if Windows is not running (i.e., plain DOS)
  // * AL = major version and AH = minor version if Windows is running (e.g., AL=4, AH=0 for Windows 95)
  memset(&sregs, 0, sizeof(sregs));
  regs.w.ax = 5632;
  int386x(47, &regs, &regs, &sregs);
  w95 = regs.h.al;
  if (regs.h.al < 3u || regs.h.al > 4u)
    w95 = 0;
  else
    w95 = -1;*/

  //for our purposes we'll pretend to be in DOS
  w95 = 0;
}

//-------------------------------------------------------------------------------------------------

void *malloc2(int iSize, void *pPtr, int *pRegsDi)
{
  void *result; // eax
  result = malloc(iSize);
  pPtr = result;
  return result;
}

//-------------------------------------------------------------------------------------------------

void free2(void *ptr)
{
  free(ptr);
  --hibuffers;
}

//-------------------------------------------------------------------------------------------------
