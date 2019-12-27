#include "definitions.h"
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>
#include <math.h>
#include "io.h"

/** EXTERNAL VARIABLES **/

extern GLdouble _window_ratio;
extern GLdouble _ortho_x_min,_ortho_x_max;
extern GLdouble _ortho_y_min,_ortho_y_max;
extern GLdouble _ortho_z_min,_ortho_z_max;

extern object3d *_first_object;
extern object3d *_selected_object;
extern lista_camera *_selected_camera;
extern objetos_luz global_lights[];
extern GLint flat_smooth;  

//int index1, index2, index3;
vector3 normal;

/*void init(void){
   GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
   GLfloat mat_shininess[] = { 50.0 };
   GLfloat light_position[] = { -1.0, -1.0, -1.0, 0.0 };
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel (GL_SMOOTH);

   glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
   glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
   glLightfv(GL_LIGHT0, GL_POSITION, light_position);

   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glEnable(GL_DEPTH_TEST);
}*/


vector3 calculate_surface_normal(int index1, int index2, int index3, vertex *vertex_table){
    vector3 normal_vector;
    
    vector3 u;
    u.x = vertex_table[index2].coord.x - vertex_table[index1].coord.x;
    u.y = vertex_table[index2].coord.y - vertex_table[index1].coord.y;
    u.z = vertex_table[index2].coord.z - vertex_table[index1].coord.z;

    vector3 v;
    v.x = vertex_table[index3].coord.x - vertex_table[index1].coord.x;
    v.y = vertex_table[index3].coord.y - vertex_table[index1].coord.y;
    v.z = vertex_table[index3].coord.z - vertex_table[index1].coord.z;

    normal_vector.x = (u.y * v.z) - (u.z - v.y);
    normal_vector.y = (u.z * v.x) - (u.x - v.z);
    normal_vector.z = (u.x * v.y) - (u.y * v.x);

    return normal_vector;
}

void normal_vectors(){
    GLint f, v, v_index, i;
    GLint index1, index2, index3;
    GLfloat norma;
    vector3 vector_normal;
    vector3 vector_normal_init;
    vector_normal_init.x = 0; 
    vector_normal_init.y = 0; 
    vector_normal_init.z = 0;

    for (i = 0; i < _selected_object->num_vertices; i++)
        _selected_object->vertex_table[i].normal_vector = vector_normal_init;
    
    for (f = 0; f < _selected_object->num_faces; f++){
        index1 = _selected_object->face_table[f].vertex_table[0];
        index2 = _selected_object->face_table[f].vertex_table[1];
        index3 = _selected_object->face_table[f].vertex_table[2];

        vector_normal = calculate_surface_normal(index1, index2, index3, _selected_object->vertex_table);

        _selected_object->face_table[f].normal_vector = vector_normal;

        norma = sqrt(pow(_selected_object->face_table[f].normal_vector.x, 2) +
        pow(_selected_object->face_table[f].normal_vector.y, 2) +
        pow(_selected_object->face_table[f].normal_vector.z, 2));

        _selected_object->face_table[f].normal_vector.x /= norma;
        _selected_object->face_table[f].normal_vector.y /= norma;
        _selected_object->face_table[f].normal_vector.z /= norma;

        for (v = 0; v < _selected_object->face_table[f].num_vertices; v++){
            v_index = _selected_object->face_table[f].vertex_table[v];
            _selected_object->vertex_table[v_index].normal_vector.x += _selected_object->face_table[f].normal_vector.x;
            _selected_object->vertex_table[v_index].normal_vector.y += _selected_object->face_table[f].normal_vector.y;
            _selected_object->vertex_table[v_index].normal_vector.z += _selected_object->face_table[f].normal_vector.z; 
        }
    }

    for (i = 0; i < _selected_object->num_vertices; i++){
        norma = sqrt(pow(_selected_object->vertex_table[i].normal_vector.x, 2) +
        pow(_selected_object->vertex_table[i].normal_vector.y, 2) +
        pow(_selected_object->vertex_table[i].normal_vector.z, 2));

        _selected_object->vertex_table[i].normal_vector.x /= norma;
        _selected_object->vertex_table[i].normal_vector.y /= norma;
        _selected_object->vertex_table[i].normal_vector.z /= norma;
    }
}

/**
 * @brief Function to draw the axes
 */
void draw_axes()
{
    /*Draw X axis*/
    glColor3f(KG_COL_X_AXIS_R,KG_COL_X_AXIS_G,KG_COL_X_AXIS_B);
    glBegin(GL_LINES);
    glVertex3d(KG_MAX_DOUBLE,0,0);
    glVertex3d(-1*KG_MAX_DOUBLE,0,0);
    glEnd();
    /*Draw Y axis*/
    glColor3f(KG_COL_Y_AXIS_R,KG_COL_Y_AXIS_G,KG_COL_Y_AXIS_B);
    glBegin(GL_LINES);
    glVertex3d(0,KG_MAX_DOUBLE,0);
    glVertex3d(0,-1*KG_MAX_DOUBLE,0);
    glEnd();
    /*Draw Z axis*/
    glColor3f(KG_COL_Z_AXIS_R,KG_COL_Z_AXIS_G,KG_COL_Z_AXIS_B);
    glBegin(GL_LINES);
    glVertex3d(0,0,KG_MAX_DOUBLE);
    glVertex3d(0,0,-1*KG_MAX_DOUBLE);
    glEnd();
}


/**
 * @brief Callback reshape function. We just store the new proportions of the window
 * @param width New width of the window
 * @param height New height of the window
 */
void reshape(int width, int height) {
    glViewport(0, 0, width, height);

    /*  Take care, the width and height are integer numbers, but the ratio is a GLdouble so, in order to avoid
     *  rounding the ratio to integer values we need to cast width and height before computing the ratio */
    _window_ratio = (GLdouble) width / (GLdouble) height;
}


/**
 * @brief Callback display function
 */
void display(void) {
    GLint v_index, v, f, i;
    object3d *aux_obj = _first_object;

    /* Clear the screen */
    glClear(GL_DEPTH_BUFFER_BIT| GL_COLOR_BUFFER_BIT); 

    /* Define the projection */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if (_selected_camera->current_camera->proj->type == PROJECTION_PERSP){
        glFrustum(_selected_camera->current_camera->proj->left,
        _selected_camera->current_camera->proj->right,
        _selected_camera->current_camera->proj->bottom,
        _selected_camera->current_camera->proj->top,
        _selected_camera->current_camera->proj->near,
        _selected_camera->current_camera->proj->far);
    } else {
        glOrtho(_selected_camera->current_camera->proj->left * _window_ratio,
        _selected_camera->current_camera->proj->right * _window_ratio,
        _selected_camera->current_camera->proj->bottom,
        _selected_camera->current_camera->proj->top,
        _selected_camera->current_camera->proj->near,
        _selected_camera->current_camera->proj->far);    
    }

  

    /*First, we draw the axes*/
    //draw_axes();

    /* Now we start drawing the object */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glLoadMatrixf(_selected_camera->current_camera->m);

    for (i = 0; i < 8; i++){
        if (global_lights[i].is_on == 1){
            glPushMatrix();
            glMultMatrixf(global_lights[i].m_obj);
            put_light(i);
            glPopMatrix();
        }
    }

    /*Now each of the objects in the list*/
    while (aux_obj != 0) {

        /* Select the color, depending on whether the current object is the selected one or not */
        if (aux_obj == _selected_object){
            glColor3f(KG_COL_SELECTED_R,KG_COL_SELECTED_G,KG_COL_SELECTED_B);
            
        }else{
            glColor3f(KG_COL_NONSELECTED_R,KG_COL_NONSELECTED_G,KG_COL_NONSELECTED_B);

        }
        glMaterialfv(GL_FRONT, GL_AMBIENT, aux_obj->material_light->m_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, aux_obj->material_light->m_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, aux_obj->material_light->m_specular);
        glMaterialfv(GL_FRONT, GL_SHININESS, aux_obj->material_light->no_shininess);
        
        
        /* Draw the object; for each face create a new polygon with the corresponding vertices */
        glPushMatrix();

        glMultMatrixf(aux_obj->list_matrix->m);

        for (f = 0; f < aux_obj->num_faces; f++) {
            glBegin(GL_POLYGON);

            if (aux_obj->flat_smooth == 0){
                glNormal3d(aux_obj->face_table[f].normal_vector.x,
                aux_obj->face_table[f].normal_vector.y,
                aux_obj->face_table[f].normal_vector.z);
            }

            for (v = 0; v < aux_obj->face_table[f].num_vertices; v++) {

                v_index = _selected_object->face_table[f].vertex_table[v];

                if (aux_obj->flat_smooth == 1){
                    glNormal3d(aux_obj->vertex_table[v_index].normal_vector.x,
                    aux_obj->vertex_table[v_index].normal_vector.y,
                    aux_obj->vertex_table[v_index].normal_vector.z);
                }

                glVertex3d(aux_obj->vertex_table[v_index].coord.x,
                aux_obj->vertex_table[v_index].coord.y,
                aux_obj->vertex_table[v_index].coord.z);
            }


            glEnd();
        }
        

        glPopMatrix();
        aux_obj = aux_obj->next;
    }
    /*Do the actual drawing*/
    glFlush();
    glutSwapBuffers();
}
