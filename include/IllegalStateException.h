#ifndef RTREE_ILLEGALSTATEEXCEPTION_H
#define RTREE_ILLEGALSTATEEXCEPTION_H

#include "IException.h"

namespace RTree
{
	namespace Tools
	{
		namespace Exceptions
		{
			class IllegalStateException : public Contracts::Common::IException
			{
				std::string m_message;
			public:
				IllegalStateException(std::string t_message);
				~IllegalStateException() {};
				virtual std::string message();
			};
		}
	}
}

#endif