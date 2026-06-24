#include <cx/winMcw.h>

class cxmRenderIF : public cxModuleApp
{
private:
	int fired;

public:
	cxmRenderIF();
	void init();
	virtual BOOL OnIdle( LONG lCount );
	int user_func();
	void disconnect_in(int tag);
	void connect_out();
	void disconnect_out();

	void *pDocTemplate;
	void *theDoc;
};