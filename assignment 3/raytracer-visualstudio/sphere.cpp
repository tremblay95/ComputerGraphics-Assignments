//
//  Framework for a raytracer
//  File: sphere.cpp
//
//  Created for the Computer Science course "Introduction Computer Graphics"
//  taught at the University of Groningen by Tobias Isenberg.
//
//  Authors:
//    Maarten Everts
//    Jasper van de Gronde
//
//  This framework is inspired by and uses code of the raytracer framework of 
//  Bert Freudenberg that can be found at
//  http://isgwww.cs.uni-magdeburg.de/graphik/lehre/cg2/projekt/rtprojekt.html 
//

#include "sphere.h"
#include <iostream>
#include <math.h>
#include <algorithm>

/************************** Sphere **********************************/

Hit Sphere::intersect(const Ray &ray)
{
    /****************************************************
    * RT1.1: INTERSECTION CALCULATION
    *
    * Given: ray, position, r
    * Sought: intersects? if true: *t
    * 
    * Insert calculation of ray/sphere intersection here. 
    *
    * You have the sphere's center (C) and radius (r) as well as
    * the ray's origin (ray.O) and direction (ray.D).
    *
    * If the ray does not intersect the sphere, return false.
    * Otherwise, return true and place the distance of the
    * intersection point from the ray origin in *t (see example).
    ****************************************************/
    double t = 1000;

    //[Intersections and Normals, Intersections] - Marc Tremblay 100555250
	double a = ray.D.dot(ray.D);
	Vector EC = ray.O - position;
	double b = 2 * (ray.D.dot(EC));
	double c = EC.dot(EC) - pow(r, 2);

	// Find roots
	double b24ac = (b * b) - 4 * a*c;
	if (a != 0 && b24ac >= 0)
	{
		double root_1 = (-b + sqrt(b24ac)) / (2 * a);
		double root_2 = (-b - sqrt(b24ac)) / (2 * a);
		// Check that both roots are positive and select the smallest
		if (root_1 > 0 && root_2 > 0)
			t = min(root_1, root_2);
		else
		{
			// If only one root is negative, select the larger of the two
			if(!(root_1 <= 0 && root_2 <= 0))
				t = max(root_1, root_2);
			// Otherwise, return NO_HIT
			else
				return Hit::NO_HIT();
		}
	}
	else
		return Hit::NO_HIT();
	//End [Intersections and Normals, Intersections]


    /****************************************************
    * RT1.2: NORMAL CALCULATION
    *
    * Given: t, C, r
    * Sought: N
    * 
    * Insert calculation of the sphere's normal at the intersection point.
    ****************************************************/
	// [Intersections and Normals, Normals]- Marc Tremblay 100555250
    Vector N  = ((ray.O + t * ray.D) - position).normalized();
	// End [Intersections and Normals, Normals]

    return Hit(t,N);
}
