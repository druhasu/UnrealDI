#pragma once

#include "DependencyContainerProxy.generated.h"

class FObjectContainerBuilder;
class IInjector;

USTRUCT(BlueprintType)
struct FDependencyContainer
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UObject> Object;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FName> RegistrationType;
};

UCLASS(Blueprintable)
class UNREALDI_API UDependencyContainerProxy : public UObject
{
	GENERATED_BODY()

public:

	void InitDependencies(TScriptInterface<IInjector> InInjector);
	void Initialize(FObjectContainerBuilder& ContainerBuilder);
	void Deinitialize();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FDependencyContainer> Dependencies;

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
	bool HasDuplicates(const TArray<FName>& InArray) const;
#endif

private:

	UPROPERTY()
	TArray<UObject*> SpawnedObjects;
	
	FDependencyContainer* DependencyContainer;

	static TValueOrError<FImplementedInterface, FString> FindInterfaceRecursive(UClass* Class, const FName& InterfaceName);
};