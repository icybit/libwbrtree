#ifndef RTREE_ISERIALIZABLE_H
#define RTREE_ISERIALIZABLE_H

#include <stdint.h>

namespace RTree
{
	namespace Contracts
	{
		namespace Common
		{
			class ISerializable
			{
			public:
				virtual ~ISerializable() {};
				virtual uint32_t getByteArraySize() const = 0;
				virtual void loadFromByteArray(const uint8_t* t_data) = 0;
				virtual void storeToByteArray(uint8_t** t_data, uint32_t& t_length) = 0;
			};
		}
	}
}

#endif