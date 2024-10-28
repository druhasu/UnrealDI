// Copyright Andrei Sudarikov. All Rights Reserved.

#include "K2Node_InitDependencies.h"
#include "DI/Impl/UnrealDIBlueprintLibrary.h"
#include "DI/Impl/DependenciesRegistry.h"
#include "EdGraphSchema_K2.h"
#include "KismetCompiler.h"
#include "GraphEditorSettings.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "K2Node_CustomEvent.h"
#include "K2Node_CallFunction.h"
#include "K2Node_Self.h"
#include "Misc/EngineVersionComparison.h"

void UK2Node_InitDependencies::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
    Super::ExpandNode(CompilerContext, SourceGraph);

    /*
     * Pseudocode of what is generated here:
     *
     * void InitDependencies_ClassName(Dependency[0], Dependency[1], Dependency[2], Dependency[3])
     * {
     *     InitDependencies_SuperClassName(Dependency[0], Dependency[1]);
     *     Then(Dependency[0], Dependency[1], Dependency[2], Dependency[3]);
     *     ...
     * }
     */

    auto FindSuperClassInitDependencies = [&](UClass* InClass) -> UFunction*
    {
        InClass = Cast<UBlueprintGeneratedClass>(InClass->GetSuperClass());
        while (InClass)
        {
            FName FunctionName = UnrealDI_Impl::FDependenciesRegistry::MakeInitDependenciesFunctionName(InClass);
            UFunction* SuperInitDependenciesFunction = InClass->FindFunctionByName(FunctionName, EIncludeSuperFlag::ExcludeSuper);
            if (SuperInitDependenciesFunction != nullptr)
            {
                return SuperInitDependenciesFunction;
            }
            InClass = Cast<UBlueprintGeneratedClass>(InClass->GetSuperClass());
        }

        return nullptr;
    };

    auto FindMatchingPin = [](UK2Node* Node, UObject* InPinSubCategoryObject) -> UEdGraphPin*
    {
        UEdGraphPin** FoundPin = Node->Pins.FindByPredicate([&](UEdGraphPin* Pin) { return Pin->PinType.PinSubCategoryObject == InPinSubCategoryObject; });
        return FoundPin ? *FoundPin : nullptr;
    };

    UEdGraphPin* ExecPin = FindPin(UEdGraphSchema_K2::PN_Then);
    UFunction* SuperInitDependenciesFunction = FindSuperClassInitDependencies(CompilerContext.TargetClass);
    const UEdGraphSchema_K2* Schema = CompilerContext.GetSchema();

    // create node for new InitDependencies function
#if UE_VERSION_OLDER_THAN(5,5,0)
    UK2Node_CustomEvent* InitDependenciesEventNode = CompilerContext.SpawnIntermediateEventNode<UK2Node_CustomEvent>(this, ExecPin, SourceGraph);
#else
    UK2Node_CustomEvent* InitDependenciesEventNode = CompilerContext.SpawnIntermediateNode<UK2Node_CustomEvent>(this, SourceGraph);
#endif
    InitDependenciesEventNode->CustomFunctionName = UnrealDI_Impl::FDependenciesRegistry::MakeInitDependenciesFunctionName(CompilerContext.TargetClass);

    TArray<FName, TInlineAllocator<16>> PinsToMove;

    // copy pins from Super class InitDependencies function
    if (SuperInitDependenciesFunction != nullptr)
    {
        for (TFieldIterator<FProperty> It(SuperInitDependenciesFunction, EFieldIterationFlags::None); It; ++It)
        {
            if (It->HasAllPropertyFlags(CPF_Parm))
            {
                if (FObjectProperty* ObjectProperty = CastField<FObjectProperty>(*It))
                {
                    TSharedPtr<FUserPinInfo>& NewPin = InitDependenciesEventNode->UserDefinedPins.Emplace_GetRef(MakeShared<FUserPinInfo>());
                    NewPin->PinName = It->GetFName();
                    NewPin->DesiredPinDirection = EGPD_Output;
                    NewPin->PinType.PinCategory = UEdGraphSchema_K2::PC_Object;
                    NewPin->PinType.PinSubCategoryObject = ObjectProperty->PropertyClass;
                    PinsToMove.Add(NewPin->PinName);
                }
                else if (FInterfaceProperty* InterfaceProperty = CastField<FInterfaceProperty>(*It))
                {
                    TSharedPtr<FUserPinInfo>& NewPin = InitDependenciesEventNode->UserDefinedPins.Emplace_GetRef(MakeShared<FUserPinInfo>());
                    NewPin->PinName = It->GetFName();
                    NewPin->DesiredPinDirection = EGPD_Output;
                    NewPin->PinType.PinCategory = UEdGraphSchema_K2::PC_Interface;
                    NewPin->PinType.PinSubCategoryObject = InterfaceProperty->InterfaceClass;
                    PinsToMove.Add(NewPin->PinName);
                }
            }
        }
    }

    // configure pins of new InitDependencies function from our own dependencies
    for (const FInitDependenciesNodeEntry& Entry : Dependencies)
    {
        if (Entry.DependencyType != nullptr)
        {
            TSharedPtr<FUserPinInfo>& NewPin = InitDependenciesEventNode->UserDefinedPins.Emplace_GetRef(MakeShared<FUserPinInfo>());
            NewPin->PinName = Entry.DependencyType->GetFName();
            NewPin->DesiredPinDirection = EGPD_Output;
            NewPin->PinType.PinCategory = Entry.DependencyType->IsChildOf<UInterface>() ? UEdGraphSchema_K2::PC_Interface : UEdGraphSchema_K2::PC_Object;
            NewPin->PinType.PinSubCategoryObject = Entry.DependencyType;

            PinsToMove.Add(NewPin->PinName);
        }
    }

    // create all pins
    InitDependenciesEventNode->AllocateDefaultPins();
    UEdGraphPin* LastExecPin = InitDependenciesEventNode->GetThenPin();

    if (SuperInitDependenciesFunction != nullptr)
    {
        // emit call to InitDependencies of Super class
        UK2Node_CallFunction* CallSuperInitDependenciesNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
        CallSuperInitDependenciesNode->FunctionReference.SetExternalMember(GET_MEMBER_NAME_CHECKED(UUnrealDIBlueprintLibrary, CallFunctionIndirect), UUnrealDIBlueprintLibrary::StaticClass());
        CallSuperInitDependenciesNode->AllocateDefaultPins();

        // create Self node
        UK2Node_Self* SelfNode = CompilerContext.SpawnIntermediateNode<UK2Node_Self>(this, SourceGraph);
        SelfNode->AllocateDefaultPins();

        // connect ThisObject pin
        UEdGraphPin* ThisObjectPin = CallSuperInitDependenciesNode->FindPin(TEXT("ThisObject"));
        ensure(Schema->TryCreateConnection(SelfNode->FindPin(UEdGraphSchema_K2::PN_Self), ThisObjectPin));

        // initialize FunctionName pin
        UEdGraphPin* FunctionNamePin = CallSuperInitDependenciesNode->FindPin(TEXT("FunctionName"));
        FunctionNamePin->DefaultValue = SuperInitDependenciesFunction->GetName();

        // connect argument pins
        for (TFieldIterator<FProperty> It(SuperInitDependenciesFunction, EFieldIterationFlags::None); It; ++It)
        {
            if (It->HasAllPropertyFlags(CPF_Parm))
            {
                if (FObjectProperty* ObjectProperty = CastField<FObjectProperty>(*It))
                {
                    UEdGraphPin* DependencyPin = FindMatchingPin(InitDependenciesEventNode, ObjectProperty->PropertyClass);
                    UEdGraphPin* ArgumentPin = CallSuperInitDependenciesNode->CreatePin(EGPD_Input, DependencyPin->PinType, DependencyPin->PinName);
                    ensure(Schema->TryCreateConnection(DependencyPin, ArgumentPin));
                }
                else if (FInterfaceProperty* InterfaceProperty = CastField<FInterfaceProperty>(*It))
                {
                    UEdGraphPin* DependencyPin = FindMatchingPin(InitDependenciesEventNode, InterfaceProperty->InterfaceClass);
                    UEdGraphPin* ArgumentPin = CallSuperInitDependenciesNode->CreatePin(EGPD_Input, DependencyPin->PinType, DependencyPin->PinName);
                    ensure(Schema->TryCreateConnection(DependencyPin, ArgumentPin));
                }
            }
        }

        // connect execution pins
        ensure(Schema->TryCreateConnection(LastExecPin, CallSuperInitDependenciesNode->GetExecPin()));
        LastExecPin = CallSuperInitDependenciesNode->GetThenPin();
    }

    // connect own dependencies pins
    for (FName PinName : PinsToMove)
    {
        UEdGraphPin* EventNodePin = InitDependenciesEventNode->FindPin(PinName);
        UEdGraphPin* ThisNodePin = FindPin(PinName);

        if (ThisNodePin != nullptr)
        {
            CompilerContext.MovePinLinksToIntermediate(*ThisNodePin, *EventNodePin);
        }
    }

    // connect execution pin
    CompilerContext.MovePinLinksToIntermediate(*ExecPin, *LastExecPin);
}

void UK2Node_InitDependencies::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
    UClass* ActionKey = GetClass();

    if (ActionRegistrar.IsOpenForRegistration(ActionKey))
    {
        UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create<ThisClass>();
        NodeSpawner->DefaultMenuSignature.Category = INVTEXT("Dependency Injection");
        ActionRegistrar.AddBlueprintAction(NodeSpawner);
    }
}

void UK2Node_InitDependencies::AllocateDefaultPins()
{
    CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);

    for (UK2Node_InitDependencies* Node : GetAllNodesInClassHierarchy())
    {
        for (const FInitDependenciesNodeEntry& Entry : Node->Dependencies)
        {
            if (Entry.DependencyType != nullptr)
            {
                const bool bIsInterface = Entry.DependencyType->IsChildOf<UInterface>();

                CreatePin(EGPD_Output, bIsInterface ? UEdGraphSchema_K2::PC_Interface : UEdGraphSchema_K2::PC_Object, Entry.DependencyType, Entry.DependencyType->GetFName());
            }
        }
    }

    Super::AllocateDefaultPins();
}

bool UK2Node_InitDependencies::CanPasteHere(const UEdGraph* TargetGraph) const
{
    if (!Super::CanPasteHere(TargetGraph))
    {
        return false;
    }

    UBlueprint* Blueprint = FBlueprintEditorUtils::FindBlueprintForGraph(TargetGraph);
    if (!Blueprint)
    {
        // this graph does not belong to a blueprint
        return false;
    }

    // only one node allowed per blueprint
    TArray<UK2Node_InitDependencies*> Nodes;
    FBlueprintEditorUtils::GetAllNodesOfClass<UK2Node_InitDependencies>(Blueprint, Nodes);

    // we can only paste it if blueprint has no nodes of this class
    return Nodes.Num() == 0;
}

bool UK2Node_InitDependencies::CanDuplicateNode() const
{
    // only one node allowed per blueprint, so no duplication
    return false;
}

void UK2Node_InitDependencies::ValidateNodeDuringCompilation(FCompilerResultsLog& MessageLog) const
{
    const bool bHasInvalidDependencies = Dependencies.ContainsByPredicate([](const FInitDependenciesNodeEntry& Entry)
    {
        return !Entry.DependencyType;
    });

    if (bHasInvalidDependencies)
    {
        MessageLog.Error(TEXT("Node @@ contains invalid dependencies"), this);
    }
}

FText UK2Node_InitDependencies::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
    return INVTEXT("Init Dependencies");
}

FText UK2Node_InitDependencies::GetTooltipText() const
{
    return NSLOCTEXT("UnrealDI", "InitDependenciesNode.Tooltip", "Defines Event that receives dependencies from DI container");
}

FLinearColor UK2Node_InitDependencies::GetNodeTitleColor() const
{
    return GetDefault<UGraphEditorSettings>()->EventNodeTitleColor;
}

FSlateIcon UK2Node_InitDependencies::GetIconAndTint(FLinearColor& OutColor) const
{
    static FSlateIcon Icon("EditorStyle", "GraphEditor.Event_16x");
    return Icon;
}

TArray<UK2Node_InitDependencies*> UK2Node_InitDependencies::GetAllNodesInClassHierarchy()
{
    TArray<UK2Node_InitDependencies*> Result{ this };
    UBlueprint* Blueprint = GetParentBlueprint(GetBlueprint());

    while (Blueprint != nullptr)
    {
        TArray<UK2Node_InitDependencies*> Nodes;
        FBlueprintEditorUtils::GetAllNodesOfClass<UK2Node_InitDependencies>(Blueprint, Nodes);

        if (Nodes.Num() > 0)
        {
            Result.Add(Nodes[0]);
        }

        Blueprint = GetParentBlueprint(Blueprint);
    }

    Algo::Reverse(Result);

    return Result;
}

bool UK2Node_InitDependencies::FilterAction(FBlueprintActionFilter const& Filter, FBlueprintActionInfo& ActionInfo)
{
    if (ActionInfo.GetNodeClass() != ThisClass::StaticClass())
    {
        // do not filter other nodes spawners
        return false;
    }

    for (UBlueprint* Blueprint : Filter.Context.Blueprints)
    {
        // only one node allowed per blueprint
        TArray<UK2Node_InitDependencies*> Nodes;
        FBlueprintEditorUtils::GetAllNodesOfClass<UK2Node_InitDependencies>(Blueprint, Nodes);

        if (Nodes.Num() > 0)
        {
            return true;
        }
    }

    return false;
}

UBlueprint* UK2Node_InitDependencies::GetParentBlueprint(UBlueprint* Blueprint) const
{
    UBlueprintGeneratedClass* ParentClass = Cast<UBlueprintGeneratedClass>(Blueprint->ParentClass);
    return ParentClass != nullptr ? Cast<UBlueprint>(ParentClass->ClassGeneratedBy) : nullptr;
}
