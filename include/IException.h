#ifndef RTREE_IEXCEPTION_H
#define RTREE_IEXCEPTION_H

#include <string>

namespace RTree
{
	namespace Contracts
	{
		namespace Common
		{
			class IException
			{
			public:
				virtual ~IException() {};
				virtual std::string message() = 0;
			};
		}
	}
}

#endif