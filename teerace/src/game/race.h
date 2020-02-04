/* race Class by Sushi and Redix */
#ifndef GAME_RACE_H
#define GAME_RACE_H

// helper class
class IRace
{
public:
	static int TimeFromSecondsStr(const char *pStr); // x.xxx
	static int TimeFromStr(const char *pStr); // x minute(s) x.xxx second(s) / x.xxx second(s) / xx:xx.xxx
	static int TimeFromFinishMessage(const char *pStr, char *pNameBuf, int NameBufSize); // xxx finished in: x minute(s) x.xxx second(s)

	static void FormatTimeLong(char *pBuf, int Size, int Time, bool ForceMinutes = false);
	static void FormatTimeShort(char *pBuf, int Size, int Time, bool ForceMinutes = false);
	static void FormatTimeDiff(char *pBuf, int Size, int Time, bool Milli = true);
};

#endif
