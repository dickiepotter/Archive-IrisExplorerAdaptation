#include <cx/cxParameter.api.h>
#include <cx/DataAccess.h>
#include <cx/DataTypes.h>
#include <cx/DataOps.h>
#include <cx/Geometry.h>
#include <cx/PortAccess.h>
#include <cx/UI.h>





#include <Inventor/nodes/SoInfo.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoTransform.h>

#include <Inventor/draggers/SoTranslate1Dragger.h>


#include "SoFotis3Dragger.h"

#ifdef __cplusplus
	extern "C" {
#endif

void mainFunc ( void  );
void initHook ( void  );

#ifdef __cplusplus
}
#endif



void initHook ( void  )
{

   cxGeoInit();

   SoFotis3Dragger::initClass() ;

}


void mainFunc ( void  )
{

   int dataInPort, geomOutPort ;
  
   cxGeometry *outGeom ;

   char *string, *tmp ;
   static char nameport[50] ;
   static float vx, vy, vz ;
   static float minvx, minvy, minvz ;
   static float maxvx, maxvy, maxvz ;
   static float posx, posy, posz ;
   static int mode;

   SoSeparator *root ;

   geomOutPort= cxOutputPortOpen("geometry") ;

   dataInPort = cxInputPortOpen("input") ;

   if (cxInputDataChanged(dataInPort))
   {
      string = cxParamStrGet((cxParameter *) cxInputDataGet(dataInPort)) ;

      if (string != NULL)
      {
      /* "name:port posx posy posz minvx vx maxvx minvy vy maxvy minvz vz maxvz" */

         tmp = strtok(string," ") ;
         strncpy(nameport, tmp, 50);  /* get into static storage */

         if(strstr(tmp,"vector") == NULL) {
             cxModAlert("Unsupported interactor");
             return;
         }                             /* continue tokenising */
         posx = (float) atof(strtok(NULL," ")) ;
         posy = (float) atof(strtok(NULL," ")) ;
         posz = (float) atof(strtok(NULL," ")) ;
         minvx = (float) atof(strtok(NULL," ")) ;
         vx = (float) atof(strtok(NULL," ")) ;
         maxvx = (float) atof(strtok(NULL," ")) ;
         minvy = (float) atof(strtok(NULL," ")) ;
         vy = (float) atof(strtok(NULL," ")) ;
         maxvy = (float) atof(strtok(NULL," ")) ;
         minvz = (float) atof(strtok(NULL," ")) ;
         vz = (float) atof(strtok(NULL," ")) ;
         maxvz = (float) atof(strtok(NULL," ")) ;
		 mode = (int) atof(strtok(NULL," "));

         /* free the string allocated by cxParamStrGet */
         free((void *)string);
 
         outGeom = cxGeoNew() ;
         cxGeoBufferSelect(outGeom) ;

         cxGeoBufferPortSet(geomOutPort) ;

         cxGeoRoot() ;
         cxGeoDelete() ;

         root = new SoSeparator ;
         root->ref() ;  

         SoTransform *myTrans = new SoTransform ;

         myTrans->translation.setValue(posx,posy,posz) ;
  
         root->addChild(myTrans) ;

         SoFotis3Dragger *myDragger = new SoFotis3Dragger ;
       
         myDragger->setTheName(nameport) ;
         myDragger->minimum.setValue(minvx,minvy,minvz) ;
         myDragger->maximum.setValue(maxvx,maxvy,maxvz) ;
         myDragger->value.setValue(vx,vy,vz) ;
		 if (mode == -1)  /* mode -1 is a non-interactive placeholder t */
			myDragger->once.setValue(0) ;
		 else
			myDragger->once.setValue(1) ;

         myDragger->mode.setValue(mode) ;

         root->addChild(myDragger) ;

         cxGeoInventorDefine(root) ;
         root->unref() ;
         cxGeoBufferClose(outGeom) ;

         cxOutputDataSet(geomOutPort, outGeom) ;
       }
   }






}

