// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "K2Node.h"
#include "K2Node_InitDependencies.generated.h"

USTRUCT()
struct FInitDependenciesNodeEntry
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere)
    TObjectPtr<UClass> DependencyType;
};

UCLASS()
class UK2Node_InitDependencies : public UK2Node
{
    GENERATED_BODY()

public:
    //~ Begin UK2Node Interface
    void ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
    void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
    bool IsActionFilteredOut(class FBlueprintActionFilter const& Filter) override { return false; }
    bool ShouldShowNodeProperties() const override { return true; }
    //~ End UK2Node Interface

    //~ Begin UEdGraphNode Interface
    void AllocateDefaultPins() override;
    bool CanPasteHere(const UEdGraph* TargetGraph) const override;
    bool CanDuplicateNode() const override;
    void ValidateNodeDuringCompilation(class FCompilerResultsLog& MessageLog) const override;
    FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
    FText GetTooltipText() const override;
    FLinearColor GetNodeTitleColor() const override;
    FSlateIcon GetIconAndTint(FLinearColor& OutColor) const override;
    //~ End UEdGraphNode Interface

    TArray<UK2Node_InitDependencies*> GetAllNodesInClassHierarchy();
    static bool FilterAction(FBlueprintActionFilter const& Filter, FBlueprintActionInfo& ActionInfo);

    UPROPERTY(EditAnywhere)
    TArray<FInitDependenciesNodeEntry> Dependencies;

private:
    UBlueprint* GetParentBlueprint(UBlueprint* Blueprint) const;
};
