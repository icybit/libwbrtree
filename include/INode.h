#ifndef RTREE_INODE_H
#define RTREE_INODE_H

#include <stdint.h>
#include "IEntry.h"
#include "ISerializable.h"

namespace RTree
{
	namespace Contracts
	{
		class INode : public IEntry, public Common::ISerializable
		{
		public:
			virtual ~INode() {};
			virtual uint32_t getChildrenCount() const = 0;
			virtual int64_t getChildIdentifier(uint32_t index) const = 0;
			virtual void getChildData(uint32_t index, uint32_t& len, uint8_t** data) const = 0;
			virtual void getChildShape(uint32_t index, IShape** out) const = 0;
			virtual uint32_t getLevel() const = 0;
			virtual bool isIndex() const = 0;
			virtual bool isLeaf() const = 0;
		};
	}
}

#endif