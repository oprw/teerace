/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_VERSION_H
#define GAME_VERSION_H
#include <generated/nethash.cpp>
//#define GAME_VERSION "0.7.3, mkRace " MKRACE_VERSION ""
#define GAME_VERSION "0.7.3, " TEERACE_VERSION
#define GAME_NETVERSION_HASH_FORCED "802f1be60a05665f"
#define GAME_NETVERSION "0.7 " GAME_NETVERSION_HASH_FORCED
#define RACE_VERSION "4.0-dev"
#define DDRACE_VERSION "1.1"
#define MKRACE_VERSION "1.2.1"
#define TEERACE_VERSION "0.2.2"
#define CLIENT_VERSION 0x0703
static const char GAME_RELEASE_VERSION[8] = "0.7.3.1";
#endif
