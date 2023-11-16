#ifndef AN_MATH_H_
#define AN_MATH_H_

#define AN_PI 3.14159265358979323846

float anCos(float x);
float anSin(float x);
float anTan(float x);
float anAtan(float x);
float anAtan2(float y, float x);
float anMax(float x, float y);
float anMin(float x, float y);
float anAbs(float x);
float anDegreesToRadians(float x);
float anRadiansToDegrees(float x);
void anInitializeRandomDevice();
int anRand();
int anRand(int max);
int anRand(int min, int max);
float anRandf(float max);
float anRandf(float min, float max);

#endif
