#include "ObjectInterfacePerModule.h"

#include "IObject.h"
#include "IUpdateable.h"
#include "InterfaceIds.h"
#include <iostream>


class RuntimeObject02 : public TInterface<IID_IUPDATEABLE, IUpdateable>
{
	int var;
public:
	RuntimeObject02()
	{
		var = rand();
	}
	virtual void Update(float deltaTime)
	{
		//std::cout << "Runtime Object 02 update called! " << rand << " \n";
	}
};

REGISTERCLASS(RuntimeObject02);
