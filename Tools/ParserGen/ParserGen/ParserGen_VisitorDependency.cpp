#include "ParserGen.h"

void AddCreateDependency(ParsingSymbol* type, ParsingSymbolManager* manager, SortedList<ParsingSymbol*>& visitedTypes, VisitorDependency& dependency)
{
	List<ParsingSymbol*> children;
	SearchChildClasses(type, manager->GetGlobal(), manager, children);
	if (children.Count() > 0)
	{
		while (type->GetDescriptorSymbol())
		{
			type = type->GetDescriptorSymbol();
		}
		if (!dependency.virtualDependencies.Contains(type))
		{
			dependency.virtualDependencies.Add(type);
		}
	}
	else
	{
		if (!dependency.createDependencies.Contains(type))
		{
			dependency.createDependencies.Add(type);
		}
	}
}

void SearchCreateDependencies(ParsingSymbol* type, ParsingSymbolManager* manager, SortedList<ParsingSymbol*>& visitedTypes, VisitorDependency& dependency)
{
	if (type->GetType() != ParsingSymbol::ClassType) return;
	if (visitedTypes.Contains(type)) return;
	visitedTypes.Add(type);

	if (auto parentType = type->GetDescriptorSymbol())
	{
		SearchCreateDependencies(parentType, manager, visitedTypes, dependency);
	}

	vint count = type->GetSubSymbolCount();
	for (vint i = 0; i < count; i++)
	{
		auto field = type->GetSubSymbol(i);
		if (field->GetType() == ParsingSymbol::ClassField)
		{
			auto fieldType = field->GetDescriptorSymbol();
			switch (fieldType->GetType())
			{
			case ParsingSymbol::ClassType:
				AddCreateDependency(fieldType, manager, visitedTypes, dependency);
				break;
			case ParsingSymbol::ArrayType:
				AddCreateDependency(fieldType->GetDescriptorSymbol(), manager, visitedTypes, dependency);
				break;
			default:;
			}
		}
	}
}

void SearchDependencies(ParsingSymbol* type, ParsingSymbolManager* manager, SortedList<ParsingSymbol*>& visitedTypes, VisitorDependency& dependency)
{
	{
		List<ParsingSymbol*> children;
		SearchChildClasses(type, manager->GetGlobal(), manager, children);
		if (children.Count() > 0)
		{
			dependency.subVisitorDependencies.Add(type);
		}
		else
		{
			auto current = type;
			while (current)
			{
				if (!dependency.fillDependencies.Contains(current))
				{
					dependency.fillDependencies.Add(current);
				}
				current = current->GetDescriptorSymbol();
			}
		}
	}
	SearchCreateDependencies(type, manager, visitedTypes, dependency);
}