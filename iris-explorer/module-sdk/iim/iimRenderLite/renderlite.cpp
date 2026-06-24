#include "stdafx.h"

#include <GenericInteractor.h>
#include <MouseInteractor.h>
#include <P5GloveInteractor.h>

Widget	top;	
SoXtViewer *viewer;	
SoGroup *root;

SoSeparator** geometryGroups;
int *groupConnectionIds;
int groupCount =0;

extern "C" void renderlite
(
	cxGeometry *geo,
	int geoFlag,
	long window,
	int windowFlag
)				
{
	static int first = 1;

	// Run this code first time the user function is called ONLY
	if ( first && window )
	{
		first = 0;
		cxGeoInit();

		GenericInteractor::initClass() ;
		//MouseInteractor::initClass() ;
		GloveInteractor::initClass() ;

		SoXt::init("RenderLite");

		root = new SoSeparator();
		root->ref();

		SoCamera *camera = new SoOrthographicCamera();
		root->addChild(camera);

		SoShapeHints *n1 = new SoShapeHints();
		n1->vertexOrdering.setValue(SoShapeHints::COUNTERCLOCKWISE);
		root->addChild(n1);

		viewer = new SoXtExaminerViewer((Widget)window);
		viewer->setSceneGraph(root);
		viewer->show();
	}

	// Run this code EVERY time the user function is called
	int *connectionIds; 	
	int *changes;
	cxGeometry** geometry;

	int portID = cxInputPortOpen("geometry");
	int connectionCount =  cxInputDataConnIDGetV(portID, &connectionIds);
	int connectionCount2 = cxInputDataChangedV(portID, &changes);
	int connectionCount3 = cxInputDataGetV(portID, (void***)&geometry);

	if(connectionCount != connectionCount2 || connectionCount != connectionCount3)
		printf("Connection id indexer does not match changed connections indexer.");

	SoSeparator** newGroups = new SoSeparator*[connectionCount];
	int *newIds = new int[connectionCount];

	for(int i=0; i<connectionCount; i++)
	{
		int id =  connectionIds[i];
		int indexInGlobal = -1;
		
		SoSeparator *group = NULL;

		// Have we added this (i) connection before? yes -> then get the information.
		for(int j=0; j<groupCount; j++)
			if(groupConnectionIds[j] == id){indexInGlobal=j; group=geometryGroups[j]; break;}

		// Have we added this (i) connection before? No -> then setup new variables.
		if(indexInGlobal < 0) {group = new SoSeparator; root->addChild(group);}

		// Has the data changed since last added?
		SoTranReceiver* reciever = new SoTranReceiver(group);
		if(changes[i]) {group->removeAllChildren(); cxGeoReceive(geometry[i],reciever);}

		// Register information for next run, even if the data hasn't changed
		newGroups[i] = group;
		newIds[i] = id;
	}

	// Clean out groups that nolonger have connections
	for(int i=0; i<groupCount; i++)
		for(int j=0; j<connectionCount; j++)
			if(groupConnectionIds[i] == connectionIds[j]) {break;}
			else if(j==connectionCount-1){root->removeChild(geometryGroups[i]);}

	// Propogate registaration information to globals
	geometryGroups = newGroups;
	groupConnectionIds = newIds;
	groupCount = connectionCount;

	viewer->viewAll();
}

extern "C" void render_init(void){}
extern "C" void render_Remove(void){}
extern "C" void render_connect_input(char *,int tag){}
extern "C" void render_disconnect_input(char *,int tag){}
extern "C" void render_connect_output(char *name,int){}
extern "C" void render_disconnect_output(char *name,int){}
extern "C" void render_create(void){}
