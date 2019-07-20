#include <GL/freeglut.h>
#include <math.h>
#include <iostream>
#include <string>
using namespace std;
float posCameraX,posCameraY,posCameraZ, solX,solY,solZ, raio, angulo, spinX,spinY,spinZ, h,dh, h_max, h_min, v;
GLfloat luz_pontual[] = { 0.0, 1.0, 50.0, 1.0 };



void iluminar(){
   //LUZ
   // no mínimo 8 fontes podem ser utilizadas 
   //(iniciadas com cor preta)
   // número de fontes de luz afeta performance
    
   //LUZ 0
   
   //define características para a fonte de luz 0	
   //fonte de luz direcional (por que w==0?)
   GLfloat light0_position[] = { 0.0, 1.0, 0.0, 0.0 };
   GLfloat light0_diffuse[] = { 0.5, 0.5, 0.5, 1.0 };
   //atribui características para a fonte de luz 0
   //cor padrão: branco
   //glLightfv(GL_LIGHT2, GL_POSITION, light0_position);
   //glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
   glLightfv(GL_LIGHT2, GL_AMBIENT, light0_diffuse); 
   //LUZ 1
   //fonte de luz pontual
   //define características para a fonte de luz 1
   GLfloat light1_diffuse[] = { 0.6, 0.6, 0.6, 1.0 };
   GLfloat light1_specular[] = { 1.0, 1.0, 1.0, 1.0 };
   GLfloat light1_ambient[] = { 0.1, 0.1, 0.1, 1.0 };
   
   //atribui as características para a fonte de luz 1
   //(experimentem remover alguns dos componentes abaixo)
   glLightfv(GL_LIGHT1, GL_POSITION, luz_pontual);
   glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
   glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
   glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient); 
   
   //"acende" cada uma das luzes configuradas
   glEnable(GL_LIGHT2);
   glEnable(GL_LIGHT1);
}
void desenhar_luz(){
   glPushAttrib (GL_LIGHTING_BIT);
   
   GLfloat mat_diffuse[] = { 1.0, 1.0, 0.0, 1.0 };
   GLfloat mat_emission[] = { 1.0, 1.0, 0.0, 1.0 };
          
   //atribui características ao material
   glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
   glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
 
   glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_diffuse);
    
   glPushMatrix();
   glTranslatef(luz_pontual[0],luz_pontual[1],luz_pontual[2]); 
   
   glEnable(GL_LIGHTING);
   glColor3f (1.0, 1.0, 0.0);
   glutSolidSphere(0.7,50,50);
   glDisable(GL_LIGHTING);
   
   glPopAttrib();
   glPopMatrix();
}

void desenhar_objeto(){	
   //MATERIAL
   //define características para aparência do material	
   //exercício: testar exemplos da seção 
   //Changing Material Properties, do Red Book 
   GLfloat mat_specular[] = {  0, 1, 1.0, 1.0 };
   GLfloat mat_diffuse[] = {0.1, 0.8, 1.0, 1.0 };
   GLfloat mat_shininess[] = { 100.0 };
          
   glPushAttrib (GL_LIGHTING_BIT);
   
   //atribui características ao material
   glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
   glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
   glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
   
   glEnable(GL_LIGHTING);
   glColor3f (0.1, 0.8, 0.0);
   glutSolidSphere (raio, 420, 16);   
   glDisable(GL_LIGHTING);
   
   glPopAttrib();
}
void desenhar_eixos(){
   //glEnable(GL_LIGHTING);
    //não há efeitos de iluminação nos eixos
   float size = 5.0;
	glLineWidth(3);
    glBegin(GL_LINES);
        glColor3f (1.0, 0.0, 0.0);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(size, 0.0, 0.0);
        
        glColor3f (0.0, 1.0, 0.0);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(0.0, size, 0.0);
      
        glColor3f (0.0, 0.0, 1.0);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(0.0, 0.0, size);    
    glEnd(); //glDisable(GL_LIGHTING);
	}  
void init(void) 
{
   //cout << "Please enter the radius of the planet(ex: 0.5): ";
   //cin >> raio; //ex: 0.5
   //cout << "Please enter the sun's position(ex: 25 0 -25): ";
   //cin >> solX >> solY >> solZ; //ex: 0 1 50
   raio = 3.0;
   solX = 0;
   solY = 6.5;
   solZ = -5;
   luz_pontual[0] = solX;
   luz_pontual[1] = solY;
   luz_pontual[2] = solZ;
   luz_pontual[3] = 1.0;
   solX = 0;
   solY = 10;
   solZ = 10;
   spinX = 1.0;
   spinY = 1.0;
   spinZ = 1.0;
   posCameraX = 0.3;
   posCameraY = 0.1;
   posCameraZ = 0;
   h = 1.0;
   h_max = 2.0;
   h_min = 1.0;
   dh = 0.0;
   v = 0.0;
   glClearColor (0.0, 0.0, 0.0, 0.0);
   iluminar();
   glEnable(GL_DEPTH_TEST);
   glShadeModel (GL_SMOOTH);

   

}

void atualiza_spin(float* spin, float inc){
   *spin += inc;
   if (*spin > 360.0)
      *spin = *spin - 360.0;
   
}
void spinDisplay(void)
{  
   if( h == h_max){
      atualiza_spin(&spinX, v);
   }
   glutPostRedisplay();
}
void specialKeys(int key, int x, int y)
{  
  
   if(h  == h_max){
      switch (key) {
         case GLUT_KEY_LEFT : 
               atualiza_spin(&spinZ, 2.0);
               break;
         case GLUT_KEY_RIGHT : 
               atualiza_spin(&spinZ, -2.0);                  
               break;     
         case GLUT_KEY_UP : 
               v += 0.01;
               if(v == 5)
                  v = 5;               
               break;     
         case GLUT_KEY_DOWN : 
               if( v > 0.01)
                  v -= 0.01;
               else
                  v = 0;

               cout << v << endl;
               break;     
         
      }
      glutPostRedisplay();
   }
}


void display(void)
{
   //limpeza do zbuffer deve ser feita a cada desenho da tela
   glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   
   
   //gluLookAt (posCameraX, posCameraY, posCameraZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
   
   
   //glTranslatef(0.0, 0.0,0.0);
   //glPushMatrix();
      //glTranslatef(0.0, 0.2,0.0);
      gluLookAt (0.0, raio + h, 1.0, 0.0, 0.0, -5.0, 0.0, 1.0, 0.0);	
   //glPopMatrix();

   glRotatef(spinX, 1, 0, 0);
   glRotatef(spinY, 0, 1, 0);
   glRotatef(spinZ, 0, 0, 1);
   glLightfv(GL_LIGHT1, GL_POSITION, luz_pontual);
   
   desenhar_luz(); 
   //glPopMatrix();
   //glPopMatrix();
   //glPopMatrix();
   desenhar_eixos();
   desenhar_objeto();
    
   glutSwapBuffers();

}
void update_h(int time){
   if(dh != 0){
      h += dh;
      if(h > h_max){
         h = h_max;
         dh = 0;
      }
      if(h < h_min){
         h = h_min;
         dh = 0;
      }
   }
   glutPostRedisplay();
   glutTimerFunc(10, update_h,1);
}

void keyboard(unsigned char key, int x, int y){
   if(dh == 0){
      switch (key)
      {
      case 'd':
         if(h == h_min){
            dh = 0.01;
            glutIdleFunc(spinDisplay);
         }
        break;
      case 's':
         if(h == h_max){
            dh = -0.01;
            glutIdleFunc(NULL);
         }
         break;
      default:
         break;
      }
   }
  
}

void reshape (int w, int h)
{
   glViewport (0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity();
   //ortográfica
   /**
   if (w <= h)
      glOrtho (-1.5, 1.5, -1.5*(GLfloat)h/(GLfloat)w,
         1.5*(GLfloat)h/(GLfloat)w, -10.0, 10.0);
   else
      glOrtho (-1.5*(GLfloat)w/(GLfloat)h,
         1.5*(GLfloat)w/(GLfloat)h, -1.5, 1.5, -10.0, 10.0);
   */
   //perspectiva
   gluPerspective(90.0, w/h, 0.3, 1000.0);
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
   glutKeyboardFunc(keyboard);
   glutSpecialFunc(specialKeys);
   glutReshapeFunc(reshape);
   glutTimerFunc(10, update_h,1);
   glutMainLoop();
   return 0;
}
