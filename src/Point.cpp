#include <cmath>
#include <cstring>
#include <limits>
#include "IllegalArgumentException.h"
#include "IllegalStateException.h"
#include "IndexOutOfBoundsException.h"
#include "Point.h"
#include "Region.h"

using namespace RTree::Components::Geometries;
using namespace RTree::Contracts;
using namespace RTree::Tools::Exceptions;

Point::Point() : m_coordinates(nullptr), m_dimension(0) {}

Point::Point(const Point & t_point) : m_dimension(t_point.m_dimension)
{
	m_coordinates = new float[m_dimension];
	std::memcpy(m_coordinates, t_point.m_coordinates, m_dimension * sizeof(float));
}

//Move semantics (Move constructor) optimization for C++11
//http://stackoverflow.com/questions/3106110/what-are-move-semantics
Point::Point(Point && t_point) : Point()
{
	swap(*this, t_point);
}

Point::Point(const float * t_coordinates, const uint8_t t_dimension) : m_dimension(t_dimension)
{
	m_coordinates = new float[m_dimension];
	std::memcpy(m_coordinates, t_coordinates, m_dimension * sizeof(float));
}

Point::~Point()
{
	delete[] m_coordinates;
	m_dimension = 0;
}

void Point::swap(Point & t_first, Point & t_second)
{
	std::swap(t_first.m_dimension, t_second.m_dimension);
	std::swap(t_first.m_coordinates, t_second.m_coordinates);
}

float * Point::getCoordinates() const
{
	return m_coordinates;
}

float Point::getCoordinate(uint8_t t_dimension) const
{
	if (t_dimension >= m_dimension)
	{
		throw IndexOutOfBoundsException("Point::getCoordinate : The specified dimension does not exist.");
	}

	return m_coordinates[t_dimension];
}

void Point::setCoordinate(float t_coordinate, uint8_t t_dimension)
{
	m_coordinates[t_dimension] = t_coordinate;
}

Point & Point::operator=(Point t_point)
{
	swap(*this, t_point);

	return *this;
}

bool Point::operator==(const Point & t_point) const
{
	if (m_dimension != t_point.m_dimension)
	{
		throw IllegalArgumentException("Point::operator== : Point dimensions do not match.");
	}

	for (uint8_t i = 0; i < m_dimension; i++)
	{
		if (std::abs(m_coordinates[i] - t_point.m_coordinates[i]) > std::numeric_limits<float>::epsilon())
		{
			return false;
		}
	}

	return true;
}

Point* Point::clone()
{
	return new Point(*this);
}

uint32_t Point::getByteArraySize() const
{
	return (sizeof(uint8_t) + m_dimension * sizeof(float));
}

void Point::loadFromByteArray(const uint8_t * t_data)
{
	Point point;
	std::memcpy(&point.m_dimension, t_data, sizeof(uint8_t));
	t_data += sizeof(uint8_t);
	std::memcpy(point.m_coordinates, t_data, point.m_dimension * sizeof(float));
	t_data += point.m_dimension * sizeof(float);
	swap(*this, point);
}

void Point::storeToByteArray(uint8_t ** t_data, uint32_t & t_length)
{
	t_length = getByteArraySize();
	*t_data = new uint8_t[t_length];
	std::memcpy(*t_data, &m_dimension, sizeof(uint8_t));
	*t_data += sizeof(uint8_t);
	std::memcpy(*t_data, m_coordinates, m_dimension * sizeof(float));
	*t_data += m_dimension * sizeof(float);
}

bool Point::intersectsShape(const IShape & t_shape) const
{
	const Point* point = dynamic_cast<const Point*>(&t_shape);
	if (point != nullptr)
	{
		return false;
	}

	return t_shape.intersectsShape(*this);
}

bool Point::containsShape(const IShape & t_shape) const
{
	return false;
}

bool Point::touchesShape(const IShape & t_shape) const
{
	const Point* point = dynamic_cast<const Point*>(&t_shape);
	if (point != nullptr)
	{
		if (*this == *point)
		{
			return true;
		}
		return false;
	}

	return t_shape.touchesShape(*this);
}

void Point::getCenter(Point& t_point) const
{
	t_point = *this;
}

uint8_t Point::getDimension() const
{
	return m_dimension;
}

void Point::getMBR(Region& t_region) const
{
	t_region = Region(m_coordinates, m_coordinates, m_dimension);
}

double Point::getArea() const
{
	return 0.0;
}

double Point::getMinimumDistance(const IShape & t_shape) const
{
	const Point* point = dynamic_cast<const Point*>(&t_shape);
	if (point != nullptr)
	{
		return getMinimumDistance(*point);
	}

	return t_shape.getMinimumDistance(*this);
}

double Point::getMinimumDistance(const Point & t_point) const
{
	if (m_dimension != t_point.m_dimension)
	{
		throw IllegalStateException("Point dimensions do not match.");
	}

	double result = 0.0;
	
	for (uint8_t dim = 0; dim < m_dimension; dim++)
	{
		result += std::pow(m_coordinates[dim] - t_point.m_coordinates[dim], 2.0);
	}

	return std::sqrt(result);
}

void Point::extendInfinitely(uint8_t t_dimension)
{
	Point point;
	point.m_dimension = t_dimension;
	for (uint8_t dim = 0; dim < point.m_dimension; dim++) {
		point.m_coordinates[dim] = std::numeric_limits<float>::max();
	}
	swap(*this, point);
}