// Copyright Andrei Sudarikov. All Rights Reserved.

#include "InitDependenciesNodeDetails.h"
#include "K2Node_InitDependencies.h"
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "DetailWidgetRow.h"
#include "Kismet2/BlueprintEditorUtils.h"

void FInitDependenciesNodeDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
    TArray<TWeakObjectPtr<UObject>> Objects;
    DetailBuilder.GetObjectsBeingCustomized(Objects);

    if (Objects.Num() != 1)
    {
        // we can only edit single node
        return;
    }

    CurrentNode = Cast<UK2Node_InitDependencies>(Objects[0].Get());
    TArray<UK2Node_InitDependencies*> AllNodes = CurrentNode->GetAllNodesInClassHierarchy();

    // make default property editor hidden. we will show it later in proper category
    TSharedRef<IPropertyHandle> DependenciesPropertyHandle = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UK2Node_InitDependencies, Dependencies));
    DependenciesPropertyHandle->MarkHiddenByCustomization();

    // listen to changes in Dependencies property
    FSimpleDelegate ChangeDelegate = FSimpleDelegate::CreateSP(this, &ThisClass::OnDependenciesChanged);
    DependenciesPropertyHandle->SetOnPropertyValueChanged(ChangeDelegate);
    DependenciesPropertyHandle->SetOnChildPropertyValueChanged(ChangeDelegate);

    // build list of all dependencies in parent blueprint classes and current blueprint
    for (UK2Node_InitDependencies* Node : AllNodes)
    {
        FName CategoryName = Node->GetBlueprint()->GetFName();

        // make individual category for each class
        IDetailCategoryBuilder& Category = DetailBuilder.EditCategory(CategoryName);

        if (CurrentNode == Node)
        {
            Category.AddProperty(DependenciesPropertyHandle);
        }
        else
        {
            for (const FInitDependenciesNodeEntry& Entry : Node->Dependencies)
            {
                FText RowName = FText::FromString(GetNameSafe(Entry.DependencyType));

                Category.AddCustomRow(RowName)
                .NameContent()
                [
                    SNew(STextBlock)
                    .Font(IPropertyTypeCustomizationUtils::GetRegularFont())
                    .Text(RowName)
                ]
                .ValueContent()
                [
                    // Placeholder text. Later it will become a dropdown where you can configure how dependecy is provided
                    SNew(STextBlock)
                    .Font(IPropertyTypeCustomizationUtils::GetRegularFont())
                    .Text(INVTEXT("From Container"))
                ];
            }
        }
    }
}

void FInitDependenciesNodeDetails::OnDependenciesChanged()
{
    UK2Node_InitDependencies* TargetNode = CurrentNode.Get();

    // reconstruct current node and all nodes in derived classes
    if (TargetNode != nullptr)
    {
        const bool bOriginalValue = TargetNode->bDisableOrphanPinSaving;
        TargetNode->bDisableOrphanPinSaving = true;

        TargetNode->ReconstructNode();

        TargetNode->bDisableOrphanPinSaving = bOriginalValue;

        // we iterate over nodes here instead of classes because we are only interested in 'live' objects
        // unloaded classes are not affected by the change
        for (TObjectIterator<UK2Node_InitDependencies> It; It; ++It)
        {
            if (FBasePinChangeHelper::NodeIsNotTransient(*It) && It->GetAllNodesInClassHierarchy().Contains(TargetNode))
            {
                It->ReconstructNode();
            }
        }
    }
}
