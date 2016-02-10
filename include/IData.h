#ifndef RTREE_IDATA_H
#define RTREE_IDATA_H

#include <stdint.h>
#include "IEntry.h"

namespace RTree
{
	namespace Contracts
	{
		class IData : public IEntry
		{
		public:
			virtual ~IData() {};
			virtual void getData(uint32_t& len, uint8_t** data) const = 0;
		};
	}
}

#endif