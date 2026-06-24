#include <stdlib.h>

#include <Inventor/Win/SoWin.h>
#include <Inventor/Win/viewers/SoWinExaminerViewer.h>
#include <Inventor/Win/viewers/SoWinFlyViewer.h>
#include <Inventor/Win/viewers/SoWinPlaneViewer.h>

#include <Inventor/engines/SoElapsedTime.h>

#include <Inventor/manips/SoTrackballManip.h>

#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoRotationXYZ.h>
#include <Inventor/nodes/SoSeparator.h>


main(int , char **argv)
{
   // Initialize Inventor. This returns a main window to use.
   // If unsuccessful, exit.
   HWND myWindow = SoWin::init(argv[0]);
	if ( ! myWindow )
    exit( 1 );
           
   // Make a scene containing a red cone
   SoSeparator *root = new SoSeparator;
   root->ref();

   SoPerspectiveCamera *myCamera = new SoPerspectiveCamera;
   root->addChild(myCamera);
   root->addChild(new SoDirectionalLight);
   root->addChild(new SoTrackballManip);
   
  // SoRotationXYZ *myRotXYZ = new SoRotationXYZ;
  // root->addChild(myRotXYZ);
   
   SoMaterial *myMaterial = new SoMaterial;
   myMaterial->diffuseColor.setValue(0.0, 0.0, 1.0);   // Red
   root->addChild(myMaterial);
   root->addChild(new SoCone);

 //  myRotXYZ->axis = SoRotationXYZ::X;
 //  SoElapsedTime *myCounter = new SoElapsedTime;
 //  myRotXYZ->angle.connectFrom(&myCounter->timeOut);


   // Create a renderArea in which to see our scene graph.
   // The render area will appear within the main window.
   SoWinExaminerViewer *myRenderArea = new SoWinExaminerViewer(myWindow);

   // Make myCamera see everything.
   myCamera->viewAll(root, myRenderArea->getViewportRegion());

   // Put our scene in myRenderArea, change the title
   myRenderArea->setSceneGraph(root);
   myRenderArea->setTitle("Hello Cone");
   myRenderArea->show();

   SoWin::show(myWindow);  // Display main window
   SoWin::mainLoop();      // Main Inventor event loop
}

