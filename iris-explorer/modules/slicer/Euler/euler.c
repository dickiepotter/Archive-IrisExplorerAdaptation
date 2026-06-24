/* HW -  borrows heavily from transformgen.c */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <cx/cxParameter.api.h>
#include <cx/DataTypes.h>
#include <cx/DataAccess.h>
#include <cx/UserFuncs.h>
#include <cx/PortAccess.h>
#include <cx/Info.h>
#include <cx/ModuleCommand.h>

#include "trackball.h"

void post_matrix(float vecx, float vecy, float vecz, float * rotation)
{
       long dims[2];
       cxLattice *lattice;
       cxCoord *coord;
       char buffer[200]; 
       float *array;
       int latport;

       /* allocate the lattice */
       dims[0] = dims[1] = 4;
       lattice = cxLatDataNew(
			2,		/* number of dimensions */
			dims,		/* dimensions vector */
			1,		/* number of channels */
			cx_prim_float);	/* element type */
       if( cxDataAllocErrorGet() ) return;
       cxLatPtrGet(lattice,NULL,(void **)&array,NULL,NULL);
       coord = cxCoordDefaultNew( 2, dims );
       if( cxDataAllocErrorGet() ) {
	   cxDataRefDec( lattice );
	   return;
       }
       cxLatPtrSet(lattice, NULL, NULL, coord, NULL);

       build_rotmatrix(array,rotation);

       /* assign to the output port */
       latport = cxOutputPortOpen("Output");
       cxOutputDataSet(latport,lattice);
 
       /* do strings for (hidden) widgets used for checking arithmetic */

       /* always do vector, even if changed from downstream, as */
       /* trackball occasionally has to correct user's finger trouble */
	 sprintf(buffer,"%lf %lf %lf",vecx,vecy,vecz) ;
       cxInWdgtStrSet("vector",buffer) ;

       sprintf(buffer,"##$ %f %f %f %f",rotation[0],rotation[1],
         rotation[2],rotation[3]);
       cxInWdgtStrSet("saved angle",buffer);
}


void euler ( void  )
{
	static int firsttime = 1;
      static float vx,vy,vz;
      float posx,posy,posz;
      float minvx,minvy,minvz;
      float maxvx,maxvy,maxvz;
      static float rotang[4];
      float	delta[4];
      char *string;
	float tempvx, tempvy, tempvz;
      char buffer[200]; 
      int paramport;
      cxParameter *interactor;

	if ( firsttime || cxInputDataChanged(cxInputPortOpen("ResetXY")) ) {

            cxInputDataGet(cxInputPortOpen("ResetXY"));  /* eliminate false +ve later */

            /* set up for a vector pointing along z initially, with limits +/- 1 */
		vx = 0.0;  minvx = -1.0;  maxvx = 1.0;
		vy = 0.0;  minvy = -1.0;  maxvy = 1.0;
		vz = 1.0;  minvz = -1.0;  maxvz = 1.0;

            /* positioned at 0,0,0 */
            posx = 0.0; posy = 0.0; posz = 0.0;

            /* and define as zero rotation in euler angles */
            rotang[0] = 0.0;
            rotang[1] = 0.0;
            rotang[2] = 0.0;
            rotang[3] = 1.0;

            /* but if there's something on the vector string first time it means */
            /* we're being replaced or launched in a map, so collect the values */
            /* as if they were sent by the interactor */
 	      string = cxParamStrGet((cxParameter *) cxInputDataGet(cxInputPortOpen("vector")));
            if (strcmp(string,"") && firsttime) {
                 tempvx = atof(strtok(string," ")) ;
	           tempvy = atof(strtok(NULL," ")) ;
	           tempvz = atof(strtok(NULL," ")) ;
                 trackball(delta,vx,vy,vz,&tempvx,&tempvy,&tempvz);
                 add_eulers(delta,rotang,rotang);
                 vx = tempvx;
                 vy = tempvy;
                 vz = tempvz;
            }

            /* construct parameter string that will set up interactor */
            sprintf(buffer,"%s:vector %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %ld",
                            cxModuleInstanceNameGet(),
                            posx,posy,posz,minvx,vx,maxvx,minvy,vy,maxvy,minvz,vz,maxvz,0);
            interactor = cxParamStrNew(buffer);
            paramport = cxOutputPortOpen("Interactor");
            cxOutputDataSet(paramport,interactor);

            /* do outputs */
            post_matrix(vx,vy,vz,rotang);

            firsttime = 0;
            
		return;
	}
  	
      if (cxInputDataChanged(cxInputPortOpen("vector"))) { 
         /* must be from downstream interactor, so do the new rot matrix */
	   string = cxParamStrGet((cxParameter *) cxInputDataGet(cxInputPortOpen("vector"))) ;

         tempvx = atof(strtok(string," ")) ;
	   tempvy = atof(strtok(NULL," ")) ;
	   tempvz = atof(strtok(NULL," ")) ;

         trackball(delta,vx,vy,vz,&tempvx,&tempvy,&tempvz);
         add_eulers(delta,rotang,rotang);

         /* set up for next time */
	   vx = tempvx;
	   vy = tempvy;
	   vz = tempvz;

         /* do outputs */
         post_matrix(vx,vy,vz,rotang);
      }
}

