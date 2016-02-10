#ifndef RTREE_IENTRY_H
#define RTREE_IENTRY_H

#include <stdint.h>
#include "IObject.h"
#include "IShape.h"

namespace RTree
{
	namespace Contracts
	{
		class IEntry : public Common::IObject
		{
		public:
			virtual ~IEntry() {};
			virtual int64_t getIdentifier() const = 0;
			virtual void getShape(IShape** out) const = 0;
		};
	}
}

#endif