#include <GL/freeglut.h>
#include <math.h>
#include <iostream>
#include <string>
using namespace std;
float posCameraX,posCameraY,posCameraZ, raio, angulo;

void init (void) 
{
    cout << "Please enter the radius of the planet: ";
    cin >> raio; //ex: 0.5
    cout << "Please enter the camera's starting X Y Z: ";
    cin >> posCameraX >> posCameraY >> posCameraZ; //ex: 2 1 1
   
    /*  initialize viewing values  */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-2.0, 2.0, -2.0, 2.0, -1, 100.0);
    //gluPerspective(120.0, 1, 0.5, 1000.0);
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity();
     
    glClearColor (0.0, 0.0, 0.0, 0.0);
    glShadeModel (GL_SMOOTH);

    GLfloat light_position[] = {1.0, 0.0, 0.0, 1.0};
    GLfloat light_diffuse[] = {1.0, 1.0, 0.0, 1.0};
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);

    GLfloat lmodel_ambient[] = { 0.3, 0.3, 0.3, 1.0 };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
        
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
}
void specialKeys(int key, int x, int y)
{
   float angulo = 2*M_PI/180;
   switch (key) {
       case GLUT_KEY_LEFT : 
            posCameraX =  posCameraX*cos(-angulo) + posCameraZ*sin(-angulo);
            posCameraZ = -posCameraX*sin(-angulo) + posCameraZ*cos(-angulo);
            
            break;
       case GLUT_KEY_RIGHT : 
            posCameraX =  posCameraX*cos(angulo) + posCameraZ*sin(angulo);
            posCameraZ = -posCameraX*sin(angulo) + posCameraZ*cos(angulo);                      
            break;     
      case GLUT_KEY_UP : 
            posCameraX =  posCameraX*cos(angulo) + posCameraY*sin(angulo);
            posCameraY = -posCameraX*sin(angulo) + posCameraY*cos(angulo);                      
            break;       
   }
   glutPostRedisplay();
}
void display(void)
{
   glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   gluLookAt (posCameraX, posCameraY, posCameraZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
   glColor3f (0.2, 0.3, 1.0);
   glutSolidSphere(raio,20,20);
   glPushMatrix();
   	glTranslatef(0.0,  1.0, -1.0);   	 
        glColor3f (1.0, 1.0, 0.0);
   	glutSolidSphere(0.2,20,20);
        //GLfloat light_position[] = {0.0, 0.0, 0.0, 1.0};
	//GLfloat light_diffuse[] = {1.0, 1.0, 0.0, 1.0};
	//glLightfv(GL_LIGHT1, GL_POSITION, light_position);
	//glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
   glPopMatrix();
   
   glBegin(GL_LINES);
      glColor3f(1.0, 0.0, 0.0);
      glVertex3f(0.0, 0.0, 0.0);
      glVertex3f(1.0, 0.0, 0.0);

      glColor3f(0.0, 1.0, 0.0);
      glVertex3f(0.0, 0.0, 0.0);
      glVertex3f(0.0, 1.0, 0.0);
      
      glColor3f(0.0, 0.0, 1.0);
      glVertex3f(0.0, 0.0, 0.0);
      glVertex3f(0.0, 0.0, 1.0);
   glEnd();
   //troca de buffers, o flush é implícito aqui
   glutSwapBuffers();
}

int main(int argc, char** argv){
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize (500, 500); 
    glutInitWindowPosition (500, 100);
    glutCreateWindow (argv[0]);

    init();
    glutDisplayFunc(display); 
    glutSpecialFunc(specialKeys);
    glutMainLoop();
    
}
