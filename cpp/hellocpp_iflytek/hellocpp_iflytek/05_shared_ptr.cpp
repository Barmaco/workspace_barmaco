#include "vld.h"
#include "memory"

class Parent;
class Child 
{
public:
	~Child()
	{
		printf("");
	};

	std::weak_ptr<Parent> parent;
};

class Parent
{
public:
	~Parent()
	{
		printf("");
	};

	std::shared_ptr<Child> child;
};

void main05() 
{
	Child* childptr = new Child;
	Parent* parentptr = new Parent;

	std::shared_ptr<Child> child(childptr);
	std::shared_ptr<Parent> parent(parentptr);

	child->parent = parent;
	parent->child = child;


}