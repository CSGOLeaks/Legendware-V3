//========= Copyright   1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//
#pragma once
#include "..\math\VMatrix.hpp"

class CBoneAccessor 
{
public:
	void* m_pAnimating;
	matrix3x4_t* m_pBones;
	int m_ReadableBones;
	int m_WritableBones;
};