#ifndef RTREE_ILLEGALARGUMENTEXCEPTION_H
#define RTREE_ILLEGALARGUMENTEXCEPTION_H

#include "IException.h"

namespace RTree
{
	namespace Tools
	{
		namespace Exceptions
		{
			class IllegalArgumentException : public Contracts::Common::IException
			{
				std::string m_message;
			public:
				IllegalArgumentException(std::string t_message);
				~IllegalArgumentException() {};
				virtual std::string message();
			};
		}
	}
}

#endif