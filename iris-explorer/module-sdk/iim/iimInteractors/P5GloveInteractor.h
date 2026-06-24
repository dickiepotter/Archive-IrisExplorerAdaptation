#include "stdafx.h"
#include "P5\P5dll.h"

class SbDict;
class SbPlaneProjector;
class SbLineProjector;
class SoFieldSensor;

class GloveInteractor: public SoDragger
{
    SO_KIT_HEADER(GloveInteractor);
    SO_KIT_CATALOG_ENTRY_HEADER(axesStatic);
    SO_KIT_CATALOG_ENTRY_HEADER(fontStyle);
    SO_KIT_CATALOG_ENTRY_HEADER(intNameSep);
    SO_KIT_CATALOG_ENTRY_HEADER(intNameMat);
    SO_KIT_CATALOG_ENTRY_HEADER(intNameTrans);
    SO_KIT_CATALOG_ENTRY_HEADER(intNameText);
    SO_KIT_CATALOG_ENTRY_HEADER(axesPoints);
    SO_KIT_CATALOG_ENTRY_HEADER(axesMaterial);
    SO_KIT_CATALOG_ENTRY_HEADER(axesStyle);
    SO_KIT_CATALOG_ENTRY_HEADER(axesLine);
    SO_KIT_CATALOG_ENTRY_HEADER(axesSep);
    SO_KIT_CATALOG_ENTRY_HEADER(axesMat);
    SO_KIT_CATALOG_ENTRY_HEADER(axesPickStyle);
    SO_KIT_CATALOG_ENTRY_HEADER(blobPickStyle);
    SO_KIT_CATALOG_ENTRY_HEADER(axesXSep);
    SO_KIT_CATALOG_ENTRY_HEADER(axesXTrans);
    SO_KIT_CATALOG_ENTRY_HEADER(axesXCube);
    SO_KIT_CATALOG_ENTRY_HEADER(axesXTextTrans);
    SO_KIT_CATALOG_ENTRY_HEADER(axesXTextMat);
    SO_KIT_CATALOG_ENTRY_HEADER(axesXText);
    SO_KIT_CATALOG_ENTRY_HEADER(axesYSep);
    SO_KIT_CATALOG_ENTRY_HEADER(axesYTrans);
    SO_KIT_CATALOG_ENTRY_HEADER(axesYCube);
    SO_KIT_CATALOG_ENTRY_HEADER(axesYTextTrans);
    SO_KIT_CATALOG_ENTRY_HEADER(axesYTextMat);
    SO_KIT_CATALOG_ENTRY_HEADER(axesYText);
    SO_KIT_CATALOG_ENTRY_HEADER(axesZSep);
    SO_KIT_CATALOG_ENTRY_HEADER(axesZTrans);
    SO_KIT_CATALOG_ENTRY_HEADER(axesZCube);
    SO_KIT_CATALOG_ENTRY_HEADER(axesZTextTrans);
    SO_KIT_CATALOG_ENTRY_HEADER(axesZTextMat);
    SO_KIT_CATALOG_ENTRY_HEADER(axesZText);
    SO_KIT_CATALOG_ENTRY_HEADER(translatorTranslate);
    SO_KIT_CATALOG_ENTRY_HEADER(translatorSwitch);
    SO_KIT_CATALOG_ENTRY_HEADER(translator);
    SO_KIT_CATALOG_ENTRY_HEADER(translatorActive);
    SO_KIT_CATALOG_ENTRY_HEADER(feedbackSwitch);
    SO_KIT_CATALOG_ENTRY_HEADER(feedback);
    SO_KIT_CATALOG_ENTRY_HEADER(feedbackActive);

  public:
    GloveInteractor();
	static void initClass();
	SoSFVec3f value;
	SoSFVec3f minimum ;
    SoSFVec3f maximum ;
    
    void setTheName(char * name) ;
    void updateText(void) ;
    void updateAxes(void) ;
    void checkLimits(void) ;
	CP5DLL *glove;

  protected:
    SbPlaneProjector *planeProj;
    SbLineProjector *lineProj;
	virtual ~GloveInteractor();
	SbVec3f saveTranslation;
    static void startCB( void *, SoDragger * );
    static void motionCB( void *, SoDragger * );
    static void finishCB( void *, SoDragger * );
    SoFieldSensor *fieldSensor;
	SoTimerSensor *glovepol;
	static void TimerSensorCB( void *, SoSensor *);
    static void fieldSensorCB( void *, SoSensor * );
    static void valueChangedCB( void *, SoDragger * );
    static void metaKeyChangeCB(void *, SoDragger *) ;
    void dragStart();
    void drag();
    void dragFinish();
    virtual SbBool setUpConnections( SbBool onOff, SbBool doItAlways = FALSE );

  private:
    void printInformation(void) ;
    bool shiftDown ;
    bool controlDown ;
    bool altDown ;
    SbVec3f startLocalHitPt ;
    float length ;
	static const char geomBuffer[];
};
