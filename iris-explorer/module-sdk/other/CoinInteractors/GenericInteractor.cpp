#include "stdafx.h"
#include "GenericInteractor.h"

SO_KIT_SOURCE(GenericInteractor) ;

void GenericInteractor::initClass()
{  
   	SO_KIT_INIT_CLASS(GenericInteractor, SoDragger, "Dragger") ;
}

GenericInteractor::GenericInteractor()
{
    shiftDown = 0 ;
    controlDown = 0 ;
    altDown = 0 ;

    SO_KIT_CONSTRUCTOR(GenericInteractor);

    SO_KIT_ADD_CATALOG_ENTRY(topSeparator, SoSeparator, TRUE, geomSeparator,\x0 ,TRUE);

    SO_KIT_ADD_FIELD(geometryString,(""));
    SO_KIT_ADD_FIELD(linkName,(""));

    SO_KIT_INIT_INSTANCE();

    // add the callbacks to perform the dragging
    addStartCallback(  &GenericInteractor::startCB );
    addMotionCallback( &GenericInteractor::motionCB );
    addFinishCallback(   &GenericInteractor::finishCB );

    // Updates the translation field when the motionMatrix is set.
    addValueChangedCallback( &GenericInteractor::valueChangedCB );

    addOtherEventCallback(&GenericInteractor::metaKeyChangeCB) ;

    // Updates the motionMatrix when the translation field is set.
    fieldSensor = new SoFieldSensor( &GenericInteractor::fieldSensorCB, this);
    fieldSensor->setPriority( 0 );

    setUpConnections( TRUE, TRUE );
}

void GenericInteractor::metaKeyChangeCB(void *, SoDragger *inDragger)
{
   GenericInteractor *dragger = (GenericInteractor *) inDragger ;
   SoHandleEventAction *ha = dragger->getHandleEventAction() ;

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

void GenericInteractor::processGeometryString(void) 
{
    if (strlen(geometryString.getValue().getString()) > 2)
    {
		SoSeparator *mySep = SO_GET_ANY_PART(this, "topSeparator", SoSeparator) ;
	    
		SoNode *myNode = ::readFromBuffer((char*)geometryString.getValue().getString()) ;

		mySep->removeAllChildren() ;
		mySep->addChild(myNode) ;
    }
}

void GenericInteractor::setGeometryString(void) 
{
	  SoSeparator *mySep = SO_GET_ANY_PART(this, "topSeparator", SoSeparator) ;
      SoOutput output ;    
      char *buffer = (char *)malloc(100000*sizeof(char)) ;
      output.setBuffer(buffer, 100000, NULL) ;
      SoWriteAction *wa = new SoWriteAction((SoOutput*)&output) ;
      wa->apply(mySep) ;
      geometryString.setValue(buffer) ;
}

void GenericInteractor::setGeom(SoNode *inGeom)
{
   SoSeparator *mySep = SO_GET_ANY_PART(this, "topSeparator", SoSeparator) ;
   mySep->addChild(inGeom) ;
   setGeometryString() ;
}

GenericInteractor::~GenericInteractor()
{
    if (fieldSensor) delete fieldSensor;
}

SbBool GenericInteractor::setUpConnections( SbBool onOff, SbBool doItAlways )
{
    if ( !doItAlways && connectionsSetUp == onOff)
	return onOff;

    if ( onOff ) {

	// We connect AFTER base class.
	SoDragger::setUpConnections( onOff, FALSE );

	// Call the sensor CBs to make things are up-to-date.
	fieldSensorCB( this, NULL );

	if (fieldSensor->getAttachedField() != &geometryString)
	    fieldSensor->attach( &geometryString );
    }
    else 
	{
		if (fieldSensor->getAttachedField())
			fieldSensor->detach();
		SoDragger::setUpConnections( onOff, FALSE );
    }

    return !(connectionsSetUp = onOff);
}

void GenericInteractor::dragStart()
{
   initialMouseCoordinates = getLocalStartingPoint() ;
}

void GenericInteractor::printInformation(void) 
{
     /*char  *moduleName, *portName, *moduleData ;
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

     cxScriptCommand(finalString) ;*/
}

void GenericInteractor::setTheName(const char* name) 
{
   linkName.setValue(name) ;
}

void GenericInteractor::updateText(void){}

void GenericInteractor::drag()
{
       valueChangedCB(NULL, this);
}

void GenericInteractor::dragFinish()
{
   finalMouseCoordinates = getLocalStartingPoint() ;
   printInformation() ;
}    

void GenericInteractor::startCB( void *, SoDragger *inDragger )
{
    GenericInteractor *m = (GenericInteractor *) inDragger;
    m->dragStart();
}

void GenericInteractor::motionCB( void *, SoDragger *inDragger )
{
    GenericInteractor *m = (GenericInteractor *) inDragger;
    m->drag();
}

void GenericInteractor::finishCB( void *, SoDragger *inDragger )
{
    GenericInteractor *m = (GenericInteractor *) inDragger;
    m->dragFinish();
}

void GenericInteractor::valueChangedCB( void *, SoDragger *inDragger ){}

void GenericInteractor::fieldSensorCB( void *inDragger, SoSensor *)
{
    GenericInteractor *dragger = (GenericInteractor *) inDragger;
	dragger->processGeometryString() ;
}
