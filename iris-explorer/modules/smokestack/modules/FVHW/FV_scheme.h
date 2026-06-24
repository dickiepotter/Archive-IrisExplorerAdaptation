/*  

Written by M.Walkley  markw@comp.leeds.ac.uk 
10/01

Anything free comes with no guarantee.

*/

void ProblemRHS( Mesh *mp, Data *data );
void InitialConditions( Mesh *mp, Data *data ) ;

void GetFlux( Mesh *mp, Data *data, 
              double convectX, double convectY, double convectZ );
void TimeDerivatives( Mesh *mp, Data *data );
void Update( Mesh *mp, Data *data, double timestep );

void FaceList( Mesh *mp );
int min_node( int kf[2] );
int find_face( Mesh *mp, int *start, int *count, int kf[3], int kp );
int add_face( Mesh *mp, int *start, int *count, int kf[3], int kp, int ie );
int add_bface( Mesh *mp, int *start, int *count, int kf[3], int kp, int bc );

void MeshGeometry( Mesh *mp );

double cfl_timestep( Mesh *mp, Data *data, double CFL, double mod_a );

void error_estimate( Mesh *mp, Data *data );

void topography( Mesh *mp,
                 int nx, int ny, int nz,
                 double X1, double Y1, double Z1,
                 double X2, double Y2, double Z2 );
