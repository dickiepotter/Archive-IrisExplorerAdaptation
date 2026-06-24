/*  

Written by M.Walkley  markw@comp.leeds.ac.uk 
10/01

Anything free comes with no guarantee.

*/



double main_func(int reset,
              double timeZero, double timeMax, double CFL,
              double convectX,double convectY, double convectZ,
              int update, int output, int n,
              double X1, double Y1, double Z1,
              double X2, double Y2, double Z2 );
void OutputPyramid( Mesh *mp, Data *data );
int memclean(cxPyramid *pyr, cxErrorCode ec, cxConnection *conn);

void Setup( Mesh *mp, Data *data, int n,
	    double X1, double Y1, double Z1,
            double X2, double Y2, double Z2 );	    
void Cleanup( Mesh *mp, Data *data );
void Timestep( Mesh *mp,  Data *data,
               int *step, int call_back_step, int output, int *step_out_last,
               double convectX, double convectY, double convectZ,
               double *time, double timestep, double timeMax );

void WritePyramid( Mesh *mp, Data *data, int i_out );

void mesh3d( Mesh *mp, int n,
             int *pnx, int *pny, int *pnz,
	     double *X1, double *Y1, double *Z1,
             double *X2, double *Y2, double *Z2 );

