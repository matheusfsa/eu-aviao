#include <GL/freeglut.h>
#include <math.h>
float posCameraX,posCameraY,posCameraZ, raio, angulo;

void init (void) 
{
/*  select clearing (background) color       */
    glClearColor (0.0, 0.0, 0.0, 0.0);
    posCameraX = 0.3;
    posCameraY = 0.1;
    posCameraZ = 0.1;
/*  initialize viewing values  */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-2.0, 2.0, -2.0, 2.0, -1, 100.0);
    //gluPerspective(120.0, 1, -0.5, 100.0);
    
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
   glClear (GL_COLOR_BUFFER_BIT);
 
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   gluLookAt (posCameraX, posCameraY, posCameraZ, 0.0, 0.0, 0.0, 0.0, 1.5, 0.0);
   glColor3f (0.2, 0.3, 1.0);
   glutSolidSphere(0.5,20,20);

   glPushMatrix();

   glTranslatef(0.0,  1.0, -1.0);
    
   glColor3f (1.0, 1.0, 0.0);
   glutSolidSphere(0.2,20,20);
   
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
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize (500, 500); 
    glutInitWindowPosition (500, 100);
    glutCreateWindow (argv[0]);
    init();
    glutDisplayFunc(display); 
    glutSpecialFunc(specialKeys);
    glutMainLoop();
    
}