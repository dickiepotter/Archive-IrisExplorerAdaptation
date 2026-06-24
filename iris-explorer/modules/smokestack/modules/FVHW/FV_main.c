
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
 
#include <cx/PortAccess.h>
#include <cx/DataAccess.h>
#include <cx/cxPyramid.h>
#include <cx/Pyramid.h>
#include <cx/cxPyramid.api.h>
#include <cx/DataTypes.h>

#include "mesh.h"
#include "FV_scheme.h"
#include "FV_main.h"

double main_func(int reset, 
              double timeZero, double timeMax, double CFL, 
	      double convectX,double convectY, double convectZ,
              int update, int output, int n,
              double X1, double Y1, double Z1,
              double X2, double Y2, double Z2 )
{

  static int set_initial = 0;
  static int restart = 0;
  
  static Mesh mp;
  static Data data;

  static int step;
  static int step_out_last;
  static double time;

  int call_back_step;
    
  double mod_a = sqrt( convectX*convectX + convectY*convectY + convectZ*convectZ ); 
  double timestep;
  /*
  double timestep = (CFL/(n-1))/mod_a;
  double timestep = 0.001;
  */

/* SET LOCAL PARAMETER VALUES */
     
  printf(" Parameters\n");
  printf("  Initial time %f\n",timeZero);
  printf("  Final time   %f\n",timeMax);
  printf("  Time         %f\n",time);
  printf("  Time step    %f\n",timestep);
  printf("  Step                     %d\n",step);
  printf("  Convection field ( %f, %f, %f )\n",convectX,convectY,convectZ);

/* RETURN ON reset FLAG */
   
/* PAUSE */
  if( reset == 1 ) 
    return(time);
  
/* RESTART */
  if( reset == 2 )
  {
/* flush an empty pyramid - HW */
	  cxOutputDataSet(cxOutputPortOpen("Output"),(void *)cxPyrNew(3));
	  cxOutputDataFlush(cxOutputPortOpen("Output"));
    if( set_initial != 0 )
    {
/* DESTROY MESH DATA */	    
      Cleanup( &mp, &data );
      set_initial = 0;
    }
    return(timeZero);
  }

  if (set_initial == 0) /* START */
  {
    printf("\n\n Start\n\n");

/* SET THE OTHER PROBLEM PARAMETERS */
    step = 0;
    step_out_last = 0;
    time = timeZero;
 
/* SET UP THE MESH DATA */
    Setup( &mp, &data, n, X1,Y1,Z1,X2,Y2,Z2 );

    set_initial = 1;
  }	  
  else
    printf("\n\n Continue\n\n");
	 
/* SET NEXT CALL BACK STEP */
  call_back_step = step + update;
		   
/* TIME STEPPING */
  printf("\n\n CALL Timestep\n\n");

  timestep = cfl_timestep( &mp, &data, CFL, mod_a );

  Timestep(&mp,&data,
           &step,call_back_step,output,&step_out_last,
       	   convectX,convectY,convectZ,
           &time, timestep, timeMax );     

  if( time > timeMax )
  {
    printf("\n\n End\n\n");

/* DESTROY ALLOCATED MEMORY */
    Cleanup( &mp, &data );

/* SETUP FOR RESTART */
    set_initial = 0;
  }
      	
  return(time);	
}

/* --------------------------------
    SETUP
   -------------------------------- */

void Setup( Mesh *mp, Data *data, int n, 
	    double X1, double Y1, double Z1,
	    double X2, double Y2, double Z2 )
{
  int nx,ny,nz;

/* MESH */
	
  mesh3d( mp, n, &nx,&ny,&nz, &X1,&Y1,&Z1,&X2,&Y2,&Z2 );

  topography( mp, nx,ny,nz, X1,Y1,Z1,X2,Y2,Z2 );

/* NODE DATA */

  data->error = (double *)calloc( mp->n_nd, sizeof(double) );

/* CELL DATA */

  data->u = (double *)calloc( mp->n_el, sizeof(double) );
  data->rh = (double *)calloc( mp->n_el, sizeof(double) );
  data->udot = (double *)calloc( mp->n_el, sizeof(double) );

/* MAKE MESH FACES */

  FaceList( mp );

/* FACE DATA */

  data->flux = (double *)calloc( mp->n_fa, sizeof(double) );

/* MESH GEOMETRY DATA */

  MeshGeometry( mp ); 

/* PROBLEM RIGHT HAND SIDE FUNCTION */

  ProblemRHS( mp, data );

/* INITIAL CONDITIONS */

  InitialConditions( mp, data );

  return;	
}	

/* --------------------------------
    CLEANUP
   -------------------------------- */

void Cleanup( Mesh *mp, Data *data )
{
  free(data->u);
  free(data->rh);
  free(data->udot);
  free(data->flux);
  free(data->error);

  free(mp->nd_list);
  free(mp->el_list);
  free(mp->by_list);
  free(mp->fa_list);

  return;	
}	

/* --------------------------------
    TIMESTEP
   -------------------------------- */

void Timestep( Mesh *mp,  Data *data,
	       int *step, int call_back_step, int output, int *step_out_last,
	       double convectX, double convectY, double convectZ,
	       double *time, double timestep, double timeMax )
{
  int i;
  int i_out=0;

  while( ( *time < timeMax ) && ( *step < call_back_step ) )
  {

    printf("Step %d time %g\n",*step,*time);

    GetFlux( mp, data, convectX,convectY,convectZ );
    TimeDerivatives( mp, data );
    Update( mp, data, timestep );

    (*time) += timestep;
    (*step) ++;
    (*step_out_last) ++;

    if( *step_out_last == output )
    {	 
      error_estimate( mp, data );	    
      OutputPyramid( mp, data );
      *step_out_last = 0;
    }  
  }	  
  return;	
}	

/* ---------------------------------------------
       OUTPUT AN INTERNAL EXPLORER PYRAMID 
   --------------------------------------------- */

void
OutputPyramid( Mesh *mp, Data *data )
{
	cxLattice *lat, *lat_e;
	cxPyramid *pyr;
	cxConnection *conn;
	cxPyramidDictionary *dict;
	cxErrorCode ec = cx_err_none;
	float *coord, *latData, *coord_e, *latData_e;
	long *connec, *elem;
	long dims[1];
	long port;

	int counter = 0;
	double x,y,z;
	int ip,ie;

    int k;


/* node Lattice */
	dims[0] = mp->n_nd;
	lat = cxLatNew(1,dims,1,cx_prim_float,3,cx_coord_curvilinear);
	if (cxDataAllocErrorGet()) {
		cxModAlert("Lattice Shared Memory Error:\n\taborting output.");
		return;
	}
	/* Get Ptrs */
	cxLatPtrGet(lat,NULL,(void **)&latData,NULL,(void **)&coord);

/* node lattice coords */
	counter = 0;
    for(ip=0; ip<mp->n_nd; ++ip)                         /* COORDINATES: x,y,z PER LINE  */
    {
	coord[counter++] = mp->nd_list[ip].x[0];
	coord[counter++] = mp->nd_list[ip].x[1];
	coord[counter++] = mp->nd_list[ip].x[2];
    }

/* node lattice data */
	counter = 0;
    	for(ip=0; ip<mp->n_nd; ++ip)                   /* DATA VALUES: u(ip) PER LINE */
    	{
          latData[counter++] = (float)(data->error[ip]);
	}

/* element lattice */
        {      
          dims[0] = mp->n_el;
          lat_e = cxLatNew(1,dims,1,cx_prim_float,3,cx_coord_curvilinear);
          if (cxDataAllocErrorGet()) {
                  cxModAlert("Lattice Shared Memory Error:\n\taborting output.");
                  return;
          }
          /* Get Ptrs */
          cxLatPtrGet(lat_e,NULL,(void **)&latData_e,NULL,(void **)&coord_e);

/* element lattice coords */
          counter = 0;
          for( ie=0; ie<mp->n_el; ++ie )
          {
            int node;
            double x[4],y[4],z[4];
            for( node=0; node<4; ++node )
            {
              Node *np = mp->el_list[ie].nd[node];
              x[node] = np->x[0];
              y[node] = np->x[1];
              z[node] = np->x[2];
            }
            coord_e[counter++] = 0.25*( x[0] + x[1] + x[2] + x[3] );
            coord_e[counter++] = 0.25*( y[0] + y[1] + y[2] + y[3] );
            coord_e[counter++] = 0.25*( z[0] + z[1] + z[2] + z[3] );
          }

/* element lattice data */
          counter = 0;
          for( ie=0; ie<mp->n_el; ++ie )
            latData_e[ie] = (float)(data->u[ie]);
          
	}  

/* Create a Pyramid */
	pyr = cxPyrNew(0);
	cxPyrSet(pyr,lat);
	cxPyrLayerSet(pyr,1,NULL,NULL);
	cxPyrLayerSet(pyr,2,NULL,NULL);

	/* Set up connection list */
	conn = cxConnNew(mp->n_el,4*mp->n_el);
	if (memclean(pyr,ec,0)) {
		cxModAlert("Pyramid Shared Memory Error:\n\taborting output.");
		return;
	}
	cxConnPtrGet(conn,NULL,NULL,&elem,&connec);

	/* Set elements */
	counter = 0;
    for( k=0; k<mp->n_el+1; ++k )                                     /* LIST OF POINTERS TO EACH TETRAHEDRA */
    {
		elem[counter++] = 4*k;
    }

	/* set nodes */
	counter = 0;
    for(ie=0; ie<mp->n_el; ++ie)                 /* LIST OF NODES MAKING UP EACH TET */
    {
        int node,ip1[4];

        for (node=0; node<4; node++)
        {
            Node *np = mp->el_list[ie].nd[node];

            ip1[node] = np->id;
        } /* NODE LOOP */ 

		/* 4 NODES PER TET: note orientation */
		connec[counter++] = ip1[3];
		connec[counter++] = ip1[0];
		connec[counter++] = ip1[1];
		connec[counter++] = ip1[2];
		
    } /* LEAF ELEMENT LOOP */

/* make top layer */
          cxPyrLayerSet(pyr,3,conn,lat_e);

	if(memclean(pyr,ec,conn)) {
		cxModAlert("Pyramid Shared Memory Error:\n\taborting output.");
		return;
	}

	/* make the pyramid dictionary */
	dict = cxPyrDictDefault( 3 );
	cxPyramidDictionarySet(pyr, dict, &ec );
	if(memclean(pyr,ec,conn)) {
		cxModAlert("Dictionary Error:\n\taborting output.");
		return;
	}
	cxPyramidCompressionTypeSet(pyr,cx_compress_unique,&ec );
	if(memclean(pyr,ec,conn)) {
		cxModAlert("Compression Error:\n\taborting output.");
		return;
	}
	connec = (long *)cxPyramidCompressionIndexGet(pyr,&ec);
	if(memclean(pyr,ec,conn)) {
		cxModAlert("Index Get Error:\n\taborting output.");
		return;
	}
	*connec = cx_pyramid_dict_tetrahedron;

/*
    cxInWdgtLongSet("Control",-1);
    cxInWdgtLongSet("Control",cxParamLongGet(
         (cxParameter*)cxInputDataGet(cxInputPortOpen("Control"))));
*/

	port = cxOutputPortOpen("Output");
	cxOutputDataSet(port,(void *)pyr);
	cxOutputDataFlush(port);
/*
	port = cxOutputPortOpen("Output2");
	cxOutputDataSet(port,(void *)pyr);
	cxOutputDataFlush(port);
printf("Outputing Pyramid\n");
*/
    return;
}

/* -----------------------------------
*      EXPLORER CLEAN UP
*         ----------------------------------- */
 
int memclean(cxPyramid *pyr, cxErrorCode ec, cxConnection *conn)
{
	/* this routine will clear up memory in case of errors */
	if(cxDataAllocErrorGet() || (ec != cx_err_none))
	{
		        cxDataRefDec(pyr);
			        cxDataRefDec(conn);
				        return 1;
	} else {
		        return 0;
	}
}

/* ---------------------------------------------
    WRITE OUT AN EXPLORER PYRAMID FILE TO DISK
   --------------------------------------------- */

void
WritePyramid( Mesh *mp, Data *data, int i_out )
{
    FILE *lattice, *pyramid;
    int k, count=0;

    int ie,ip;

    char node_file[100],elem_file[100],mesh_file[100];

    sprintf(node_file,"%s%d.lat","node",i_out);
    sprintf(elem_file,"%s%d.lat","elem",i_out);
    sprintf(mesh_file,"%s%d.pyr","mesh",i_out);

/* WRITE THE COORDINATE LATTICE FILE */

    lattice = fopen(node_file,"w");                                    /* BEGIN WRITING LATTICE */

    fprintf(lattice,"#!/usr/explorer/bin/explorer cxLattice plain 1.0\n");   /* HEADER */
    fprintf(lattice," 1\n");                                                 /* DIMENSIONS */
    fprintf(lattice," %d\n",mp->n_nd);                                 /* NUMBER OF DATA POINTS PER DIMENSION*/
    fprintf(lattice," 1\n");                                                 /* NUMBER OF DATA VALUES PER NODE */
    fprintf(lattice," 3\n");                                                 /* DATA FORMAT: 3=float */
    fprintf(lattice," 2\n");                                                 /* COORDINATE TYPE: 2=curvilinear */
    fprintf(lattice," 1\n");                                                 /* NUMBER OF DATA SETS PRESENT */
    fprintf(lattice," 3\n");                                                 /* COORDINATE DIMENSIONS */

    for( ip=0; ip<mp->n_nd; ++ip )                         /* COORDINATES: x,y,z PER LINE  */
    {
        double x,y,z;

        x = mp->nd_list[ip].x[0];
        y = mp->nd_list[ip].x[1];
        z = mp->nd_list[ip].x[2];

        fprintf(lattice," %g %g %g\n",x,y,z);
    }
    for( ip=0; ip<mp->n_nd; ++ip )                         /* DATA VALUES: u(ip) PER LINE */
    {
        fprintf(lattice," %g\n",data->error[ip]);
    }

    fclose(lattice);                                                      /* END OF LATTICE */

/* WRITE THE ELEMENT LATTICE FILE */
     
    lattice = fopen(elem_file,"w");                                    /* BEGIN WRITING LATTICE */
	 
    fprintf(lattice,"#!/usr/explorer/bin/explorer cxLattice plain 1.0\n");   /* HEADER */
    fprintf(lattice," 1\n");                                                 /* DIMENSIONS */
    fprintf(lattice," %d\n",mp->n_el);                                 /* NUMBER OF DATA POINTS PER DIMENSION*/
    fprintf(lattice," 1\n");                                                 /* NUMBER OF DATA VALUES PER NODE */
    fprintf(lattice," 3\n");                                                 /* DATA FORMAT: 3=float */
    fprintf(lattice," 2\n");                                                 /* COORDINATE TYPE: 2=curvilinear */
    fprintf(lattice," 1\n");                                                 /* NUMBER OF DATA SETS PRESENT */
    fprintf(lattice," 3\n");

    for(ie=0; ie<mp->n_el; ++ie)                 /* LIST OF NODES MAKING UP EACH TET */
    {
       int node,ip[4];
       double x[4],y[4],z[4],xc,yc,zc;
	 
       for (node=0; node<4; node++)
       {
         Node *np = mp->el_list[ie].nd[node];

         x[node] = np->x[0];
         y[node] = np->x[1];
         z[node] = np->x[2];
       } /* NODE LOOP */
       xc = 0.25*( x[0] + x[1] + x[2] + x[3] );
       yc = 0.25*( y[0] + y[1] + y[2] + y[3] );
       zc = 0.25*( z[0] + z[1] + z[2] + z[3] );
       fprintf(lattice," %g %g %g\n",xc,yc,zc);                          /* CENTROID COORDINATES: x,y,z PER LINE */
    } /* LEAF ELEMENT LOOP */
  
    for(k=0; k<mp->n_el; ++k)              /* DATA VALUES: u PER LINE */
    {
      int ie = mp->el_list[k].id;
     
      fprintf(lattice," %g\n",data->u[ie]);
    }

    fclose(lattice);                                                      /* END OF LATTICE */

    pyramid = fopen(mesh_file,"w");                                    /* BEGIN WRITING PYRAMID */
    
    fprintf(pyramid,"#!/usr/explorer/bin/explorer cxPyramid plain 1.0\n");   /* HEADER */
    fprintf(pyramid,"include %s\n",node_file);                            /* INCLUDE LATTICE COORDINATES AND DATA */
    fprintf(pyramid," 3\n");                                                 /* NUMBER OF PYRAMID LAYERS */
    fprintf(pyramid," 1\n");                                                 /* COMPRESSION TYPE: 1=unique */
    fprintf(pyramid," 4\n");                                                 /* COMPRESSION TYPE: 4=tetrahedra */
    fprintf(pyramid," 0 0\n");                                               /* LAYER 1: COMPRESSED */
    fprintf(pyramid," 0 0\n");                                               /* LAYER 2: COMPRESSED */
    fprintf(pyramid," %d %d\n",mp->n_el,4*(mp->n_el));         /* LAYER 3: # TETS, # CONNECTIONS */
    for( k=0; k<mp->n_el+1; ++k )                                       /* LIST OF POINTERS TO EACH TETRAHEDRA */
    {
      fprintf(pyramid," %d",4*k);
      count += 1;
      if( count == 20 )                                                          /* BREAK LINE EVERY ONCE IN A WHILE */
      {
        fprintf(pyramid," \n");
        count = 0;
      }
    }
    if( count != 0)
      fprintf(pyramid,"\n");

    for( ie=0; ie<mp->n_el; ++ie )                 /* LIST OF NODES MAKING UP EACH TET */
    {
        int node,ip[4];

        for (node=0; node<4; node++)
        {
            Node *np = mp->el_list[ie].nd[node];

            ip[node] = np->id;
        } /* NODE LOOP */ 

        fprintf(pyramid," %d %d %d %d\n",ip[3],ip[0],ip[1],ip[2]);              /* 4 NODES PER TET: note orientation */
    } /* LEAF ELEMENT LOOP */

    fprintf(pyramid,"include %s\n",elem_file);                               /* LAYER 3: ELEMENT DATA */

    fclose(pyramid);                                                      /* END WRITING PYRAMID */

    return;
}

/*
 *****************************************************************
 
  3D TETRAHEDRAL MESH FOR A CUBE
 
 *****************************************************************
 
  REGULAR SUDIVISION INTO SUB-CUBES
 
  EACH SUB-CUBE DIVIVDED INTO 6 TETRAHEDRA
 
 *****************************************************************
*/

void mesh3d( Mesh *mp, int n, 
	     int *pnx, int *pny, int *pnz,	
             double *pX1, double *pY1, double *pZ1,
	     double *pX2, double *pY2, double *pZ2 )
{
  int ip,ie,ib;
  int i, ix,iy,iz;
  int i1,i2,i3,i4,i5,i6,i7,i8, j1,j2;

  int nx,ny,nz;	
  double dx,dy,dz;
  double X1,X2,Y1,Y2,Z1,Z2;

  int np,ne,nb;

/* MESH SIZE */

  X1 = *pX1; X2 = *pX2;
  Y1 = *pY1; Y2 = *pY2;
  Z1 = *pZ1; Z2 = *pZ2;

  nx = (int) (n*(X2-X1)); 
  ny = (int) (n*(Y2-Y1)); 
  nz = (int) (n*(Z2-Z1));

  *pnx=nx; *pny=ny; *pnz=nz;

  dx = (X2-X1)/(nx-1); 
  dy = (Y2-Y1)/(ny-1); 
  dz = (Z2-Z1)/(nz-1);  

/* MESH DIMENSIONS */

  np = nx*ny*nz;
  ne = 6*(nx-1)*(ny-1)*(nz-1);
  nb = 4*(nx-1)*(ny-1) + 4*(ny-1)*(nz-1) + 4*(nz-1)*(nx-1);

  printf(" Mesh dimensions\n");
  printf(" %g %g %g %g %g %g\n",X1,X2,Y1,Y2,Z1,Z2);
  printf(" %d %d %d\n",nx,ny,nz);
  printf(" %g %g %g\n",dx,dy,dz);

  printf(" Build structured mesh\n");
  printf("  %d nodes, %d elements\n",np,ne);

/* MAKE TETRAHEDRAL MESH */

  mp->n_nd = np;
  mp->nd_list = (Node *)calloc( np, sizeof(Node) );

  mp->n_el = ne;
  mp->el_list = (Element *)calloc( ne, sizeof(Element) );

  mp->n_by = nb;
  mp->by_list = (Boundary *)calloc( nb, sizeof(Boundary) );

/**COORDINATES**/

  X2 = X1 + (nx-1)*dx;
  Y2 = Y1 + (ny-1)*dy;
  Z2 = Z1 + (nz-1)*dz;

  *pX2 = X2; *pY2 = Y2; *pZ2 = Z2;

  ip = 0;
  for( iz=0; iz<nz; ++iz )
  {	      
    double z = Z1 + dz*iz;
    for( iy=0; iy<ny; ++iy )
    {	      
      double y = Y1 + dy*iy;
      for( ix=0; ix<nx; ++ix )
      {	      
        double x = X1 + dx*ix;

	mp->nd_list[ip].x[0] = x;
	mp->nd_list[ip].x[1] = y;
	mp->nd_list[ip].x[2] = z;
	mp->nd_list[ip].id = ip;
        ip = ip + 1;
      }
    }
  }

/**CONNECTIVITY**/

  ie = 0;
  for( iz=0; iz<nz-1; ++iz )
  {
    for( iy=0; iy<ny-1; ++iy )
    {
      for( ix=0; ix<nx-1; ++ix )
      {	      
/* LOCATION */
        int ip = iz*nx*ny + iy*nx + ix;
/* LOCAL CUBE */
        int i1 = ip;
        int i2 = ip + 1;
        int i3 = ip + nx;
        int i4 = ip + nx + 1;
        int i5 = ip + nx*ny;
        int i6 = ip + 1 + nx*ny;
        int i7 = ip + nx + nx*ny;
        int i8 = ip + nx + 1 + nx*ny;
/* DIVIDE EACH CUBE INTO 6 TETRAHEDRA */
   
        mp->el_list[ie].nd[0] = &(mp->nd_list[i2]);	
        mp->el_list[ie].nd[1] = &(mp->nd_list[i1]);	
        mp->el_list[ie].nd[2] = &(mp->nd_list[i4]);	
        mp->el_list[ie].nd[3] = &(mp->nd_list[i8]);	
	mp->el_list[ie].id = ie;
	ie = ie + 1;

        mp->el_list[ie].nd[0] = &(mp->nd_list[i2]);	
        mp->el_list[ie].nd[1] = &(mp->nd_list[i1]);	
        mp->el_list[ie].nd[2] = &(mp->nd_list[i8]);	
        mp->el_list[ie].nd[3] = &(mp->nd_list[i6]);	
	mp->el_list[ie].id = ie;
	ie = ie + 1;

        mp->el_list[ie].nd[0] = &(mp->nd_list[i1]);	
        mp->el_list[ie].nd[1] = &(mp->nd_list[i5]);	
        mp->el_list[ie].nd[2] = &(mp->nd_list[i8]);	
        mp->el_list[ie].nd[3] = &(mp->nd_list[i6]);	
	mp->el_list[ie].id = ie;
	ie = ie + 1;

        mp->el_list[ie].nd[0] = &(mp->nd_list[i4]);	
        mp->el_list[ie].nd[1] = &(mp->nd_list[i1]);	
        mp->el_list[ie].nd[2] = &(mp->nd_list[i3]);	
        mp->el_list[ie].nd[3] = &(mp->nd_list[i8]);	
	mp->el_list[ie].id = ie;
	ie = ie + 1;

        mp->el_list[ie].nd[0] = &(mp->nd_list[i3]);	
        mp->el_list[ie].nd[1] = &(mp->nd_list[i1]);	
        mp->el_list[ie].nd[2] = &(mp->nd_list[i7]);	
        mp->el_list[ie].nd[3] = &(mp->nd_list[i8]);	
	mp->el_list[ie].id = ie;
	ie = ie + 1;

        mp->el_list[ie].nd[0] = &(mp->nd_list[i1]);	
        mp->el_list[ie].nd[1] = &(mp->nd_list[i7]);	
        mp->el_list[ie].nd[2] = &(mp->nd_list[i8]);	
        mp->el_list[ie].nd[3] = &(mp->nd_list[i5]);	
	mp->el_list[ie].id = ie;
	ie = ie + 1;
      }
    }
  }

/**BOUNDARY**/

  ib=0;

/* x=X1 */
 
  for( iz=0; iz<nz-1; ++iz )
  {
    for( iy=0; iy<ny-1; ++iy )
    {
      ip = iz*nx*ny + iy*nx;
      i1 = ip;
      i3 = ip + nx;
      i5 = ip + nx*ny;
      i7 = ip + nx + nx*ny;
      ie = 6*( iz*(ny-1)*(nx-1) + iy*(nx-1) );
      j1 = ie + 4;
      j2 = ie + 5;

      mp->by_list[ib].nd[0] = &(mp->nd_list[i7]);
      mp->by_list[ib].nd[1] = &(mp->nd_list[i3]);
      mp->by_list[ib].nd[2] = &(mp->nd_list[i1]);
      mp->by_list[ib].el = &(mp->el_list[j1]);
      mp->by_list[ib].id = ib;
      mp->by_list[ib].bc_type = OUTFLOW;
      ib+=1;

      mp->by_list[ib].nd[0] = &(mp->nd_list[i5]);
      mp->by_list[ib].nd[1] = &(mp->nd_list[i7]);
      mp->by_list[ib].nd[2] = &(mp->nd_list[i1]);
      mp->by_list[ib].el = &(mp->el_list[j2]);
      mp->by_list[ib].id = ib;
      mp->by_list[ib].bc_type = OUTFLOW;
      ib+=1;
    }	    
  }	  
    
/* x=X2 */
 
  for( iz=0; iz<nz-1; ++iz )
  {
    for( iy=0; iy<ny-1; ++iy )
    {
      ip = iz*nx*ny + iy*nx + nx-2;
      i2 = ip+1;
      i4 = ip + nx + 1;
      i6 = ip + 1 + nx*ny;
      i8 = ip + nx + 1 + nx*ny;
      ie = 6*( iz*(ny-1)*(nx-1) + iy*(nx-1) + (nx-2) );
      j1 = ie + 1;
      j2 = ie;

      mp->by_list[ib].nd[0] = &(mp->nd_list[i2]);
      mp->by_list[ib].nd[1] = &(mp->nd_list[i8]);
      mp->by_list[ib].nd[2] = &(mp->nd_list[i6]);
      mp->by_list[ib].el = &(mp->el_list[j1]);
      mp->by_list[ib].id = ib;
      mp->by_list[ib].bc_type = OUTFLOW;
      ib+=1;

      mp->by_list[ib].nd[0] = &(mp->nd_list[i4]);
      mp->by_list[ib].nd[1] = &(mp->nd_list[i8]);
      mp->by_list[ib].nd[2] = &(mp->nd_list[i2]);
      mp->by_list[ib].el = &(mp->el_list[j2]);
      mp->by_list[ib].id = ib;
      mp->by_list[ib].bc_type = OUTFLOW;
      ib+=1;
    }	    
  }	  
    
/* y=Y1 */
 
  for( iz=0; iz<nz-1; ++iz )
  {
    for( ix=0; ix<nx-1; ++ix )
    {
      ip = iz*nx*ny + ix;
      i1 = ip;
      i2 = ip + 1;
      i5 = ip + nx*ny;
      i6 = ip + 1 + nx*ny;
      ie = 6*( iz*(ny-1)*(nx-1) + ix );
      j1 = ie;
      j2 = ie + 1;

      mp->by_list[ib].nd[0] = &(mp->nd_list[i1]);
      mp->by_list[ib].nd[1] = &(mp->nd_list[i2]);
      mp->by_list[ib].nd[2] = &(mp->nd_list[i6]);
      mp->by_list[ib].el = &(mp->el_list[j1]);
      mp->by_list[ib].id = ib;
      mp->by_list[ib].bc_type = OUTFLOW;
      ib+=1;

      mp->by_list[ib].nd[0] = &(mp->nd_list[i1]);
      mp->by_list[ib].nd[1] = &(mp->nd_list[i6]);
      mp->by_list[ib].nd[2] = &(mp->nd_list[i5]);
      mp->by_list[ib].el = &(mp->el_list[j2]);
      mp->by_list[ib].id = ib;
      mp->by_list[ib].bc_type = OUTFLOW;
      ib+=1;
    }	    
  }	  
    
/* y=Y2 */
 
  for( iz=0; iz<nz-1; ++iz )
  {
    for( ix=0; ix<nx-1; ++ix )
    {
      ip = iz*nx*ny + (ny-2)*nx + ix;
      i3 = ip + nx;
      i4 = ip + nx + 1;
      i7 = ip + nx + nx*ny;
      i8 = ip + 1 + nx + nx*ny;
      ie = 6*( iz*(ny-1)*(nx-1) + (ny-2)*(nx-1) + ix );
      j1 = ie + 4;
      j2 = ie + 5;

      mp->by_list[ib].nd[0] = &(mp->nd_list[i3]);
      mp->by_list[ib].nd[1] = &(mp->nd_list[i8]);
      mp->by_list[ib].nd[2] = &(mp->nd_list[i4]);
      mp->by_list[ib].el = &(mp->el_list[j1]);
      mp->by_list[ib].id = ib;
      mp->by_list[ib].bc_type = OUTFLOW;
      ib+=1;

      mp->by_list[ib].nd[0] = &(mp->nd_list[i3]);
      mp->by_list[ib].nd[1] = &(mp->nd_list[i7]);
      mp->by_list[ib].nd[2] = &(mp->nd_list[i8]);
      mp->by_list[ib].el = &(mp->el_list[j2]);
      mp->by_list[ib].id = ib;
      mp->by_list[ib].bc_type = OUTFLOW;
      ib+=1;
    }	    
  }	  
    
/* z=Z1 */
 
  for( iy=0; iy<ny-1; ++iy )
  {
    for( ix=0; ix<nx-1; ++ix )
    {
      ip = iy*nx + ix;
      i1 = ip;
      i2 = ip + 1;
      i3 = ip + nx;
      i4 = ip + nx + 1;
      ie = 6*( iy*(nx-1) + ix );
      j1 = ie + 3;
      j2 = ie;

      mp->by_list[ib].nd[0] = &(mp->nd_list[i1]);
      mp->by_list[ib].nd[1] = &(mp->nd_list[i3]);
      mp->by_list[ib].nd[2] = &(mp->nd_list[i4]);
      mp->by_list[ib].el = &(mp->el_list[j1]);
      mp->by_list[ib].id = ib;
      mp->by_list[ib].bc_type = WALL;
      ib+=1;

      mp->by_list[ib].nd[0] = &(mp->nd_list[i2]);
      mp->by_list[ib].nd[1] = &(mp->nd_list[i1]);
      mp->by_list[ib].nd[2] = &(mp->nd_list[i4]);
      mp->by_list[ib].el = &(mp->el_list[j2]);
      mp->by_list[ib].id = ib;
      mp->by_list[ib].bc_type = WALL;
      ib+=1;
    }	    
  }	  
    
/* z=Z2 */
 
  for( iy=0; iy<ny-1; ++iy )
  {
    for( ix=0; ix<nx-1; ++ix )
    {
      ip = (nz-2)*nx*ny + iy*nx + ix;
      i5 = ip + nx*ny;
      i6 = ip + 1 + nx*ny;
      i7 = ip + nx + nx*ny;
      i8 = ip + nx + 1 + nx*ny;
      ie = 6*( (nz-2)*(ny-1)*(nx-1) + iy*(nx-1) + ix );
      j1 = ie + 2;
      j2 = ie + 5;

      mp->by_list[ib].nd[0] = &(mp->nd_list[i5]);
      mp->by_list[ib].nd[1] = &(mp->nd_list[i6]);
      mp->by_list[ib].nd[2] = &(mp->nd_list[i8]);
      mp->by_list[ib].el = &(mp->el_list[j1]);
      mp->by_list[ib].id = ib;
      mp->by_list[ib].bc_type = OUTFLOW;
      ib+=1;

      mp->by_list[ib].nd[0] = &(mp->nd_list[i5]);
      mp->by_list[ib].nd[1] = &(mp->nd_list[i8]);
      mp->by_list[ib].nd[2] = &(mp->nd_list[i7]);
      mp->by_list[ib].el = &(mp->el_list[j2]);
      mp->by_list[ib].id = ib;
      mp->by_list[ib].bc_type = OUTFLOW;
      ib+=1;
    }	    
  }	  
/* OUTPUT DATA FOR CHECKS   
  for( i=0; i<mp->n_nd; ++i ) 
  {
    Node np = mp->nd_list[i];
    printf("Node %d ( %g %g %g )\n",np.id,np.x[0],np.x[1],np.x[2]);
  }
  for( i=0; i<mp->n_el; ++i ) 
  {
    Element el = mp->el_list[i];
    printf("Element %d ( %d %d %d %d )\n",el.id,(el.nd[0])->id,(el.nd[1])->id,
                                                (el.nd[2])->id,(el.nd[3])->id);
  }
  for( i=0; i<mp->n_by; ++i )
  {
    Boundary bo = mp->by_list[i];
    printf("Boundary %d ( %d %d %d ) %d\n",bo.id,(bo.nd[0])->id,(bo.nd[1])->id,(bo.nd[2])->id,
                                                 (bo.el)->id);
  }
*/
  return;
}      


