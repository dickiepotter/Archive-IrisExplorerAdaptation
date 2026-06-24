#include "stdafx.h"
#include <MouseInteractor.h>
#include <P5GloveInteractor.h>

extern "C" 
{
	void mainFunc ( void  );
	void initHook ( void  );
}

void initHook ( void  )
{
   cxGeoInit();
   //MouseInteractor::initClass() ;
   GloveInteractor::initClass() ;
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

   SoSeparator *root ;

   geomOutPort= cxOutputPortOpen("geometry") ;
   dataInPort = cxInputPortOpen("input") ;

   if (cxInputDataChanged(dataInPort))
   {
      string = cxParamStrGet((cxParameter *) cxInputDataGet(dataInPort)) ;

      if (string != NULL)
      {
      /* "name:port posx posy posz minvx vx maxvx minvy vy maxvy minvz vz maxvz"  */

         tmp = strtok(string," ") ;
         strncpy(nameport, tmp, 50);  // get into static storage 


         if(strstr(tmp,"vector") == NULL) 
		 {
             cxModAlert("Unsupported interactor");
			// Console::WriteLine("Main function called with invalid interactor request"); 
             return;
         }                            
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

         // free the string allocated by cxParamStrGet 
        // free((void *)string); //REMOVED DUE TO ERRORS IN IRIS - PROBABLE MEMORY LEAK
 
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

         //MouseInteractor *myDragger = new MouseInteractor ;
		 GloveInteractor *myDragger = new GloveInteractor ;
       
         myDragger->setTheName(nameport) ;
         myDragger->minimum.setValue(minvx,minvy,minvz) ;
         myDragger->maximum.setValue(maxvx,maxvy,maxvz) ;
         myDragger->value.setValue(vx,vy,vz) ;

         root->addChild(myDragger) ;

         cxGeoInventorDefine(root) ;
         root->unref() ;
         cxGeoBufferClose(outGeom) ;

         cxOutputDataSet(geomOutPort, outGeom) ;
		// Console::WriteLine("Main function called and new dragger added"); 
       }
   }
}

