/*
 *  Lighting and Texture
 *
 *  HW3: Add lighting and texture to HW2 with orthogonal, perspective and first-person projections
 *
 *  Key bindings:
 *  o/O        Cycle through objects (all/land/plane/houses/trees)
 *  t/T        Toggle textures on/off
 *  m/M        Toggle between orthogonal, perspective and first-person views
 *  +/-        Changes field of view for perspective
 *  a/A        Toggle axes
 *  l          Toggle lighting on/off
 *  L          Toggle smooth/flat shading
 *  i/I        Decrease/increase ambient light
 *  d/D        Decrease/increase diffuse light
 *  s/S        Decrease/increase specular light
 *  e/E        Decrease/increase emitted light
 *  n/N        Decrease/increase shininess
 *  [/]        Lower/raise light elevation
 *  arrows     Change view angle of azimuth and elevation
 *  f/F        Move forward in first-person view
 *  b/B        Move backward in first-person view
 *  PgDn/PgUp  Zoom in and out
 *  0          Reset all view angles
 *  ESC        Exit
 */

#include "CSCIx229.h"
// Projection parameters
int axes=1;       //  Display axes
int mode=0;       //  Projection mode
int th=0;         //  Azimuth of view angle rotating around (0,0,1)
int ph=0;         //  Elevation of view angle rotating around (1,0,0)
int step=0;       //  Step counter for first person movement
int t = 0;        //  Step direction for first person movement
int alpha=0;     //  First person view horizontal angle
int beta=0;      //  First person view vertical angle
int fov=55;      //  Field of view (for perspective)
double asp=1;     //  Aspect ratio
double dim=5.0;   //  Size of world
const char* text[] = {"Orthogonal","Perspective","First Person"};
double FV_Ex = 0.0;
double FV_Ey = -3.0;
double FV_Ez = 1;
// Lighting parameters
int light     =   1;  //  Lighting enabled
int smooth    =   1;  //  Smooth/flat shading
int ambient   =  60;  //  Ambient intensity (%)
int diffuse   =  60;  //  Diffuse intensity (%)
int specular  =   0;  //  Specular intensity (%)
int emission  =   0;  //  Emission intensity (%)
int shininess =   0;  //  Shininess (power of two, -1..7)
float shiny   =   1;  //  Shininess (actual GL value)
int zh        =  90;  //  Azimuth of orbiting light
float zlight  =   3;  //  Elevation of light
int obj       =   0;  //  0=all  1=land  2=airplane  3=houses  4=trees

// Texture loading and parameters
unsigned int LoadTexBMP(const char* file);  // defined in loadtexbmp.c
unsigned int texture[9];  //  0=trunk  1=plane  2=water  3=brick  4=wood  5=grass  6=leaf  7=road  8=window
int ntex = 1;  //  Textures on/off

/*
 *  Set projection
 */
static void Project()
{
   //  Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Undo previous transformations
   glLoadIdentity();
   //  Perspective transformation
   if (mode == 0)
      glOrtho(-asp*dim,+asp*dim, -dim,+dim, -dim,+dim);
   //  Orthogonal projection
   else if (mode == 1)
      gluPerspective(fov,asp,dim/4,4*dim);
   //  First person view
   else if  (mode == 2)
      gluPerspective(fov,asp,dim/4,4*dim);

   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();
}

// Draw a land of the world at the center of the world with dimensions (dx,dy,dz)
static void land(double dx, double dy, double dz)
{
   // Draw Grass
   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(0,0,0);
   // glRotated(th,0,0,1);
   glScaled(dx,dy,dz);

   //  Grass — grass texture tiled 6x6 to avoid looking over stretched
   glColor3f(1,1,1);
   glNormal3f(0, 0, 1);
   if (ntex)
   {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, texture[5]);
   }
   glBegin(GL_QUADS);
   glTexCoord2f(0,0); glVertex3f(-1,-1,0);
   glTexCoord2f(6,0); glVertex3f(+1,-1,0);
   glTexCoord2f(6,6); glVertex3f(+1,+1,0);
   glTexCoord2f(0,6); glVertex3f(-1,+1,0);
   glEnd();
   if (ntex) glDisable(GL_TEXTURE_2D);

   //  River and road sit on the same z=0 plane as the grass — use polygon
   //  offset to push them slightly toward the viewer instead of disabling
   //  depth test, so they don't draw over above-ground objects like the plane.
   //  River — offset -1 to sit above grass
   glEnable(GL_POLYGON_OFFSET_FILL);
   glPolygonOffset(-1,-1);
   glColor3f(1,1,1);
   if (ntex)
   {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, texture[2]);
   }
   glBegin(GL_QUADS);
   glNormal3f(0,0,1);
   glTexCoord2f(0,0); glVertex3f(-1,-0.1,0);
   glTexCoord2f(3,0); glVertex3f(+1,-0.1,0);
   glTexCoord2f(3,1); glVertex3f(+1,+0.1,0);
   glTexCoord2f(0,1); glVertex3f(-1,+0.1,0);
   glEnd();
   if (ntex) glDisable(GL_TEXTURE_2D);

   //  Road — offset -2 to sit above river where they cross
   glPolygonOffset(-2,-2);
   glColor3f(1,1,1);
   if (ntex)
   {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, texture[7]);
   }
   glBegin(GL_QUADS);
   glNormal3f(0,0,1);
   glTexCoord2f(0,0); glVertex3f(-0.1,-1,0);
   glTexCoord2f(1,0); glVertex3f(+0.1,-1,0);
   glTexCoord2f(1,4); glVertex3f(+0.1,+1,0);
   glTexCoord2f(0,4); glVertex3f(-0.1,+1,0);
   glEnd();
   if (ntex) glDisable(GL_TEXTURE_2D);

   glDisable(GL_POLYGON_OFFSET_FILL);
   //  Undo transofrmations
   glPopMatrix();

}

static void house(double x,double y,double z, double dx,double dy,double dz, double th) {
   //  Save transformation
   glPushMatrix();

   //  Transform house
   glTranslated(x,y,z);
   glRotated(th,0,0,1);
   glScaled(dx,dy,dz);

   //  Draw Walls — brick texture for each wall face
   if (ntex)
   {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, texture[3]);
   }
   glBegin(GL_QUADS);
   //  Right wall
   glColor3f(1,1,1);
   glNormal3f(+1, 0, 0);
   glTexCoord2f(0,1); glVertex3f(+1,-1,2);
   glTexCoord2f(0,0); glVertex3f(+1,-1,0);
   glTexCoord2f(1,0); glVertex3f(+1,+1,0);
   glTexCoord2f(1,1); glVertex3f(+1,+1,2);
   //  Left wall
   glColor3f(1,1,1);
   glNormal3f(-1, 0, 0);
   glTexCoord2f(0,0); glVertex3f(-1,-1,0);
   glTexCoord2f(0,1); glVertex3f(-1,-1,2);
   glTexCoord2f(1,1); glVertex3f(-1,+1,2);
   glTexCoord2f(1,0); glVertex3f(-1,+1,0);
   //  Back wall
   glColor3f(1,1,1);
   glNormal3f(0, +1, 0);
   glTexCoord2f(0,1); glVertex3f(-1,+1,2);
   glTexCoord2f(1,1); glVertex3f(+1,+1,2);
   glTexCoord2f(1,0); glVertex3f(+1,+1,0);
   glTexCoord2f(0,0); glVertex3f(-1,+1,0);
   //  Front wall
   glColor3f(1,1,1);
   glNormal3f(0, -1, 0);
   glTexCoord2f(0,1); glVertex3f(-1,-1,2);
   glTexCoord2f(1,1); glVertex3f(+1,-1,2);
   glTexCoord2f(1,0); glVertex3f(+1,-1,0);
   glTexCoord2f(0,0); glVertex3f(-1,-1,0);
   glEnd();
   if (ntex) glDisable(GL_TEXTURE_2D);

   glEnable(GL_POLYGON_OFFSET_FILL);
   glPolygonOffset(-1, -1); 
   // Draw a door - wood texture
   if (ntex)
   {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, texture[4]);
   }
   glBegin(GL_QUADS);
   glColor3f(1,1,1);
   glTexCoord2f(0,1); glVertex3f(-0.6,-1,1);
   glTexCoord2f(1,1); glVertex3f(0.1,-1,1);
   glTexCoord2f(1,0); glVertex3f(0.1,-1,0);
   glTexCoord2f(0,0); glVertex3f(-0.6,-1,0);
   glEnd();
   if (ntex) glDisable(GL_TEXTURE_2D);

   // Draw a window - window texture
   glColor3f(1,1,1);
   if (ntex)
   {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, texture[8]);
   }
   glBegin(GL_QUADS);
   glNormal3f(0,-1,0);
   glTexCoord2f(0,1); glVertex3f(0.3,-1,1.6);
   glTexCoord2f(1,1); glVertex3f(0.7,-1,1.6);
   glTexCoord2f(1,0); glVertex3f(0.7,-1,1.2);
   glTexCoord2f(0,0); glVertex3f(0.3,-1,1.2);
   glEnd();
   if (ntex) glDisable(GL_TEXTURE_2D);
   
   glDisable(GL_POLYGON_OFFSET_FILL);

   //  Draw Roof — wood texture
   if (ntex)
   {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, texture[4]);
   }
   glBegin(GL_TRIANGLES);
   glColor3f(1,1,1);
   glNormal3f(1, 0, 1);
   glTexCoord2f(0.5,1); glVertex3f(0,0,3);
   glTexCoord2f(0,0);   glVertex3f(1,1,2);
   glTexCoord2f(1,0);   glVertex3f(1,-1,2);

   glColor3f(1,1,1);
   glNormal3f(-1, 0, 1);
   glTexCoord2f(0.5,1); glVertex3f(0,0,3);
   glTexCoord2f(0,0);   glVertex3f(-1,1,2);
   glTexCoord2f(1,0);   glVertex3f(-1,-1,2);

   glColor3f(1,1,1);
   glNormal3f(0, -1, 1);
   glTexCoord2f(0.5,1); glVertex3f(0,0,3);
   glTexCoord2f(0,0);   glVertex3f(1,-1,2);
   glTexCoord2f(1,0);   glVertex3f(-1,-1,2);

   glColor3f(1,1,1);
   glNormal3f(0, +1, 1);
   glTexCoord2f(0.5,1); glVertex3f(0,0,3);
   glTexCoord2f(0,0);   glVertex3f(1,1,2);
   glTexCoord2f(1,0);   glVertex3f(-1,1,2);
   glEnd();
   if (ntex) glDisable(GL_TEXTURE_2D);

   //  Undo transformations
   glPopMatrix();
}


// Draw a Tree
static void tree(double x, double y, double z, double s)
{
   //  Save current transforms
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glScaled(s,s,s);

   // Draw Trunk — bark texture
   glColor3f(1,1,1);
   if (ntex)
   {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, texture[0]);
   }
   glBegin(GL_QUAD_STRIP);
   for (int th=0;th<=360;th+=20)
   {
      glNormal3d(Cos(th), Sin(th), 0);
      glTexCoord2f(th/360.0, 0); glVertex3d(0.1*Cos(th), 0.1*Sin(th), 0.0);
      glTexCoord2f(th/360.0, 1); glVertex3d(0.1*Cos(th), 0.1*Sin(th), 1.0);
   }
   glEnd();
   if (ntex) glDisable(GL_TEXTURE_2D);

   // Draw Leaves — leaf texture
   glTranslated(0, 0,1);   // move to top of trunk
   glColor3f(1,1,1);

   if (ntex)
   {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, texture[6]);
   }
   glBegin(GL_TRIANGLE_FAN);
   glNormal3d(0, 0, 1);    // tip faces straight up
   glTexCoord2f(0.5, 0.5); // tip at texture center
   glVertex3d(0, 0, 1);
   for (int th=0;th<=360;th+=20)
   {
      glNormal3d(Cos(th), Sin(th), 0.5);
      glTexCoord2f(0.5+0.5*Cos(th), 0.5+0.5*Sin(th));
      glVertex3d(0.5*Cos(th), 0.5*Sin(th),0);
   }
   glEnd();
   if (ntex) glDisable(GL_TEXTURE_2D);

   glPopMatrix();
}

static void Canopy(double th,double ph)
{
   //  The canopy is a hemisphere — vertex position on a unit sphere equals its outward normal
   glNormal3d(Sin(th)*Cos(ph) , Sin(ph) , Cos(th)*Cos(ph));
   glVertex3d(Sin(th)*Cos(ph) , Sin(ph) , Cos(th)*Cos(ph));
}

/*
 *  Draw solid airplane
 *    at (x,y,z)
 *    nose towards (dx,dy,dz)
 *    up towards (ux,uy,uz)
 *    size
 */
static void SolidPlane(double x,double y,double z,
                       double dx,double dy,double dz,
                       double ux,double uy, double uz,
                       double s)
{
   // Dimensions used to size airplane
   const double wid=0.05;
   const double nose=+0.50;
   const double cone= 0.20;
   const double wing= 0.00;
   const double strk=-0.20;
   const double tail=-0.50;
   //  Unit vector in direction of flght
   double D0 = sqrt(dx*dx+dy*dy+dz*dz);
   double X0 = dx/D0;
   double Y0 = dy/D0;
   double Z0 = dz/D0;
   //  Unit vector in "up" direction
   double D1 = sqrt(ux*ux+uy*uy+uz*uz);
   double X1 = ux/D1;
   double Y1 = uy/D1;
   double Z1 = uz/D1;
   //  Cross product gives the third vector
   double X2 = Y0*Z1-Y1*Z0;
   double Y2 = Z0*X1-Z1*X0;
   double Z2 = X0*Y1-X1*Y0;
   //  Rotation matrix
   double mat[16];
   mat[0] = X0;   mat[4] = X1;   mat[ 8] = X2;   mat[12] = 0;
   mat[1] = Y0;   mat[5] = Y1;   mat[ 9] = Y2;   mat[13] = 0;
   mat[2] = Z0;   mat[6] = Z1;   mat[10] = Z2;   mat[14] = 0;
   mat[3] =  0;   mat[7] =  0;   mat[11] =  0;   mat[15] = 1;

   //  Save current transforms
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glMultMatrixd(mat);
   glScaled(s,s,s);
   //  Nose (cone pointing in +X direction)
   glColor3f(1,1,1);
   if (ntex)
   {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, texture[1]);
   }
   glBegin(GL_TRIANGLES);
   for (int th=0;th<360;th+=30)
   {
      glNormal3f(1, Cos(th+15), Sin(th+15));
      glTexCoord2f((th+15)/360.0, 0); glVertex3f(nose, 0, 0);
      glNormal3f(1, Cos(th), Sin(th));
      glTexCoord2f(th/360.0, 1);      glVertex3f(cone, wid*Cos(th), wid*Sin(th));
      glNormal3f(1, Cos(th+30), Sin(th+30));
      glTexCoord2f((th+30)/360.0, 1); glVertex3f(cone, wid*Cos(th+30), wid*Sin(th+30));
   }
   glEnd();
   if (ntex) glDisable(GL_TEXTURE_2D);
   //  Fuselage — plane texture: s wraps around (th/360), t runs nose→tail
   glColor3f(1,1,1);
   if (ntex)
   {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, texture[1]);
   }
   glBegin(GL_QUAD_STRIP);
   for (int th=0;th<=360;th+=30)
   {
      glNormal3d(0, Cos(th), Sin(th));
      glTexCoord2d(th/360.0, 1); glVertex3d(cone, wid*Cos(th), wid*Sin(th));
      glTexCoord2d(th/360.0, 0); glVertex3d(tail, wid*Cos(th), wid*Sin(th));
   }
   glEnd();
   if (ntex) glDisable(GL_TEXTURE_2D);
   // Tailpipe — lighting disabled to appear glowing
   glDisable(GL_LIGHTING);
   glColor3f(1,0.8,0);
   glBegin(GL_TRIANGLE_FAN);
   glVertex3d(tail, 0.0, 0.0);
   for (int th=0;th<=360;th+=30)
      glVertex3d(tail,wid*Cos(th),wid*Sin(th));
   glEnd();
   glEnable(GL_LIGHTING);
   //  Canopy — the Canopy() helper
   glPushMatrix();
   glTranslated(0.15,wid,0);
   glScaled(0.05,0.03,0.03);
   glColor3f(1,1,1);
   for (int ph=-30;ph<90;ph+=30)
   {
      glBegin(GL_QUAD_STRIP);
      for (int th=0;th<=360;th+=30)
      {
         Canopy(th,ph);
         Canopy(th,ph+30);
      }
      glEnd();
   }
   glPopMatrix();

   //  Wings and vertical tail — same as plane texture
   glColor3f(1,1,1);
   glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);
   if (ntex)
   {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, texture[1]);
   }
   glBegin(GL_TRIANGLES);
   //  Left wing
   glNormal3d(0, 1, 0);
   glTexCoord2f(0,0); glVertex3d(wing, 0.0, wid);
   glTexCoord2f(1,0); glVertex3d(tail, 0.0, wid);
   glTexCoord2f(1,1); glVertex3d(tail, 0.0, 0.5);
   //  Right wing — reversed winding so geometric normal also points (0,+1,0)
   glNormal3d(0, 1, 0);
   glTexCoord2f(1,1); glVertex3d(tail, 0.0,-0.5);
   glTexCoord2f(1,0); glVertex3d(tail, 0.0,-wid);
   glTexCoord2f(0,0); glVertex3d(wing, 0.0,-wid);
   //  Vertical tail
   glNormal3d(0, 0, 1);
   glTexCoord2f(0,0); glVertex3d(strk, wid, 0.0);
   glTexCoord2f(0,1); glVertex3d(tail, 0.3,  0.0);
   glTexCoord2f(1,0); glVertex3d(tail, wid,  0.0);
   glEnd();
   if (ntex) glDisable(GL_TEXTURE_2D);
   glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);
   //  Undo transformations
   glPopMatrix();
}


/*
 *  OpenGL (GLUT) calls this routine to display the scene
 */
void display()
{
   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);
   //  Undo previous transformations
   glLoadIdentity();
   //  Orthogonal - set world orientation
   if (mode==0)
   {
      glRotatef(ph,1,0,0); // Rotate around x-axis (elevation)
      glRotatef(th,0,0,1); // Rotate around z-axis (azimuth)
   }
   //  Perspective - set eye position
   else if (mode==1)
   {
      // Set the eye position
      double Ex = 2*dim*Sin(ph)*Sin(th);
      double Ey = 2*dim*Sin(ph)*Cos(th);
      double Ez = 2*dim*Cos(ph);
      // Set the up vector
      double Ux = Cos(ph)*Sin(th);
      double Uy = Cos(ph)*Cos(th);
      double Uz = -Sin(ph);

      gluLookAt(Ex,Ey,Ez, 0,0,0, Ux,Uy,Uz);
   }
   // First person - set eye position and orientation
   else if (mode==2)
   {
      if (step !=0 && t!=0){
         double dx = t*Sin(alpha)*0.1;
         double dy = t*Cos(alpha)*0.1;
         FV_Ex += dx;
         FV_Ey += dy;
         t = 0;
      }
      gluLookAt(FV_Ex,FV_Ey,FV_Ez, FV_Ex+Sin(alpha), FV_Ey+Cos(alpha), FV_Ez+Sin(beta), 0, 0, 1);     
   }

   //  Flat or smooth shading
   glShadeModel(smooth ? GL_SMOOTH : GL_FLAT);

   //  Set up the light source
   if (light)
   {
      float Ambient[]  = {0.01*ambient,  0.01*ambient,  0.01*ambient,  1.0};
      float Diffuse[]  = {0.01*diffuse,  0.01*diffuse,  0.01*diffuse,  1.0};
      float Specular[] = {0.01*specular, 0.01*specular, 0.01*specular, 1.0};
      float Position[] = {5*Cos(zh), 5*Sin(zh), zlight, 1.0};
      // Auto normalize normal vectors
      glEnable(GL_NORMALIZE);

      //  Turn on the lighting system
      glEnable(GL_LIGHTING);

      glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
      glEnable(GL_COLOR_MATERIAL);

      //  Activate light
      glEnable(GL_LIGHT0);

      //  Set the properties of the light source
      glLightfv(GL_LIGHT0, GL_AMBIENT,  Ambient);
      glLightfv(GL_LIGHT0, GL_DIFFUSE,  Diffuse);
      glLightfv(GL_LIGHT0, GL_SPECULAR, Specular);
      glLightfv(GL_LIGHT0, GL_POSITION, Position);

      //  Draw a white ball to show where the light is
      glDisable(GL_LIGHTING);
      glColor3f(1,1,1);
      glPushMatrix();
      glTranslatef(Position[0],Position[1],Position[2]);
      glutSolidSphere(0.1, 8, 8);
      glPopMatrix();
      glEnable(GL_LIGHTING);
   }
   else
      glDisable(GL_LIGHTING);

   //  Draw selected object(s) — o/O cycles through them
   if (obj==0 || obj==1)
      land(3,3,3);
   if (obj==0 || obj==2)
      SolidPlane(0,-0.5,0.05, 0,1,0, 0,0,1, 1);
   if (obj==0 || obj==3)
   {
      house(1,1,0 , 0.5,0.5,0.5 , 0);
      house(-2,-1,0 , 0.3,0.2,0.5,90);
   }
   if (obj==0 || obj==4)
   {
      tree(-2,-2,0, 0.5);
      tree(1,2,0, 0.4);
      tree(1,-2,0, 0.8);
      tree(-1,1,0, 0.2);
   }

   //  Disable lighting before drawing axes and text
   glDisable(GL_LIGHTING);

   //  Draw axes
   glColor3f(1,1,1);
   if (axes)
   {
      const double len=4.5;  //  Length of axes
      glBegin(GL_LINES);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(len,0.0,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,len,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,0.0,len);
      glEnd();
      //  Label axes
      glRasterPos3d(len,0.0,0.0);
      Print("X");
      glRasterPos3d(0.0,len,0.0);
      Print("Y");
      glRasterPos3d(0.0,0.0,len);
      Print("Z");
   }
   //  Display parameters
   glWindowPos2i(5,5);
   if (mode == 2)
      Print("Angle=%d,%d  FOV(+/)=%d Projection(M)=%s  Light(l)=%s  Elevation([/])=%.1f",alpha,ph,fov,text[mode],light?"On":"Off",zlight);
   else
      Print("Angle=%d,%d  Dim=%.1f FOV(+/)=%d Projection(M)=%s  Light(l)=%s  Elevation([/])=%.1f",th,ph,dim,fov,text[mode],light?"On":"Off",zlight);
   if (light)
   {
      glWindowPos2i(5,25);
      Print("Ambient(I)=%d  Diffuse(D)=%d  Specular(S)=%d  Emission(E)=%d  Shininess(N)=%.0f",
            ambient,diffuse,specular,emission,shiny);
   }

      //  Render the scene and make it visible
   ErrCheck("display");
   glFlush();
   glutSwapBuffers();
}

/*
 *  GLUT calls this routine when idle to animate the light
 */
void idle()
{
   double t = glutGet(GLUT_ELAPSED_TIME)/2000.0;
   zh = fmod(90*t, 360.0);
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{
   if (mode != 2){
      //  Right arrow key - increase angle by 5 degrees
      if (key == GLUT_KEY_RIGHT) th += 5;
      //  Left arrow key - decrease angle by 5 degrees
      else if (key == GLUT_KEY_LEFT) th -= 5;
      //  Up arrow key - increase elevation by 5 degrees
      else if (key == GLUT_KEY_UP) ph += 5;
      //  Down arrow key - decrease elevation by 5 degrees
      else if (key == GLUT_KEY_DOWN) ph -= 5;
   }
   else if (mode == 2){
      //  Right arrow key - increase angle by 5 degrees
      if (key == GLUT_KEY_RIGHT) alpha += 5;
      //  Left arrow key - decrease angle by 5 degrees
      else if (key == GLUT_KEY_LEFT) alpha -= 5;
      //  Up arrow key - increase elevation by 5 degrees
      else if (key == GLUT_KEY_UP) beta += 5;
      //  Down arrow key - decrease elevation by 5 degrees
      else if (key == GLUT_KEY_DOWN) beta -= 5;
   }
   //  PageUp/PageDown zoom — works in all modes
   if (key == GLUT_KEY_PAGE_UP)
      dim += 0.1;
   else if (key == GLUT_KEY_PAGE_DOWN && dim>1)
      dim -= 0.1;
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   alpha %= 360;
   //  Update projection
   Project();
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch,int x,int y)
{
   //  Exit on ESC
   if (ch == 27)
      exit(0);
   //  Reset view angle
   else if (ch == '0'){
      th = ph = step = 0;
      FV_Ex = 0;
      FV_Ey = -3.0;
      alpha = 0;
   }
   //  Toggle axes
   else if (ch == 'a' || ch == 'A')
      axes = 1-axes;
   //  Change field of view angle
   else if (ch == '-' && ch>1)
      fov--;
   else if (ch == '+' && ch<179)
      fov++;
   else if (ch == 'm' || ch == 'M')
      mode = (mode+1)%3;
   else if (ch == 'f' || ch == 'F'){
      t = +1;
      step = 1;
   }
   else if (ch == 'b' || ch == 'B'){
      t = -1;
      step = 1;
   }
   //  Cycle objects (o=forward, O=backward): all→land→airplane→houses→trees
   else if (ch == 'o')
      obj = (obj+1)%5;
   else if (ch == 'O')
      obj = (obj+4)%5;
   //  Toggle textures on/off
   else if (ch == 't' || ch == 'T')
      ntex = 1-ntex;
   //  Toggle lighting on/off
   else if (ch == 'l')
      light = 1-light;
   //  Toggle smooth/flat shading
   else if (ch == 'L')
      smooth = 1-smooth;
   //  Ambient level
   else if (ch == 'i' && ambient > 0)   ambient -= 5;
   else if (ch == 'I' && ambient < 100) ambient += 5;
   //  Diffuse level
   else if (ch == 'd' && diffuse > 0)   diffuse -= 5;
   else if (ch == 'D' && diffuse < 100) diffuse += 5;
   //  Specular level
   else if (ch == 's' && specular > 0)  specular -= 5;
   else if (ch == 'S' && specular < 100)specular += 5;
   //  Emission level
   else if (ch == 'e' && emission > 0)  emission -= 5;
   else if (ch == 'E' && emission < 100)emission += 5;
   //  Shininess
   else if (ch == 'n' && shininess > -1) shininess -= 1;
   else if (ch == 'N' && shininess <  7) shininess += 1;
   //  Light elevation
   else if (ch == '[') zlight -= 0.2;
   else if (ch == ']') zlight += 0.2;
   //  Translate shininess index to GL value
   shiny = shininess<0 ? 0 : pow(2.0, shininess);

   //  Reproject
   Project();
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
   //  Ratio of the width to the height of the window
   asp = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
   //  Set projection
   Project();
}

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[])
{
   //  Initialize GLUT
   glutInit(&argc,argv);
   //  Request double buffered, true color window with Z buffering at 600x600
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   glutInitWindowSize(600,600);
   glutCreateWindow("HW3: Yan Gu");
#ifdef USEGLEW
   //  Initialize GLEW
   if (glewInit()!=GLEW_OK) Fatal("Error initializing GLEW\n");
#endif
   //  Set callbacks
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutSpecialFunc(special);
   glutKeyboardFunc(key);
   glutIdleFunc(idle);
   //  Load textures
   texture[0] = LoadTexBMP("trunk.bmp");   //  tree trunk
   texture[1] = LoadTexBMP("plane.bmp");   //  airplane fuselage
   texture[2] = LoadTexBMP("water.bmp");   //  river
   texture[3] = LoadTexBMP("brick.bmp");   //  house walls
   texture[4] = LoadTexBMP("wood.bmp");    //  house roof
   texture[5] = LoadTexBMP("grassland.bmp");   //  land
   texture[6] = LoadTexBMP("leaf.bmp");    //  tree top
   texture[7] = LoadTexBMP("road.bmp");   //  road
   texture[8] = LoadTexBMP("window.bmp"); //  house window
   // Set texture parameters
   for (int i=0;i<9;i++)
   {
      glBindTexture(GL_TEXTURE_2D, texture[i]);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   }

   glutMainLoop();
   return 0;
}
