#include "stdafx.h"

extern "C" 
{
	void UserFunction ( void  );
	extern void Init(void);
	extern void Remove(void);
	extern void ConnectInput(const char *, int);
	extern void DiconnectInput(const char *, int);
	extern void ConnectOutput(const char *, int);
	extern void DisconnectOutput(const char *, int);
	extern void Create(void);
}
