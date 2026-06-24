#ifndef  _SO_FOTIS_GENERIC_
#define  _SO_FOTIS_GENERIC_

#define temp __IvStaticImport
#undef __IvStaticImport
#define __IvStaticImport 


#include <Inventor/SbLinear.h>
#include <Inventor/draggers/SoDragger.h>
#include <Inventor/fields/SoSFVec3f.h>
#include <Inventor/fields/SoSFVec2f.h>
#include <Inventor/sensors/SoSensor.h>
#include <Inventor/fields/SoSFString.h>

class SbDict;
class SbPlaneProjector;
class SbLineProjector;
class SoFieldSensor;

// C-api: prefix=SoXlate1Drag
// C-api: public=translation
class SoFotisGeneric : public SoDragger
{

  public:								      
    static SoType	getClassTypeId();	/* Returns class type id */   
    virtual SoType	getTypeId() const;	/* Returns type id	*/    
  protected:								      
    virtual const SoFieldData	*getFieldData() const;			      
    static const SoFieldData **getFieldDataPtr();			      
  private:								      
    static __IvStaticImport SoType	classTypeId;		/* Type id		*/    \
    static __IvStaticImport SbBool	firstInstance; /* true until 2nd c'tor call */	      \
    static __IvStaticImport SoFieldData		*fieldData;				      \
    static __IvStaticImport const SoFieldData	**parentFieldData  ;
	static void *createInstance() ;
    SO__KIT_CATALOG_HEADER(SoFotisGeneric) ;

    SO_KIT_CATALOG_ENTRY_HEADER(topSeparator);
    SO_KIT_CATALOG_ENTRY_HEADER(intNameText);


  public:

  SoSFString geometryString ;
  SoSFString linkName ;
  
    // Constructors
    SoFotisGeneric();

    static void initClass() ;

    void setTheName(const char * name) ;
    void updateText(void) ;
    void updateAxes(void) ;
    void checkLimits(void) ; //WHEN THE VALUE IS UPDATED FROM ANOTHER APPLICATION
    void setGeom(SoNode *inGeom) ;

  protected:

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

    virtual ~SoFotisGeneric();

  private:

    void processGeometryString() ;
    void setGeometryString() ;
    void printInformation(void) ;

    SbVec3f initialMouseCoordinates ;
    SbVec3f finalMouseCoordinates ;

    bool shiftDown ;
    bool controlDown ;
    bool altDown ;

};    

#undef __IvStaticImport
#define __IvStaticImport temp
#undef temp

#endif  /* _SO_FOTIS_GENERIC_ */
