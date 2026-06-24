#include "stdafx.h"
#include "MouseInteractor.h"

//using namespace System;

const char MouseInteractor::geomBuffer[] =  
	{
		0x23,0x49,0x6e,0x76,0x65,0x6e,0x74,0x6f,0x72,0x20,0x56,0x32,0x2e,0x31,0x20,
		0x62,0x69,0x6e,0x61,0x72,0x79,0x20,0x20,0xa,0x0,0x0,0x0,0x9,0x53,0x65,
		0x70,0x61,0x72,0x61,0x74,0x6f,0x72,0x0,0x0,0x0,0x0,0x0,0x0,0x2,0x0,
		0x0,0x0,0x0,0x0,0x0,0x0,0x5,0x0,0x0,0x0,0x3,0x44,0x45,0x46,0x0,
		0x0,0x0,0x0,0xc,0x66,0x6f,0x74,0x69,0x73,0x33,0x53,0x74,0x61,0x74,0x69,
		0x63,0x0,0x0,0x0,0x9,0x53,0x65,0x70,0x61,0x72,0x61,0x74,0x6f,0x72,0x0,
		0x0,0x0,0x0,0x0,0x0,0x2,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x3,0x0,
		0x0,0x0,0x8,0x4d,0x61,0x74,0x65,0x72,0x69,0x61,0x6c,0x0,0x0,0x0,0x0,
		0x0,0x0,0x0,0x1,0x0,0x0,0x0,0xc,0x64,0x69,0x66,0x66,0x75,0x73,0x65,
		0x43,0x6f,0x6c,0x6f,0x72,0x0,0x0,0x0,0x1,0x3f,0x80,0x0,0x0,0x0,0x0,
		0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x9,0x50,
		0x69,0x63,0x6b,0x53,0x74,0x79,0x6c,0x65,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
		0x0,0x0,0x0,0x1,0x0,0x0,0x0,0x5,0x73,0x74,0x79,0x6c,0x65,0x0,0x0,
		0x0,0x0,0x0,0x0,0xa,0x55,0x4e,0x50,0x49,0x43,0x4b,0x41,0x42,0x4c,0x45,
		0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x6,0x53,0x70,0x68,0x65,0x72,
		0x65,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x0,0x0,0x0,0x6,
		0x72,0x61,0x64,0x69,0x75,0x73,0x0,0x0,0x3d,0xca,0xc0,0x83,0x0,0x0,0x0,
		0x0,0x0,0x0,0x0,0x3,0x44,0x45,0x46,0x0,0x0,0x0,0x0,0x10,0x66,0x6f,
		0x74,0x69,0x73,0x33,0x54,0x72,0x61,0x6e,0x73,0x6c,0x61,0x74,0x6f,0x72,0x0,
		0x0,0x0,0x9,0x53,0x65,0x70,0x61,0x72,0x61,0x74,0x6f,0x72,0x0,0x0,0x0,
		0x0,0x0,0x0,0x2,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x3,0x0,0x0,0x0,
		0x8,0x54,0x65,0x78,0x74,0x75,0x72,0x65,0x32,0x0,0x0,0x0,0x0,0x0,0x0,
		0x0,0x0,0x0,0x0,0x0,0x8,0x4d,0x61,0x74,0x65,0x72,0x69,0x61,0x6c,0x0,
		0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x0,0x0,0x0,0xc,0x64,0x69,0x66,0x66,
		0x75,0x73,0x65,0x43,0x6f,0x6c,0x6f,0x72,0x0,0x0,0x0,0x1,0x0,0x0,0x0,
		0x0,0x3f,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
		0x0,0x6,0x53,0x70,0x68,0x65,0x72,0x65,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
		0x0,0x0,0x1,0x0,0x0,0x0,0x6,0x72,0x61,0x64,0x69,0x75,0x73,0x0,0x0,
		0x3d,0xca,0xc0,0x83,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x3,0x44,0x45,0x46,
		0x0,0x0,0x0,0x0,0x16,0x66,0x6f,0x74,0x69,0x73,0x33,0x54,0x72,0x61,0x6e,
		0x73,0x6c,0x61,0x74,0x6f,0x72,0x41,0x63,0x74,0x69,0x76,0x65,0x0,0x0,0x0,
		0x0,0x0,0x9,0x53,0x65,0x70,0x61,0x72,0x61,0x74,0x6f,0x72,0x0,0x0,0x0,
		0x0,0x0,0x0,0x2,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x3,0x0,0x0,0x0,
		0x8,0x54,0x65,0x78,0x74,0x75,0x72,0x65,0x32,0x0,0x0,0x0,0x0,0x0,0x0,
		0x0,0x0,0x0,0x0,0x0,0x8,0x4d,0x61,0x74,0x65,0x72,0x69,0x61,0x6c,0x0,
		0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x0,0x0,0x0,0xc,0x64,0x69,0x66,0x66,
		0x75,0x73,0x65,0x43,0x6f,0x6c,0x6f,0x72,0x0,0x0,0x0,0x1,0x3f,0x0,0x0,
		0x0,0x3f,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
		0x0,0x6,0x53,0x70,0x68,0x65,0x72,0x65,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
		0x0,0x0,0x1,0x0,0x0,0x0,0x6,0x72,0x61,0x64,0x69,0x75,0x73,0x0,0x0,
		0x3d,0xca,0xc0,0x83,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x3,0x44,0x45,0x46,
		0x0,0x0,0x0,0x0,0xe,0x66,0x6f,0x74,0x69,0x73,0x31,0x46,0x65,0x65,0x64,
		0x62,0x61,0x63,0x6b,0x0,0x0,0x0,0x0,0x0,0x9,0x53,0x65,0x70,0x61,0x72,
		0x61,0x74,0x6f,0x72,0x0,0x0,0x0,0x0,0x0,0x0,0x2,0x0,0x0,0x0,0x0,
		0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x3,0x44,0x45,0x46,0x0,0x0,0x0,0x0,
		0x14,0x66,0x6f,0x74,0x69,0x73,0x31,0x46,0x65,0x65,0x64,0x62,0x61,0x63,0x6b,
		0x41,0x63,0x74,0x69,0x76,0x65,0x0,0x0,0x0,0x9,0x53,0x65,0x70,0x61,0x72,
		0x61,0x74,0x6f,0x72,0x0,0x0,0x0,0x0,0x0,0x0,0x2,0x0,0x0,0x0,0x0,
		0x0,0x0,0x0,0x0
	};

SO_KIT_SOURCE(MouseInteractor);

void MouseInteractor::initClass()
{ 
	SO_KIT_INIT_CLASS(MouseInteractor, SoDragger, "Dragger") ;
}

MouseInteractor::MouseInteractor()
{
	SO_KIT_CONSTRUCTOR(MouseInteractor);
	
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

	if (SO_KIT_IS_FIRST_INSTANCE())
		readDefaultParts("fotis3Dragger.iv", geomBuffer, sizeof(geomBuffer) );

	SO_KIT_ADD_FIELD(value, (0.0, 0.0, 0.0));
	SO_KIT_ADD_FIELD(minimum, (0.0, 0.0, 0.0));
	SO_KIT_ADD_FIELD(maximum, (0.0, 0.0, 0.0));

	SO_KIT_INIT_INSTANCE();

	setPartAsDefault("axesStatic", "fotis3Static");
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
	axesmat->diffuseColor.setValue(.7f, .7f, .0f) ;
	SoPickStyle *pick = SO_GET_ANY_PART(this, "axesPickStyle", SoPickStyle) ;
	pick->style = SoPickStyle::UNPICKABLE ;
	pick = SO_GET_ANY_PART(this, "blobPickStyle", SoPickStyle) ;
	pick->style = SoPickStyle::SHAPE ;
	SoTranslation *axestrans = SO_GET_ANY_PART(this, "axesXTrans", SoTranslation) ;
	axestrans->translation.setValue(.0, .0, .0) ;
	SoCube *axescube = SO_GET_ANY_PART(this, "axesXCube", SoCube) ;
	axescube->height = 0.05f ;
	axescube->depth = 0.05f ;
	axescube->width = 0.0f ;
	axescube = SO_GET_ANY_PART(this, "axesYCube", SoCube) ;
	axescube->height = 0.0f ;
	axescube->depth = 0.05f ;
	axescube->width = 0.05f ;
	axescube = SO_GET_ANY_PART(this, "axesZCube", SoCube) ;
	axescube->height = 0.05f ;
	axescube->depth = 0.0f ;
	axescube->width = 0.05f ;

	SoTranslation *trans = SO_GET_ANY_PART(this, "axesXTextTrans", SoTranslation) ;
	trans->translation.setValue(.05f, .0f, .0f) ;
	SoMaterial *mat = SO_GET_ANY_PART(this, "axesXTextMat", SoMaterial) ;
	mat->diffuseColor.setValue(1.0f, 1.0f, 1.0f) ;
	trans = SO_GET_ANY_PART(this, "axesYTextTrans", SoTranslation) ;
	trans->translation.setValue(.0f, .05f, .0f) ;
	mat = SO_GET_ANY_PART(this, "axesYTextMat", SoMaterial) ;
	mat->diffuseColor.setValue(1.0f, 1.0f, 1.0f) ;
	trans = SO_GET_ANY_PART(this, "axesZTextTrans", SoTranslation) ;
	trans->translation.setValue(.0f, .0f, .05f) ;
	mat = SO_GET_ANY_PART(this, "axesZTextMat", SoMaterial) ;
	mat->diffuseColor.setValue(1.0f, 1.0f, 1.0f) ;
	trans = SO_GET_ANY_PART(this, "intNameTrans", SoTranslation) ;
	trans->translation.setValue(-.2f, .0f, .0f) ;

	SoText2 *myText = SO_GET_ANY_PART(this, "intNameText", SoText2) ;
	myText->justification = SoText2::RIGHT ;

	setPartAsDefault("translator",      "fotis3Translator");
	setPartAsDefault("translatorActive","fotis3TranslatorActive");
	setPartAsDefault("feedback",        "fotis3Feedback");
	setPartAsDefault("feedbackActive",  "fotis3FeedbackActive");
	setSwitchValue( translatorSwitch.getValue(), 0 );
	setSwitchValue( feedbackSwitch.getValue(), 0 );

	planeProj = new SbPlaneProjector();
	lineProj  = new SbLineProjector() ;

	addStartCallback( &MouseInteractor::startCB );
	addMotionCallback( &MouseInteractor::motionCB );
	addFinishCallback( &MouseInteractor::finishCB );
	
	// Updates the translation field when the motionMatrix is set.
	addValueChangedCallback( &MouseInteractor::valueChangedCB );
	addOtherEventCallback(&MouseInteractor::metaKeyChangeCB) ;
	
	// Updates the motionMatrix when the translation field is set.
	fieldSensor = new SoFieldSensor( &MouseInteractor::fieldSensorCB, this);
	fieldSensor->setPriority( 0 );

	setUpConnections( TRUE, TRUE );
}

MouseInteractor::~MouseInteractor()
{
	delete planeProj;
}


void MouseInteractor::metaKeyChangeCB(void *, SoDragger *inDragger)
{
	MouseInteractor *dragger = (MouseInteractor *) inDragger ;
	SoHandleEventAction *ha = dragger->getHandleEventAction() ;

	const SoEvent *event = dragger->getEvent() ;
	dragger->shiftDown = event->wasShiftDown() ;
	dragger->controlDown = event->wasCtrlDown() ;
	dragger->altDown = event->wasAltDown() ;
}

SbBool MouseInteractor::setUpConnections( SbBool onOff, SbBool doItAlways )
{
	if ( !doItAlways && connectionsSetUp == onOff)
		return onOff;

	if ( onOff ) 
	{
		// We connect AFTER base class.
		SoDragger::setUpConnections( onOff, FALSE );

		// Call the sensor CBs to make things are up-to-date.
		fieldSensorCB( this, NULL );

		// Connect the field sensors
		if (fieldSensor->getAttachedField() != &value)
			fieldSensor->attach( &value );
	}
	else 
	{
		if (fieldSensor->getAttachedField())
			fieldSensor->detach();

		SoDragger::setUpConnections( onOff, FALSE );
	}

	return !(connectionsSetUp = onOff);
}

void MouseInteractor::dragStart()
{
	setSwitchValue( translatorSwitch.getValue(), 1 );
	setSwitchValue( feedbackSwitch.getValue(), 1 );
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

void MouseInteractor::checkLimits(void)
{
	if (value.getValue()[0] < minimum.getValue()[0]) value.setValue(minimum.getValue()[0],  value.getValue()[1],value.getValue()[2]) ;
	if (value.getValue()[1] < minimum.getValue()[1]) value.setValue(  value.getValue()[0],minimum.getValue()[1],value.getValue()[2]) ;
	if (value.getValue()[2] < minimum.getValue()[2]) value.setValue(  value.getValue()[0],  value.getValue()[1],minimum.getValue()[2]) ;

	if (value.getValue()[0] > maximum.getValue()[0]) value.setValue(maximum.getValue()[0],  value.getValue()[1],value.getValue()[2]) ;
	if (value.getValue()[1] > maximum.getValue()[1]) value.setValue(  value.getValue()[0],maximum.getValue()[1],value.getValue()[2]) ;
	if (value.getValue()[2] > maximum.getValue()[2]) value.setValue(  value.getValue()[0],  value.getValue()[1],maximum.getValue()[2]) ;
}

void MouseInteractor::printInformation(void) 
{
	char  *moduleName, *portName, *moduleData ;
	char buf[1024];
	char finalString[1024] ;

	SoText2 *myText = SO_GET_ANY_PART(this, "intNameText", SoText2) ;

	sprintf_s(buf,"%s:%f %f %f",myText->string[0].getString(), value.getValue()[0],value.getValue()[1],value.getValue()[2]) ;

	moduleName = strtok(buf,":") ;
	portName = strtok(NULL,":") ;
	moduleData = strtok(NULL,"\n") ;

	strcpy(finalString,"(set-param \"") ;
	strcat_s(finalString,moduleName) ;
	strcat_s(finalString,"\" \"") ;
	strcat_s(finalString,portName) ;
	strcat_s(finalString,"\" \"") ;
	strcat_s(finalString,moduleData) ;
	strcat_s(finalString,"\")") ;

	cxScriptCommand(finalString) ;
}

void MouseInteractor::setTheName(char* name) 
{
	SoText2 *myText = SO_GET_ANY_PART(this, "intNameText", SoText2) ;
	myText->string = name ;
}

void MouseInteractor::updateText(void) 
{
	char buffer[50] ;

	SoText2 *myText = SO_GET_ANY_PART(this, "axesXText", SoText2) ;
	sprintf_s(buffer,"%f", value.getValue()[0]) ;
	myText->string = buffer ;

	myText = SO_GET_ANY_PART(this, "axesYText", SoText2) ;
	sprintf_s(buffer,"%f", value.getValue()[1]) ;
	myText->string = buffer ;

	myText = SO_GET_ANY_PART(this, "axesZText", SoText2) ;
	sprintf_s(buffer,"%f", value.getValue()[2]) ;
	myText->string = buffer ;
}

void MouseInteractor::updateAxes(void)
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

void MouseInteractor::drag()
{
	SbVec3f currentValue ;
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
	else
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
		}
	}

	valueChangedCB(NULL, this);
	updateText() ;
	updateAxes() ;
}

void MouseInteractor::dragFinish()
{
	setSwitchValue( translatorSwitch.getValue(), 0 );
	setSwitchValue( feedbackSwitch.getValue(), 0 );
	printInformation() ;
}    

void MouseInteractor::startCB( void *, SoDragger *inDragger )
{
	MouseInteractor *m = (MouseInteractor *) inDragger;
	m->dragStart();
}

void MouseInteractor::motionCB( void *, SoDragger *inDragger )
{
	MouseInteractor *m = (MouseInteractor *) inDragger;
	m->drag();
}

void MouseInteractor::finishCB( void *, SoDragger *inDragger )
{
	MouseInteractor *m = (MouseInteractor *) inDragger;
	m->dragFinish();
}

void MouseInteractor::valueChangedCB( void *, SoDragger *inDragger )
{
	MouseInteractor *m = (MouseInteractor *) inDragger;
	SoTranslation *myPart = SO_GET_ANY_PART(m, "translatorTranslate", SoTranslation) ;
	SbVec3f trans = myPart->translation.getValue();
	m->fieldSensor->detach();

	if (( m->value.getValue()[0] != trans[0] ) || (m->value.getValue()[1] != trans[1] ) || (m->value.getValue()[2] != trans[2] ))
	{
		m->value.setValue(trans[0], trans[1], trans[2]) ;
	}

	m->fieldSensor->attach( &(m->value) );
}

void MouseInteractor::fieldSensorCB( void *inDragger, SoSensor *)
{
	MouseInteractor *dragger = (MouseInteractor *) inDragger;
	SoTranslation *myPart = SO_GET_ANY_PART(dragger, "translatorTranslate", SoTranslation) ;
	myPart->translation.setValue(dragger->value.getValue()[0],dragger->value.getValue()[1],dragger->value.getValue()[2]) ;
	dragger->checkLimits() ;
	dragger->updateText() ;
	dragger->updateAxes() ;
}