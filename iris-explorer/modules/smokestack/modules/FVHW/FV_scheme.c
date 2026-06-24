
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "mesh.h"

#include "FV_scheme.h"

/* ---
    RIGHT HAND SIDE FUNCTION 
                          --- */

void ProblemRHS( Mesh *mp, Data *data )
{
  double xr=0.2; double yr=0.5; double zr=0.2;
  double x[4],y[4],z[4], xc,yc,zc, r;

  int i,k;

  for( k=0; k<mp->n_el; ++k ) 
  {
    int ie = mp->el_list[k].id;
    double volume = mp->el_list[k].volume;

    for( i=0; i<4; ++i )
    {
      Node *np = mp->el_list[k].nd[i];
      x[i] = np->x[0];
      y[i] = np->x[1];
      z[i] = np->x[2];
    }
    xc = 0.25*( x[0] + x[1] + x[2] + x[3] );    
    yc = 0.25*( y[0] + y[1] + y[2] + y[3] );    
    zc = 0.25*( z[0] + z[1] + z[2] + z[3] );    

    r = (xc-xr)*(xc-xr) + (yc-yr)*(yc-yr) + (zc-zr)*(zc-zr);

    data->rh[ie] = volume*10.0*exp( -100.0*r );
    /*
    data->rh[ie] = 0.0;
    */
  }
  return;
}

/* ---
    INITIAL SOLUTION
                  --- */

void InitialConditions( Mesh *mp, Data *data )
{
  int k;

  for( k=0; k<mp->n_el; ++k )
  {
    int ie = mp->el_list[k].id;

    data->u[ie] = 0.0;
    /*
    data->u[ie] = 1.0;
    */
  }  
  return;
}

/* ---
    FLUX CALCULATION 
                  --- */

void GetFlux( Mesh *mp, Data *data, 
              double convectX, double convectY, double convectZ )
{
  int i,j,k;

  for( i=0; i<mp->n_fa; ++i )
    data->flux[i] = 0.0;

  for( i=0; i<mp->n_fa; ++i )
  {
    double a_dot_n =  convectX*mp->fa_list[i].normal[0]
                    + convectY*mp->fa_list[i].normal[1]	    
                    + convectZ*mp->fa_list[i].normal[2];

/* BOUNDARY FACE */

    if( mp->fa_list[i].el[1] == NULL )
    {
      int bc = mp->fa_list[i].bc_type;

/* WALL: ZERO FLUX */

      if( bc == WALL )
	data->flux[i] = 0.0;      

/* OUTFLOW */

      else if( a_dot_n >= 0.0 )
      {
        int ie = (mp->fa_list[i].el[0])->id;

        data->flux[i] += a_dot_n*data->u[ie];
      }	      

/* INFLOW: ASSUME ZERO DATA AT INFLOW BOUNDARIES */

      else
        data->flux[i] += 0.0;
    }

/* INTERIOR FACE */

    else
    {
      if( a_dot_n >= 0.0 )
      {
        int ie = (mp->fa_list[i].el[0])->id;

        data->flux[i] += a_dot_n*data->u[ie];
      }	      
      else
      {
        int ie = (mp->fa_list[i].el[1])->id;

        data->flux[i] += a_dot_n*data->u[ie];
      }	      
    }	    
  }	  
  	
  return;	
}

/* ---
    TIME DERIVATIVE EVALUATION 
                            --- */

void TimeDerivatives( Mesh *mp, Data *data )
{
  int i,j;

  for( i=0; i<mp->n_el; ++i )
    data->udot[i] = data->rh[i];

  for( j=0; j<mp->n_fa; ++j )
  {
    double face_flux = data->flux[j];
    int ie0 = (mp->fa_list[j].el[0])->id;
    data->udot[ie0] -= face_flux;

    if( mp->fa_list[j].el[1] != NULL )
    {	    
      int ie1 = (mp->fa_list[j].el[1])->id;
      data->udot[ie1] += face_flux;
    }  
  }

  for( i=0; i<mp->n_el; ++i )
    data->udot[i] = data->udot[i]/mp->el_list[i].volume; 

  return;	
}

/* ---
    UPDATE SOLUTION 
                 --- */

void Update( Mesh *mp, Data *data, double timestep )
{
  int i;

  for( i=0; i<mp->n_el; ++i )
    data->u[i] += timestep*data->udot[i];

  return;	
}

/* ---
    MAKE THE LIST OF FACES 
                        --- */

void FaceList( Mesh *mp )
{
  int *count = (int *)calloc( mp->n_nd, sizeof(int) );
  int *start = (int *)calloc( 1+mp->n_nd, sizeof(int) );

  int i,j,k, kp[4],kf[3], ip_min,jf;

/* INITIALISE */

  for( i=0; i<mp->n_nd; ++i )
    count[i] = 0;

/* COUNT FACES */

  for( k=0; k<mp->n_el; ++k )
  {
    for( j=0; j<4; ++j )
    {
      Node *np = mp->el_list[k].nd[j];
      kp[j] = np->id;      
    }	  

    kf[0] = kp[1]; kf[1] = kp[3]; kf[2] = kp[2];
    ip_min = min_node( kf );
    count[ip_min]++;

    kf[0] = kp[0]; kf[1] = kp[2]; kf[2] = kp[3];
    ip_min = min_node( kf );  
    count[ip_min]++;

    kf[0] = kp[0]; kf[1] = kp[3]; kf[2] = kp[1];
    ip_min = min_node( kf );
    count[ip_min]++;

    kf[0] = kp[0]; kf[1] = kp[1]; kf[2] = kp[2];
    ip_min = min_node( kf );
    count[ip_min]++;
  }	  

  for( j=0; j<mp->n_by; ++j )
  {
    for( i=0; i<3; ++i )
    {
      Node *np = mp->by_list[j].nd[i];
      kp[i] = np->id;      
    }	  

    kf[0] = kp[0]; kf[1] = kp[2]; kf[2] = kp[1];
    ip_min = min_node( kf );
    count[ip_min]++;
  }

/* SET start POINTERS */

  start[0] = 0;
  for( i=0; i<mp->n_nd; ++i )
    start[i+1] = start[i] + count[i]/2;

/* ALLOCATE MEMORY */

  mp->n_fa = start[mp->n_nd];
  mp->fa_list = (Face *)calloc( mp->n_fa, sizeof(Face) );

/* INITIALISE */

  for( i=0; i<mp->n_nd; ++i )
    count[i] = 0;

/* SET FACES */  

  for( k=0; k<mp->n_el; ++k )
  {
    for( j=0; j<4; ++j )
    {
      Node *np = mp->el_list[k].nd[j];
      kp[j] = np->id;      
    }	  

    kf[0] = kp[1]; kf[1] = kp[3]; kf[2] = kp[2];
    ip_min = min_node( kf );
    jf = add_face( mp, start, count, kf, ip_min, k );

    kf[0] = kp[0]; kf[1] = kp[2]; kf[2] = kp[3];
    ip_min = min_node( kf );  
    jf = add_face( mp, start, count, kf, ip_min, k );

    kf[0] = kp[0]; kf[1] = kp[3]; kf[2] = kp[1];
    ip_min = min_node( kf );
    jf = add_face( mp, start, count, kf, ip_min, k );

    kf[0] = kp[0]; kf[1] = kp[1]; kf[2] = kp[2];
    ip_min = min_node( kf );
    jf = add_face( mp, start, count, kf, ip_min, k );
  }	  

  for( j=0; j<mp->n_by; ++j )
  {
    for( i=0; i<3; ++i )
    {
      Node *np = mp->by_list[j].nd[i];
      kp[i] = np->id;      
    }	  

    kf[0] = kp[0]; kf[1] = kp[2]; kf[2] = kp[1];
    ip_min = min_node( kf );
    jf = add_bface( mp, start, count, kf, ip_min, mp->by_list[j].bc_type );
  }

  free(count);
  free(start);

/* PRINT FACES FOR CHECKS

  for( i=0; i<mp->n_fa; ++i )
  {
    for( j=0; j<3; ++j )
    {
      Node *np = mp->fa_list[i].nd[j];
      kp[j] = np->id;
    }
    kf[0] = (mp->fa_list[i].el[0])->id;
    if( mp->fa_list[i].el[1] == NULL )
      kf[1] = -1;
    else
      kf[1] = (mp->fa_list[i].el[1])->id;

    printf(" Face %d ( %d %d %d ) %d %d\n",i,kp[0],kp[1],kp[2],kf[0],kf[1]);
  }
*/
  return;
}

/* ---
    ORIENT FACE
             --- */

int min_node( int kf[2] )
{
  int kf_min = MIN( MIN( kf[0], kf[1] ), kf[2] );
 
  if( kf_min == kf[1] )
  { int kt=kf[0]; kf[0]=kf[1]; kf[1]=kf[2]; kf[2]=kt; }

  else if( kf_min == kf[2] )
  { int kt=kf[0]; kf[0]=kf[2]; kf[2]=kf[1]; kf[1]=kt; }

  return(kf_min);
}

/* ---
    FIND FACE
           --- */

int find_face( Mesh *mp, int *start, int *count, int kf[3], int kp )
{
  int i;

  for( i=start[kp]; i<start[kp]+count[kp]; ++i )
  {
    Node *np1 = mp->fa_list[i].nd[1];  	  
    Node *np2 = mp->fa_list[i].nd[2];  	  
    if( ( kf[2] == np1->id ) && ( kf[1] == np2->id ) )
/* OLD FACE */	    
      return(i);
  }	  

/* NEW FACE */
  count[kp]++;   

  return(-1);
}  	  

/* ---
    ADD FACE
          --- */

int add_face( Mesh *mp, int *start, int *count, int kf[3], int kp, int ie )
{
  int i_end = start[kp]+count[kp];

  int i;

  for( i=start[kp]; i<i_end; ++i )
  {
    Node *np1 = mp->fa_list[i].nd[1];  	  
    Node *np2 = mp->fa_list[i].nd[2];  	  
    if( ( kf[2] == np1->id ) && ( kf[1] == np2->id ) )
    {	    
/* OLD FACE */	    
      mp->fa_list[i].el[1] = &(mp->el_list[ie]);	    
      return(i);
    }
  }  
/* NEW FACE */

  mp->fa_list[i_end].nd[0] = &(mp->nd_list[kf[0]]);
  mp->fa_list[i_end].nd[1] = &(mp->nd_list[kf[1]]);
  mp->fa_list[i_end].nd[2] = &(mp->nd_list[kf[2]]);
  mp->fa_list[i_end].el[0] = &(mp->el_list[ie]);
  mp->fa_list[i_end].el[1] = NULL;
  mp->fa_list[i_end].id = i_end;
  mp->fa_list[i_end].bc_type = NOT_BOUNDARY;

  count[kp]++;   

  return(-1);
}  	  

/* ---
    ADD BOUNDARY FACE
                   --- */

int add_bface( Mesh *mp, int *start, int *count, int kf[3], int kp, int bc )
{
  int i_end = start[kp]+count[kp];

  int i;

  for( i=start[kp]; i<i_end; ++i )
  {
    Node *np1 = mp->fa_list[i].nd[1];  	  
    Node *np2 = mp->fa_list[i].nd[2];  	  
    if( ( kf[2] == np1->id ) && ( kf[1] == np2->id ) )
    {	    
/* OLD FACE */	    
      mp->fa_list[i].bc_type = bc;
      return(i);
    }
  }  
/* NEW FACE */

  printf(" add_bface: THIS SHOULDN'T HAPPEN!\n");

  return(-1);
}  	  

/* ---
    MESH GEOMETRY
               --- */

void MeshGeometry( Mesh *mp )
{
  int i,j,k;
  double x[4],y[4],z[4];
  double sixth = 1.0/6.0;
  double x10,x20,x30,y10,y20,y30,
         z10,z20,z30,x21,y21,z21, 
         xn,yn,zn;

  for( i=0; i<mp->n_el; ++i )
  {
    for( j=0; j<4; ++j )
    {
      Node *np = mp->el_list[i].nd[j];
      x[j] = np->x[0];      
      y[j] = np->x[1];      
      z[j] = np->x[2];      
    }	    
    x10 = x[1]-x[0];  y10 = y[1]-y[0];  z10 = z[1]-z[0];
    x20 = x[2]-x[0];  y20 = y[2]-y[0];  z20 = z[2]-z[0];
    x30 = x[3]-x[0];  y30 = y[3]-y[0];  z30 = z[3]-z[0];
   
    mp->el_list[i].volume = ( - x10*( y20*z30 - y30*z20 )
                              + x20*( y10*z30 - y30*z10 )
                              - x30*( y10*z20 - y20*z10 ) )*sixth;
  }

  for( i=0; i<mp->n_fa; ++i )
  {
    for( j=0; j<3; ++j )
    {
      Node *np = mp->fa_list[i].nd[j];
      x[j] = np->x[0];      
      y[j] = np->x[1];      
      z[j] = np->x[2];      
    }
    x10 = x[1]-x[0]; y10 = y[1]-y[0]; z10 = z[1]-z[0];
    x21 = x[2]-x[1]; y21 = y[2]-y[1]; z21 = z[2]-z[1];

    xn = y10*z21 - z10*y21;
    yn = z10*x21 - x10*z21;
    zn = x10*y21 - y10*x21;    

    mp->fa_list[i].normal[0] = xn;
    mp->fa_list[i].normal[1] = yn;
    mp->fa_list[i].normal[2] = zn;    

    mp->fa_list[i].area = 0.5*sqrt( xn*xn + yn*yn + zn*zn );
  }	  
	
  return;
}	

/* ---
    CFL BASED TIMESTEP SELECTION 
                              --- */

double cfl_timestep( Mesh *mp, Data *data, double CFL, double mod_a )
{
  int jf;
  double h_min = 1.0e10;

  for( jf=0; jf<mp->n_fa; ++jf )
  {
    double area = mp->fa_list[jf].area;

    Element *el = mp->fa_list[jf].el[0];
    double volume = el->volume;
    double h_el = 3.0*volume/area;
   
    h_min = MIN( h_min, h_el );

    if( mp->fa_list[jf].el[1] != NULL )
    {
      Element *el = mp->fa_list[jf].el[1];
      double volume = el->volume;
      double h_el = 3.0*volume/area;
   
      h_min = MIN( h_min, h_el );
    }
  }
  return( CFL*h_min/mod_a );
}	

/* ---
    SIMPLE ERROR ESTIMATE USING FIRST DERIVATIVES
                                               --- */

void error_estimate( Mesh *mp, Data *data )
{
  int ip, jf, j,k;

  double *h, *mass, **gradu;

  double e_max = 0.0;

/* ALLOCATE MEMORY */

  h = (double *)calloc( mp->n_nd, sizeof(double) );
  mass = (double *)calloc( mp->n_nd, sizeof(double) );
  gradu = (double **)calloc( mp->n_nd, sizeof(double *) );
  for( ip=0; ip<mp->n_nd; ++ip )
    gradu[ip] = (double *)calloc( 3, sizeof(double) );

/* INITIALISE */

  for( ip=0; ip<mp->n_nd; ++ip )
  {
    h[ip] = 1.0e10;	  
    mass[ip] = 0.0;  
    for( k=0; k<3; ++k )
      gradu[ip][k] = 0.0;
  }  

/* ASSEMBLE */

  for( jf=0; jf<mp->n_fa; ++jf )
  {
    double area = mp->fa_list[jf].area;

    int k, kf[3],ke[4], kft,ket, kp;
    Element *el = mp->fa_list[jf].el[0];
    int ie = el->id;
    double volume = el->volume;
    double h_el = 3.0*volume/area;
   
    for( k=0; k<3; ++k )
    {
      Node *np = mp->fa_list[jf].nd[k];
      kf[k] = np->id;
    }
    kft = kf[0] + kf[1] + kf[2];

    for( k=0; k<4; ++k )
    {
      Node *np = el->nd[k];
      ke[k] = np->id;
    }
    ket = ke[0] + ke[1] + ke[2] + ke[3];

    kp = ket - kft;

    h[kp] = MIN( h[kp], h_el );
    mass[kp] += volume;
    for( j=0; j<3; ++j )
      gradu[kp][j] += mp->fa_list[jf].normal[j]*data->u[ie];

    if( mp->fa_list[jf].el[1] != NULL )
    {
      Element *el = mp->fa_list[jf].el[1];
      int ie = el->id;
      double volume = el->volume;
      double h_el = 3.0*volume/area;
   
      for( k=0; k<4; ++k )
      {
        Node *np = el->nd[k];
        ke[k] = np->id;
      }
      ket = ke[0] + ke[1] + ke[2] + ke[3];

      kp = ket - kft;

      h[kp] = MIN( h[kp], h_el );
      mass[kp] += volume;
      for( j=0; j<3; ++j )
        gradu[kp][j] -= mp->fa_list[jf].normal[j]*data->u[ie];
    }
    else
    {
      for( k=0; k<3; ++k )
      {
        Node *np = mp->fa_list[jf].nd[k];
        kp = np->id;

        for( j=0; j<3; ++j )
          gradu[kp][j] += mp->fa_list[jf].normal[j]*data->u[ie];
      }      
    }	    
  }

/* COMPUTE */

  for( ip=0; ip<mp->n_nd; ++ip )
  {	  
    double rm = 1.0/mass[ip];
    double dudx = gradu[ip][0]*rm;
    double dudy = gradu[ip][1]*rm;
    double dudz = gradu[ip][2]*rm;

    double e = h[ip]*sqrt( dudx*dudx + dudy*dudy + dudz*dudz );

    e_max = MAX( e_max, e );

    data->error[ip] = e;
  }  

  e_max = 1.0/MAX( e_max, 1.0e-6 );

  for( ip=0; ip<mp->n_nd; ++ip )
    data->error[ip] *= e_max;

/* FREE MEMORY */

  for( ip=0; ip<mp->n_nd; ++ip )
    free( gradu[ip] );
  free( gradu );
  free( mass );
  free( h );

  return;
}

/* ---
    MODIFY GRID FOR BOTTOM PROFILE
                                --- */

void topography( Mesh *mp, 
                 int nx, int ny, int nz,
                 double X1, double Y1, double Z1,
                 double X2, double Y2, double Z2 )
{	
  int ix, iy, iz;

  double dx = (X2-X1)/(float)nx;
  double dy = (Y2-Y1)/(float)ny;

  for( ix=0; ix<nx; ++ix )
  {  
    double x = X1 + dx*ix;

    for( iy=0; iy<ny; ++iy )
    {  
      double y = Y1 + dy*iy;

      double z0 = Z1 + 0.2*exp(-10.0*(x-2.0)*(x-2.0));
      double dz = (Z2-z0)/(float)nz;

      for( iz=0; iz<nz; ++iz )
      {
        double z = z0 + dz*iz;	      
	mp->nd_list[ ix + nx*iy + nx*ny*iz ].x[2] = z;
      }
    }
  }
  return;	  
}

