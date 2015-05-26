// $Id: shape.cpp,v 1.7 2014-05-08 18:32:56-07 - - $

#include <typeinfo>
#include <unordered_map>
#include <cmath>
#include <GL/freeglut.h>
#include <string>
using namespace std;

#include "shape.h"
#include "util.h"

static unordered_map<void*,string> fontname {
   {GLUT_BITMAP_8_BY_13       , "Fixed-8x13"    },
   {GLUT_BITMAP_9_BY_15       , "Fixed-9x15"    },
   {GLUT_BITMAP_HELVETICA_10  , "Helvetica-10"  },
   {GLUT_BITMAP_HELVETICA_12  , "Helvetica-12"  },
   {GLUT_BITMAP_HELVETICA_18  , "Helvetica-18"  },
   {GLUT_BITMAP_TIMES_ROMAN_10, "Times-Roman-10"},
   {GLUT_BITMAP_TIMES_ROMAN_24, "Times-Roman-24"},
};

static unordered_map<string,void*> fontcode {
   {"Fixed-8x13"    , GLUT_BITMAP_8_BY_13       },
   {"Fixed-9x15"    , GLUT_BITMAP_9_BY_15       },
   {"Helvetica-10"  , GLUT_BITMAP_HELVETICA_10  },
   {"Helvetica-12"  , GLUT_BITMAP_HELVETICA_12  },
   {"Helvetica-18"  , GLUT_BITMAP_HELVETICA_18  },
   {"Times-Roman-10", GLUT_BITMAP_TIMES_ROMAN_10},
   {"Times-Roman-24", GLUT_BITMAP_TIMES_ROMAN_24},
};

const uchar RED[] = {0xFF, 0x00, 0x00};
const uchar CYAN[] = {0x00, 0xFF, 0xFF};
const uchar BLUE[] = {0x00, 0x00, 0xFF};
const uchar YELLOW[] = {0xFF, 0xFF, 0x00};

ostream& operator<< (ostream& out, const vertex& where) {
   out << "(" << where.xpos << "," << where.ypos << ")";
   return out;
}

shape::shape() {
   DEBUGF ('c', this);
}

text::text (void* glut_bitmap_font, const string& textdata):
      glut_bitmap_font(glut_bitmap_font), textdata(textdata) {
   DEBUGF ('c', this);
}

//Ellipse constructors
ellipse::ellipse (GLfloat width, GLfloat height):
dimension ({width, height}) {
   DEBUGF ('c', this);
}

circle::circle (GLfloat diameter): ellipse (diameter, diameter) {
   DEBUGF ('c', this);
}

//Polygon constructors 
polygon::polygon (const vertex_list& vertices): vertices(vertices) {
   DEBUGF ('c', this);
}

rectangle::rectangle (GLfloat width, GLfloat height): polygon({{0.0,0.0},{width,0.0},{width,height},{0.0,height}}){
   DEBUGF ('c', this << "(" << width << "," << height << ")");
}

square::square (GLfloat width): rectangle (width, width) {
   DEBUGF ('c', this);
}

triangle::triangle (const vertex_list& verticies):
			polygon(verticies){}

right_triangle::right_triangle (const GLfloat base, const GLfloat height): triangle({{0.0,0.0},{0.0,height},{base, 0.0}}){		
}
			
isosceles::isosceles (const GLfloat base, const GLfloat height): triangle({{0.0,0.0},{base,0},{base/2,height}}){
}
			
equalateral::equalateral (const GLfloat width): triangle({{0,0},{width,0},{width/2,width}}){
}

//Draw functions
void text::draw (const vertex& center, const rgbcolor& color, int i) const {
	//int str_width = glutBigmapLength (glut_bitmap_font, (GLubyte*)textdata.c_str());
	//int str_height = glutBitmapHeight (glut_bitmap_font);
	
	glColor3ubv (color.ubvec);
	
	//float xpos = str_width/2.0;
	//float ypos = str_height/2.0;
	
	glRasterPos2f(center.xpos, center.ypos);
	//void* fon = static_cast<void*>(&fontcode.find(glut_bitmap_font));
	for(auto ch: textdata){
		glutBitmapCharacter (GLUT_BITMAP_8_BY_13 , ch);
	}
	glutSwapBuffers();
   DEBUGF ('d', this << "(" << center << "," << color << ")");
}

void ellipse::draw (const vertex& center, const rgbcolor& color, int i) const {
   glBegin (GL_POLYGON);
   glEnable (GL_LINE_SMOOTH);
   glColor3ubv (color.ubvec);
   const float delta = 2 * M_PI / 32;
   for (float theta = 0; theta < 2 * M_PI; theta += delta) {
      float xpos = dimension.xpos * cos (theta) + center.xpos;
      float ypos = dimension.ypos * sin (theta) + center.ypos;
      glVertex2f (xpos, ypos);
   }
   glEnd();	
   
   if(i){
		glBegin (GL_LINE_LOOP);
	   glEnable (GL_LINE_SMOOTH);
	   glColor3ubv (CYAN);
	   const float delta = 2 * M_PI / 32;
	   for (float theta = 0; theta < 2 * M_PI; theta += delta) {
		  float xpos = dimension.xpos * cos (theta) + center.xpos;
		  float ypos = dimension.ypos * sin (theta) + center.ypos;
		  glVertex2f (xpos, ypos);
	   }
	   glEnd();	
   }
   DEBUGF ('d', this << "(" << center << "," << color << ")");
}

void polygon::draw (const vertex& center, const rgbcolor& color, int i) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   glBegin (GL_POLYGON);
   glEnable (GL_LINE_SMOOTH);
   glColor3ubv (color.ubvec);
   
   for (int i=0; i < vertices.size(); ++i) {
		glVertex2f (vertices[i].xpos+center.xpos, vertices[i].ypos+center.ypos);
   }
   glEnd();	
   if(i){
		glBegin (GL_LINE_LOOP);
	   glEnable (GL_LINE_SMOOTH);
	   glColor3ubv (CYAN);
	   
	   for (int i=0; i < vertices.size(); ++i) {
			glVertex2f (vertices[i].xpos+center.xpos, vertices[i].ypos+center.ypos);
	   }
	   glEnd();
   }
}

void shape::show (ostream& out) const {
   out << this << "->" << demangle (*this) << ": ";
}

//Show functions
void text::show (ostream& out) const {
   shape::show (out);
   out << glut_bitmap_font << "(" << glut_bitmap_font
       << ") \"" << textdata << "\"";
}

void ellipse::show (ostream& out) const {
   shape::show (out);
   out << "{" << dimension << "}";
}

void polygon::show (ostream& out) const {
   shape::show (out);
   out << "{" << vertices << "}";
}

ostream& operator<< (ostream& out, const shape& obj) {
   obj.show (out);
   return out;
}

