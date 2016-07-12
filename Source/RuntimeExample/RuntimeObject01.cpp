#include "ObjectInterfacePerModule.h"

#include "IObject.h"
#include "IUpdateable.h"
#include "InterfaceIds.h"
#include <iostream>


class RuntimeObject01 : public TInterface<IID_IUPDATEABLE,IUpdateable>
{
	int var;
public:
	RuntimeObject01()
	{
		var = rand();
	}
	virtual void Update( float deltaTime )
	{
		//std::cout << "Runtime Object 01 update called! " << rand << " \n";
	}
};

REGISTERCLASS(RuntimeObject01);
