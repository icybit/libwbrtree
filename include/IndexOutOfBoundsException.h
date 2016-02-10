#ifndef RTREE_INDEXOUTOFBOUNDSEXCEPTION_H
#define RTREE_INDEXOUTOFBOUNDSEXCEPTION_H

#include "IException.h"

namespace RTree
{
	namespace Tools
	{
		namespace Exceptions
		{
			class IndexOutOfBoundsException : public Contracts::Common::IException
			{
				std::string m_message;
			public:
				IndexOutOfBoundsException(std::string t_message);
				~IndexOutOfBoundsException() {};
				virtual std::string message();
			};
		}
	}
}

#endif