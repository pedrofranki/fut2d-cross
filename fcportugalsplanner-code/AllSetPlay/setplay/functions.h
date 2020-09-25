
#ifndef FUNCTION_H
#define FUNCTION_H

#include <math.h>
#include <stdio.h>

#define SINPOINTS 10000


class FuncSin {
   public:
	   FuncSin() {
		   int i;
		   for(i=0;i<SINPOINTS;i++)
			   vals[i] = sin(-M_PI + i*2.0*M_PI/SINPOINTS);
		   
		   /***TEST
		   for(float a=-720.0; a< 720.0; a+=5.0)
			   printf("%7.1f:%7.3f:%7.3f:%7.3f\n", a, sin(a/180.0*M_PI),degVal(a),sin(a/180.0*M_PI)-degVal(a));
		   for(float a=-5*M_PI; a< 5*M_PI; a+=5.0/180.0*M_PI)
			   printf("%7.1f:%7.3f:%7.3f:%7.3f\n", a, sin(a/180.0*M_PI),degVal(a),sin(a/180.0*M_PI)-degVal(a));
		   *****/
	   }

	   float radVal(float rad) {

#endif
		   
		   while(rad < -M_PI) rad+=2*M_PI; 
		   while(rad > M_PI)  rad-=2*M_PI; 

		   int ind = (int)((rad+M_PI)*SINPOINTS/(2.0*M_PI));
		   if (ind < 0) ind = 0;
		   if (ind >=SINPOINTS) ind = SINPOINTS-1;
		   return vals[ind];
	   };

	   float degVal(float deg) {

#endif

		   while(deg < -180.0) deg+=360.0; 
		   while(deg >  180.0) deg-=360.0; 

		   int ind = (int)((deg+180.0)*SINPOINTS/(360.0));
		   if (ind < 0) ind = 0;
		   if (ind >=SINPOINTS) ind = SINPOINTS-1;
		   return vals[ind];
	   };

	   float vals[SINPOINTS];
};

#define COSPOINTS 10000

class FuncCos {
   public:
	   FuncCos() {
		   int i;
		   for(i=0;i<COSPOINTS;i++)
			   vals[i] = cos(-M_PI + i*2.0*M_PI/COSPOINTS);
	   }

	   float radVal(float rad) {

#endif
		   
		   while(rad < -M_PI) rad+=2*M_PI; 
		   while(rad > M_PI)  rad-=2*M_PI; 

		   int ind = (int)( (rad+M_PI)*COSPOINTS/(2.0*M_PI));
		   if (ind < 0) ind = 0;
		   if (ind >=COSPOINTS) ind = COSPOINTS-1;
		   return vals[ind];
	   };

	   float degVal(float deg) {

#endif

		   while(deg < -180.0) deg+=360.0; 
		   while(deg >  180.0) deg-=360.0; 

		   int ind = (int)((deg+180.0)*COSPOINTS/(360.0));
		   if (ind < 0) ind = 0;
		   if (ind >=COSPOINTS) ind = COSPOINTS-1;
		   return vals[ind];
	   };

	   float vals[COSPOINTS];
};

#define SQRTPOINTS 2500000
#define SQRTMAX    2500

class FuncSqrt {
   public:
	   FuncSqrt() {
		   int i;
		   for(i=0;i<SQRTPOINTS;i++)
			   vals[i] = sqrt(i/(1.0*SQRTPOINTS) * SQRTMAX);

//		   for(float x=0; x< 3000.0; x+=3.0)
//			   printf("%7.1f:%7.3f:%7.3f:%7.3f\n", x, sqrt(x),val(x),sqrt(x)-val(x));
	   }

	   float val(float x) {

#endif

		   int ind = (int)(SQRTPOINTS/(1.0*SQRTMAX) * x);
		   if (ind < 0) ind = 0;
		   else if (ind >=SQRTPOINTS) return sqrt(x);
		   return vals[ind];
	   };

	   float vals[SQRTPOINTS];
};

extern FuncSin funcSin;
extern FuncCos funcCos;
extern FuncSqrt funcSqrt;

#endif
