#include "helper.h"
/* Check if the ray and sphere intersect */
bool intersectRaySphere(ray * r, sphere * s, float * t) {

  bool retval = false;

  /* A = d.d, the vector dot product of the direction */
  float A = vectorDot( &r -> dir, &r -> dir);

  /* We need a vector representing the distance between the start of 
   * the ray and the position of the circle.
   * This is the term (p0 - c) 
   */
  vector dist = vectorSub( &r -> start, &s -> pos);

  /* 2d.(p0 - c) */
  float B = 2 * vectorDot( &r -> dir, &dist);

  /* (p0 - c).(p0 - c) - r^2 */
  float C = vectorDot( &dist, &dist) - (s -> radius * s -> radius);

  /* Solving the discriminant */
  float discr = B * B - 4 * A * C;

  /* If the discriminant is negative, there are no real roots.
   * Returnfalse in that case as the ray misses the sphere.
   * Return true in all other cases (can be one or two intersections)
   * t represents the distance between the start of the ray and
   * the point on the sphere where it intersects.
   */
  if (discr < 0)
    retval = false;
  else {
    float sqrtdiscr = sqrtf(discr);
    float t0 = (-B + sqrtdiscr) / (2);
    float t1 = (-B - sqrtdiscr) / (2);

    /* We want the closest one */
    if (t0 > t1)
      t0 = t1;

    /* Verify t1 larger than 0 and less than the original t */
    if ((t0 > 0.001f) && (t0 < * t)) {
      * t = t0;
      retval = true;
    } else
      retval = false;
  }

  return retval;
}
