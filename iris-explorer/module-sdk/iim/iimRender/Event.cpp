#include "stdafx.h"
#include "cxmRenderIF.h"
#include "GenericInteractor.h"
#include "MouseInteractor.h"

cxmRenderIF renderIF;

extern "C" 
{
	void render_init(void)
	{
		//Sleep(10000);	
		renderIF.init();

		GenericInteractor::initClass() ;
		MouseInteractor::initClass() ;
	}

	int render() 
	{
		return renderIF.user_func();
	}

	void render_disconnect_input(char *,int tag)
	{
	  renderIF.disconnect_in(tag);
	}

	void render_connect_output(char *name,int)
	{
		if (strcmp(name,"Output Camera"))
			return;
		renderIF.connect_out();
	}

	void render_disconnect_output(char *name,int)
	{
		if ( strcmp(name,"Output Camera") )
			return;

		if ( cxOutputConnectsGet( cxOutputPortOpen(name) ) == 1 )
		{
		  renderIF.disconnect_out();
		}
	}
}