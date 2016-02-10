#ifndef RTREE_REGION_H
#define RTREE_REGION_H

#include "IObject.h"
#include "IShape.h"

namespace RTree
{
	namespace Components
	{
		namespace Geometries
		{
			class Point;

			class Region : public Contracts::Common::IObject, public virtual Contracts::IShape
			{
				Point* m_low;
				Point* m_high;
				uint8_t m_dimension;
			public:
				Region();
				Region(const Region& t_region);
				Region(Region&& t_region);
				Region(const Point& t_low, const Point& t_high);
				Region(const float* t_low, const float* t_high, uint8_t t_dimension);
				~Region();
				void swap(Region& t_first, Region& t_second);

				Point* getLowPoint() const;
				Point* getHighPoint() const;

				virtual Region & operator=(Region t_region);
				virtual bool operator==(const Region& t_region) const;

				virtual Region* clone();

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

				void combinePoint(const Point &t_point);
				void combineRegion(const Region &t_region);
				double getIntersectionArea(const Region &t_region) const;
				Region* getIntersectingRegion(const Region &t_region) const;
				double getMargin() const;
				bool validate() const;

			private:
				bool containsPoint(const Point &t_point) const;
				bool containsRegion(const Region &t_region) const;
				void extendInfinitely(uint8_t t_dimension);
				double getMinimumDistance(const Point &t_point) const;
				double getMinimumDistance(const Region &t_region) const;
				void init(const float* t_low, const float* t_high, uint8_t t_dimension);
				bool intersectsPoint(const Point &t_point) const;
				bool intersectsRegion(const Region &t_region) const;
				bool touchesPoint(const Point &t_point) const;
				bool touchesRegion(const Region &t_region) const;
				bool validate(const float* t_low, const float* t_high, uint8_t t_dimension) const;
			};
		}
	}
}

#endif