#include "stdafx.h"

class SbDict;
class SbPlaneProjector;
class SbLineProjector;
class SoFieldSensor;

class GenericInteractor : public SoDragger
{
  public:								      
		static SoType	getClassTypeId();
		virtual SoType	getTypeId() const;

  protected:								      
		virtual const SoFieldData	*getFieldData() const;			      
		static const SoFieldData **getFieldDataPtr();	

  private:								      
		static __IvStaticImport SoType	classTypeId;	
		static __IvStaticImport SbBool	firstInstance; 
		static __IvStaticImport SoFieldData		*fieldData;				    
		static __IvStaticImport const SoFieldData	**parentFieldData  ;
		static void *createInstance() ;

		SO__KIT_CATALOG_HEADER(GenericInteractor) ;
		SO_KIT_CATALOG_ENTRY_HEADER(topSeparator);
		SO_KIT_CATALOG_ENTRY_HEADER(intNameText);

  public:
	  SoSFString geometryString ;
	  SoSFString linkName ;
	  GenericInteractor();
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
    virtual SbBool setUpConnections( SbBool onOff, SbBool doItAlways = FALSE );
    virtual ~GenericInteractor();

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
