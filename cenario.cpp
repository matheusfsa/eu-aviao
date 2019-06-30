#include <GL/freeglut.h>
#include <math.h>
#include <iostream>
#include <string>
using namespace std;
float posCameraX,posCameraY,posCameraZ, solX,solY,solZ, raio, angulo;
void init(void) 
{
   cout << "Please enter the radius of the planet: ";
   cin >> raio; //ex: 0.5
   cout << "Please enter the sun's position: ";
   cin >> solX >> solY >> solZ; //ex: 0 1 -1
   posCameraX = 4;
   posCameraY = 4;
   posCameraZ = 4;

   //GLfloat light_position[] = { -1.0, -1.0, -1.0, 1.0 };
   GLfloat light_diffuse[] = {1.0, 1.0, 0.0, 1.0};
   GLfloat mShiny[] = {120.0};    
   GLfloat lmodel_ambient[] = { 0.1, 0.1, 0.1, 1.0 };
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel (GL_SMOOTH);

   /*  initialize viewing values  */
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(-2.0, 2.0, -2.0, 2.0, -1, 100.0);
   
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   //glLightfv(GL_LIGHT0, GL_POSITION, light_position);
   glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mShiny);
   glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);

   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_COLOR_MATERIAL);

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
   GLfloat light_position[] = { solX, solY, solZ, 1.0 };
   GLfloat light_pos2[] = { 0, 0, 0.0, 1.0 };
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   
   gluLookAt (posCameraX, posCameraY, posCameraZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
   glLightfv(GL_LIGHT0, GL_POSITION, light_position);   
   
   glColor3f (0.7, 0.7, 1.0);
   glutSolidSphere (raio, 20, 16);   

   glPushMatrix();
   	glTranslatef(solX,  solY, solZ);   	 
        glColor3f (1.0, 1.0, 0.0);
   	glutSolidSphere(raio*1.5,20,20);
   glPopMatrix();

   //visualização dos eixos
   glBegin(GL_LINES);
        glColor3f (1.0, 0.0, 0.0);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(1.0, 0.0, 0.0);
        
        glColor3f (0.0, 1.0, 0.0);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(0.0, 1.0, 0.0);
      
        glColor3f (0.0, 0.0, 1.0);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(0.0, 0.0, 1.0);    
    glEnd();
    
   //troca de buffers, o flush é implícito aqui
   glutSwapBuffers();   

}

void reshape (int w, int h)
{
   glViewport (0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity();
   if (w <= h)
      glOrtho (-1.5, 1.5, -1.5*(GLfloat)h/(GLfloat)w,
         1.5*(GLfloat)h/(GLfloat)w, -10.0, 10.0);
   else
      glOrtho (-1.5*(GLfloat)w/(GLfloat)h,
         1.5*(GLfloat)w/(GLfloat)h, -1.5, 1.5, -10.0, 10.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
   glutInitWindowSize (500, 500); 
   glutInitWindowPosition (500, 100);
   glutCreateWindow (argv[0]);
   init ();
   glutDisplayFunc(display); 
   glutSpecialFunc(specialKeys);
   glutReshapeFunc(reshape);
   glutMainLoop();
   return 0;
}
