#pragma once

template <typename FuncType>
__forceinline static FuncType call_virtual(void* ppClass, int index)
{
	int* pVTable = *(int**) ppClass;
	int dwAddress = pVTable[index];
	return (FuncType)(dwAddress);
}