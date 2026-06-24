#include <stdio.h>
#include <Inventor/SbLinear.h>
#include <Inventor/SoDB.h>
#include <Inventor/sensors/SoFieldSensor.h>
#include <Inventor/SoInput.h>
#include <Inventor/SoPath.h>

#include <Inventor/projectors/SbPlaneProjector.h>
#include <Inventor/projectors/SbLineProjector.h>

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoText2.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoPickStyle.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoFont.h>

#include <Inventor/events/SoEvent.h>
#include <Inventor/events/SoKeyboardEvent.h>

#include <cx/UI.h>

#include "SoFotis3Dragger.h"
#include "SoFotis3DraggerGeom.h"


SO_KIT_SOURCE(SoFotis3Dragger);

void SoFotis3Dragger::initClass()
{
//    static int once=0;
//    if (!once) {   
   	SO_KIT_INIT_CLASS(SoFotis3Dragger, SoDragger, "Dragger") ;
//        once=1;
//    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
SoFotis3Dragger::SoFotis3Dragger()
//
////////////////////////////////////////////////////////////////////////
{
//    initClass();

printf("Open Inventor version: %s\n",SoDB::getVersion()) ;

    SO_KIT_CONSTRUCTOR(SoFotis3Dragger);

    // Put this stuff under the geomSeparator so it will draw more
    // efficiently.

    SO_KIT_ADD_CATALOG_ENTRY(axesStatic, SoSeparator, TRUE, geomSeparator,\x0  ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(axesPickStyle, SoPickStyle, TRUE, geomSeparator,\x0 ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(fontStyle, SoFont, TRUE, geomSeparator,\x0 ,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(intNameSep, SoSeparator, TRUE, geomSeparator,\x0 ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(intNameMat, SoMaterial, TRUE, intNameSep,\x0 ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(intNameTrans, SoTranslation, TRUE, intNameSep,\x0 ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(intNameText, SoText2, TRUE, intNameSep,\x0 ,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(axesMaterial, SoMaterial, TRUE, geomSeparator,\x0 ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(axesPoints, SoCoordinate3, TRUE, geomSeparator,\x0 ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(axesStyle, SoDrawStyle, TRUE, geomSeparator,\x0 ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(axesLine, SoLineSet, TRUE, geomSeparator,\x0 ,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(axesSep, SoSeparator, TRUE, geomSeparator,\x0 ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(axesMat, SoMaterial, TRUE, axesSep,\x0 ,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(axesXSep, SoSeparator, TRUE, axesSep,\x0 ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(axesXTrans, SoTranslation, TRUE, axesXSep,\x0 ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(axesXCube, SoCube, TRUE, axesXSep,\x0 ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(axesXTextTrans, SoTranslation, TRUE,axesXSep,\x0 ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(axesXTextMat, SoMaterial, TRUE,axesXSep,\x0 ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(axesXText, SoText2, TRUE,axesXSep,\x0 ,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(axesYSep, SoSeparator, TRUE, axesSep,\x0 ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(axesYTrans, SoTranslation, TRUE, axesYSep,\x0 ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(axesYCube, SoCube, TRUE, axesYSep,\x0 ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(axesYTextTrans, SoTranslation, TRUE,axesYSep,\x0 ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(axesYTextMat, SoMaterial, TRUE,axesYSep,\x0 ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(axesYText, SoText2, TRUE,axesYSep,\x0 ,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(axesZSep, SoSeparator, TRUE, axesSep,\x0 ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(axesZTrans, SoTranslation, TRUE, axesZSep,\x0 ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(axesZCube, SoCube, TRUE, axesZSep,\x0 ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(axesZTextTrans, SoTranslation, TRUE,axesZSep,\x0 ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(axesZTextMat, SoMaterial, TRUE,axesZSep,\x0 ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(axesZText, SoText2, TRUE,axesZSep,\x0 ,TRUE);


    SO_KIT_ADD_CATALOG_ENTRY(translatorTranslate, SoTranslation, TRUE, geomSeparator,\x0 ,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(blobPickStyle, SoPickStyle, TRUE, geomSeparator,\x0 ,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(translatorSwitch, SoSwitch, TRUE,geomSeparator,\x0 ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(translator, SoSeparator, TRUE,translatorSwitch,\x0 ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(translatorActive, SoSeparator, TRUE,translatorSwitch,\x0 ,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(feedbackSwitch, SoSwitch, TRUE,geomSeparator,\x0 ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(feedback, SoSeparator, TRUE,feedbackSwitch,\x0 ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(feedbackActive, SoSeparator, TRUE,	feedbackSwitch,\x0 ,TRUE);

    // read geometry for shared parts
    if (SO_KIT_IS_FIRST_INSTANCE())
	readDefaultParts("fotis3Dragger.iv", geomBuffer, sizeof(geomBuffer) );

    SO_KIT_ADD_FIELD(value, (0.0, 0.0, 0.0));
    SO_KIT_ADD_FIELD(minimum, (0.0, 0.0, 0.0));
    SO_KIT_ADD_FIELD(maximum, (0.0, 0.0, 0.0));

    SO_KIT_INIT_INSTANCE();

    // create the parts...
//F
   setPartAsDefault("axesStatic",      "fotis3Static");

//void set1Value(int index, float x, float y, float z)

  SoMaterial *myMat = SO_GET_ANY_PART(this, "axesMaterial", SoMaterial) ;
  myMat->diffuseColor.setValue(0.0,0.0,1.0) ;

  myMat = SO_GET_ANY_PART(this, "intNameMat", SoMaterial) ;
  myMat->diffuseColor.setValue(1.0,1.0,1.0) ;

  SoCoordinate3 *myCoords = SO_GET_ANY_PART(this, "axesPoints", SoCoordinate3) ;
  myCoords->point.set1Value(0, 0.0,0.0,0.0) ;
  myCoords->point.set1Value(1, 0.0,0.0,0.0) ;

  SoLineSet *lineS = SO_GET_ANY_PART (this, "axesLine", SoLineSet) ;
  lineS = new SoLineSet ;

  SoDrawStyle * dStyle = SO_GET_ANY_PART (this, "axesStyle", SoDrawStyle) ;
  dStyle->lineWidth = 3 ;


    SoFont *myFont = SO_GET_ANY_PART(this, "fontStyle", SoFont) ;
    myFont->size = 20 ;

    SoMaterial *axesmat = SO_GET_ANY_PART(this, "axesMat", SoMaterial) ;
    axesmat->diffuseColor.setValue(.7, .7, .0) ;

    SoPickStyle *pick = SO_GET_ANY_PART(this, "axesPickStyle", SoPickStyle) ;
    pick->style = SoPickStyle::UNPICKABLE ;

    pick = SO_GET_ANY_PART(this, "blobPickStyle", SoPickStyle) ;
    pick->style = SoPickStyle::SHAPE ;

    SoTranslation *axestrans = SO_GET_ANY_PART(this, "axesXTrans", SoTranslation) ;
    axestrans->translation.setValue(.0, .0, .0) ;

    SoCube *axescube = SO_GET_ANY_PART(this, "axesXCube", SoCube) ;
    axescube->height = 0.05 ;
    axescube->depth = 0.05 ;
    axescube->width = 0.0 ;

    axescube = SO_GET_ANY_PART(this, "axesYCube", SoCube) ;
    axescube->height = 0.0 ;
    axescube->depth = 0.05 ;
    axescube->width = 0.05 ;

    axescube = SO_GET_ANY_PART(this, "axesZCube", SoCube) ;
    axescube->height = 0.05 ;
    axescube->depth = 0.0 ;
    axescube->width = 0.05 ;

    SoTranslation *trans = SO_GET_ANY_PART(this, "axesXTextTrans", SoTranslation) ;
    trans->translation.setValue(.05, .0, .0) ;

    SoMaterial *mat = SO_GET_ANY_PART(this, "axesXTextMat", SoMaterial) ;
    mat->diffuseColor.setValue(1.0, 1.0, 1.0) ;

    trans = SO_GET_ANY_PART(this, "axesYTextTrans", SoTranslation) ;
    trans->translation.setValue(.0, .05, .0) ;

    mat = SO_GET_ANY_PART(this, "axesYTextMat", SoMaterial) ;
    mat->diffuseColor.setValue(1.0, 1.0, 1.0) ;

    trans = SO_GET_ANY_PART(this, "axesZTextTrans", SoTranslation) ;
    trans->translation.setValue(.0, .0, .05) ;

    mat = SO_GET_ANY_PART(this, "axesZTextMat", SoMaterial) ;
    mat->diffuseColor.setValue(1.0, 1.0, 1.0) ;

    trans = SO_GET_ANY_PART(this, "intNameTrans", SoTranslation) ;
    trans->translation.setValue(-.2, .0, .0) ;

    SoText2 *myText = SO_GET_ANY_PART(this, "intNameText", SoText2) ;
    myText->justification = SoText2::RIGHT ;

//F

   setPartAsDefault("translator",      "fotis3Translator");
   setPartAsDefault("translatorActive","fotis3TranslatorActive");
   setPartAsDefault("feedback",        "fotis3Feedback");
   setPartAsDefault("feedbackActive",  "fotis3FeedbackActive");

    // Set the switches to 0...
    setSwitchValue( translatorSwitch.getValue(), 0 );
    setSwitchValue( feedbackSwitch.getValue(), 0 );

    // Create the line projector
    planeProj = new SbPlaneProjector();
    lineProj  = new SbLineProjector() ;

    // add the callbacks to perform the dragging
    addStartCallback(  &SoFotis3Dragger::startCB );
    addMotionCallback( &SoFotis3Dragger::motionCB );
    addFinishCallback(   &SoFotis3Dragger::finishCB );

    // Updates the translation field when the motionMatrix is set.
    addValueChangedCallback( &SoFotis3Dragger::valueChangedCB );

    addOtherEventCallback(&SoFotis3Dragger::metaKeyChangeCB) ;

    // Updates the motionMatrix when the translation field is set.
    fieldSensor = new SoFieldSensor( &SoFotis3Dragger::fieldSensorCB, this);
    fieldSensor->setPriority( 0 );

    setUpConnections( TRUE, TRUE );
}





void SoFotis3Dragger::metaKeyChangeCB(void *, SoDragger *inDragger)
{
   SoFotis3Dragger *dragger = (SoFotis3Dragger *) inDragger ;

   SoHandleEventAction *ha = dragger->getHandleEventAction() ;

   //check for the SHIFT key

   const SoEvent *event = dragger->getEvent() ;
   dragger->shiftDown = event->wasShiftDown() ;
   dragger->controlDown = event->wasCtrlDown() ;
   dragger->altDown = event->wasAltDown() ;
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
SoFotis3Dragger::~SoFotis3Dragger()
//
////////////////////////////////////////////////////////////////////////
{
    delete planeProj;
    if (fieldSensor)
        delete fieldSensor;
}

//    detach/attach any sensors, callbacks, and/or field connections.
//    Called by:            start/end of SoBaseKit::readInstance
//    and on new copy by:   start/end of SoBaseKit::copy.
//    Classes that redefine must call setUpConnections(TRUE,TRUE) 
//    at end of constructor.
//    Returns the state of the node when this was called.
SbBool
SoFotis3Dragger::setUpConnections( SbBool onOff, SbBool doItAlways )
{
    if ( !doItAlways && connectionsSetUp == onOff)
	return onOff;

    if ( onOff ) {

	// We connect AFTER base class.
	SoDragger::setUpConnections( onOff, FALSE );

	// Call the sensor CBs to make things are up-to-date.
	fieldSensorCB( this, NULL );

	// Connect the field sensors
	if (fieldSensor->getAttachedField() != &value)
	    fieldSensor->attach( &value );
    }
    else {

	// We disconnect BEFORE base class.

	// Disconnect the field sensors.
	if (fieldSensor->getAttachedField())
	    fieldSensor->detach();

	SoDragger::setUpConnections( onOff, FALSE );
    }

    return !(connectionsSetUp = onOff);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Set up the highlighting, projector, and the initial hit on
//    the dragger
//
// Use: private
//
void
SoFotis3Dragger::dragStart()
//
////////////////////////////////////////////////////////////////////////
{
    // Set the switches to 1...
    setSwitchValue( translatorSwitch.getValue(), 1 );
    setSwitchValue( feedbackSwitch.getValue(), 1 );

    // Establish the projector line in working space.
    // Working space is space at end of motion matrix.
    // translate direction is defined as (1,0,0) in local space.

    startLocalHitPt = getLocalStartingPoint();

    SbLine workSpaceAxis(SbVec3f(0,0,0), SbVec3f(0,0,1)) ;
 
    SbVec3f planeOrigin = workSpaceAxis.getClosestPoint(startLocalHitPt) ;

    planeProj->setPlane(SbPlane(SbVec3f(0,0,1),planeOrigin));

    SoCoordinate3 *c3 = SO_GET_ANY_PART(this, "axesPoints", SoCoordinate3) ;

    SbVec3f normalVec = c3->point[1] / c3->point[1].length() ;

    SbVec3f startVec, endVec ;

    length = c3->point[1].length() ;

    startVec = c3->point[1] ;
    endVec = c3->point[1] ;

    if (shiftDown && !controlDown)  //translate on X AXIS
    {
       startVec[0] += -1 ;
       endVec[0] += 1 ;
       startVec[0] *= 10 ;
       endVec[0] *= 10 ;
    }
    else if (controlDown && !shiftDown)  //translate on Y AXIS
    {
       startVec[1] += -1 ;
       endVec[1] += 1 ;
       startVec[1] *= 10 ;
       endVec[1] *= 10 ;
    }
    else if (controlDown && shiftDown)
    {
       startVec[2] += -1 ;
       endVec[2] += 1 ;
       startVec[2] *= 10 ;
       endVec[2] *= 10 ;
    }

    SbLine line = SbLine(startVec, endVec) ;
    lineProj->setLine(line) ;

    SoTranslation *myPart = SO_GET_ANY_PART(this, "translatorTranslate", SoTranslation) ;

    saveTranslation = myPart->translation.getValue() ;
}



void SoFotis3Dragger::checkLimits(void)
{
   if (value.getValue()[0] < minimum.getValue()[0]) value.setValue(minimum.getValue()[0],  value.getValue()[1],value.getValue()[2]) ;
   if (value.getValue()[1] < minimum.getValue()[1]) value.setValue(  value.getValue()[0],minimum.getValue()[1],value.getValue()[2]) ;
   if (value.getValue()[2] < minimum.getValue()[2]) value.setValue(  value.getValue()[0],  value.getValue()[1],minimum.getValue()[2]) ;

   if (value.getValue()[0] > maximum.getValue()[0]) value.setValue(maximum.getValue()[0],  value.getValue()[1],value.getValue()[2]) ;
   if (value.getValue()[1] > maximum.getValue()[1]) value.setValue(  value.getValue()[0],maximum.getValue()[1],value.getValue()[2]) ;
   if (value.getValue()[2] > maximum.getValue()[2]) value.setValue(  value.getValue()[0],  value.getValue()[1],maximum.getValue()[2]) ;

}



void SoFotis3Dragger::printInformation(void) 
{
     char  *moduleName, *portName, *moduleData ;
     char buf[1024];
     char finalString[1024] ;
//     int sock;
//     struct sockaddr_un server;

     SoText2 *myText = SO_GET_ANY_PART(this, "intNameText", SoText2) ;
    
     sprintf(buf,"%s:%f %f %f",myText->string[0].getString(),    	value.getValue()[0],value.getValue()[1],value.getValue()[2]) ;


       moduleName = strtok(buf,":") ;
       portName = strtok(NULL,":") ;
       moduleData = strtok(NULL,"\n") ;

       strcpy(finalString,"(set-param \"") ;
       strcat(finalString,moduleName) ;
       strcat(finalString,"\" \"") ;
       strcat(finalString,portName) ;
       strcat(finalString,"\" \"") ;
       strcat(finalString,moduleData) ;
       strcat(finalString,"\")") ;

     printf("%s\n",finalString) ;

     cxScriptCommand(finalString) ;


//     /* Create socket */
/*     sock = socket(AF_UNIX, SOCK_STREAM, 0);
     if (sock < 0) {
          perror("opening stream socket");
          exit(1);
     }
*/
     /* Connect socket using name specified by command line. */
/*     server.sun_family = AF_UNIX;
     strcpy(server.sun_path, "INTERACTION_SOCKET");

     if (connect(sock, &server, sizeof(struct sockaddr_un)) < 0) {
          close(sock);
          perror("connecting stream socket");
          return ;
     }

     if (::write(sock, buf, strlen(buf)) < 0)
          perror("writing on stream socket");
     else
     {   printf("Send...\n") ; close(sock) ; }
*/
}


void SoFotis3Dragger::setTheName(char* name) 
{
    SoText2 *myText = SO_GET_ANY_PART(this, "intNameText", SoText2) ;
    myText->string = name ;
}

void SoFotis3Dragger::updateText(void) 
{
    //UPDATE THE TEXT NODE
    char buffer[50] ;

    SoText2 *myText = SO_GET_ANY_PART(this, "axesXText", SoText2) ;
    sprintf(buffer,"%f", value.getValue()[0]) ;
    myText->string = buffer ;

    myText = SO_GET_ANY_PART(this, "axesYText", SoText2) ;
    sprintf(buffer,"%f", value.getValue()[1]) ;
    myText->string = buffer ;

    myText = SO_GET_ANY_PART(this, "axesZText", SoText2) ;
    sprintf(buffer,"%f", value.getValue()[2]) ;
    myText->string = buffer ;
}

void SoFotis3Dragger::updateAxes(void)
{
   SoCoordinate3 * myC = SO_GET_ANY_PART(this, "axesPoints", SoCoordinate3) ;

   float newCoord[3] ;
   newCoord[0] = value.getValue()[0] ;
   newCoord[1] = value.getValue()[1] ;
   newCoord[2] = value.getValue()[2] ;

   myC->point.set1Value(1, newCoord) ;

    //UPDATE THE X AXIS

    SoCube *myCube = SO_GET_ANY_PART(this, "axesXCube", SoCube) ; 
    myCube->width = value.getValue()[0] ;

    SoTranslation *myTrans = SO_GET_ANY_PART(this, "axesXTrans", SoTranslation) ; 
    myTrans->translation.setValue(value.getValue()[0]/2,0.0,0.0) ;

    SoTranslation *trans = SO_GET_ANY_PART(this, "axesXTextTrans", SoTranslation) ;
    trans->translation.setValue(value.getValue()[0]/2 + 0.1, .0, .0) ;


    myCube = SO_GET_ANY_PART(this, "axesYCube", SoCube) ; 
    myCube->height = value.getValue()[1] ;

    myTrans = SO_GET_ANY_PART(this, "axesYTrans", SoTranslation) ; 
    myTrans->translation.setValue(0.0, value.getValue()[1]/2,0.0) ;

    trans = SO_GET_ANY_PART(this, "axesYTextTrans", SoTranslation) ;
    trans->translation.setValue(.0, value.getValue()[1]/2 + 0.1, .0) ;

    myCube = SO_GET_ANY_PART(this, "axesZCube", SoCube) ; 
    myCube->depth = value.getValue()[2] ;

    myTrans = SO_GET_ANY_PART(this, "axesZTrans", SoTranslation) ; 
    myTrans->translation.setValue(0.0, 0.0, value.getValue()[2]/2) ;

    trans = SO_GET_ANY_PART(this, "axesZTextTrans", SoTranslation) ;
    trans->translation.setValue(.0, .0, value.getValue()[2]/2 + 0.1) ;
}



////////////////////////////////////////////////////////////////////////
//
// Description:
//    Stretch the dragger according to the motion along the line
//    projector
//
// Use: private
//
void
SoFotis3Dragger::drag()
//
////////////////////////////////////////////////////////////////////////
{
   SbVec3f currentValue ;

    // Set up the projector space and view.
    // Working space is space at end of motion matrix.


    SbVec3f newHitPt ;


    if ((shiftDown && !controlDown) || (!shiftDown && controlDown) || (shiftDown && controlDown) )
    {
       lineProj->setViewVolume( getViewVolume() );    
       lineProj->setWorkingSpace( getLocalToWorldMatrix() );

       newHitPt = lineProj->project( getNormalizedLocaterPosition()); 

       if (newHitPt[0] <= maximum.getValue()[0] && newHitPt[0] >= minimum.getValue()[0] && 
           newHitPt[1] <= maximum.getValue()[1] && newHitPt[1] >= minimum.getValue()[1] &&
           newHitPt[2] <= maximum.getValue()[2] && newHitPt[2] >= minimum.getValue()[2] )
       {
          SoTranslation *myPart = SO_GET_ANY_PART(this, "translatorTranslate", SoTranslation) ;
          myPart->translation.setValue(newHitPt[0],newHitPt[1],newHitPt[2]) ;
       }

       
    }
/*    else if (controlDown && !shiftDown)
    {
       planeProj->setViewVolume( getViewVolume() );    
       planeProj->setWorkingSpace( getLocalToWorldMatrix() );

       newHitPt = planeProj->project( getNormalizedLocaterPosition()); 

       SoCoordinate3 *c3 = SO_GET_ANY_PART(this, "axesPoints", SoCoordinate3) ;

       SbVec3f unitVec = newHitPt/newHitPt.length() ;

       SbVec3f newCoord = unitVec*length ;

       if (newCoord[0] <= maximum.getValue()[0] && newCoord[0] >= minimum.getValue()[0] && 
           newCoord[1] <= maximum.getValue()[1] && newCoord[1] >= minimum.getValue()[1])
       {
          SoTranslation *myPart = SO_GET_ANY_PART(this, "translatorTranslate", SoTranslation) ;
          myPart->translation.setValue(newCoord[0],newCoord[1],0.0) ;
       }
    }
*/    else
    {	
       planeProj->setViewVolume( getViewVolume() );    
       planeProj->setWorkingSpace( getLocalToWorldMatrix() );

       newHitPt = planeProj->project( getNormalizedLocaterPosition()); 
       // Figure out the translation relative to start position.
       SbVec3f motion = newHitPt - startLocalHitPt;

       currentValue = saveTranslation  ;

       if (currentValue[0] <= maximum.getValue()[0] && currentValue[0] >= minimum.getValue()[0] && 
           currentValue[1] <= maximum.getValue()[1] && currentValue[1] >= minimum.getValue()[1])
       {
       SoTranslation *myPart = SO_GET_ANY_PART(this, "translatorTranslate", SoTranslation) ;
       
       currentValue[0] = saveTranslation[0]+motion[0] ;
       if (currentValue[0] >= maximum.getValue()[0] ) currentValue[0] = maximum.getValue()[0] ;
       if (currentValue[0] <= minimum.getValue()[0] ) currentValue[0] = minimum.getValue()[0] ;

       currentValue[1] = saveTranslation[1]+motion[1] ;
       if (currentValue[1] >= maximum.getValue()[1] ) currentValue[1] = maximum.getValue()[1] ;
       if (currentValue[1] <= minimum.getValue()[1] ) currentValue[1] = minimum.getValue()[1] ;

       myPart->translation.setValue(currentValue[0],currentValue[1],currentValue[2]) ;

//       myPart->translation.setValue(currentValue[0],myPart->translation.getValue()[1] ,0.0) ;


       }
   }

       // fake a motion matrix change :)
       valueChangedCB(NULL, this);
   updateText() ;
   updateAxes() ;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//
// Use: private
//
void
SoFotis3Dragger::dragFinish()
//
////////////////////////////////////////////////////////////////////////
{
    // Set the switches to 0...
    setSwitchValue( translatorSwitch.getValue(), 0 );
    setSwitchValue( feedbackSwitch.getValue(), 0 );

    printInformation() ;
}    

////////////////////////////////////////////////////////////////////
//  Stubs for callbacks
////////////////////////////////////////////////////////////////////
void
SoFotis3Dragger::startCB( void *, SoDragger *inDragger )
{
    SoFotis3Dragger *m = (SoFotis3Dragger *) inDragger;
    m->dragStart();
}

void
SoFotis3Dragger::motionCB( void *, SoDragger *inDragger )
{
    SoFotis3Dragger *m = (SoFotis3Dragger *) inDragger;
    m->drag();
}

void
SoFotis3Dragger::finishCB( void *, SoDragger *inDragger )
{
    SoFotis3Dragger *m = (SoFotis3Dragger *) inDragger;
    m->dragFinish();
}

void
SoFotis3Dragger::valueChangedCB( void *, SoDragger *inDragger )
{
    SoFotis3Dragger *m = (SoFotis3Dragger *) inDragger;
    //SbMatrix motMat = m->getMotionMatrix();
    
    SoTranslation *myPart = SO_GET_ANY_PART(m, "translatorTranslate", SoTranslation) ;
    SbVec3f trans = myPart->translation.getValue();

    //SbVec3f    trans, scale;
    //SbRotation rot, scaleOrient;
    //getTransformFast( motMat, trans, rot, scale, scaleOrient );

    // Disconnect the field sensor
    m->fieldSensor->detach();

    if (( m->value.getValue()[0] != trans[0] ) || (m->value.getValue()[1] != trans[1] ) || (m->value.getValue()[2] != trans[2] ))
    {
	m->value.setValue(trans[0], trans[1], trans[2]) ;
    }

    // Reconnect the field sensor
    m->fieldSensor->attach( &(m->value) );
}

void
SoFotis3Dragger::fieldSensorCB( void *inDragger, SoSensor *)
{
    SoFotis3Dragger *dragger = (SoFotis3Dragger *) inDragger;

    // Incorporate the new field value into the matrix...
//    SbMatrix motMat = dragger->getMotionMatrix();
//    dragger->workFieldsIntoTransform( motMat );

//    dragger->setMotionMatrix( motMat );

    SoTranslation *myPart = SO_GET_ANY_PART(dragger, "translatorTranslate", SoTranslation) ;
    myPart->translation.setValue(dragger->value.getValue()[0],dragger->value.getValue()[1],dragger->value.getValue()[2]) ;
    dragger->checkLimits() ;
    dragger->updateText() ;
    dragger->updateAxes() ;
}
