#ifndef RTREE_IOBJECT_H
#define RTREE_IOBJECT_H

namespace RTree 
{
	namespace Contracts 
	{
		namespace Common 
		{
			class IObject 
			{
			public:
				virtual ~IObject() {};
				virtual IObject* clone() = 0;
			};
		}
	}
}

#endif