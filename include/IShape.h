#ifndef RTREE_ISHAPE_H
#define RTREE_ISHAPE_H

#include <stdint.h>
#include "ISerializable.h"

namespace RTree
{
	namespace Components
	{
		namespace Geometries
		{
			class Point;
			class Region;
		}
	}

	namespace Contracts
	{
		class Components::Geometries::Point;
		class Components::Geometries::Region;

		class IShape : public Common::ISerializable
		{
		public:
			virtual ~IShape() {};
			virtual bool intersectsShape(const IShape& t_shape) const = 0;
			virtual bool containsShape(const IShape& t_shape) const = 0;
			virtual bool touchesShape(const IShape& t_shape) const = 0;
			virtual void getCenter(Components::Geometries::Point& t_point) const = 0;
			virtual uint8_t getDimension() const = 0;
			virtual void getMBR(Components::Geometries::Region& t_region) const = 0;
			virtual double getArea() const = 0;
			virtual double getMinimumDistance(const IShape& t_shape) const = 0;
		};
	}
}

#endif