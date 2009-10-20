
#include <stdio.h>
#include <math.h>
#include <assert.h>

#include "vector_melax.h"

float  sqr(float a) {return a*a;}

// vector (floating point) implementation

float magnitude(VectorM v) {
    return (float)sqrt(sqr(v.x) + sqr( v.y)+ sqr(v.z));
}
VectorM normalize(VectorM v) {
    float d=magnitude(v);
    if (d==0) {
		printf("Cant normalize ZERO vector\n");
		assert(0);
		d=0.1f;
	}
    v.x/=d;
    v.y/=d;
    v.z/=d;
    return v;
}

VectorM operator+(VectorM v1,VectorM v2) {return VectorM(v1.x+v2.x,v1.y+v2.y,v1.z+v2.z);}
VectorM operator-(VectorM v1,VectorM v2) {return VectorM(v1.x-v2.x,v1.y-v2.y,v1.z-v2.z);}
VectorM operator-(VectorM v)            {return VectorM(-v.x,-v.y,-v.z);}
VectorM operator*(VectorM v1,float s)   {return VectorM(v1.x*s,v1.y*s,v1.z*s);}
VectorM operator*(float s, VectorM v1)  {return VectorM(v1.x*s,v1.y*s,v1.z*s);}
VectorM operator/(VectorM v1,float s)   {return v1*(1.0f/s);}
float  operator^(VectorM v1,VectorM v2) {return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;}
VectorM operator*(VectorM v1,VectorM v2) {
    return VectorM(
                v1.y * v2.z - v1.z*v2.y,
                v1.z * v2.x - v1.x*v2.z,
                v1.x * v2.y - v1.y*v2.x);
}
VectorM planelineintersection(VectorM n,float d,VectorM p1,VectorM p2){
	// returns the point where the line p1-p2 intersects the plane n&d
        VectorM dif  = p2-p1;
        float dn= n^dif;
        float t = -(d+(n^p1) )/dn;
        return p1 + (dif*t);
}
int concurrent(VectorM a,VectorM b) {
	return(a.x==b.x && a.y==b.y && a.z==b.z);
}


// Matrix Implementation
matrix transpose(matrix m) {
	return matrix(	VectorM(m.x.x,m.y.x,m.z.x),
					VectorM(m.x.y,m.y.y,m.z.y),
					VectorM(m.x.z,m.y.z,m.z.z));
}
VectorM operator*(matrix m,VectorM v){
	m=transpose(m); // since column ordered
	return VectorM(m.x^v,m.y^v,m.z^v);
}
matrix operator*(matrix m1,matrix m2){
	m1=transpose(m1);
	return matrix(m1*m2.x,m1*m2.y,m1*m2.z);
}

//Quaternion Implementation    
Quaternion operator*(Quaternion a,Quaternion b) {
	Quaternion c;
	c.r = a.r*b.r - a.x*b.x - a.y*b.y - a.z*b.z; 
	c.x = a.r*b.x + a.x*b.r + a.y*b.z - a.z*b.y; 
	c.y = a.r*b.y - a.x*b.z + a.y*b.r + a.z*b.x; 
	c.z = a.r*b.z + a.x*b.y - a.y*b.x + a.z*b.r; 
	return c;
}
Quaternion operator-(Quaternion q) {
	return Quaternion(q.r*-1,q.x,q.y,q.z);
}
Quaternion operator*(Quaternion a,float b) {
	return Quaternion(a.r*b, a.x*b, a.y*b, a.z*b);
}
VectorM operator*(Quaternion q,VectorM v) {
	return q.getmatrix() * v;
}
VectorM operator*(VectorM v,Quaternion q){
	assert(0);  // must multiply with the quat on the left
	return VectorM(0.0f,0.0f,0.0f);
}

Quaternion operator+(Quaternion a,Quaternion b) {
	return Quaternion(a.r+b.r, a.x+b.x, a.y+b.y, a.z+b.z);
}
float operator^(Quaternion a,Quaternion b) {
	return  (a.r*b.r + a.x*b.x + a.y*b.y + a.z*b.z); 
}
Quaternion slerp(Quaternion a,Quaternion b,float interp){
	if((a^b) <0.0) {
		a.r=-a.r;
		a.x=-a.x;
		a.y=-a.y;
		a.z=-a.z;
	}
	float theta = (float)acos(a^b);
	if(theta==0.0f) { return(a);}
	return a*(float)(sin(theta-interp*theta)/sin(theta)) + b*(float)(sin(interp*theta)/sin(theta));
}

