//
//  Framework for a raytracer
//  File: scene.cpp
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

#include "scene.h"
#include "material.h"
#include <algorithm>
#define MAX_RECURSION 5

Color Scene::trace(const Ray &ray)
{
	return shade(ray, 0);
}

Color Scene::shade(const Ray &ray, int depth)
{
	// Find hit object and distance
	Hit min_hit(std::numeric_limits<double>::infinity(), Vector());
	Object *obj = NULL;
	for (unsigned int i = 0; i < objects.size(); ++i) {
		Hit hit(objects[i]->intersect(ray));
		if (hit.t<min_hit.t) {
			min_hit = hit;
			obj = objects[i];
		}
	}

	// No hit? Return background color.
	if (!obj) return Color(.1, .1, .1);

	Material *material = obj->material;            //the hit objects material
	Point hit = ray.at(min_hit.t);                 //the hit point
	Vector N = min_hit.N;                          //the normal at hit point
	Vector V = -ray.D;                             //the view vector


	 /****************************************************
	 * This is where you should insert the color
	 * calculation (Phong model).
	 *
	 * Given: material, hit, N, V, lights[]
	 * Sought: color
	 *
	 * Hints: (see triple.h)
	 *        Triple.dot(Vector) dot product
	 *        Vector+Vector      vector sum
	 *        Vector-Vector      vector difference
	 *        Point-Point        yields vector
	 *        Vector.normalize() normalizes vector, returns length
	 *        double*Color        scales each color component (r,g,b)
	 *        Color*Color        dito
	 *        pow(a,b)           a to the power of b
	 ****************************************************/

	 //[Phong Illumination] - Marc Tremblay 100555250
	Color cr = material->color;			// reflection coeff
	Color Ia = material->ka * (Color(.1, .1, .1)  * cr);	// ambient light colour
	Color cl;							// light colour
	Color I = Ia;						// overall light colour



	for (int i = 0; i < lights.size(); i++)
	{
		Vector L;						// direction to light
		Vector R;						// direction of reflected light
		L = lights[i]->position - hit;	// calc light direction
		L.normalize();					// normalize light direction
		
		R = -L - (2 * -L.dot(N) * N);	// calculate light reflection
		R.normalize();					// normalize light reflection
		cl = lights[i]->color;			// assign light colour
	//End [Phong Illumination]
	//[Shadows / Extensions:Soft Shadows] - Marc Tremblay 100555250
		float m_shade = 1.0f;			// initialize shadow multiplier
		if (shadows)
		{
			Vector Ln = -L.normalized();						// Light normal
			Vector U = Ln.cross(Vector(0, 1, 0)).normalized();	// x basis vector
			Vector V = U.cross(Ln).normalized();				// y basis vector

			Point SP;					//shadow ray casting point
			Vector SD;					//shadow ray direction

			double s_avg = 0.0;

			for (float x = -25.f; x <= 25.f; x += 10.f)
			{
				for (float y = -25.f; y <= 25.f; y += 10.f)
				{
					SP = lights[i]->position;
					SP += x * U + y * V;
					SD = hit - SP;
					s_avg += compShadow(SP, SD.normalized(), SD.length());
				}
			}
			m_shade = s_avg / 36;
			//m_shade = compShadow(lights[i]->position, Ln, (hit - lights[i]->position).length());
			
		}

		// calculate colour
		I += m_shade * (material->kd * cl * cr * max(0.0, L.dot(N))
			+ material->ks * cl * pow(max(0.0, R.dot(V)), material->n));
	//End [Shadows / Extensions:Soft Shadows]
	}
	//[Reflection] - Marc Tremblay 100555250
	if (depth < MAX_RECURSION)
	{
		if (material->reflect != 0.0)
		{
			Vector rfl = ray.D - (2 * ray.D.dot(N) * N);
			rfl.normalize();
			Ray rfl_ray = Ray(hit + .01 * N, rfl);
			I += material->reflect * shade(rfl_ray, depth + 1);
		}
	//End [Reflection]
	//[Extensions:Refraction] - Marc Tremblay 100555250
		if (material->refract != 0.0)
		{
			bool inside = false;
			if (ray.D.dot(N) > 0) N = -N, inside = true;
			Vector rfr = calcRefract(N, ray.D, material->eta, depth);
			Ray rfr_ray = Ray(hit + 0.01 * rfr, rfr);
			I += material->refract * shade(rfr_ray, depth + 1);
		}
	}
	//End [Extensions:Refraction]
	return I;
}
//[Shadows / Extensions:Soft Shadows] - Marc Tremblay 100555250
float Scene::compShadow(Point P, Vector D, double max)
{
	Ray shade = Ray(P, D);
	// from hit point, in the direction of the light
	for (unsigned int j = 0; j < objects.size(); j++)
	{
		// temp variables for checking shadows
		Hit s_hit(objects[j]->intersect(shade));
		// if shadowray's t is less than the distance from light to hit
		if (s_hit.t < max - .01)
		{
			return 0.0f; // shadow is cast
		}
	}
	return 1.0f; // no shadow is cast
}
//End [Shadows / Extensions:Soft Shadows]
//[Extensions:Refraction] - Marc Tremblay 100555250
Vector Scene::calcRefract(Vector N, Vector D, double ior, bool inside)
{
	double eta = (inside) ? ior : 1 / ior;
	double costheta = -N.dot(D);
	double sqrterm = (1 - eta*eta * (1 - costheta*costheta));
	return (D * eta + N * (eta * costheta - sqrt(sqrterm))).normalized();
}
//End [Extensions:Refraction]
void Scene::render(Image &img)
{
    int w = img.width();
    int h = img.height();
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            Point pixel(x, h-1-y, 0);
            Ray ray(eye, (pixel-eye).normalized());
            Color col = trace(ray);
            col.clamp();
            img(x,y) = col;
        }
    }
}

void Scene::addObject(Object *o)
{
    objects.push_back(o);
}

void Scene::addLight(Light *l)
{
    lights.push_back(l);
}

void Scene::setEye(Triple e)
{
    eye = e;
}
