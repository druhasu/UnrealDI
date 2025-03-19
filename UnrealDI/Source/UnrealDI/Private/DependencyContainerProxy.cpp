#include "Di/Impl/DependencyContainerProxy.h"
#include "DI/IInjector.h"
#include "DI/ObjectContainerBuilder.h"
#include "Misc/DataValidation.h"

void UDependencyContainerProxy::InitDependencies(TScriptInterface<IInjector> InInjector)
{
	for (UObject* const Dependency : SpawnedObjects)
	{
		InInjector->Inject(Dependency);
	}
}

void UDependencyContainerProxy::Initialize(FObjectContainerBuilder& ContainerBuilder)
{
	for (auto& [ObjectType, Registrations] : Dependencies)
	{
		UObject* const NewDependency = NewObject<UObject>(this, ObjectType);
		SpawnedObjects.Add(NewDependency);
		for (const FName& Registration : Registrations)
		{
			TValueOrError<FImplementedInterface, FString> Interface = FindInterfaceRecursive(NewDependency->GetClass(), Registration);
			if (Interface.HasError())
			{
				UE_LOG(LogTemp, Error, TEXT("%s"), *Interface.GetError())
				continue;
			}
			ContainerBuilder.RegisterInstance(NewDependency).As(Interface.GetValue().Class);
		}
	}
}

void UDependencyContainerProxy::Deinitialize()
{
	for (UObject* Element : SpawnedObjects)
	{
		Element->MarkAsGarbage();
		Element = nullptr;
	}
	SpawnedObjects.Empty();
}

#if WITH_EDITOR
EDataValidationResult UDependencyContainerProxy::IsDataValid(class FDataValidationContext& Context) const
{
	EDataValidationResult ValidationResult = Super::IsDataValid(Context);
	constexpr EDataValidationResult Invalid = EDataValidationResult::Invalid;
	if (ValidationResult == EDataValidationResult::Invalid)
	{
		return Invalid;
	}
	for (auto& [Key, Value] : Dependencies)
	{
		if (Key == nullptr)
		{
			Context.AddError(FText::FromString(TEXT("Dependencies has nullptr!")));
			ValidationResult = CombineDataValidationResults(ValidationResult, Invalid);
			continue;
		}
		if (Value.IsEmpty())
		{
			Context.AddError(FText::Format(INVTEXT("Registration Type for {0} is Empty!"), FText::FromString(Key->GetName())));
			ValidationResult = CombineDataValidationResults(ValidationResult, Invalid);
			continue;
		}
		if (HasDuplicates(Value))
		{
			Context.AddError(FText::Format(INVTEXT("Registration Types for {0} has duplication"), FText::FromString(Key->GetName())));
			ValidationResult = CombineDataValidationResults(ValidationResult, Invalid);
			continue;
		}
		for (const FName& Dependency : Value)
		{
			if (TValueOrError<FImplementedInterface, FString> Interface = FindInterfaceRecursive(Key.Get(), Dependency); Interface.HasError())
			{
				Context.AddError(FText::FromString(Interface.GetError()));
				ValidationResult = CombineDataValidationResults(ValidationResult, Invalid);
			}
		}
	}
	return ValidationResult; 
}

bool UDependencyContainerProxy::HasDuplicates(const TArray<FName>& InArray) const
{
	const TSet Set(InArray);
	return Set.Num() != InArray.Num();
}
#endif

TValueOrError<FImplementedInterface, FString> UDependencyContainerProxy::FindInterfaceRecursive(UClass* Class, const FName& InterfaceName)
{
	if (!Class)
	{
		return MakeError("Invalid Class Has Been Passed into FindInterface function");
	}
	UClass* HeadCopy = Class;
	while (HeadCopy != UObject::StaticClass())
	{
		for (const FImplementedInterface& Interface : HeadCopy->Interfaces)
		{
			if (Interface.Class.GetFName().IsEqual(InterfaceName))
			{
				return MakeValue(Interface);
			}
		}
		HeadCopy = HeadCopy->GetSuperClass();
	}
	return MakeError(FString::Format(TEXT("Interface {0} was not found in Class : {1}"), {InterfaceName.ToString(), Class->GetName()}));
}
