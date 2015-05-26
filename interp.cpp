// $Id: interp.cpp,v 1.14 2014-05-08 18:32:56-07 - - $

#include <map>
#include <memory>
#include <string>
#include <vector>
using namespace std;

#include <GL/freeglut.h>

#include "debug.h"
#include "interp.h"
#include "shape.h"
#include "util.h"


map<string,interpreter::interpreterfn> interpreter::interp_map {
   {"define" , &interpreter::do_define },
   {"draw"   , &interpreter::do_draw   },
   {"border" , &interpreter::change_boarder},
   {"moveby" , &interpreter::change_moveby},
};

map<string,interpreter::factoryfn> interpreter::factory_map {
   {"text"     , &interpreter::make_text     },
   {"ellipse"  , &interpreter::make_ellipse  },
   {"circle"   , &interpreter::make_circle   },
   {"polygon"  , &interpreter::make_polygon  },
   {"rectangle", &interpreter::make_rectangle},
   {"square"   , &interpreter::make_square   },
   {"triangle" , &interpreter::make_triangle },
   {"right_triangle" , &interpreter::make_right_triangle },
   {"isosceles", &interpreter::make_isosceles },
   {"equilateral", &interpreter::make_equalateral },
};

interpreter::~interpreter() {
   for (const auto& itor: objmap) {
      cout << "objmap[" << itor.first << "] = "
           << *itor.second << endl;
   }
}

void interpreter::interpret (const parameters& params) {
   DEBUGF ('i', params);
   param begin = params.cbegin();
   string command = *begin;
   auto itor = interp_map.find (command);
   if (itor == interp_map.end()) throw runtime_error ("syntax error");
   interpreterfn func = itor->second;
   (this->*func) (++begin, params.cend());
}

void interpreter::do_define (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   string name = *begin;
   objmap.insert ({name, make_shape (++begin, end)});
}


void interpreter::do_draw (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   //if (end - begin != 3) throw runtime_error ("syntax error");
   string name = begin[1];
   shape_map::const_iterator itor = objmap.find (name);
   if (itor == objmap.end()) {
      throw runtime_error (name + ": no such shape");
   }
   vertex where {from_string<GLfloat> (begin[2]),
                 from_string<GLfloat> (begin[3])};
   rgbcolor color {begin[0]};
   //itor->second->draw (where, color);
   object s(itor->second, where, color);
   window::push_back(s);
}

void interpreter::change_boarder (param begin, param end){
	//bor_width = atof(*(param+1).c_str());
}

void interpreter::change_moveby (param begin, param end){
	window::setdelta(atof(begin->c_str()));
	cout << "Moveby is set to " << atof(begin->c_str()) << endl;
}

shape_ptr interpreter::make_shape (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   string type = *begin++;
   auto itor = factory_map.find(type);
   if (itor == factory_map.end()) {
      throw runtime_error (type + ": no such shape");
   }
   factoryfn func = itor->second;
   return (this->*func) (begin, end);
}

shape_ptr interpreter::make_text (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   string temp;
   for(param i=begin+1; i != end; ++i){
		temp = temp + *i;
		temp = temp + " ";
   }
   return make_shared<text> (nullptr , temp);
}

shape_ptr interpreter::make_ellipse (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   GLfloat w = atof((*begin).c_str());
   GLfloat h = atof((*(begin+1)).c_str());
   return make_shared<ellipse> (w, h);
}

shape_ptr interpreter::make_circle (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   GLfloat w = atof((*begin).c_str());
   return make_shared<circle> (w);
}

shape_ptr interpreter::make_polygon (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   vertex_list v;
   for(auto i=begin; i != end; ++i){
		vertex ve = {atof(i->c_str()), atof((i+1)->c_str())};
		v.push_back(ve);
   }
   return make_shared<polygon> (v);
}

shape_ptr interpreter::make_rectangle (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   GLfloat w = atof((*begin).c_str());
   GLfloat h = atof((*(begin+1)).c_str());
   return make_shared<rectangle> (w, h);
}

shape_ptr interpreter::make_square (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   GLfloat w = atof((*begin).c_str());
   return make_shared<square> (w);
}

shape_ptr interpreter::make_triangle (param begin, param end){
	vertex_list v;
	for(auto i=begin; i != end; i+=2){
		vertex ve = {atof(i->c_str()), atof((i+1)->c_str())};
		v.push_back(ve);
	}
	return make_shared<triangle> (v);
}
shape_ptr interpreter::make_right_triangle (param begin, param end){
	GLfloat w = atof((*begin).c_str());
	GLfloat h = atof((*(begin+1)).c_str());
	return make_shared<right_triangle> (w, h);
}
shape_ptr interpreter::make_isosceles (param begin, param end){
	GLfloat w = atof((*begin).c_str());
    GLfloat h = atof((*(begin+1)).c_str());
	return make_shared<isosceles> (w, h);
}
shape_ptr interpreter::make_equalateral (param begin, param end){
	GLfloat w = atof((*begin).c_str());
	return make_shared<equalateral> (w);
}

