#ifndef RTREE_POINT_H
#define RTREE_POINT_H

#include "IObject.h"
#include "IShape.h"

namespace RTree
{
	namespace Components
	{
		namespace Geometries
		{
			class Region;

			class Point : public Contracts::Common::IObject, public virtual Contracts::IShape
			{
				float* m_coordinates;
				uint8_t m_dimension;
			public:
				Point();
				Point(const Point& t_point);
				Point(Point&& t_point);
				Point(const float* t_coordinates, uint8_t t_dimension);
				~Point();
				void swap(Point& t_first, Point& t_second);

				float* getCoordinates() const;
				float getCoordinate(uint8_t t_dimension) const;
				void setCoordinate(float t_coordinate, uint8_t t_dimension);

				virtual Point& operator=(Point t_point);
				virtual bool operator==(const Point& t_point) const;

				virtual Point* clone();

				virtual uint32_t getByteArraySize() const;
				virtual void loadFromByteArray(const uint8_t* t_data);
				virtual void storeToByteArray(uint8_t** t_data, uint32_t& t_length);

				virtual bool intersectsShape(const IShape& t_shape) const;
				virtual bool containsShape(const IShape& t_shape) const;
				virtual bool touchesShape(const IShape& t_shape) const;
				virtual void getCenter(Point& t_point) const;
				virtual uint8_t getDimension() const;
				virtual void getMBR(Region& t_region) const;
				virtual double getArea() const;
				virtual double getMinimumDistance(const IShape& t_shape) const;

			private:
				double getMinimumDistance(const Point& t_point) const;
				void extendInfinitely(uint8_t t_dimension);
			};
		}
	}
}

#endif