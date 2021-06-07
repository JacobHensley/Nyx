#pragma once
#include "AABB.h"
#include "Triangle.h"
#include <glm/glm.hpp>

namespace Nyx {

	struct Ray
	{
		glm::vec3 Origin, Direction;

		Ray()
			: Origin(0), Direction(0)
		{
		}

		Ray(const glm::vec3& origin, const glm::vec3& direction)
			: Origin(origin), Direction(direction)
		{
		}

        bool IntersectsTriangle(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3)
        {
            const float EPSILON = (float)0.0000001;
            glm::vec3 vertex0 = p1;
            glm::vec3 vertex1 = p2;
            glm::vec3 vertex2 = p3;
            glm::vec3 edge1, edge2, h, s, q;
            float a, f, u, v;
            edge1 = vertex1 - vertex0;
            edge2 = vertex2 - vertex0;
            h = glm::cross(Direction, edge2);
            a = glm::dot(edge1, h);
            if (a > -EPSILON && a < EPSILON)
                return false;    // This ray is parallel to this triangle.
            f = (float)(1.0 / a);
            s = Origin - vertex0;
            u = f * glm::dot(s, h);
            if (u < 0.0 || u > 1.0)
                return false;
            q = glm::cross(s, edge1);
            v = f * glm::dot(Direction, q);
            if (v < 0.0 || u + v > 1.0)
                return false;
            // At this stage we can compute t to find out where the intersection point is on the line.
            float t = f * glm::dot(edge2, q);
            if (t > EPSILON) // ray intersection
            {
                //  outIntersectionPoint = rayOrigin + rayVector * t;
                return true;
            }
            else // This means that there is a line intersection but not a ray intersection.
                return false;
        }

        bool IntersectsTriangle(const Triangle& triangle)
        {
            return IntersectsTriangle(triangle.Points[0], triangle.Points[1], triangle.Points[2]);
        }

        bool IntersectsAABB(const AABB& aabb, float& t) const
        {
            glm::vec3 dirfrac;
            // r.dir is unit direction vector of ray
            dirfrac.x = 1.0f / Direction.x;
            dirfrac.y = 1.0f / Direction.y;
            dirfrac.z = 1.0f / Direction.z;
            // lb is the corner of AABB with minimal coordinates - left bottom, rt is maximal corner
            // r.org is origin of ray
            const glm::vec3& lb = aabb.Min;
            const glm::vec3& rt = aabb.Max;
            float t1 = (lb.x - Origin.x) * dirfrac.x;
            float t2 = (rt.x - Origin.x) * dirfrac.x;
            float t3 = (lb.y - Origin.y) * dirfrac.y;
            float t4 = (rt.y - Origin.y) * dirfrac.y;
            float t5 = (lb.z - Origin.z) * dirfrac.z;
            float t6 = (rt.z - Origin.z) * dirfrac.z;

            float tmin = glm::max(glm::max(glm::min(t1, t2), glm::min(t3, t4)), glm::min(t5, t6));
            float tmax = glm::min(glm::min(glm::max(t1, t2), glm::max(t3, t4)), glm::max(t5, t6));

            // if tmax < 0, ray (line) is intersecting AABB, but the whole AABB is behind us
            if (tmax < 0)
            {
                t = tmax;
                return false;
            }

            // if tmin > tmax, ray doesn't intersect AABB
            if (tmin > tmax)
            {
                t = tmax;
                return false;
            }

            t = tmin;
            return true;
        }
	};

}
