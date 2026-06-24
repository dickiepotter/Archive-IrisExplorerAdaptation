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


#include <Inventor/actions/SoWriteAction.h>
#include <cx/UI.h>

#include "SoFotisGeneric.h"

SO_KIT_SOURCE(SoFotisGeneric) ;

/*    SoType		 SoFotisGeneric::classTypeId;				      
    SoFieldData	       *SoFotisGeneric::fieldData;				      
    const SoFieldData **SoFotisGeneric::parentFieldData;			      
    SbBool		SoFotisGeneric::firstInstance = TRUE ;

    SO__NODE_METHODS(SoFotisGeneric) ;
*/

void SoFotisGeneric::initClass()
{
//    static int once=0;
//    if (!once) {   
   	SO_KIT_INIT_CLASS(SoFotisGeneric, SoDragger, "Dragger") ;
//        once=1;
//    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
SoFotisGeneric::SoFotisGeneric()
//
////////////////////////////////////////////////////////////////////////
{
//    initClass();

    shiftDown = 0 ;
    controlDown = 0 ;
    altDown = 0 ;


    SO_KIT_CONSTRUCTOR(SoFotisGeneric);

    // Put this stuff under the geomSeparator so it will draw more
    // efficiently.

    SO_KIT_ADD_CATALOG_ENTRY(topSeparator, SoSeparator, TRUE, geomSeparator,\x0 ,TRUE);

    SO_KIT_ADD_FIELD(geometryString,(""));
    SO_KIT_ADD_FIELD(linkName,(""));

    // read geometry for shared parts
//    if (SO_KIT_IS_FIRST_INSTANCE())
//	readDefaultParts("fotis3Dragger.iv", geomBuffer, sizeof(geomBuffer) );

    SO_KIT_INIT_INSTANCE();

    // add the callbacks to perform the dragging
    addStartCallback(  &SoFotisGeneric::startCB );
    addMotionCallback( &SoFotisGeneric::motionCB );
    addFinishCallback(   &SoFotisGeneric::finishCB );

    // Updates the translation field when the motionMatrix is set.
    addValueChangedCallback( &SoFotisGeneric::valueChangedCB );

    addOtherEventCallback(&SoFotisGeneric::metaKeyChangeCB) ;

    // Updates the motionMatrix when the translation field is set.
    fieldSensor = new SoFieldSensor( &SoFotisGeneric::fieldSensorCB, this);
    fieldSensor->setPriority( 0 );

    setUpConnections( TRUE, TRUE );
}

void SoFotisGeneric::metaKeyChangeCB(void *, SoDragger *inDragger)
{
   SoFotisGeneric *dragger = (SoFotisGeneric *) inDragger ;

   SoHandleEventAction *ha = dragger->getHandleEventAction() ;

   //check for the SHIFT key

   const SoEvent *event = dragger->getEvent() ;
   dragger->shiftDown = event->wasShiftDown() ;
   dragger->controlDown = event->wasCtrlDown() ;
   dragger->altDown = event->wasAltDown() ;
}


SoNode * readFromBuffer(char * string)
{
   // Read from the string.
   SoInput in;
   in.setBuffer(string, strlen(string));

   SoNode *result = new SoSeparator ;
   SoDB::read(&in, result);

   return result;
}


void SoFotisGeneric::processGeometryString(void) 
{
    if (strlen(geometryString.getValue().getString()) > 2)
    {

    SoSeparator *mySep = SO_GET_ANY_PART(this, "topSeparator", SoSeparator) ;
    
    SoNode *myNode = ::readFromBuffer((char*)geometryString.getValue().getString()) ;

    mySep->removeAllChildren() ;
    mySep->addChild(myNode) ;
    }
}

void SoFotisGeneric::setGeometryString(void) 
{
    SoSeparator *mySep = SO_GET_ANY_PART(this, "topSeparator", SoSeparator) ;

      SoOutput output ;    

      char *buffer = (char *)malloc(100000*sizeof(char)) ;

      output.setBuffer(buffer, 100000, NULL) ;

      SoWriteAction *wa = new SoWriteAction((SoOutput*)&output) ;
      wa->apply(mySep) ;

      geometryString.setValue(buffer) ;

}


void SoFotisGeneric::setGeom(SoNode *inGeom)
{
   SoSeparator *mySep = SO_GET_ANY_PART(this, "topSeparator", SoSeparator) ;

   mySep->addChild(inGeom) ;

   setGeometryString() ;
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
SoFotisGeneric::~SoFotisGeneric()
//
////////////////////////////////////////////////////////////////////////
{
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
SoFotisGeneric::setUpConnections( SbBool onOff, SbBool doItAlways )
{
    if ( !doItAlways && connectionsSetUp == onOff)
	return onOff;

    if ( onOff ) {

	// We connect AFTER base class.
	SoDragger::setUpConnections( onOff, FALSE );

	// Call the sensor CBs to make things are up-to-date.
	fieldSensorCB( this, NULL );

	// Connect the field sensors
	if (fieldSensor->getAttachedField() != &geometryString)
	    fieldSensor->attach( &geometryString );
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
SoFotisGeneric::dragStart()
//
////////////////////////////////////////////////////////////////////////
{
    // Set the switches to 1...
//    setSwitchValue( translatorSwitch.getValue(), 1 );
//    setSwitchValue( feedbackSwitch.getValue(), 1 );

    // Establish the projector line in working space.
    // Working space is space at end of motion matrix.
    // translate direction is defined as (1,0,0) in local space.

   initialMouseCoordinates = getLocalStartingPoint() ;
//   printf("IMC::%f %f %f\n",initialMouseCoordinates[0],initialMouseCoordinates[1],initialMouseCoordinates[2]) ;
}



void SoFotisGeneric::printInformation(void) 
{
     char  *moduleName, *portName, *moduleData ;
     char buf[1024], buf2[1024];
     char finalString[1024] ;
     int s, c, a ;

     if (shiftDown) s = 1 ;
     if (controlDown) c = 1 ;
     if (altDown) a = 1 ;
   
     sprintf(buf,"%s",linkName.getValue().getString()) ;

     moduleName = strtok(buf,":") ;
     portName = strtok(NULL,":") ;
     moduleData = strtok(NULL,":") ;

	 if (moduleData == NULL) moduleData = "" ;

     strcpy(finalString,"(set-param \"") ;
     strcat(finalString,moduleName) ;
     strcat(finalString,"\" \"") ;
     strcat(finalString,portName) ;
     strcat(finalString,"\" \"") ;

     sprintf(buf2,":S=%d<>C=%d<>A=%d:%f<>%f:",s,c,a,finalMouseCoordinates[0],finalMouseCoordinates[1]) ;
     strcat(finalString,buf2) ;
     strcat(finalString,moduleData) ;
     strcat(finalString,"\")") ;

     printf("->>>%s\n",finalString) ;

     cxScriptCommand(finalString) ;
}


void SoFotisGeneric::setTheName(const char* name) 
{
   linkName.setValue(name) ;
}

void SoFotisGeneric::updateText(void) 
{
/*    //UPDATE THE TEXT NODE
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
*/}



////////////////////////////////////////////////////////////////////////
//
// Description:
//    Stretch the dragger according to the motion along the line
//    projector
//
// Use: private
//
void
SoFotisGeneric::drag()
//
////////////////////////////////////////////////////////////////////////
{
       valueChangedCB(NULL, this);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//
// Use: private
//
void
SoFotisGeneric::dragFinish()
//
////////////////////////////////////////////////////////////////////////
{
    // Set the switches to 0...
//    setSwitchValue( translatorSwitch.getValue(), 0 );
//    setSwitchValue( feedbackSwitch.getValue(), 0 );

   finalMouseCoordinates = getLocalStartingPoint() ;

   printInformation() ;
}    

////////////////////////////////////////////////////////////////////
//  Stubs for callbacks
////////////////////////////////////////////////////////////////////
void
SoFotisGeneric::startCB( void *, SoDragger *inDragger )
{
    SoFotisGeneric *m = (SoFotisGeneric *) inDragger;
    m->dragStart();
}

void
SoFotisGeneric::motionCB( void *, SoDragger *inDragger )
{
    SoFotisGeneric *m = (SoFotisGeneric *) inDragger;
    m->drag();
}

void
SoFotisGeneric::finishCB( void *, SoDragger *inDragger )
{
    SoFotisGeneric *m = (SoFotisGeneric *) inDragger;
    m->dragFinish();
}

void
SoFotisGeneric::valueChangedCB( void *, SoDragger *inDragger )
{
/*
    SoFotisGeneric *m = (SoFotisGeneric *) inDragger;
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
*/}

void
SoFotisGeneric::fieldSensorCB( void *inDragger, SoSensor *)
{
    SoFotisGeneric *dragger = (SoFotisGeneric *) inDragger;

dragger->processGeometryString() ;

    // Incorporate the new field value into the matrix...
//    SbMatrix motMat = dragger->getMotionMatrix();
//    dragger->workFieldsIntoTransform( motMat );

//    dragger->setMotionMatrix( motMat );

/*    SoTranslation *myPart = SO_GET_ANY_PART(dragger, "translatorTranslate", SoTranslation) ;
    myPart->translation.setValue(dragger->value.getValue()[0],dragger->value.getValue()[1],dragger->value.getValue()[2]) ;
    dragger->checkLimits() ;
    dragger->updateText() ;
    dragger->updateAxes() ;*/
}
