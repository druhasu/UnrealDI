// Copyright Andrei Sudarikov. All Rights Reserved.

#include "DI/IInjectOnConstruction.h"

UObjectContainer* IInjectOnConstruction::NextConstructorContainer = nullptr;
IInjectOnConstruction* IInjectOnConstruction::NextExpectedObject = nullptr;
const UObjectBase* IInjectOnConstruction::NextUObject = nullptr;