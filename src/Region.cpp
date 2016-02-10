#include <cmath>
#include <cstring>
#include <limits>
#include "IllegalArgumentException.h"
#include "IllegalStateException.h"
#include "Region.h"
#include "Point.h"

using namespace RTree::Components::Geometries;
using namespace RTree::Tools::Exceptions;

Region::Region() : m_dimension(0)
{
	m_low = nullptr;
	m_high = nullptr;
}

Region::Region(const Region & t_region)
{
	init(t_region.m_low->getCoordinates(), t_region.m_high->getCoordinates(), t_region.getDimension());
}

Region::Region(Region && t_region) : Region()
{
	swap(*this, t_region);
}

Region::Region(const Point & t_low, const Point & t_high)
{
	init(t_low.getCoordinates(), t_high.getCoordinates(), t_low.getDimension());
}

Region::Region(const float * t_low, const float * t_high, uint8_t t_dimension)
{
	init(t_low, t_high, t_dimension);
}

Region::~Region()
{
	m_dimension = 0;
	delete m_low;
	delete m_high;
}

void Region::swap(Region & t_first, Region & t_second)
{
	std::swap(t_first.m_dimension, t_second.m_dimension);
	std::swap(t_first.m_low, t_second.m_low);
	std::swap(t_first.m_high, t_second.m_high);
}

Point * Region::getLowPoint() const
{
	return m_low;
}

Point * Region::getHighPoint() const
{
	return m_high;
}

Region & Region::operator=(Region t_region)
{
	swap(*this, t_region);

	return *this;
}

bool Region::operator==(const Region & t_region) const
{
	if (m_dimension != t_region.m_dimension)
	{
		throw IllegalArgumentException("Region:operator== : Region dimensions do not match.");
	}

	if (m_low != t_region.m_low || m_high != t_region.m_high)
	{
		return false;
	}

	return true;
}

Region * Region::clone()
{
	return new Region(*this);
}

uint32_t Region::getByteArraySize() const
{
	return (sizeof(uint8_t) + m_low->getByteArraySize() + m_high->getByteArraySize());
}

void Region::loadFromByteArray(const uint8_t * t_data)
{
	Region region;
	Point low, high;
	std::memcpy(&region.m_dimension, t_data, sizeof(uint8_t));
	t_data += sizeof(uint8_t);
	low.loadFromByteArray(t_data);
	high.loadFromByteArray(t_data);
	region.m_low = &low;
	region.m_high = &high;
	swap(*this, region);
}

void Region::storeToByteArray(uint8_t ** t_data, uint32_t & t_length)
{
	t_length = getByteArraySize();
	*t_data = new uint8_t[t_length];
	std::memcpy(*t_data, &m_dimension, sizeof(uint8_t));
	*t_data += sizeof(uint8_t);
	m_low->storeToByteArray(t_data, t_length);
	m_high->storeToByteArray(t_data, t_length);
}

bool Region::intersectsShape(const IShape & t_shape) const
{
	const Region *region = dynamic_cast<const Region*>(&t_shape);
	if (region != nullptr)
	{
		return intersectsRegion(*region);
	}

	const Point *point = dynamic_cast<const Point*>(&t_shape);
	if (point != nullptr)
	{
		return intersectsPoint(*point);
	}

	throw IllegalStateException("Region::intersectsShape : Encountered an unexpected shape.");
}

bool Region::containsShape(const IShape & t_shape) const
{
	const Region *region = dynamic_cast<const Region*>(&t_shape);
	if (region != nullptr)
	{
		return containsRegion(*region);
	}

	const Point *point = dynamic_cast<const Point*>(&t_shape);
	if (point != nullptr)
	{
		return containsPoint(*point);
	}

	throw IllegalStateException("Region::containsShape : Encountered an unexpected shape.");
}

bool Region::touchesShape(const IShape & t_shape) const
{
	const Region *region = dynamic_cast<const Region*>(&t_shape);
	if (region != nullptr)
	{
		return touchesRegion(*region);
	}

	const Point *point = dynamic_cast<const Point*>(&t_shape);
	if (point != nullptr)
	{
		return touchesPoint(*point);
	}

	throw IllegalStateException("Region::touchesShape : Encountered an unexpected shape.");
}

void Region::getCenter(Point & t_point) const
{
	float *coordinates = new float[m_dimension];
	for (uint8_t dim = 0; dim < m_dimension; dim++)
	{
		coordinates[dim] = (m_low->getCoordinate(dim) + m_high->getCoordinate(dim)) / 2.0f;
	}
	Point point(coordinates, m_dimension);
	t_point = point;
	delete coordinates;
}

uint8_t Region::getDimension() const
{
	return m_dimension;
}

void Region::getMBR(Region & t_region) const
{
	t_region = *this;
}

double Region::getArea() const
{
	double area = 1.0;

	for (uint8_t dim = 0; dim < m_dimension; dim++)
	{
		area *= m_high->getCoordinate(dim) - m_low->getCoordinate(dim);
	}

	return area;
}

double Region::getMinimumDistance(const IShape & t_shape) const
{
	const Region *region = dynamic_cast<const Region*>(&t_shape);
	if (region != nullptr)
	{
		return getMinimumDistance(*region);
	}

	const Point *point = dynamic_cast<const Point*>(&t_shape);
	if (point != nullptr)
	{
		return getMinimumDistance(*point);
	}

	throw IllegalStateException("Region::getMinimumDistance : Encountered an unexpected shape.");
}

void Region::combinePoint(const Point & t_point)
{
	if (m_dimension != t_point.getDimension())
	{
		throw IllegalArgumentException("Region:combinePoint : Region and point dimensions do not match.");
	}

	for (uint8_t dim = 0; dim < m_dimension; dim++)
	{
		m_low->setCoordinate(std::fmin(m_low->getCoordinate(dim), t_point.getCoordinate(dim)), dim);
		m_high->setCoordinate(std::fmax(m_high->getCoordinate(dim), t_point.getCoordinate(dim)), dim);
	}
}

void Region::combineRegion(const Region & t_region)
{
	if (m_dimension != t_region.m_dimension)
	{
		throw IllegalArgumentException("Region:combineRegion : Region dimensions do not match.");
	}

	for (uint8_t dim = 0; dim < m_dimension; dim++)
	{
		m_low->setCoordinate(std::fmin(m_low->getCoordinate(dim), t_region.m_low->getCoordinate(dim)), dim);
		m_high->setCoordinate(std::fmax(m_high->getCoordinate(dim), t_region.m_high->getCoordinate(dim)), dim);
	}
}

double Region::getIntersectionArea(const Region & t_region) const
{
	if (m_dimension != t_region.m_dimension)
	{
		throw IllegalArgumentException("Region:getIntersectionArea : Region dimensions do not match.");
	}

	double area = 1.0;
	float f0, f1;

	for (uint8_t dim = 0; dim < m_dimension; dim++)
	{
		if (t_region.m_low->getCoordinate(dim) > m_high->getCoordinate(dim) ||
			t_region.m_high->getCoordinate(dim) < m_low->getCoordinate(dim))
		{
			return 0.0;
		}
		
		f0 = std::fmax(m_low->getCoordinate(dim), t_region.m_low->getCoordinate(dim));
		f1 = std::fmin(m_high->getCoordinate(dim), t_region.m_high->getCoordinate(dim));
		area *= f1 - f0;
	}

	return area;
}

Region * Region::getIntersectingRegion(const Region & t_region) const
{
	if (m_dimension != t_region.m_dimension)
	{
		throw IllegalArgumentException("Region:getIntersectingRegion : Region dimensions do not match.");
	}

	Region *intersection = new Region();
	intersection->extendInfinitely(m_dimension);

	for (uint8_t dim = 0; dim < m_dimension; dim++)
	{
		if (t_region.m_low->getCoordinate(dim) > m_high->getCoordinate(dim) ||
			t_region.m_high->getCoordinate(dim) < m_low->getCoordinate(dim))
		{
			delete intersection;
			return nullptr;
		}

		intersection->m_low->setCoordinate(std::fmax(m_low->getCoordinate(dim), t_region.m_low->getCoordinate(dim)), dim);
		intersection->m_high->setCoordinate(std::fmin(m_high->getCoordinate(dim), t_region.m_high->getCoordinate(dim)), dim);
	}

	return intersection;
}

double Region::getMargin() const
{
	double multiplicity = std::pow(2.0, static_cast<double>(m_dimension) - 1.0);
	double margin = 0.0;

	for (uint8_t dim = 0; dim < m_dimension; dim++)
	{
		margin += (m_high->getCoordinate(dim) - m_low->getCoordinate(dim)) * multiplicity;
	}

	return margin;
}

bool Region::validate() const
{
	return validate(m_low->getCoordinates(), m_high->getCoordinates(), m_dimension);
}

bool Region::containsPoint(const Point & t_point) const
{
	if (m_dimension != t_point.getDimension())
	{
		throw IllegalArgumentException("Region:containsPoint : Region and point dimensions do not match.");
	}

	for (uint8_t dim = 0; dim < m_dimension; dim++)
	{
		if (t_point.getCoordinate(dim) < m_low->getCoordinate(dim) || t_point.getCoordinate(dim) > m_high->getCoordinate(dim)) 
		{
			return false;
		}
	}

	return true;
}

bool Region::containsRegion(const Region & t_region) const
{
	if (m_dimension != t_region.m_dimension)
	{
		throw IllegalArgumentException("Region:containsRegion : Region dimensions do not match.");
	}

	for (uint8_t dim = 0; dim < m_dimension; dim++)
	{
		if (m_low->getCoordinate(dim) > t_region.m_low->getCoordinate(dim) ||
			m_high->getCoordinate(dim) < t_region.m_high->getCoordinate(dim))
		{
			return false;
		}
	}

	return true;
}

void Region::extendInfinitely(uint8_t t_dimension)
{
	Region region;
	region.m_dimension = t_dimension;
	for (uint8_t dim = 0; dim < region.m_dimension; dim++)
	{
		region.m_low->setCoordinate(-std::numeric_limits<float>::max(), dim);
		region.m_high->setCoordinate(std::numeric_limits<float>::max(), dim);
	}
	swap(*this, region);
}

double Region::getMinimumDistance(const Point & t_point) const
{
	if (m_dimension != t_point.getDimension())
	{
		throw IllegalArgumentException("Region:getMinimumDistance : Region and point dimensions do not match.");
	}

	double result = 0.0;

	for (uint8_t dim = 0; dim < m_dimension; dim++)
	{
		if (t_point.getCoordinate(dim) < m_low->getCoordinate(dim))
		{
			result += std::pow(m_low->getCoordinate(dim) - t_point.getCoordinate(dim), 2.0);
		}
		else if (t_point.getCoordinate(dim) > m_high->getCoordinate(dim))
		{
			result += std::pow(t_point.getCoordinate(dim) - m_high->getCoordinate(dim), 2.0);
		}
	}

	return std::sqrt(result);
}

double Region::getMinimumDistance(const Region & t_region) const
{
	if (m_dimension != t_region.m_dimension)
	{
		throw IllegalArgumentException("Region:getMinimumDistance : Region dimensions do not match.");
	}

	double result = 0.0;

	for (uint8_t dim = 0; dim < m_dimension; dim++)
	{
		if (t_region.m_high->getCoordinate(dim) < m_low->getCoordinate(dim))
		{
			result += std::pow(m_low->getCoordinate(dim) - t_region.m_high->getCoordinate(dim), 2.0);
		}
		else if (t_region.m_low->getCoordinate(dim) > m_high->getCoordinate(dim))
		{
			result += std::pow(t_region.m_low->getCoordinate(dim) - m_high->getCoordinate(dim), 2.0);
		}
	}

	return std::sqrt(result);
}

void Region::init(const float * t_low, const float * t_high, uint8_t t_dimension)
{
	bool result = validate(t_low, t_high, t_dimension);
	if (result)
	{
		m_dimension = t_dimension;
		m_low = new Point(t_low, t_dimension);
		m_high = new Point(t_high, t_dimension);
	}
	else 
	{
		m_dimension = 0;
		delete m_low;
		delete m_high;
	}
}

bool Region::intersectsPoint(const Point & t_point) const
{
	return containsPoint(t_point);
}

bool Region::intersectsRegion(const Region & t_region) const
{
	if (m_dimension != t_region.m_dimension)
	{
		throw IllegalArgumentException("Region:intersectsRegion : Region dimensions do not match.");
	}

	for (uint8_t dim = 0; dim < m_dimension; dim++)
	{
		if (t_region.m_high->getCoordinate(dim) < m_low->getCoordinate(dim) ||
			t_region.m_low->getCoordinate(dim) > m_high->getCoordinate(dim))
		{
			return false;
		}
	}

	return true;
}

bool Region::touchesPoint(const Point & t_point) const
{
	if (m_dimension != t_point.getDimension())
	{
		throw IllegalArgumentException("Region:touchesPoint : Region and point dimensions do not match.");
	}

	for (uint8_t dim = 0; dim < m_dimension; dim++)
	{
		if (std::abs(m_low->getCoordinate(dim) - t_point.getCoordinate(dim)) <= std::numeric_limits<float>::epsilon() ||
			std::abs(t_point.getCoordinate(dim) - m_high->getCoordinate(dim)) <= std::numeric_limits<float>::epsilon())
		{
			return true;
		}
	}

	return false;
}

bool Region::touchesRegion(const Region & t_region) const
{
	if (m_dimension != t_region.m_dimension)
	{
		throw IllegalArgumentException("Region:touchesRegion : Region dimensions do not match.");
	}

	for (uint8_t dim = 0; dim < m_dimension; dim++)
	{
		if (std::abs(m_low->getCoordinate(dim) - t_region.m_low->getCoordinate(dim)) <= std::numeric_limits<float>::epsilon() ||
			std::abs(t_region.m_high->getCoordinate(dim) - m_high->getCoordinate(dim)) <= std::numeric_limits<float>::epsilon())
		{
			return true;
		}
	}

	return false;
}

bool Region::validate(const float * t_low, const float * t_high, uint8_t t_dimension) const
{
	for (uint8_t dim = 0; dim < t_dimension; dim++)
	{
		if ((t_low[dim] - t_high[dim]) > std::numeric_limits<float>::epsilon())
		{
			return false;
		}
	}

	return true;
}
