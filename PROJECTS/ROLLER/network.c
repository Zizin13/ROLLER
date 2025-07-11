#include "network.h"
#include "sound.h"
#include "3d.h"
#include "func3.h"
#include "frontend.h"
#include "moving.h"
#include "car.h"
#include "control.h"
#include "colision.h"
#include "roller.h"
//-------------------------------------------------------------------------------------------------

int sync_errors = 0;        //000A6100
int net_type = 1;           //000A6104
int slave_pause = 0;        //000A6108
int net_started = 0;        //000A610C
int next_resync = 0;        //000A6110
int gamers_playing[4];      //00178470
uint32 test_mini[2];        //001786C0
int test_multiple[16];      //001786C8
tRecordPacket p_record;     //00178708
int net_players[16];        //00178718
int16 player_checks[512][16]; //00178758
int address[64];            //0017C758
int player_ready[16];       //0017C858
int syncptr;                //0017C928
int syncleft;               //0017C92C
int syncnode;               //0017C930
int syncframe;              //0017C934
int received_seed;          //0017C938
int received_records;       //0017C93C
int frame_number;           //0017C940
int start_multiple;         //0017C944
tSyncHeader p_header;       //0017C948
int test_seed;              //0017C958
int resync;                 //0017C95C
int message_received;       //0017C960
int my_age;                 //0017C964
int message_number;         //0017C968
int message_node;           //0017C96C
int read_check;             //0017C970
int write_check;            //0017C974
int network_mistake;        //0017C97C
int pauser;                 //0017C980
uint32 broadcast_mode;      //0017C984
int message_sent;           //0017C988
int random_seed;            //0017C98C
int dostopsamps;            //0017C990
int lost_message;           //0017C994
int duff_message;           //0017C998
int check_set;              //0017C99C
int master;                 //0017C9A0
tSyncHeader in_header;      //0017C9A4
int active_nodes;           //0017C9B0
int net_quit;               //0017C9B4
tDataPacket slave_data;     //0017C9B8
char p_data[14];            //0017C9BE
char received_message[14];  //0017C9CC
int16 wConsoleNode;         //0017C9DA

//-------------------------------------------------------------------------------------------------

void Initialise_Network(int iSelectNetSlot)
{
  int iResetNetwork; // [esp+0h] [ebp-1Ch]

  iResetNetwork = 0;
  active_nodes = 0;
  net_quit = 0;
  master = 0xFFFFFD65;
  wConsoleNode = 0;
  network_on = 0;
  net_loading = 0;
  memset(copy_multiple, 0, sizeof(copy_multiple));

  for (int i = 0; i < 16; ++i) {
    player_ready[i] = 0;
    for (int j = 0; i < 512; ++j) {
      player_checks[j][i] = -1;
    }
  }

  message_node = -1;
  message_number = -1;
  message_received = -1;
  message_sent = -1;
  time_to_start = 0;
  received_seed = 0;
  frame_number = 0;
  active_nodes = 0;

  //TODO network
  //if ((net_type || !net_started) && gssCommsInitSystem(20u))
  //  iResetNetwork = -1;
  if (!iResetNetwork) {
    I_Quit = 0;
    broadcast_mode = 0;
    writeptr = 0;
    readptr = 0;
    remove_messages(-1);
    if (!net_type)
      net_started = -1;

    for (int i = 0; i < 16; ++i) {
      net_players[i] = 0;
      player_started[i] = 0;
    }

    players_waiting = 0;
    if (!network_champ_on)
      Players_Cars[player1_car] = -1;
    my_age = 0;
    //TODO network
    //gssCommsGetNetworkAddr(address[0]);
    //gssCommsGetConsoleNode();
    network_on = 1;
    if (iSelectNetSlot || network_slot < 0)
      broadcast_mode = 0xFFFFFE38;              // force slave broadcast?
    else
      broadcast_mode = -1;
    tick_on = -1;
    while (broadcast_mode)
      UpdateSDL(); //added by ROLLER
    //TODO network
    //gssCommsSortNodes();
    received_records = 1;
    switch_sets = 0;
    switch_same = 0;
    switch_types = 0;
  }
}

//-------------------------------------------------------------------------------------------------

void close_network()
{
  int iNode; // edx
  int i; // edx

  network_slot = -1;
  if (network_on) {
    tick_on = 0;
    //while (gssCommsPostListen())
    //  ;
    iNode = 0;
    wConsoleNode = 0;// gssCommsNetAddrToNode((int)&address);
    while (iNode < wConsoleNode) {
      //if (gssCommsDeleteNode(0)) {
      //  printf("FAILED TO DELETE NODE a (%d)!!!\n", iNode);
      //  doexit();
      //}
      //gssCommsSortNodes();
      ++iNode;
    }
    for (i = wConsoleNode + 1; i < network_on; ++i) {
      //if (gssCommsDeleteNode(1)) {
      //  printf("FAILED TO DELETE NODE b (%d)!!!\n", i);
      //  doexit();
      //}
      //gssCommsSortNodes();
    }
    //gssCommsGetActiveNodes();
    wConsoleNode = 0;// gssCommsNetAddrToNode((int)&address);
    if (exiting)
      goto LABEL_18;
  } else if ((net_started || net_type) && exiting) {
  LABEL_18:
    ;
    //gssCommsSetType(1);
    //gssCommsUnInitSystem();
    //gssCommsSetType(0);
    //gssCommsUnInitSystem();
  }

  for (int i = 0; i < 16; ++i)
      net_players[i] = -667;

  if (network_on && !frontend_on) {
    Players_Cars[0] = my_car;
    name_copy(player_names[0], my_name);
    manual_control[0] = my_control;
    player_invul[0] = my_invul;
  }
  if (network_on)
    player_type = 0;
  network_on = 0;
  player1_car = 0;
  active_nodes = 0;
  wConsoleNode = 0;
  received_seed = 0;
  frame_number = 0;
  time_to_start = 0;
  if (game_type != 1 && replaytype != 2)
    network_champ_on = 0;
  broadcast_mode = 0;
  clear_network_game();
  //if (net_type)
  //  gssCommsUnInitSystem();
}

//-------------------------------------------------------------------------------------------------

void send_net_error()
{
  int i; // esi

  if (network_on) {
    p_header.byConsoleNode = (uint8)wConsoleNode;
    p_header.uiId = PACKET_ID_NET_ERROR;
    if (wConsoleNode == master) {
      for (i = 0; i < network_on; ++i) {
        if (i != wConsoleNode) {
          //TODO network
          //gssCommsSendData(&p_header, sizeof(tSyncHeader), p_data, 0, i);
        }
      }
    }
  }
}

//-------------------------------------------------------------------------------------------------

void send_game_error()
{
  int iDataSent; // eax
  int i; // esi

  if (network_on) {
    p_header.byConsoleNode = (uint8)wConsoleNode;
    p_header.uiId = PACKET_ID_GAME_ERROR;
    network_mistake = -1;
    if (wConsoleNode == master) {
      for (i = 0; i < network_on; ++i) {
        if (i != wConsoleNode) {
          //TODO network
          iDataSent = 0;// gssCommsSendData(&p_header, sizeof(tSyncHeader), p_data, 0, i);
          if (!iDataSent)
            i = network_on;
        }
      }
      if (iDataSent)
        network_error = 666;
    }
  }
}

//-------------------------------------------------------------------------------------------------

void send_network_sync_error()
{
  int i; // esi

  if (network_on) {
    p_header.byConsoleNode = (uint8)wConsoleNode;
    p_header.uiId = PACKET_ID_SYNC_ERROR;
    if (wConsoleNode == master) {
      for (i = 0; i < network_on; ++i) {
        if (i != wConsoleNode) {
          //TODO network
          //gssCommsSendData(&p_header, sizeof(tSyncHeader), p_data, 0, i);
        }
      }
    } else {
      //TODO network
      //gssCommsSendData(&p_header, sizeof(tSyncHeader), p_data, 0, master);
    }
  }
}

//-------------------------------------------------------------------------------------------------

void send_resync(int iFrameNumber)
{
  if (network_on) {
    resync = iFrameNumber;
    p_header.byConsoleNode = (uint8)wConsoleNode;
    p_header.uiId = PACKET_ID_RESYNC;
    //TODO network
    //gssCommsSendData(&p_header, sizeof(tSyncHeader), &resync, sizeof(int32), master);
  }
}

//-------------------------------------------------------------------------------------------------

void send_nocd_error()
{
  int i; // esi

  if (network_on) {
    p_header.byConsoleNode = (uint8)wConsoleNode;
    p_header.uiId = PACKET_ID_NOCD;
    if (wConsoleNode == (int16)master) {
      for (i = 0; i < network_on; ++i) {
        if (i != wConsoleNode) {
          //TODO network
          //gssCommsSendData(&p_header, sizeof(tSyncHeader), p_data, 0, i);
        }
      }
    } else {
      //TODO network
      //gssCommsSendData(&p_header, sizeof(tSyncHeader), p_data, 0, master);
    }
  }
}

//-------------------------------------------------------------------------------------------------

void send_quit()
{
  int i; // esi

  if (network_on) {
    p_header.byConsoleNode = (uint8)wConsoleNode;
    p_header.uiId = PACKET_ID_QUIT;
    if (wConsoleNode == master) {
      for (i = 0; i < network_on; ++i) {
        if (i != wConsoleNode) {
          //TODO network
          //while (!gssCommsSendData(&p_header, sizeof(tSyncHeader), p_data, 0, i))
          //  UpdateSDL(); //added by ROLLER
        }
      }
      net_quit = -1;
    } else {
      //TODO network
      //while (!gssCommsSendData(&p_header, sizeof(tSyncHeader), p_data, 0, master))
      //  UpdateSDL(); //added by ROLLER
    }
  }
}

//-------------------------------------------------------------------------------------------------

void send_ready()
{
  int iDataSent; // eax

  if (network_on) {
    p_header.byConsoleNode = (uint8)wConsoleNode;
    if (localCD)
      p_header.uiId = PACKET_ID_READY;
    else
      p_header.uiId = PACKET_ID_NOCD;
    if (wConsoleNode != master || player_ready[master]) {
      do {
        //TODO network
        //_disable();
        iDataSent = 1;// gssCommsSendData(&p_header, sizeof(tSyncHeader), p_data, 0, master);
        //_enable();
        UpdateSDL(); //added by ROLLER
      } while (!iDataSent);
    } else {
      player_ready[master] = -1;
      active_nodes++;
    }
  }
}

//-------------------------------------------------------------------------------------------------

void send_record_to_master(int iRecordIdx)
{
  if (network_on) {
    p_header.byConsoleNode = (uint8)wConsoleNode;
    p_header.uiId = PACKET_ID_RECORD;
    p_record.fRecordLap = RecordLaps[iRecordIdx];
    strncpy(p_record.szRecordName, RecordNames[iRecordIdx], sizeof(p_record.szRecordName));
    p_record.unRecordCar = RecordCars[iRecordIdx];

    //TODO network
    //while (!gssCommsSendData(&p_header, sizeof(tSyncHeader), &p_record, sizeof(tRecordPacket), master))
    //  UpdateSDL(); //added by ROLLER
  }
}

//-------------------------------------------------------------------------------------------------

void send_record_to_slaves(int iRecordIdx)
{
  if (network_on) {
    p_header.byConsoleNode = (uint8)wConsoleNode;
    p_header.uiId = PACKET_ID_RECORD;
    p_record.fRecordLap = RecordLaps[iRecordIdx];
    strncpy(p_record.szRecordName, RecordNames[iRecordIdx], sizeof(p_record.szRecordName));
    p_record.unRecordCar = RecordCars[iRecordIdx];

    for (int i = 0; i < network_on; ++i) {
      iRecordIdx = wConsoleNode;
      if (wConsoleNode != i) {
        //TODO network
        //while (!gssCommsSendData(&p_header, sizeof(tSyncHeader), &p_record, sizeof(tRecordPacket), i)) {
        //  UpdateSDL(); //added by ROLLER
        //}
      }
    }
  }
}

//-------------------------------------------------------------------------------------------------

void send_mes(int iNetworkMessageIdx, int iNode)
{
  char *szNetworkMessage; // eax
  int iNodeIdx; // esi
  int iNetPlayers; // edi

  if (network_on) {
    szNetworkMessage = network_messages[iNetworkMessageIdx];
    if (*szNetworkMessage) {
      message_sent = iNetworkMessageIdx;

      strncpy(p_data, network_messages[iNetworkMessageIdx], sizeof(p_data));

      // setup header
      p_header.uiId = PACKET_ID_SEND_MES;
      p_header.byConsoleNode = (uint8)wConsoleNode;
      if (iNode == -1) {
        iNodeIdx = 0;
        if (network_on > 0) {
          iNetPlayers = 0;
          do {
            if (iNodeIdx != wConsoleNode && net_players[iNetPlayers]) {
              //TODO network
              //while (!gssCommsSendData(&p_header, sizeof(tSyncHeader), p_data, sizeof(p_data), iNodeIdx))
              //  UpdateSDL(); //added by ROLLER
            }
            ++iNetPlayers;
            ++iNodeIdx;
          } while (iNodeIdx < network_on);
        }
      } else {
        //TODO network
        //while (!gssCommsSendData(&p_header, sizeof(tSyncHeader), p_data, sizeof(p_data), iNode))
        //  UpdateSDL(); //added by ROLLER
      }
    } else {
      message_sent = 4;
    }
  }
}

//-------------------------------------------------------------------------------------------------

void send_seed(int iRandomSeed)
{
  if (network_on) {
    test_seed = iRandomSeed;
    p_header.uiId = PACKET_ID_SEED;
    p_header.byConsoleNode = (uint8)wConsoleNode;
    for (int i = 0;  i < network_on; ++i) {
      if (i != wConsoleNode) {
        //TODO network
        //while (!gssCommsSendData(&p_header, 12, &test_seed, 4, i))
        //  UpdateSDL(); //added by ROLLER
      }
    }

    srand(iRandomSeed);
    random_seed = iRandomSeed;
  }
}

//-------------------------------------------------------------------------------------------------

void send_single(uint32 uiData)
{
  if (!network_on) return;

  // Initialize header and data packet
  p_header.uiId = PACKET_ID_SINGLE;
  p_header.byConsoleNode = (uint8)player1_car;

  slave_data.uiData = uiData;

  // Calculate checksum or sequence number if valid read position
  if (read_check == write_check || read_check < 0) {
    // Invalid state - set default value
    slave_data.nChecksum = 0xFE00;
  } else {
    // Compute special value using player_checks array
    short nChecksum = player_checks[read_check][player1_car];

    // Create composite value:
    // - Lower 7 bits of checksum shifted to bits 9-15
    // - Bit 8 from read_check
    // - Lower 8 bits from read_check
    unsigned int uiComposite = (nChecksum & 0x7F) << 9;
    uiComposite |= (read_check & 0x100);  // Bit 8
    uiComposite |= (read_check & 0xFF);    // Bits 0-7

    slave_data.nChecksum = (short)uiComposite;
  }

  //TODO network
  //gssCommsSendData(&p_header, sizeof(p_header), slave_data, sizeof(tDataPacket), master);

  // Mark check as processed
  player_checks[read_check][player1_car] = -1;  // 0xFFFF

  // Advance read position if valid and not caught up
  if (read_check != write_check && read_check >= 0) {
    read_check = (read_check + 1) & 0x1FF;  // Wrap around at 512
  }
}

//-------------------------------------------------------------------------------------------------

void send_pause()
{
  tSyncHeader syncHeader; // [esp+0h] [ebp-1Ch] BYREF
  bool bPaused; // [esp+Ch] [ebp-10h] BYREF

  if (network_on) {
    syncHeader.byConsoleNode = (uint8)wConsoleNode;
    syncHeader.uiId = PACKET_ID_PAUSE;
    bPaused = paused == 0;
    //TODO network
    //while (!gssCommsSendData(&syncHeader, sizeof(tSyncHeader), &bPaused, sizeof(bool), master))
    //  UpdateSDL(); //added by ROLLER
  }
}

//-------------------------------------------------------------------------------------------------

void send_slot()
{
  tSyncHeader syncHeader;
  //int iSlot;// = network_slot?

  if (network_on) {
    syncHeader.byConsoleNode = (uint8)network_slot;
    syncHeader.uiId = PACKET_ID_SLOT;
    //TODO network
    //while (!gssCommsSendData(&syncHeader, sizeof(tSyncHeader), &iSlot, sizeof(int), 21))
    //  UpdateSDL(); //added by ROLLER
  }
}

//-------------------------------------------------------------------------------------------------

void transmitpausetoslaves()
{
  int iNode; // esi
  int iNetPlayerIdx; // edi
  tSyncHeader syncHeader; // [esp+0h] [ebp-28h] BYREF
  bool bPaused; // [esp+Ch] [ebp-1Ch] BYREF

  if (network_on) {
    syncHeader.byConsoleNode = pauser;
    syncHeader.uiId = PACKET_ID_PAUSE;
    bPaused = paused;
    iNode = 0;
    if (network_on > 0) {
      iNetPlayerIdx = 0;
      do {
        if (iNode != master && net_players[iNetPlayerIdx]) {
          //Todo network
          //while (!gssCommsSendData(&syncHeader, sizeof(tSyncHeader), bPaused, sizeof(bool), iNode))
          //  UpdateSDL(); //added by ROLLER
        }
        ++iNetPlayerIdx;
        ++iNode;
      } while (iNode < network_on);
    }
  }
}

//-------------------------------------------------------------------------------------------------

void send_multiple()
{/*
  int v0; // eax
  int *v1; // esi
  int v2; // edi
  int v3; // esi
  int v4; // eax
  int v5; // edi
  int v6; // edx
  _DWORD *v7; // esi
  int i; // eax
  int v9; // edx
  int v10; // edi
  int v11; // edi

  if (network_on) {
    p_header_variable_2 = wConsoleNode;
    p_header_variable_1 = 1751933798;
    p_header_variable_3 = frame_number;
    if (net_type) {
      v6 = -1;
      if (memcmp(
        (const char *)&copy_multiple[16 * (((_WORD)writeptr - 1) & 0x1FF) + player_to_car[0]],
        (const char *)&copy_multiple[16 * writeptr + player_to_car[0]],
        4)) {
        v6 = 0;
      }
      if (memcmp(
        (const char *)&copy_multiple[16 * (((_WORD)writeptr - 1) & 0x1FF) + player_to_car_variable_1],
        (const char *)&copy_multiple[16 * writeptr + player_to_car_variable_1],
        4)) {
        v6 = 0;
      }
      if (!frame_number)
        v6 = 0;
      if (v6) {
        *((_DWORD *)&p_header + 1) = 1751933812;
        v3 = abs32(start_multiple);
        v4 = -1;
        if (v3 >= 0) {
          v11 = v3;
          do {
            if (!v4)
              break;
            if (v3 == master || !net_players[v11])
              v4 = -1;
            else
              v4 = gssCommsSendData(v3);
            if (v4) {
              --v11;
              --v3;
            }
          } while (v3 >= 0);
        }
      } else {
        v7 = &test_mini;
        for (i = 0; i < 2; ++i) {
          ++v7;
          v9 = copy_multiple[16 * writeptr + player_to_car[i]];
          *(v7 - 1) = v9;
        }
        v3 = abs32(start_multiple);
        v4 = -1;
        if (v3 >= 0) {
          v10 = v3;
          do {
            if (!v4)
              break;
            if (v3 == master || !net_players[v10])
              v4 = -1;
            else
              v4 = gssCommsSendData(v3);
            if (v4) {
              --v10;
              --v3;
            }
          } while (v3 >= 0);
        }
      }
    } else {
      v0 = 0;
      if (numcars > 0) {
        v1 = (int *)&test_multiple;
        do {
          v2 = copy_multiple[16 * writeptr + v0++];
          *v1++ = v2;
        } while (v0 < numcars);
      }
      v3 = abs32(start_multiple);
      v4 = -1;
      if (v3 >= 0) {
        v5 = v3;
        do {
          if (!v4)
            break;
          v4 = v3 == master || !net_players[v5] ? -1 : gssCommsSendData(v3);
          if (v4) {
            --v5;
            --v3;
          }
        } while (v3 >= 0);
      }
    }
    if (v4) {
      start_multiple = network_on - 1;
      ++frame_number;
    } else {
      start_multiple = -v3;
    }
  }*/
}

//-------------------------------------------------------------------------------------------------

void receive_multiple()
{
  tPlayerInfoPacket playerInfoPacket; // [esp+0h] [ebp-60h] BYREF
  char szMesPacket[14]; // [esp+28h] [ebp-38h] BYREF
  //void *pPacket; // [esp+38h] [ebp-28h] BYREF
  int iPaused; // [esp+40h] [ebp-20h] BYREF
  int *pSeed; // [esp+44h] [ebp-1Ch]

  if (network_on && !net_quit) {
    pSeed = &test_seed;
    while (false) {
    //while (gssCommsGetHeader(&in_header, 12, &pPacket)) {
      switch (in_header.uiId) {
        case PACKET_ID_SEND_MES:
          //TODO network
          //gssCommsGetBlock(pPacket, szMesPacket, 14);
          message_received = in_header.byConsoleNode;
          strncpy(received_message, szMesPacket, sizeof(received_message));
          goto LABEL_54;
        case PACKET_ID_QUIT:
          net_quit = -1;
          //TODO network
          //gssCommsPostListen();
          continue;
        case PACKET_ID_PLAYER_CARS:
          if (net_type) {
            if (network_on > 0) {
              for (int i = 0; i < network_on; ++i) {
                player_ready[i] = -1;
              }
            }
            active_nodes = network_on;
            net_loading = 0;
            //TODO network
            //gssCommsGetBlock(pPacket, test_mini, 8);
            for (int i = 0; i < 2; ++i) {
              copy_multiple[writeptr][player_to_car[i]].uiFullData = test_mini[i];
            }
          } else {
            if (network_on > 0) {

              for (int i = 0; i < network_on; ++i) {
                player_ready[i] = -1;
              }
            }
            active_nodes = network_on;
            net_loading = 0;
            //TODO network
            //gssCommsGetBlock(pPacket, &copy_multiple[writeptr], 64);
          }
          network_timeout = frames;
          ++ticks_received;
          if (in_header.unFrameId == frame_number) {
            ++frame_number;
            writeptr = (writeptr + 1) & 0x1FF;
            next_resync = -1;
            //TODO network
            //gssCommsPostListen();
            continue;
          }
          if (in_header.unFrameId <= frame_number)
            goto LABEL_54;
          ++lost_message;
          //TODO network
          //gssCommsPostListen();
          if (in_header.unFrameId - frame_number > 500) {
            send_network_sync_error();
            network_sync_error = -1;
            //TODO network
            //gssCommsPostListen();
            continue;
          }
          if (frames <= next_resync)
            goto LABEL_54;
          //TODO network
          //if (gssCommsGetType())
          //  gssclrrx();
          goto LABEL_17;
        case PACKET_ID_SEED:
          //TODO network
          //gssCommsGetBlock(pPacket, &test_seed, 4);
          srand(*pSeed);
          random_seed = *pSeed;
          received_seed = -1;
          //TODO network
          //gssCommsPostListen();
          continue;
        case PACKET_ID_PAUSE:
          //TODO network
          //gssCommsGetBlock(pPacket, &iPaused, 4);
          if (paused != iPaused && iPaused)
            dostopsamps = -1;
          paused = iPaused;
          if (!iPaused)
            goto LABEL_54;
          pauser = in_header.byConsoleNode;
          //TODO network
          //gssCommsPostListen();
          continue;
        case PACKET_ID_PLAYER_INFO:
          //TODO network
          //gssCommsGetBlock(pPacket, &playerInfoPacket, sizeof(tPlayerInfoPacket));
          Players_Cars[in_header.byConsoleNode] = playerInfoPacket.iPlayerCar;
          check_cars();
          name_copy(player_names[in_header.byConsoleNode], playerInfoPacket.szPlayerName);
          TrackLoad = playerInfoPacket.iTrackLoad;
          game_type = playerInfoPacket.iGameType;
          competitors = playerInfoPacket.iCompetitors;
          level = playerInfoPacket.iLevel;
          damage_level = playerInfoPacket.iDamageLevel & 0xF;
          if ((playerInfoPacket.iDamageLevel & 0x40) != 0)
            player_invul[in_header.byConsoleNode] = -1;
          else
            player_invul[in_header.byConsoleNode] = 0;
          manual_control[in_header.byConsoleNode] = playerInfoPacket.iManualControl;
          //TODO network
          //gssCommsPostListen();
          continue;
        case PACKET_ID_NET_ERROR:
          network_error = 999;
          //TODO network
          //gssCommsPostListen();
          continue;
        case PACKET_ID_SYNC_ERROR:
          goto LABEL_13;
        case PACKET_ID_GAME_ERROR:
          network_error = 666;
          //TODO network
          //gssCommsPostListen();
          continue;
        case PACKET_ID_NOCD:
          cd_error = -1;
          //TODO network
          //gssCommsPostListen();
          continue;
        case PACKET_ID_SEND_HERE:
          load_times[in_header.byConsoleNode] = in_header.unFrameId;
          //TODO network
          //gssCommsPostListen();
          continue;
        case PACKET_ID_MULTIPLE:
          if (in_header.unFrameId == frame_number) {

            int iPrevIdx = (writeptr - 1) & 0x1FF;
            int iCar0 = player_to_car[0];
            int iCar1 = player_to_car[1];
            copy_multiple[writeptr][iCar0] = copy_multiple[iPrevIdx][iCar0];
            copy_multiple[writeptr][iCar1] = copy_multiple[iPrevIdx][iCar1];

            network_timeout = frames;
            ++ticks_received;
            ++frame_number;
            writeptr = (writeptr + 1) & 0x1FF;
            next_resync = -1;
            //TODO network
            //gssCommsPostListen();
            continue;
          }
          if (in_header.unFrameId <= frame_number)
            goto LABEL_54;
          ++lost_message;
          //TODO network
          //gssCommsPostListen();
          if (in_header.unFrameId - frame_number <= 500) {
            if (frames <= next_resync) {
            LABEL_54:
              ;
              //TODO network
              //gssCommsPostListen();
            } else {
              //TODO network
              //if (gssCommsGetType())
              //  gssclrrx();
            LABEL_17:
              ++sync_errors;
              send_resync(frame_number);
              next_resync = frames + 16;
              //TODO network
              //gssCommsPostListen();
            }
          } else {
            send_network_sync_error();
          LABEL_13:
            network_sync_error = -1;
            //TODO network
            //gssCommsPostListen();
          }
          break;
        default:
          ++duff_message;
          goto LABEL_54;
      }
    }
  }
}

//-------------------------------------------------------------------------------------------------

void receive_all_singles()
{
  int byConsoleNode; // eax
  tTransmitInitPacket transmitInitPacket; // [esp+0h] [ebp-140h] BYREF
  tPlayerInfoPacket playerInfoPacket; // [esp+E8h] [ebp-58h] BYREF
  char szMesPacket[14]; // [esp+110h] [ebp-30h] BYREF
  //void *pData; // [esp+120h] [ebp-20h] BYREF
  int iFrame;

  if (network_on) {
    //TODO network
    //while (gssCommsGetHeader(&in_header, sizeof(tSyncHeader), &pData)) {
    while (false) {
      byConsoleNode = in_header.byConsoleNode;
      if (net_players[car_to_player[byConsoleNode]]) {
        switch (in_header.uiId) {
          case PACKET_ID_TRANSMIT_INIT:
            //TODO network
            //gssCommsGetBlock(pData, &transmitInitPacket, sizeof(tTransmitInitPacket));
            if (transmitInitPacket.iTimeToStart != 0xFFFFFE38)
              goto LABEL_31;
            send_slot();
            //TODO network
            //gssCommsPostListen();
            continue;
          case PACKET_ID_SEND_MES:
            //TODO network
            //gssCommsGetBlock(pData, szMesPacket, 14);
            message_received = in_header.byConsoleNode;

            strncpy(received_message, szMesPacket, sizeof(received_message));

            goto LABEL_31;
          case PACKET_ID_QUIT:
            //TODO network
            //gssCommsPostListen();
            send_quit();
            //TODO network
            //gssCommsPostListen();
            continue;
          case PACKET_ID_SINGLE:
            //TODO network
            //gssCommsGetBlock(pData, &slave_data, 6);
            copy_multiple[writeptr][in_header.byConsoleNode].uiFullData = slave_data.uiData;
            net_time[car_to_player[in_header.byConsoleNode]] = frames;
            //TODO network
            //gssCommsPostListen();
            continue;
          case PACKET_ID_READY:
            if (player_ready[in_header.byConsoleNode])
              goto LABEL_31;
            ++active_nodes;
            player_ready[byConsoleNode] = -1;
            netCD = -1;
            //TODO network
            //gssCommsPostListen();
            continue;
          case PACKET_ID_PAUSE:
            slave_pause = -1;
            pauser = in_header.byConsoleNode;
            //TODO network
            //gssCommsPostListen();
            continue;
          case PACKET_ID_PLAYER_INFO: 
            //TODO network
            //gssCommsGetBlock(pData, &playerInfoPacket, sizeof(tPlayerInfoPacket));
            Players_Cars[in_header.byConsoleNode] = playerInfoPacket.iPlayerCar;
            check_cars();
            name_copy(player_names[in_header.byConsoleNode], playerInfoPacket.szPlayerName);
            TrackLoad = playerInfoPacket.iTrackLoad;
            game_type = playerInfoPacket.iGameType;
            competitors = playerInfoPacket.iCompetitors;
            level = playerInfoPacket.iLevel;
            damage_level = playerInfoPacket.iDamageLevel & 0xF;
            if ((playerInfoPacket.iDamageLevel & 0x40) != 0)
              player_invul[in_header.byConsoleNode] = -1;
            else
              player_invul[in_header.byConsoleNode] = 0;
            manual_control[in_header.byConsoleNode] = playerInfoPacket.iManualControl;
            //TODO network
            //gssCommsPostListen();
            continue;
          case PACKET_ID_SYNC_ERROR:
            goto LABEL_23;
          case PACKET_ID_NOCD:
            if (player_ready[in_header.byConsoleNode])
              goto LABEL_31;
            ++active_nodes;
            player_ready[byConsoleNode] = -1;
            //TODO network
            //gssCommsPostListen();
            continue;
          case PACKET_ID_RESYNC: 
            ++lost_message;
            //TODO network
            //gssCommsPostListen();
            syncnode = in_header.byConsoleNode;
            //TODO network
            //gssCommsGetBlock(pData, iFrame, 4);
            syncframe = iFrame;
            if (frame_number - iFrame <= 500) {
              //TODO network
              //if (gssCommsGetType())
              //  gssclrtx();
              syncleft = frame_number - syncframe;
              syncptr = syncframe & 0x1FF;
              if (check_set && syncleft > 7) {
                memset(player_checks, -1, sizeof(player_checks));
                check_set = 0;
              }
              do_sync_stuff();
              //TODO network
              //gssCommsPostListen();
            } else {
            LABEL_23:
              //TODO network
              //gssCommsPostListen();
              send_network_sync_error();
              network_sync_error = -1;
              //TODO network
              //gssCommsPostListen();
            }
            break;
          case PACKET_ID_SEND_HERE:
            load_times[in_header.byConsoleNode] = in_header.unFrameId;
            //TODO network
            //gssCommsPostListen();
            break;
          default:
            ++duff_message;
            goto LABEL_31;
        }
      } else {
      LABEL_31:
        ;
        //TODO network
        //gssCommsPostListen();
      }
    }
  }
}

//-------------------------------------------------------------------------------------------------

void do_sync_stuff(void)
{
  uint32 *pCopy;
  uint8 byConsoleNode = (uint8)wConsoleNode;
  uint16 unFrame = syncframe;

  if (net_type == 0) {
    // Full sync: send all cars to all nodes
    while (syncleft > 0) {
      in_header.uiId = PACKET_ID_PLAYER_CARS;
      in_header.byConsoleNode = byConsoleNode;
      in_header.unFrameId = unFrame;

      // Pack data for all cars
      pCopy = test_multiple;
      for (int i = 0; i < numcars; i++)
        *pCopy++ = copy_multiple[syncptr][player_to_car[i]].uiFullData;

      // Send 64 bytes (16 * 4-byte ints)
      //if (!gssCommsSendData(&in_header, 12, syncnode, test_multiple, 64))
      //  break;

      syncptr = (syncptr + 1) & 0x1FF; // wrap around 512
      syncframe++;
      syncleft--;
    }
  } else {
    // Minimal sync: send only player-to-car pairs
    while (syncleft > 0) {
      in_header.uiId = PACKET_ID_PLAYER_CARS;
      in_header.byConsoleNode = byConsoleNode;
      in_header.unFrameId = unFrame;

      // Pack data for player-to-car mappings
      pCopy = test_mini;
      for (int i = 0; i < 2; i++) {
        *pCopy++ = copy_multiple[syncptr][player_to_car[i]].uiFullData;
      }

      // Send 8 bytes (2 * 4-byte ints)
      //if (!gssCommsSendData(&in_header, 12, syncnode, test_mini, 8))
      //  break;

      syncptr = (syncptr + 1) & 0x1FF;
      syncframe++;
      syncleft--;
    }
  }
}

//-------------------------------------------------------------------------------------------------

int TransmitInit()
{
  int iSuccess; // eax
  int32 iCarIdx; // eax
  int32 iGameType; // eax
  int32 iLevelFlags; // eax
  char *szDefaultNamesDst; // ecx
  char *szDefaultNameItr; // ebx
  tTransmitInitPacket initPacket; // [esp+0h] [ebp-108h] BYREF
  tSyncHeader header;

  iSuccess = -1;
  if (network_on) {
    header.byConsoleNode = player1_car;
    header.uiId = PACKET_ID_TRANSMIT_INIT;
    initPacket.iTrackLoad = TrackLoad;
    name_copy(initPacket.szPlayerName, player_names[wConsoleNode]);
    initPacket.address[0] = address[0];
    initPacket.address[1] = address[1];
    initPacket.address[2] = address[2];
    initPacket.address[3] = address[3];
    if (broadcast_mode == -9999)
      iCarIdx = car_request - 1;
    else
      iCarIdx = Players_Cars[wConsoleNode];
    initPacket.iCarIdx = iCarIdx;
    if (net_type)
      ++my_age;
    initPacket.iMyAge = my_age;
    initPacket.iNetworkOn = network_on;
    if (game_type <= 2)
      iGameType = game_type;
    else
      iGameType = last_type;
    initPacket.iGameType = iGameType;
    initPacket.iCompetitors = competitors;
    initPacket.iManualControl = manual_control[wConsoleNode];
    iLevelFlags = level;
    if ((cheat_mode & CHEAT_MODE_DEATH_MODE) != 0)
      iLevelFlags = level | 0x0100;
    if (player_invul[player1_car])
      iLevelFlags |= 0x0200;
    if ((cheat_mode & CHEAT_MODE_KILLER_OPPONENTS) != 0)
      iLevelFlags |= 0x0400;
    if ((cheat_mode & CHEAT_MODE_ICY_ROAD) != 0)
      iLevelFlags |= 0x0800;
    if ((cheat_mode & CHEAT_MODE_50HZ_TIMER) != 0)
      iLevelFlags |= 0x1000;
    if ((cheat_mode & CHEAT_MODE_DOUBLE_TRACK) != 0)
      iLevelFlags |= 0x2000;
    if ((cheat_mode & CHEAT_MODE_100HZ_TIMER) != 0)
      iLevelFlags |= 0x4000;
    if ((cheat_mode & CHEAT_MODE_CLONES) != 0)
      iLevelFlags |= 0x8000;
    if ((cheat_mode & CHEAT_MODE_TINY_CARS) != 0)
      iLevelFlags |= 0x10000;
    initPacket.iLevelFlags = iLevelFlags;
    initPacket.iDamageLevel = damage_level;
    initPacket.iStartPressed = StartPressed;
    if (time_to_start == 45)
      time_to_start = 0;
    initPacket.iTimeToStart = time_to_start;
    initPacket.iFalseStart = false_starts;
    initPacket.iTextureMode = (textures_off & 0x10000) != 0;
    initPacket.iNetworkChampOn = network_champ_on;
    szDefaultNamesDst = initPacket.default_names[0];
    szDefaultNameItr = default_names[0];
    initPacket.iNetworkSlot = network_slot;
    do {
      name_copy(szDefaultNamesDst, szDefaultNameItr);
      szDefaultNameItr += 9;
      szDefaultNamesDst += 9;
    } while (szDefaultNameItr != default_names[16]);

    return -1; //TODO network
    //return gssCommsSendData(
    //         &initPacket.header,
    //         sizeof(tSyncHeader),
    //         &initPacket,
    //         sizeof(tTransmitInitPacket),
    //         21);
  }
  return iSuccess;
}

//-------------------------------------------------------------------------------------------------

void StartNode(int iNode)
{
  my_age = 0;
  //TODO network
  //gssCommsGetNetworkAddr(address);
  //gssCommsGetConsoleNode();
  network_on = 1;
  if (iNode || network_slot < 0)
    broadcast_mode = 0xFFFFFE38;
  else
    broadcast_mode = -1;
  tick_on = -1;
}

//-------------------------------------------------------------------------------------------------

int CheckNewNodes()
{
  return 0; /*
  int v0; // ebx
  int result; // eax
  __int64 v2; // rax
  int v3; // eax
  int v4; // edx
  __int64 v5; // rax
  char *v6; // eax
  int v7; // ebp
  int v8; // edi
  int v9; // esi
  char *v10; // edi
  char *v11; // eax
  _DWORD *v12; // edi
  char *v13; // eax
  __int64 v14; // rax
  int v15; // ebp
  int i; // edi
  int v17; // esi
  _DWORD *v18; // edi
  unsigned int v19; // eax
  int v20; // ebp
  char *v21; // ebx
  char *v22; // esi
  int v23; // ecx
  int v24; // eax
  int v25; // eax
  int v26; // eax
  int j; // eax
  int v28; // ebp
  __int64 v29; // rax
  __int64 found; // rax
  int v31; // ecx
  int v32; // edx
  int v33; // edx
  int v34; // ebx
  int v35; // eax
  int v36; // edx
  char *v37; // esi
  __int16 v38; // fps
  _BOOL1 v39; // c0
  char v40; // c2
  _BOOL1 v41; // c3
  int v42; // edx
  int v43; // [esp+0h] [ebp-18Ch] BYREF
  int v44; // [esp+4h] [ebp-188h]
  int v45; // [esp+8h] [ebp-184h]
  int v46; // [esp+Ch] [ebp-180h]
  char v47[16]; // [esp+10h] [ebp-17Ch] BYREF
  int v48; // [esp+20h] [ebp-16Ch]
  int v49; // [esp+24h] [ebp-168h]
  int v50; // [esp+28h] [ebp-164h]
  int v51; // [esp+2Ch] [ebp-160h]
  int v52; // [esp+30h] [ebp-15Ch]
  int v53; // [esp+34h] [ebp-158h]
  int v54; // [esp+38h] [ebp-154h]
  int v55; // [esp+3Ch] [ebp-150h]
  int v56; // [esp+40h] [ebp-14Ch]
  unsigned int v57; // [esp+44h] [ebp-148h]
  int v58; // [esp+48h] [ebp-144h]
  int v59; // [esp+4Ch] [ebp-140h]
  int v60; // [esp+50h] [ebp-13Ch]
  int v61; // [esp+54h] [ebp-138h]
  _BYTE v62[144]; // [esp+58h] [ebp-134h] BYREF
  _BYTE v63[32]; // [esp+E8h] [ebp-A4h] BYREF
  char v64[12]; // [esp+108h] [ebp-84h] BYREF
  int v65; // [esp+114h] [ebp-78h]
  char v66[12]; // [esp+118h] [ebp-74h] BYREF
  int v67; // [esp+124h] [ebp-68h]
  int v68; // [esp+128h] [ebp-64h]
  int v69; // [esp+12Ch] [ebp-60h]
  int v70; // [esp+130h] [ebp-5Ch]
  int v71; // [esp+134h] [ebp-58h]
  int v72; // [esp+138h] [ebp-54h]
  int v73; // [esp+13Ch] [ebp-50h]
  float v74[3]; // [esp+140h] [ebp-4Ch] BYREF
  __int16 v75; // [esp+14Eh] [ebp-3Eh]
  char v76[4]; // [esp+150h] [ebp-3Ch] BYREF
  int v77; // [esp+154h] [ebp-38h]
  unsigned __int8 v78; // [esp+158h] [ebp-34h]
  unsigned __int16 v79; // [esp+15Ah] [ebp-32h]
  int v80; // [esp+15Ch] [ebp-30h] BYREF
  int v81; // [esp+160h] [ebp-2Ch]
  int *v82; // [esp+164h] [ebp-28h]
  char *v83; // [esp+168h] [ebp-24h]
  char *v84; // [esp+16Ch] [ebp-20h]
  int v85; // [esp+170h] [ebp-1Ch]

  v82 = &test_seed;
  while (1) {
    v0 = (int)&v80;
    result = gssCommsGetHeader(v76, 12, &v80);
    if (!result)
      return result;
    LODWORD(v2) = v77 - 1751933793;
    HIDWORD(v2) = active_nodes + 1;
    switch (v77) {
      case 1751933793:
        v0 = 232;
        v60 = 0;
        v61 = 0;
        v2 = __PAIR64__(&v43, gssCommsGetBlock(v80, &v43, 232));
        if (network_slot >= 0) {
          if (v61 < 0 && !I_Quit) {
            v14 = send_broadcast(-457);
            test = HIDWORD(v14);
            gssCommsPostListen(v14, HIDWORD(v14), 232);
            continue;
          }
          test = 4;
          v15 = 0;
          for (i = 0; i < network_on; ++i) {
            HIDWORD(v2) = &v43;
            v0 = 0;
            LODWORD(v2) = (char *)&address + 16 * i;
            v17 = v2 + 16;
            do {
              if (*(_BYTE *)v2 != *(_BYTE *)HIDWORD(v2))
                v0 = -1;
              LODWORD(v2) = v2 + 1;
              ++HIDWORD(v2);
            } while ((_DWORD)v2 != v17);
            if (!v0) {
              v15 = i + 10;
              i = network_on + 6;
            }
          }
          if (!v15) {
            test = 5;
            if (time_to_start == -1)
              goto LABEL_163;
            LODWORD(v2) = v60;
            if (v60 != network_champ_on)
              goto LABEL_163;
            LODWORD(v2) = v61;
            if (v61 != network_slot)
              goto LABEL_163;
            if (network_on >= 16)
              goto LABEL_163;
            if (I_Quit)
              goto LABEL_163;
            test = 20;
            LODWORD(v2) = gssCommsAddNode(&v43);
            if ((unsigned int)v2 > 1)
              goto LABEL_163;
            test = 6;
            gssCommsSortNodes(v2);
            v18 = (_DWORD *)((char *)&address + 16 * network_on);
            *v18++ = v43;
            *v18++ = v44;
            *v18 = v45;
            v18[1] = v46;
            v19 = v57;
            switch_types = 0;
            if (v57 == -667)
              v19 = 0;
            if (v19) {
              LODWORD(v2) = time_to_start;
              if (time_to_start != -666)
                goto LABEL_163;
            LABEL_65:
              LODWORD(v2) = reset_network(-1);
              gssCommsPostListen(v2, HIDWORD(v2), v0);
              continue;
            }
            test = 7;
            name_copy((int)&player_names[9 * network_on], v47);
            Players_Cars[network_on] = v49;
            manual_control[network_on] = v52;
            player_started[network_on] = v56;
            HIDWORD(v2) = network_on;
            v20 = CheckNames(&player_names[9 * network_on], network_on);
            check_cars(v20);
            if (my_age < v48) {
              v21 = v62;
              v22 = default_names;
              do {
                name_copy((int)v22, v21);
                v21 += 9;
                v22 += 9;
              } while (v21 != v63);
              my_age = v48;
              TrackLoad = v50;
              game_type = v51;
              if (v51 == 1)
                Race = ((_BYTE)TrackLoad - 1) & 7;
              else
                network_champ_on = 0;
              competitors = v54;
              level = v53;
              v0 = v53;
              if ((v53 & 0x100) != 0) {
                v23 = cheat_mode;
                LOBYTE(v23) = cheat_mode | 2;
                cheat_mode = v23;
              }
              player_invul[network_on] = ((level & 0x200) == 0) - 1;
              if ((level & 0x400) != 0) {
                v24 = cheat_mode;
                BYTE1(v24) = BYTE1(cheat_mode) | 2;
                cheat_mode = v24;
              }
              if ((level & 0x800) != 0) {
                v0 = cheat_mode;
                BYTE1(v0) = BYTE1(cheat_mode) | 4;
                cheat_mode = v0;
              }
              if ((level & 0x1000) != 0) {
                if ((level & 0x4000) != 0) {
                  if ((cheat_mode & 0x2000) == 0) {
                    v0 = cheat_mode;
                    BYTE1(v0) = BYTE1(cheat_mode) | 0x28;
                    cheat_mode = v0;
                    release_ticktimer();
                    goto LABEL_84;
                  }
                } else if ((cheat_mode & 0x800) == 0) {
                  v25 = cheat_mode;
                  BYTE1(v25) = BYTE1(cheat_mode) | 8;
                  cheat_mode = v25;
                  release_ticktimer();
                LABEL_84:
                  claim_ticktimer();
                }
              }
              if ((level & 0x2000) != 0)
                cheat_mode |= 0x1000u;
              if (((unsigned int)cstart_branch_1 & level) != 0) {
                v26 = cheat_mode;
                BYTE1(v26) = BYTE1(cheat_mode) | 0x80;
                cheat_mode = v26;
              }
              HIDWORD(v2) = level;
              if ((level & 0x8000) != 0 && switch_same >= 0)
                switch_same = v49 + 666;
              level &= 0xFu;
              damage_level = v55;
              false_starts = v58;
              if (v59) {
                if (((unsigned int)cstart_branch_1 & textures_off) == 0) {
                  textures_off |= (unsigned int)cstart_branch_1;
                  goto LABEL_97;
                }
              } else if (((unsigned int)cstart_branch_1 & textures_off) != 0) {
                textures_off &= ~0x10000u;
              LABEL_97:
                switch_sets = -1;
              }
            }
            players_waiting = 0;
            for (j = 0; j <= network_on; ++j) {
              if (player_started[j])
                ++players_waiting;
            }
            ++my_age;
            LODWORD(v2) = network_on++;
            if (!I_Quit)
              v2 = send_broadcast(-1);
            if (!v20)
              goto LABEL_163;
            cheat_mode = 0;
            gssCommsPostListen(v2, HIDWORD(v2), 0);
            continue;
          }
          test = 8;
          v28 = v15 - 10;
          ++my_age;
          if (v28 <= 0)
            goto LABEL_163;
          test = 9;
          HIDWORD(v2) = v57;
          if (!v57) {
            test = 10;
            name_copy((int)&player_names[9 * v28], v47);
            Players_Cars[v28] = v49;
            v31 = CheckNames(&player_names[9 * v28], v28);
            check_cars(v31);
            TrackLoad = v50;
            if (v51 < 3) {
              if (v51 == game_type)
                game_type = v51;
              else
                switch_types = v51 + 1;
            }
            competitors = v54;
            level = v53;
            if ((v53 & 0x100) != 0) {
              v32 = cheat_mode;
              LOBYTE(v32) = cheat_mode | 2;
              cheat_mode = v32;
            }
            player_invul[v28] = ((level & 0x200) == 0) - 1;
            if ((level & 0x400) != 0)
              cheat_mode |= 0x200u;
            if ((level & 0x800) != 0) {
              v33 = cheat_mode;
              BYTE1(v33) = BYTE1(cheat_mode) | 4;
              cheat_mode = v33;
            }
            if ((level & 0x1000) != 0) {
              if ((level & 0x4000) != 0) {
                if ((cheat_mode & 0x2000) == 0) {
                  v34 = cheat_mode;
                  BYTE1(v34) = BYTE1(cheat_mode) | 0x28;
                  cheat_mode = v34;
                  release_ticktimer();
                  goto LABEL_131;
                }
              } else if ((cheat_mode & 0x800) == 0) {
                v35 = cheat_mode;
                BYTE1(v35) = BYTE1(cheat_mode) | 8;
                cheat_mode = v35;
                release_ticktimer();
              LABEL_131:
                claim_ticktimer();
              }
            }
            if ((level & 0x2000) != 0)
              cheat_mode |= 0x1000u;
            if (((unsigned int)cstart_branch_1 & level) != 0) {
              v36 = cheat_mode;
              BYTE1(v36) = BYTE1(cheat_mode) | 0x80;
              cheat_mode = v36;
            }
            if ((level & 0x8000) != 0 && switch_same >= 0)
              switch_same = v49 + 666;
            level &= 0xFu;
            damage_level = v55;
            v0 = (int)v62;
            v37 = default_names;
            do {
              HIDWORD(v2) = v0;
              name_copy((int)v37, (char *)v0);
              v0 += 9;
              v37 += 9;
            } while ((_BYTE *)v0 != v63);
            false_starts = v58;
            if (v59) {
              if (((unsigned int)cstart_branch_1 & textures_off) == 0) {
                textures_off |= (unsigned int)cstart_branch_1;
                goto LABEL_146;
              }
            } else if (((unsigned int)cstart_branch_1 & textures_off) != 0) {
              textures_off &= ~0x10000u;
            LABEL_146:
              switch_sets = -1;
            }
            manual_control[v28] = v52;
            player_started[v28] = v56;
            players_waiting = 0;
            LODWORD(v2) = 0;
            if (network_on > 0) {
              do {
                if (player_started[(_DWORD)v2])
                  ++players_waiting;
                LODWORD(v2) = v2 + 1;
              } while ((int)v2 < network_on);
            }
            if (!v31)
              goto LABEL_163;
            cheat_mode = 0;
            gssCommsPostListen(v2, HIDWORD(v2), v0);
            continue;
          }
          if (v57 < 0xFFFFFD66) {
            if (v57 == -667) {
              v29 = send_broadcast(-1);
              gssCommsPostListen(v29, HIDWORD(v29), v0);
              continue;
            }
          } else {
            if (v57 <= 0xFFFFFD66)
              goto LABEL_65;
            if (v57 == -457)
              goto LABEL_163;
          }
          time_to_start = -1;
          found = FoundNodes(v2);
          gssCommsPostListen(found, HIDWORD(found), v0);
        } else {
          v0 = 1;
          test = 1;
          if (v61 < 0)
            goto LABEL_163;
          test = 2;
          LODWORD(v2) = v61 - 1;
          v83 = (char *)(v61 - 1);
          if (v61 - 1 < 0 || (int)v2 > 4)
            goto LABEL_163;
          LODWORD(v2) = v57;
          if (v57 < 0xFFFFFD66)
            goto LABEL_25;
          if (v57 <= 0xFFFFFD66) {
            v5 = send_broadcast(-456);
            LODWORD(v5) = v83;
            gamers_playing[(_DWORD)v83] = 0;
            gssCommsPostListen(v5, HIDWORD(v5), 1);
            continue;
          }
          if (v57 == -1) {
            v6 = v83;
            gamers_playing[(_DWORD)v83] = -2;
            gssCommsPostListen(v6, &v43, 1);
          } else {
          LABEL_25:
            if (v60) {
              gamers_playing[(_DWORD)v83] = -2;
              gssCommsPostListen(v2, &v43, 1);
            } else {
              v85 = 0;
              v7 = gamers_playing[(_DWORD)v83];
              if (v7 < 0) {
                v7 = 0;
                gamers_playing[(_DWORD)v83] = 0;
              }
              v8 = 0;
              if (v7 > 0) {
                v84 = (char *)&gamers_address + 256 * (_DWORD)v83;
                v81 = v7 + 678;
                do {
                  HIDWORD(v2) = &v43;
                  v0 = 0;
                  LODWORD(v2) = &v84[16 * v8];
                  v9 = v2 + 16;
                  do {
                    if (*(_BYTE *)v2 != *(_BYTE *)HIDWORD(v2))
                      v0 = -1;
                    LODWORD(v2) = v2 + 1;
                    ++HIDWORD(v2);
                  } while ((_DWORD)v2 != v9);
                  if (!v0) {
                    v85 = v8 + 678;
                    v8 = v81;
                  }
                  ++v8;
                } while (v8 < v7);
              }
              if (v60)
                goto LABEL_163;
              v10 = v83;
              v11 = (char *)&gamers_names + 144 * (_DWORD)v83;
              if (v85) {
                v13 = name_copy((int)&v11[9 * v85 - 6102], v47);
              } else {
                name_copy((int)&v11[9 * v7], v47);
                v12 = (_DWORD *)((char *)&gamers_address + 256 * (_DWORD)v10 + 16 * v7);
                v13 = v83;
                *v12++ = v43;
                *v12++ = v44;
                *v12 = v45;
                v12[1] = v46;
                ++gamers_playing[(_DWORD)v13];
              }
              gssCommsPostListen(v13, v47, v0);
            }
          }
        }
        break;
      case 1751933796:
        net_quit = -1;
        gssCommsPostListen(v2, HIDWORD(v2), &v80);
        continue;
      case 1751933799:
        LODWORD(v2) = 4 * v78;
        if (*(int *)((char *)player_ready + v2))
          goto LABEL_163;
        ++active_nodes;
        player_ready[v78] = -1;
        netCD = -1;
        gssCommsPostListen(v2, HIDWORD(v2), &v80);
        continue;
      case 1751933800:
        gssCommsGetBlock(v80, &test_seed, 4);
        srand(*v82);
        random_seed = *v82;
        received_seed = -1;
        gssCommsPostListen(random_seed, &test_seed, 4);
        continue;
      case 1751933802:
        v0 = 40;
        gssCommsGetBlock(v80, v66, 40);
        v42 = v78;
        Players_Cars[v78] = v67;
        name_copy((int)&player_names[9 * v42], v66);
        TrackLoad = v68;
        game_type = v69;
        competitors = v72;
        level = v71;
        damage_level = v73 & 0xF;
        if ((v73 & 0x40) != 0)
          player_invul[v78] = -1;
        else
          player_invul[v78] = 0;
        HIDWORD(v2) = v78;
        LODWORD(v2) = v70;
        manual_control[v78] = v70;
        goto LABEL_163;
      case 1751933803:
        ++received_records;
        HIDWORD(v2) = v74;
        v0 = 16;
        gssCommsGetBlock(v80, v74, 16);
        WORD1(v2) = HIWORD(TrackLoad);
        v39 = v74[0] < (double)RecordLaps[TrackLoad];
        v40 = 0;
        v41 = v74[0] == RecordLaps[TrackLoad];
        LOWORD(v2) = v38;
        if (v74[0] < (double)RecordLaps[TrackLoad]) {
          RecordLaps[TrackLoad] = v74[0];
          RecordCars[TrackLoad] = v75;
          LODWORD(v2) = 0;
          do {
            LODWORD(v2) = v2 + 1;
            HIDWORD(v2) = 9 * TrackLoad;
            LOBYTE(v0) = *((_BYTE *)v74 + v2 + 3);
            *((_BYTE *)&fudge_wait + 9 * TrackLoad + v2 + 3) = v0;
          } while ((int)v2 < 9);
        }
        goto LABEL_163;
      case 1751933807:
        LODWORD(v2) = 4 * v78;
        if (*(int *)((char *)player_ready + v2))
          goto LABEL_163;
        ++active_nodes;
        player_ready[v78] = -1;
        gssCommsPostListen(v2, HIDWORD(v2), &v80);
        continue;
      case 1751933809:
        LODWORD(v2) = v78;
        gamers_address_variable_1[v78] = -2;
        gssCommsPostListen(v2, HIDWORD(v2), &v80);
        continue;
      case 1751933810:
        v3 = v78;
        v4 = v79;
        load_times[v78] = v79;
        gssCommsPostListen(v3, v4, &v80);
        continue;
      case 1751933811:
        v0 = 48;
        gssCommsGetBlock(v80, v63, 48);
        v2 = gssCommsNetAddrToNode(&address);
        if (HIDWORD(v2) != (_DWORD)v2) {
          LODWORD(v2) = v65;
          if (v65 == network_slot) {
            rec_status = 36;
            HIDWORD(v2) = v64;
            name_copy((int)&rec_mes_name, v64);
            LODWORD(v2) = 0;
            do {
              LODWORD(v2) = v2 + 1;
              BYTE4(v2) = v62[v2 + 143];
              *((_BYTE *)&LoadCarTextures_variable_1 + v2) = BYTE4(v2);
            } while ((int)v2 < 32);
          }
        }
        goto LABEL_163;
      default:
      LABEL_163:
        gssCommsPostListen(v2, HIDWORD(v2), v0);
        continue;
    }
  }*/
}

//-------------------------------------------------------------------------------------------------

void FoundNodes()
{
  if (!master) return;  // Only process if we're the master

  // Update network state
  // TODO network
  //gssCommsSortNodes();
  //network_on = gssCommsGetActiveNodes();
  //wConsoleNode = gssCommsGetConsoleNode();

  // Reset master flag and set player count
  master = 0;
  start_multiple = network_on - 1;

  // Transfer player settings to network slot
  Players_Cars[wConsoleNode] = Players_Cars[player1_car];
  manual_control[wConsoleNode] = manual_control[player1_car];
  player_invul[wConsoleNode] = player_invul[player1_car];

  // Copy player name to network slot
  name_copy(player_names[wConsoleNode], player_names[player1_car]);

  // Update player1 to current network slot
  player1_car = wConsoleNode;

  // Initialize net_players array
  if (network_on > 0) {
    for (int i = 0; i < network_on; i++) {
      net_players[i] = -1;  // 0xFFFFFFFF
    }
  }
}

//-------------------------------------------------------------------------------------------------

void SendPlayerInfo()
{
  tPlayerInfoPacket playerInfo; // [esp+0h] [ebp-44h] BYREF

  if (network_on) {
    //setup playerInfo packet
    name_copy(playerInfo.szPlayerName, player_names[player1_car]);
    playerInfo.iPlayerCar = Players_Cars[player1_car];
    playerInfo.iTrackLoad = TrackLoad;
    playerInfo.iGameType = game_type;
    playerInfo.iCompetitors = competitors;
    playerInfo.iLevel = level;
    playerInfo.iDamageLevel = damage_level;
    if (player_invul[player1_car])
      playerInfo.iDamageLevel |= 0x00000040;
    playerInfo.iManualControl = manual_control[player1_car];
    
    //setup header
    p_header.byConsoleNode = (uint8)wConsoleNode;
    p_header.uiId = PACKET_ID_PLAYER_INFO;

    //send to all nodes
    for (int i = 0; i < network_on; ++i) {
      if (wConsoleNode != i) {
        //TODO network
        //while (!gssCommsSendData(&p_header, sizeof(tSyncHeader), &playerInfo, sizeof(tPlayerInfoPacket), i))
        //  UpdateSDL(); //added by ROLLER
      }
    }
  }
}

//-------------------------------------------------------------------------------------------------

void prepare_net_message(int iMessageMode, int iMessageNumber)
{
  message_node = iMessageMode;
  message_number = iMessageNumber;
}

//-------------------------------------------------------------------------------------------------

void SendAMessage()
{
  int iNode; // [esp+4h] [ebp-4Ch]
  tMessagePacket messagePacket; // [esp+8h] [ebp-48h] BYREF
  tSyncHeader syncHeader; // [esp+38h] [ebp-18h] BYREF

  if (!broadcast_mode && send_message_to >= 0) {
    //TODO network
    syncHeader.byConsoleNode = -1;// gssCommsNetAddrToNode(address);
    syncHeader.uiId = PACKET_ID_MESSAGE;
    name_copy(messagePacket.szPlayerName, player_names[player1_car]);

    strncpy(messagePacket.szMessage, rec_mes_buf, sizeof(messagePacket.szMessage));

    messagePacket.iNetworkSlot = network_slot;

    //TODO network
    //if (send_message_to)
    //  iNode = gssCommsNetAddrToNode(&address[send_message_to]);
    //else
      iNode = 21;

    //TODO network
    //if (gssCommsSendData(&syncHeader, sizeof(tSyncHeader), &messagePacket, sizeof(tMessagePacket), iNode))
    //  send_status = 18;
    //else
      send_status = -18;
    send_message_to = -1;
  }
}

//-------------------------------------------------------------------------------------------------

void BroadcastNews()
{
  int iInitSuccess8; // eax
  int iInitSuccess7; // eax
  int iInitSuccess6; // eax
  int j; // eax
  int iInitSuccess5; // eax
  int iInitSuccess4; // eax
  int i; // eax
  int iInitSuccess3; // eax
  int iInitSuccess2; // eax
  int iInitSuccess10; // eax
  int iOldTimeToStart; // edx
  int iInitSuccess9; // eax
  int iInitSuccess; // eax

  if (!broadcast_mode)
    return;
  if (broadcast_mode < 0xFFFFFD63) {
    if (broadcast_mode < 0xFFFFF562) {
      if (broadcast_mode < 0xFFFFD8F1) {
      LABEL_81:
        iInitSuccess = -1;
        if (network_on)
          iInitSuccess = TransmitInit();
        if (iInitSuccess) {
          check_cars();
          broadcast_mode = 0;
        }
        return;
      }
      if (broadcast_mode <= 0xFFFFD8F1) {
        if (total_wins[car_request + 15] < 2) {
          iInitSuccess2 = -1;
          if (network_on)
            iInitSuccess2 = TransmitInit();
          if (!iInitSuccess2)
            return;
          car_request = -car_request;
          check_cars();
        } else {
          car_request = 0;
        }
        broadcast_mode = 0;
        return;
      }
      if (broadcast_mode != 0xFFFFF211)
        goto LABEL_81;
      if (wConsoleNode != master)
        send_record_to_master(TrackLoad);
    } else {
      if (broadcast_mode <= 0xFFFFF562) {
        send_record_to_slaves(TrackLoad);
        send_seed(random_seed);
        broadcast_mode = 0;
        return;
      }
      if (broadcast_mode < 0xFFFFFD61)
        goto LABEL_81;
      if (broadcast_mode <= 0xFFFFFD61) {
        if (master) {
          iInitSuccess3 = -1;
          time_to_start = -1;
          if (network_on)
            iInitSuccess3 = TransmitInit();
          if (!iInitSuccess3) {
            time_to_start = 0;
            return;
          }
          FoundNodes();
        }
        broadcast_mode = 0;
      } else {
        StartPressed = 0;
        iInitSuccess4 = -1;
        if (network_on)
          iInitSuccess4 = TransmitInit();
        if (iInitSuccess4) {
          player_started[0] = 0;
          players_waiting = 0;
          for (i = 0; i < network_on; ++i) {
            if (player_started[i])
              ++players_waiting;
          }
          broadcast_mode = 0;
        }
      }
    }
  } else if (broadcast_mode <= 0xFFFFFD63) {
    iInitSuccess5 = -1;
    if (network_on)
      iInitSuccess5 = TransmitInit();
    if (iInitSuccess5) {
      waste = CheckNames(player_names[player1_car], player1_car);
      broadcast_mode = 0;
    }
  } else if (broadcast_mode < 0xFFFFFD66) {
    if (broadcast_mode <= 0xFFFFFD64) {
      StartPressed = -1;
      iInitSuccess6 = -1;
      if (network_on)
        iInitSuccess6 = TransmitInit();
      if (iInitSuccess6) {
        player_started[0] = -1;
        players_waiting = 0;
        for (j = 0; j < network_on; ++j) {
          if (player_started[j])
            ++players_waiting;
        }
        broadcast_mode = 0;
      }
    } else {
      time_to_start = -667;
      iInitSuccess7 = -1;
      if (network_on)
        iInitSuccess7 = TransmitInit();
      time_to_start = 0;
      if (iInitSuccess7)
        broadcast_mode = 0;
    }
  } else if (broadcast_mode <= 0xFFFFFD66) {
    time_to_start = -666;
    iInitSuccess8 = -1;
    if (network_on)
      iInitSuccess8 = TransmitInit();
    time_to_start = 0;
    if (iInitSuccess8) {
      I_Quit = -1;
      broadcast_mode = 0;
    }
  } else {
    if (broadcast_mode < 0xFFFFFE38) {
      if (broadcast_mode == 0xFFFFFE37) {
        iOldTimeToStart = time_to_start;
        time_to_start = 0xFFFFFE37;
        iInitSuccess9 = -1;
        if (network_on)
          iInitSuccess9 = TransmitInit();
        if (iInitSuccess9)
          broadcast_mode = 0;
        time_to_start = iOldTimeToStart;
      }
      goto LABEL_81;
    }
    if (broadcast_mode <= 0xFFFFFE38) {
      time_to_start = 0xFFFFFE38;
      //_STOSD(gamers_playing, 0, v0, 4u);
      memset(gamers_playing, 0, sizeof(gamers_playing));
      iInitSuccess10 = -1;
      if (network_on)
        iInitSuccess10 = TransmitInit();
      if (iInitSuccess10)
        broadcast_mode = 0;
      time_to_start = 0;
    } else {
      if (broadcast_mode != 0xFFFFFEC6)
        goto LABEL_81;
      FoundNodes();
      SendPlayerInfo();
      if (wConsoleNode != master)
        send_record_to_master(TrackLoad);
      broadcast_mode = 0;
    }
  }
}

//-------------------------------------------------------------------------------------------------

void remove_messages(int iClear)
{
  //uint8 buffer[4]; // [esp-Ch] [ebp-10h] BYREF
  //
  //// first message processing loop
  //do {
  //  gssCommsPostListen();                       // check for incoming messages
  //  UpdateSDL(); //added by ROLLER
  //} while (gssCommsGetHeader(&in_header, sizeof(tSyncHeader), buffer));// continue until there are no more messages
  //
  //// additional clearing if condition is true
  //if (iClear) {
  //  while (gssCommsPostListen())
  //    UpdateSDL(); //added by ROLLER
  //}
  clear_network_game();
}

//-------------------------------------------------------------------------------------------------

void reset_network(int iResetBroadcastMode)
{
  int iNode; // edx
  int i; // edx

  iNode = 0;
  gamers_playing[0] = 0;
  gamers_playing[1] = 0;
  gamers_playing[2] = 0;
  gamers_playing[3] = 0;
  //TODO network
  wConsoleNode = 0;// gssCommsNetAddrToNode(address);
  while (iNode < wConsoleNode) {
    //TODO network
    //if (gssCommsDeleteNode(0)) {
    //  printf("FAILED TO DELETE NODE c (%d)!!!\n", iNode);
    //  doexit();
    //}
    //gssCommsSortNodes();
    ++iNode;
  }
  for (i = wConsoleNode + 1; i < network_on; ++i) {
    //TODO network
    //if (gssCommsDeleteNode(1)) {
    //  printf("FAILED TO DELETE NODE (%d)!!!\n", i);
    //  doexit();
    //}
    //gssCommsSortNodes();
  }

  for (int i = 0; i < 16; ++i) {
    player_started[i] = 0;
    net_players[i] = 0;
  }

  player1_car = 0;
  received_seed = 0;
  frame_number = 0;
  time_to_start = 0;
  broadcast_mode = 0;
  players_waiting = 0;
  active_nodes = 0;
  net_quit = 0;
  master = -667;
  wConsoleNode = 0;
  I_Quit = 0;
  my_age = 0;
  //TODO network
  //gssCommsGetNetworkAddr(address[0]);
  //gssCommsGetConsoleNode();
  network_on = 1;
  if (network_slot >= 0)
    broadcast_mode = -1;
  else
    broadcast_mode = 0xFFFFFE38;
  tick_on = -1;
  if (iResetBroadcastMode) {
    send_broadcast(broadcast_mode);
    broadcast_mode = 0;
  }
  //TODO network
  //gssCommsSortNodes();
  received_records = 1;
  switch_sets = 0;
  switch_types = 0;
  players = network_on;
  message_received = -1;
  message_sent = -1;
  if (StartPressed) {
    player_started[0] = -1;
    players_waiting = 1;
  }
}

//-------------------------------------------------------------------------------------------------

void clear_network_game()
{
  // for (int iPlayer = 0; iPlayer < 16; ++iPlayer) {
  //   player_ready[iPlayer] = 0;
  //   // Start at offset = iPlayer * 2 and step by 32
  //   for (int iOffset = iPlayer * 2; iOffset < 0x4000 / 2; iOffset += 16)  // step is 32 bytes = 16 shorts
  //   {
  //     player_checks[iOffset] = -1;
  //   }
  // }
  // memset(copy_multiple, 0, sizeof(copy_multiple));
}

//-------------------------------------------------------------------------------------------------

void reset_net_wait()
{
  for (int i = 0; i < 16; ++i) {
    player_started[i] = 0;
  }

  time_to_start = 0;
  players_waiting = 0;
}

//-------------------------------------------------------------------------------------------------

unsigned int send_broadcast(unsigned int uiBroadcastMode)
{
  uint32 uiOldTimeToStart;

  if (uiBroadcastMode < 0xFFFFFE38) {
    if (uiBroadcastMode == 0xFFFFFE37) {
      uiOldTimeToStart = time_to_start;
      time_to_start = 0xFFFFFE37;
      do {
        uiBroadcastMode = -1;
        if (network_on)
          uiBroadcastMode = TransmitInit();
      } while (!uiBroadcastMode);
      time_to_start = uiOldTimeToStart;
    }
  } else if (uiBroadcastMode <= 0xFFFFFE38) {
    uiOldTimeToStart = time_to_start;
    time_to_start = 0xFFFFFE38;
    do {
      uiBroadcastMode = -1;
      if (network_on)
        uiBroadcastMode = TransmitInit();
    } while (!uiBroadcastMode);
    time_to_start = uiOldTimeToStart;
  } else if (uiBroadcastMode == -1) {
    do {
      uiBroadcastMode = -1;
      if (network_on)
        uiBroadcastMode = TransmitInit();
    } while (!uiBroadcastMode);
  }
  return uiBroadcastMode;
}

//-------------------------------------------------------------------------------------------------

void send_here(int iNode, int iFrame)
{
  int i; // esi
  tSyncHeader syncHeader; // [esp+0h] [ebp-1Ch] BYREF
  //int iData; // [esp+Ch] [ebp-10h] BYREF

  if (network_on) {
    syncHeader.byConsoleNode = iNode;
    syncHeader.unFrameId = iFrame;
    syncHeader.uiId = PACKET_ID_SEND_HERE;
    if (iNode == master) {
      for (i = 0; i < network_on; ++i) {
        if (i != master) {
          //TODO network
          //gssCommsSendData(&syncHeader, sizeof(tSyncHeader), iData, sizeof(int), i);
        }
      }
    } else {
      //TODO network
      //gssCommsSendData(&syncHeader, sizeof(tSyncHeader), iData, sizeof(int), master);
    }
  }
}

//-------------------------------------------------------------------------------------------------
