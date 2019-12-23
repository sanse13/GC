#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <GL/gl.h>

/** DEFINITIONS **/

#define KG_WINDOW_TITLE                     "Practica GPO"
#define KG_WINDOW_WIDTH                     600
#define KG_WINDOW_HEIGHT                    400
#define KG_WINDOW_X                         50
#define KG_WINDOW_Y                         50

#define KG_MSSG_SELECT_FILE                 "Idatz ezazu fitxategiaren path-a: "
#define KG_MSSG_FILENOTFOUND                "Fitxategi hori ez da existitzen!!"
#define KG_MSSG_INVALIDFILE                 "Arazoren bat egon da fitxategiarekin ..."
#define KG_MSSG_EMPTYFILE                   "Fitxategia hutsik dago"
#define KG_MSSG_FILEREAD                    "Fitxategiaren irakurketa buruta"

#define KG_STEP_MOVE                        5.0f
#define KG_STEP_ROTATE                      10.0f
#define KG_STEP_ZOOM                        0.75
#define KG_STEP_CAMERA_ANGLE                5.0f

#define KG_ORTHO_X_MIN_INIT                -5
#define KG_ORTHO_X_MAX_INIT                 5
#define KG_ORTHO_Y_MIN_INIT                -5
#define KG_ORTHO_Y_MAX_INIT                 5
#define KG_ORTHO_Z_MIN_INIT                -100
#define KG_ORTHO_Z_MAX_INIT                 10000

#define KG_COL_BACK_R                       0.30f
#define KG_COL_BACK_G                       0.30f
#define KG_COL_BACK_B                       0.30f
#define KG_COL_BACK_A                       1.00f

#define KG_COL_SELECTED_R                   1.00f
#define KG_COL_SELECTED_G                   0.75f
#define KG_COL_SELECTED_B                   0.00f

#define KG_COL_NONSELECTED_R                1.00f
#define KG_COL_NONSELECTED_G                1.00f
#define KG_COL_NONSELECTED_B                1.00f

#define KG_COL_X_AXIS_R                     1.0f
#define KG_COL_X_AXIS_G                     0.0f
#define KG_COL_X_AXIS_B                     0.0f

#define KG_COL_Y_AXIS_R                     0.0f
#define KG_COL_Y_AXIS_G                     1.0f
#define KG_COL_Y_AXIS_B                     0.0f

#define KG_COL_Z_AXIS_R                     0.0f
#define KG_COL_Z_AXIS_G                     0.0f
#define KG_COL_Z_AXIS_B                     1.0f

#define KG_MAX_DOUBLE                       10E25

#define PROJECTION_PERSP 1
#define PROJECTION_ORT 2

#define SOL 0
#define BOMBILLA 1
#define FOCO 2
#define FOCO_OBJETO 3

#define FLAT 0
#define SMOOTH 1
/** STRUCTURES **/

/****************************
 * Structure to store the   *
 * coordinates of 3D points *
 ****************************/
typedef struct {
    GLdouble x, y, z;
} point3;

/*****************************
 * Structure to store the    *
 * coordinates of 3D vectors *
 *****************************/
typedef struct {
    GLdouble x, y, z;
} vector3;

/****************************
 * Structure to store the   *
 * colors in RGB mode       *
 ****************************/
typedef struct {
    GLfloat r, g, b;
} color3;

/****************************
 * Structure to store       *
 * objects' vertices         *
 ****************************/
typedef struct {
    point3 coord;                       /* coordinates,x, y, z */
    GLint num_faces;                    /* number of faces that share this vertex */
    vector3 normal_vector;
} vertex;

/****************************
 * Structure to store       *
 * objects' faces or        *
 * polygons                 *
 ****************************/
typedef struct {
    GLint num_vertices;                 /* number of vertices in the face */
    GLint *vertex_table;                /* table with the index of each vertex */
    vector3 normal_vector;
} face;


typedef struct list_matrix
{
    GLfloat m[16];
    struct list_matrix *nextptr;
} list_matrix;


typedef struct projection
{
    GLint type;
    GLfloat angle;
    GLfloat near;
    GLfloat far;
    GLfloat bottom;
    GLfloat top;
    GLfloat left;
    GLfloat right;
} projection;


typedef struct camera
{
    projection *proj;
    GLfloat m[16];
    GLfloat m_invert[16];
} camera;

typedef struct lista_camera
{
    camera *current_camera;
    struct lista_camera *next;
} lista_camera;

typedef struct transformaciones
{
    vector3 translate;
    vector3 rotate;
    vector3 scale;
} transformaciones;
/****************************
 * Structure to store a     *
 * pile of 3D objects       *
 ****************************/
typedef struct
{
    GLfloat m_diffuse[4];
    GLfloat m_ambient[4];
    GLfloat m_specular[4];
    GLfloat m_position[4];
    GLfloat no_shininess[1];
}material_light;



struct object3d
{
    GLint num_vertices;                 /* number of vertices in the object*/
    vertex *vertex_table;               /* table of vertices */
    GLint num_faces;                    /* number of faces in the object */
    face *face_table;                   /* table of faces */
    point3 min;                         /* coordinates' lower bounds */
    point3 max;                         /* coordinates' bigger bounds */
    struct object3d *next;              /* next element in the pile of objects */
    list_matrix *list_matrix;
    GLint flat_smooth;
    material_light *material_light;
};



typedef struct
{
    GLfloat diffuse[4];
    GLfloat ambient[4];
    GLfloat specular[4];
    GLfloat position[4];
    GLint is_on;
    GLint type;
    GLfloat spot_direction[3];
    GLfloat m_obj[16];
    GLfloat cut_off;
}objetos_luz;



typedef struct object3d object3d;

#endif // DEFINITIONS_H
