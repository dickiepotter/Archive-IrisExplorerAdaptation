#include "mUserFunction.h"

using namespace System;
using namespace System::Runtime::InteropServices;
using namespace IrisSharp;

long mUserFunction(void) {return Module::Instance->UserFunction();}

void mInit(void) {Module::Instance->Init();}

void mRemove(void) {Module::Instance->Remove();}

void mConnectIn(const char * portName, int id) {Module::Instance->ConnectIn( Marshal::PtrToStringAnsi(IntPtr::IntPtr((void*)portName)), id );}

void mDisconnectIn(const char * portName, int id) {Module::Instance->DisconnectIn( Marshal::PtrToStringAnsi(IntPtr::IntPtr((void*)portName)), id );}

void mConnectOut(const char * portName, int id){Module::Instance->ConnectOut( Marshal::PtrToStringAnsi(IntPtr::IntPtr((void*)portName)), id );}

void mDisconnectOut(const char * portName, int id){Module::Instance->DisconnectOut( Marshal::PtrToStringAnsi(IntPtr::IntPtr((void*)portName)), id );}

void mCreate(void){Module::Instance->Create();}

long mPortCheckIn ( int linkC, void *links[], long *cxMDWPortID, long *linkIDs ) {return 0;}
long mPortCheckOut ( int linkC, void *links[], long *cxMDWPortID ){return 0;}
