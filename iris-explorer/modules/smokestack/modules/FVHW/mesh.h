/*  

Written by M.Walkley  markw@comp.leeds.ac.uk 
10/01

Anything free comes with no guarantee.

*/




/* --
    BOUNDARY CONDITION MARKERS
                             -- */

#define NOT_BOUNDARY  0
#define DIRICHLET     1
#define INTEGRAL      2

#define INFLOW        3
#define WALL          4
#define OUTFLOW       5


/* --
    NODE
       -- */

typedef struct fv_node
{
    double   x[3];

    int      id;
}
Node;

/* --
    ELEMENT
          -- */

typedef struct fv_element
{
    Node    *nd[4];

    double   volume;

    int      id;
    int      type;
}
Element;

/* --
    FACE
       -- */

typedef struct fv_face
{
    Node     *nd[3];
    Element  *el[2];

    double    area;
    double    normal[3];

    int       id;
    int       bc_type;
}
Face;

/* --
    BOUNDARY
           -- */

typedef struct fv_boundary
{
    Node    *nd[3];
    Element *el;

    int      id;
    int      bc_type;
}
Boundary;

/* --
    MESH
       -- */

typedef struct fv_mesh
{
    int        n_nd;
    int        n_el;
    int        n_fa;
    int        n_by;

    Node      *nd_list;
    Element   *el_list;
    Face      *fa_list;
    Boundary  *by_list;
}
Mesh;

/* --
    SOLVER
         -- */

typedef struct fv_data
{
    double *u;
    double *rh;
    double *flux;    
    double *udot;
    double  delta_t;
    double *mass;
    double *error;
}
Data;

/* --
    MISC MACROS 
              -- */

#define MAX(a,b) ((a)>(b) ? (a):(b) )
#define MIN(a,b) ((a)<(b) ? (a):(b) )
#define ABS(a)   ((a)<(0.0) ? (-(a)):(a) )

