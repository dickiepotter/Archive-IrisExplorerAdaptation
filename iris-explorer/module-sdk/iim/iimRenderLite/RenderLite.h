#include "stdafx.h"

extern "C" 
{
	void renderlite 
	(
		 cxGeometry *	geo, 
		 int    		geoFlag, 
		 long   		window, 
		 int    		windowFlag 
	);

	extern void render_init(void);
	extern void render_disconnect_input(const char *, int);
	extern void render_connect_output(const char *, int);
	extern void render_disconnect_output(const char *, int);
}
