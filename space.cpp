//SDL stuff
#include<SDL/SDL.h>
//OpenGL Stuff
#include<GL/gl.h>
#include<GL/glut.h> // includes GL/glu.h
//Texture Stuff
#include<SOIL/SOIL.h> // to load textures

#include<math.h>
#include<vector>
// cout testing stuff
#include<iostream>

static GLdouble screenborder_right_x;
static GLdouble screenborder_left_x;
static GLdouble screenborder_top_y;
static GLdouble screenborder_bottom_y;
static GLint aliens_dir = 1;

class Ship{
private:
  GLdouble x;
  GLdouble y;
  GLdouble z;
  GLdouble side;
  GLuint texture;
public:
  Ship(GLdouble x, GLdouble y, GLdouble z, GLdouble side, GLuint tex){
    this->x=x;
    this->y=y;
    this->z=z;
    this->side=side;
    this->texture=tex;
  }
  GLdouble getX(){
    return x;
  }
  GLdouble getY(){
    return y;
  }
  GLdouble getZ(){
    return z;
  }
  GLdouble getSide(){
    return side;
  }
  void move(int dir){
    x += 0.05*dir;
  }  
  void draw()
  {
    glEnable( GL_TEXTURE_2D );
    glBindTexture( GL_TEXTURE_2D, texture );
    glBegin(GL_QUADS);
      // glColor3f(255, 255, 255);
      glTexCoord2f(1.0f, 1.0f); glVertex3f(x+side/2, y, z-side/4); // top right
      glTexCoord2f(0.0f, 1.0f); glVertex3f(x-side/2, y, z-side/4); // top left
      glTexCoord2f(0.0f, 0.0f); glVertex3f(x-side/2, y, z+side/4); // bottom left
      glTexCoord2f(1.0f, 0.0f); glVertex3f(x+side/2, y, z+side/4); // bottom right
    glEnd();
    glDisable( GL_TEXTURE_2D ); 
  }
};
class Alien{
private:
  GLdouble x;
  GLdouble y;
  GLdouble z;
  GLdouble side;
  GLdouble movInc;
  GLuint texture;
public:
  Alien(GLdouble x, GLdouble y, GLdouble z, GLdouble side, GLuint tex){
    this->x=x;
    this->y=y;
    this->z=z;
    this->side=side;
    this->movInc=0.005;
    this->texture=tex;
  }
  GLdouble getX(){
    return x;
  }
  GLdouble getY(){
    return y;
  }
  GLdouble getZ(){
    return z;
  }
  GLdouble getSide(){
    return side;
  }
  void move(int dir, bool down)
  {
    if(down)
      z += movInc*5;
    x += movInc*dir;
  }
  void draw()
  {
    glEnable( GL_TEXTURE_2D );
    glBindTexture( GL_TEXTURE_2D, texture);
    glBegin(GL_QUADS);
      glTexCoord2f(1.0f, 1.0f); glVertex3f(x+side/4, y, z-side/4); // top right
      glTexCoord2f(0.0f, 1.0f); glVertex3f(x-side/4, y, z-side/4); // top left
      glTexCoord2f(0.0f, 0.0f); glVertex3f(x-side/4, y, z+side/4); // bottom left
      glTexCoord2f(1.0f, 0.0f); glVertex3f(x+side/4, y, z+side/4); // bottom right
    glEnd();
    glDisable( GL_TEXTURE_2D );    
  }
};

class Proyectile{
private:
  GLdouble x;
  GLdouble y;
  GLdouble z;
  GLdouble side;
  GLdouble movInc;
  bool destroyed;
public:
  Proyectile(GLdouble x, GLdouble y, GLdouble z){
    this->x=x;
    this->y=y;
    this->z=z;
    this->side = 0.03;
    this->movInc = 0.02;
    this->destroyed=false;
  } 
  GLdouble getX(){
    return x;
  }
  GLdouble getY(){
    return y;
  }
  GLdouble getZ(){
    return z;
  }
  GLdouble getSide(){
    return side;
  }
  bool isDestroyed(){
    return destroyed;
  }
  void draw()
  {
    glBegin(GL_QUADS);
      glColor3f(255, 0, 0);
      glVertex3f(x+side/4, y, z-side/2); // top-right
      glVertex3f(x-side/4, y, z-side/2); // top-left
      glColor3f(255, 255, 255);
      glVertex3f(x-side/4, y, z+side/2); // bottom-left
      glVertex3f(x+side/4, y, z+side/2); // bottom-right
    glEnd();
  }
  void move(int dir)
  {
    z += movInc*dir;
  }
  template<typename T>
  bool hit(T &a, float prec)
  {
    bool result=false;
    GLdouble dist;
    if(!destroyed){
      dist = sqrt((x-a.getX()) * (x-a.getX()) + (y-a.getY())*(y-a.getY()) + (z-a.getZ())*(z-a.getZ()));
      if(dist-side-a.getSide() <= prec){
        result = true;
        this->destroyed=true;
      }
    }
    return result;
  }
};

void initGL()
{
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45,640.0/480.0,1.0,500.0);
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_DEPTH_TEST);
}

void initSDL(GLfloat w, GLfloat h)
{
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_SetVideoMode(w, h, 32, SDL_SWSURFACE|SDL_OPENGL);
}

void screenCoord2worldCoord(GLdouble screen_x, GLdouble screen_y, GLdouble * world_x, GLdouble * world_y, GLdouble * world_z)
{
    GLdouble modelview[16], projection[16];  
    GLint viewport[4];
    //get the projection matrix     
    glGetDoublev( GL_PROJECTION_MATRIX, projection );  
    //get the modelview matrix      
    glGetDoublev( GL_MODELVIEW_MATRIX, modelview );  
    //get the viewport      
    glGetIntegerv( GL_VIEWPORT, viewport );
    gluUnProject(screen_x, screen_y, 0, modelview, projection, viewport, world_x, world_y, world_z);  
}


std::vector<GLuint> loadTextures(const char ** texture_files, unsigned int size){
  std::vector<GLuint> textures;
  for(int i = 0; i < size; i++){
    textures.push_back(SOIL_load_OGL_texture
      (
        texture_files[i],
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
      )
    );
    if( !textures[i] )// error loading file
    {
      std::cout << "Texture " << textures[i] << " not loaded. " << std::endl;
    }
  }
  return textures;
}

void display(Ship & spaceShip, 
  std::vector<Alien> & aliens, 
  std::vector<Proyectile> & spaceShip_proyectiles, 
  std::vector<Proyectile> & alien_proyectiles)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    // inclino ligeramente la pantalla hacia mí
    glTranslatef(spaceShip.getX(), spaceShip.getY(), spaceShip.getZ());
    glRotatef(45, 1, 0, 0);
    glTranslatef(-1*spaceShip.getX(), -1*spaceShip.getY(), -1*spaceShip.getZ());
    
    std::vector<Proyectile>::iterator it_proy;
    bool hit=false, down = false;

    spaceShip.draw();
    // compruebo si los proyectiles de la nave le dan a algún alien
    it_proy = spaceShip_proyectiles.begin();
    while(it_proy != spaceShip_proyectiles.end()){
      // comprueba si el proyectil le da a algún alien
      for(int j=0; j<aliens.size() && !hit; j++){
        if( (*it_proy).hit(aliens[j], 0.15) ){
          hit=true;
          aliens.erase(aliens.begin()+j);
        }
      }
      // Si no está destruido el proyectil y está dentro de la pantalla
      if( !(*it_proy).isDestroyed() and (*it_proy).getZ()>-5 ){
        // mueve los proyectiles hacia los aliens
        (*it_proy).move(-1);
        (*it_proy).draw();
        it_proy++;
      }else{ 
        // el proyectil está destruido
        it_proy = spaceShip_proyectiles.erase(it_proy);
      }
    }
    // compruebo si los proyectiles de los aliens le dan a la nave
    it_proy = alien_proyectiles.begin();
    while(it_proy != alien_proyectiles.end()){
      if( (*it_proy).hit(spaceShip, 0.4) ){
          // end of the game
      }
      // Si no está destruido el proyectil y está dentro de la pantalla
      if( !(*it_proy).isDestroyed() and (*it_proy).getZ()<0){
          // mueve los proyectiles hacia la nave
          (*it_proy).move(1);
          (*it_proy).draw();
          it_proy++;
      }else{ 
          // el proyectil está destruido
          it_proy = alien_proyectiles.erase(it_proy);
      }
    }
    // compruebo si algún alien ha llegado al borde de la pantalla
    for(int i=0; i<aliens.size() && !down; i++){
      if( aliens[i].getX() < screenborder_left_x or aliens[i].getX() > screenborder_right_x ){
        aliens_dir *=-1;
        down = true;
      }
    }
    // muevo los aliens y si algún alien llegó al borde de la pantalla, los muevo ligeramente hacia abajo
    for(int i=0; i<aliens.size(); i++){
      aliens[i].move(aliens_dir, down);
      aliens[i].draw();
    }
}


int main(int argc, char* args[])
{
    //altura y anchura de la ventana
    float height=600, width=800;
    initSDL(width, height);
    initGL();
    srand(time(NULL));
    
    SDL_Event event;
    bool loop=true;
    int alien_z = -3, random_number, i;
    GLdouble zW, offset;
    // cargar en memoria las texturas
    const char * texture_files[7] = {"img/11.png", "img/12.png", "img/21.png", "img/22.png", "img/31.png", "img/32.png", "img/ship.png"};
    std::vector<GLuint> tex = loadTextures(texture_files, 7);
    // obtener límites de la pantalla 
    screenCoord2worldCoord(0, 0, &screenborder_left_x, &screenborder_bottom_y, &zW);
    screenCoord2worldCoord(width, height, &screenborder_right_x, &screenborder_top_y, &zW);
    // Proyectiles de la nave y de los aliens
    std::vector<Proyectile> spaceShip_proyectiles, alien_proyectiles;
    std::vector<Alien> aliens;
    // Desplazamiento para situar a los aliens en la pantalla
    offset = (screenborder_left_x-screenborder_right_x)/8;
    // Genera la matriz de aliens
    for(int i=0; i<6; i++){
      for(int j=0; j<6; j++){
        aliens.push_back(Alien(screenborder_right_x+i*offset, screenborder_bottom_y, alien_z-j*offset, offset, tex[j]));
      }
    }
    // Genera la nave, en el centro inferior de la pantalla
    Ship spaceShip(screenborder_left_x+screenborder_right_x/2.0, screenborder_bottom_y, zW-0.25, 2*offset, tex.back());
      
    while (loop)
    {
      while (SDL_PollEvent(&event))
      {
        switch(event.type)
        {
          case SDL_QUIT:
            loop=false;
            break;
          case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
              case SDLK_SPACE: // disparo un proyectil
                spaceShip_proyectiles.push_back(Proyectile(spaceShip.getX(), spaceShip.getY(), spaceShip.getZ()));
                break;
              case SDLK_RIGHT:
                spaceShip.move(1); // muevo la nave hacia la derecha
                break;
              case SDLK_LEFT:
                spaceShip.move(-1); // muevo la nave hacia la izquierda
                break;
            }
        }
      }
      random_number = rand();

      //Si aún quedan aliens
      if( aliens.size()>0 ){
        // asegura que el numero aleatorio quede dentro de rango del vector aliens
        i = random_number%aliens.size();
        // si el número es tan aleatorio como queremos
        if(random_number%3==0 and random_number%11==0){
          // añado un proyectil que disparará un alien
          alien_proyectiles.push_back(Proyectile(aliens[i].getX(), aliens[i].getY(), aliens[i].getZ()));
        }
      }
      // vamos a mostrar por pantalla la nave, los aliens y los proyectiles que ha lanzado cada uno
      display(spaceShip, aliens, spaceShip_proyectiles, alien_proyectiles);
      SDL_GL_SwapBuffers();
    }
    SDL_Quit();
    return 0;
}