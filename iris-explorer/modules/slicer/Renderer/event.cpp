#include <cx/PortAccess.h>
#include <cx/DataAccess.h>
#include <cx/DataTypes.h>
#include <cx/DataOps.h>
#include <cx/Geometry.h>
#include <cx/Pick.h>
#include <cx/Info.h>
#include <cx/UI.h>
#include "RenderIF.h"

cxmRenderIF renderIF;

/////////////////////////////////////////////////////////////////////////
//
// Module Control Wrapper CallBack Functions 
//
/////////////////////////////////////////////////////////////////////////
// BEGIN_IVWGEN

#include "SoFotisGeneric.h"
#include "SoFotis3Dragger.h"

extern "C" {
void 
render_init(void)
{
	renderIF.init();

	SoFotisGeneric::initClass() ;
	SoFotis3Dragger::initClass() ;
}

int 
render() 
{
  return renderIF.user_func();
}

void 
render_disconnect_input(char *,int tag)
{
  renderIF.disconnect_in(tag);
}

void 
render_connect_output(char *name,int)
{
	if (strcmp(name,"Output Camera"))
		return;
	renderIF.connect_out();
}

void 
render_disconnect_output(char *name,int)
{
	if ( strcmp(name,"Output Camera") )
		return;
	
// Number of connections is decremented until after
// returning from this call, therefore check for 1 instead of zero

	if ( cxOutputConnectsGet( cxOutputPortOpen(name) ) == 1 )
	{
	  renderIF.disconnect_out();
	}
}


}