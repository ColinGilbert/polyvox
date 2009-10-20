//
// This module contains a bunch of well understood functions
// I apologise if the conventions used here are slightly
// different than what you are used to.
// 
 
#ifndef GENERIC_VECTOR_H
#define GENERIC_VECTOR_H

#include <stdio.h>
#include <math.h>


class VectorM {
  public:
	float x,y,z;
	VectorM(float _x=0.0,float _y=0.0,float _z=0.0){x=_x;y=_y;z=_z;};
	operator float *() { return &x;};

	float fBoundaryCost;
};

float magnitude(VectorM v);
VectorM normalize(VectorM v);

VectorM operator+(VectorM v1,VectorM v2);
VectorM operator-(VectorM v);
VectorM operator-(VectorM v1,VectorM v2);
VectorM operator*(VectorM v1,float s)   ;
VectorM operator*(float s,VectorM v1)   ;
VectorM operator/(VectorM v1,float s)   ;
float   operator^(VectorM v1,VectorM v2);  // DOT product
VectorM operator*(VectorM v1,VectorM v2);   // CROSS product
VectorM planelineintersection(VectorM n,float d,VectorM p1,VectorM p2);

class matrix{
 public:
	VectorM x,y,z;
	matrix(){x=VectorM(1.0f,0.0f,0.0f);
	         y=VectorM(0.0f,1.0f,0.0f);
	         z=VectorM(0.0f,0.0f,1.0f);};
	matrix(VectorM _x,VectorM _y,VectorM _z){x=_x;y=_y;z=_z;};
};
matrix transpose(matrix m);
VectorM operator*(matrix m,VectorM v);
matrix operator*(matrix m1,matrix m2);

class Quaternion{
 public:
	 float r,x,y,z;
	 Quaternion(){x=y=z=0.0f;r=1.0f;};
	 Quaternion(VectorM v,float t){v=normalize(v);r=(float)cos(t/2.0);v=v*(float)sin(t/2.0);x=v.x;y=v.y;z=v.z;};
	 Quaternion(float _r,float _x,float _y,float _z){r=_r;x=_x;y=_y;z=_z;};
	 float angle(){return (float)(acos(r)*2.0);}
	 VectorM axis(){VectorM a(x,y,z); return a*(float)(1/sin(angle()/2.0));}
	 VectorM xdir(){return VectorM(1-2*(y*y+z*z),  2*(x*y+r*z),  2*(x*z-r*y));}
	 VectorM ydir(){return VectorM(  2*(x*y-r*z),1-2*(x*x+z*z),  2*(y*z+r*x));}
	 VectorM zdir(){return VectorM(  2*(x*z+r*y),  2*(y*z-r*x),1-2*(x*x+y*y));}
	 matrix  getmatrix(){return matrix(xdir(),ydir(),zdir());}
	 //operator matrix(){return getmatrix();}
};
Quaternion operator-(Quaternion q);
Quaternion operator*(Quaternion a,Quaternion b);
VectorM    operator*(Quaternion q,VectorM v);
VectorM    operator*(VectorM v,Quaternion q);
Quaternion slerp(Quaternion a,Quaternion b,float interp);

#endif
