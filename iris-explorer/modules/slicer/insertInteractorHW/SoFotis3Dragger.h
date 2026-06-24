#ifndef  _SO_FOTIS_3_DRAGGER_
#define  _SO_FOTIS_3_DRAGGER_

#define temp __IvStaticImport
#undef __IvStaticImport
#define __IvStaticImport 


#include <Inventor/SbLinear.h>
#include <Inventor/draggers/SoDragger.h>
#include <Inventor/fields/SoSFVec3f.h>
#include <Inventor/fields/SoSFVec2f.h>
#include <Inventor/sensors/SoSensor.h>

class SbDict;
class SbPlaneProjector;
class SbLineProjector;
class SoFieldSensor;

// C-api: prefix=SoXlate1Drag
// C-api: public=translation
class SoFotis3Dragger : public SoDragger
{

    SO_KIT_HEADER(SoFotis3Dragger);

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

    SbVec3f saveTranslation; //THIS USED IN dragStart to save the translation

  public:
  
    SoSFVec3f value;

    // Constructors
    SoFotis3Dragger();

    SoSFVec3f minimum ;
    SoSFVec3f maximum ;

    static void initClass() ;

    void setTheName(char * name) ;
    void updateText(void) ;
    void updateAxes(void) ;
    void checkLimits(void) ; //WHEN THE VALUE IS UPDATED FROM ANOTHER APPLICATION

  protected:

    SbPlaneProjector *planeProj; // projector for planar movement
    SbLineProjector *lineProj; // projector for planar movement

    static void startCB( void *, SoDragger * );
    static void motionCB( void *, SoDragger * );
    static void finishCB( void *, SoDragger * );

    SoFieldSensor *fieldSensor;

    static void fieldSensorCB( void *, SoSensor * );
    static void valueChangedCB( void *, SoDragger * );

    static void metaKeyChangeCB(void *, SoDragger *) ;

    void dragStart();
    void drag();
    void dragFinish();

    // detach/attach any sensors, callbacks, and/or field connections.
    // Called by:            start/end of SoBaseKit::readInstance
    // and on new copy by:   start/end of SoBaseKit::copy.
    // Classes that redefine must call setUpConnections(TRUE,TRUE) 
    // at end of constructor.
    // Returns the state of the node when this was called.
    virtual SbBool setUpConnections( SbBool onOff, SbBool doItAlways = FALSE );

    virtual ~SoFotis3Dragger();

  private:
    static const char geomBuffer[];

    void printInformation(void) ;

    bool shiftDown ;
    bool controlDown ;
    bool altDown ;

    SbVec3f startLocalHitPt ;
    float length ;
};    

#undef __IvStaticImport
#define __IvStaticImport temp
#undef temp


#endif  /* _SO_FOTIS_1_DRAGGER_ */
