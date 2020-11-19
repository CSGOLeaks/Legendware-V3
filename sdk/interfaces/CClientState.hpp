#pragma once

#include <cstdint>

#include "..\math\Vector.hpp"
#include "..\misc\bf_write.h"

#define GenDefineVFunc(...) ( this, __VA_ARGS__ ); }
#define VFUNC( index, func, sig ) auto func { return call_virtual< sig >( this, index ) GenDefineVFunc

class CClockDriftMgr {
public:
	float m_ClockOffsets[ 17 ];   //0x0000
	uint32_t m_iCurClockOffset; //0x0044
	uint32_t m_nServerTick;     //0x0048
	uint32_t m_nClientTick;     //0x004C
}; 

class INetChannel {
public:
	char pad_0x0000[ 0x18 ]; //0x0000
	__int32 m_nOutSequenceNr; //0x0018 
	__int32 m_nInSequenceNr; //0x001C 
	__int32 m_nOutSequenceNrAck; //0x0020 
	__int32 m_nOutReliableState; //0x0024 
	__int32 m_nInReliableState; //0x0028 
	__int32 m_nChokedPackets; //0x002C

	void Transmit(bool onlyreliable)
	{
		using Fn = bool(__thiscall*)(void*, bool);
		call_virtual<Fn>(this, 49)(this, onlyreliable);
	}

	void send_datagram()
	{
		using Fn = int(__thiscall*)(void*, void*);
		call_virtual<Fn>(this, 46)(this, NULL);
	}

	void SetTimeOut(float seconds)
	{
		using Fn = void(__thiscall*)(void*, float);
		return call_virtual<Fn>(this, 4)(this, seconds);
	}

	int RequestFile(const char* filename)
	{
		using Fn = int(__thiscall*)(void*, const char*);
		return call_virtual<Fn>(this, 62)(this, filename);
	}
};

class INetMessage
{
public:
	virtual	~INetMessage() {};

	// Use these to setup who can hear whose voice.
	// Pass in client indices (which are their ent indices - 1).

	virtual void	SetNetChannel(INetChannel * netchan) = 0; // netchannel this message is from/for
	virtual void	SetReliable(bool state) = 0;	// set to true if it's a reliable message

	virtual bool	Process(void) = 0; // calles the recently set handler to process this message

	virtual	bool	ReadFromBuffer(bf_read &buffer) = 0; // returns true if parsing was OK
	virtual	bool	WriteToBuffer(bf_write &buffer) = 0;	// returns true if writing was OK

	virtual bool	IsReliable(void) const = 0;  // true, if message needs reliable handling

	virtual int				GetType(void) const = 0; // returns module specific header tag eg svc_serverinfo
	virtual int				GetGroup(void) const = 0;	// returns net message group of this message
	virtual const char		*GetName(void) const = 0;	// returns network message name, eg "svc_serverinfo"
	virtual INetChannel		*GetNetChannel(void) const = 0;
	virtual const char		*ToString(void) const = 0; // returns a human readable string about message content
};

class CClientState
{
public:
	char _0x0000[156];
	INetChannel* m_ptrNetChannel; //0x009C 
	__int32 m_iChallengeNr; //0x00A0 
	char _0x00A4[100];
	__int32 m_iSignonState; //0x0108 
	char _0x010C[8];
	float m_flNextCmdTime; //0x0114 
	__int32 m_iServerCount; //0x0118 
	__int32 m_iCurrentSequence; //0x011C 
	char _0x0120[4];
	__int32 m_iClockDriftMgr; //0x0124 
	char _0x0128[68];
	__int32 m_iServerTick; //0x016C 
	__int32 m_iClientTick; //0x0170 
	__int32 m_iDeltaTick; //0x0174 
	bool m_bPaused; //0x0178 
	char _0x017C[12];
	char m_szLevelName[260]; //0x0188 
	char m_szLevelNameShort[40]; //0x028C 
	char m_szGroupName[40]; //0x02B4 
	char m_szSecondName[32]; //0x02DC 
	char _0x02FC[140];
	__int32 m_nMaxClients; //0x0388 
	char _0x038C[18820];
	float m_flLastServerTickTime; //0x4D10 
	bool m_bInSimulation; //0x4D14 
	__int32 m_iOldTickCount; //0x4D18 
	float m_flTickRemainder; //0x4D1C 
	float m_flFrameTime; //0x4D20 
	__int32 m_iLastOutgoingCommand; //0x4D24 
	__int32 m_iChockedCommands; //0x4D28 
	__int32 m_iLastCommandAck; //0x4D2C 
	__int32 m_iCommandAck; //0x4D30 
	__int32 m_iSoundSequence; //0x4D34 
	char _0x4D38[80];
	Vector m_vecViewAngles; //0x4D88 
	char _0x4D94[55];
	void* m_ptrEvents; //0x4E64  
}; 