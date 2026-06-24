long mUserFunction(void);
void mInit(void);
void mRemove(void);
void mConnectIn(const char *, int);
void mDisconnectIn(const char *, int) ;
void mConnectOut(const char *, int);
void mDisconnectOut(const char *, int);
void mCreate(void);

long mPortCheckIn ( int linkC, void *links[], long *cxMDWPortID, long *linkIDs ) ;
long mPortCheckOut ( int linkC, void *links[], long *cxMDWPortID );