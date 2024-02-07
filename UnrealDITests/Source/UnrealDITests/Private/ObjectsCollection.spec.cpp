// Copyright Andrei Sudarikov. All Rights Reserved.

#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"

#include "DI/ObjectsCollection.h"
#include "MockReader.h"

BEGIN_DEFINE_SPEC(FObjectsCollectionSpec, "UnrealDI.ObjectsCollection", EAutomationTestFlags::ClientContext | EAutomationTestFlags::EditorContext | EAutomationTestFlags::ServerContext | EAutomationTestFlags::EngineFilter)
TArrayView<UObject*> CreateSourceData() const;
END_DEFINE_SPEC(FObjectsCollectionSpec)

void FObjectsCollectionSpec::Define()
{
    It("Should support ranged for loop with UObject", [this]()
    {
        TArrayView<UObject*> Source = CreateSourceData();

        TObjectsCollection<UMockReader> Collection(Source.GetData(), Source.Num());
        TestEqual("Collection.Num()", Collection.Num(), 3);

        int32 Index = 0;
        for (UMockReader* Object : Collection)
        {
            TestEqual<UObject*>(FString::Printf(TEXT("Collection[%d]"), Index), Object, Source[Index]);
            ++Index;
        }
    });

    It("Should support ranged for loop with TScriptInterface", [this]()
    {
        TArrayView<UObject*> Source = CreateSourceData();

        TObjectsCollection<IReader> Collection(Source.GetData(), Source.Num());
        TestEqual("Collection.Num()", Collection.Num(), 3);

        int32 Index = 0;
        for (TScriptInterface<IReader> Object : Collection)
        {
            TestEqual<UObject*>(FString::Printf(TEXT("Collection[%d]"), Index), Object.GetObject(), Source[Index]);
            ++Index;
        }
    });

    It("Should return Array with UObject", [this]()
    {
        TArrayView<UObject*> Source = CreateSourceData();

        TObjectsCollection<UMockReader> Collection(Source.GetData(), Source.Num());
        TestEqual("Collection.Num()", Collection.Num(), Source.Num());

        TArray<UMockReader*> Result = Collection.ToArray();
        TestEqual("Result.Num()", Result.Num(), Collection.Num());
        
        for (int32 Index = 0; Index< Result.Num(); ++Index)
        {
            TestEqual<UObject*>(FString::Printf(TEXT("Result[%d]"), Index), Result[Index], Source[Index]);
        }
    });

    It("Should return Array with TScriptInterface", [this]()
    {
        TArrayView<UObject*> Source = CreateSourceData();

        TObjectsCollection<IReader> Collection(Source.GetData(), Source.Num());
        TestEqual("Collection.Num()", Collection.Num(), Source.Num());

        TArray<TScriptInterface<IReader>> Result = Collection.ToArray();
        TestEqual("Result.Num()", Result.Num(), Collection.Num());

        for (int32 Index = 0; Index < Result.Num(); ++Index)
        {
            TestEqual<UObject*>(FString::Printf(TEXT("Result[%d]"), Index), Result[Index].GetObject(), Source[Index]);
        }
    });

    It("Should fill Array with UObject", [this]()
    {
        TArrayView<UObject*> Source = CreateSourceData();

        TObjectsCollection<UMockReader> Collection(Source.GetData(), Source.Num());
        TestEqual("Collection.Num()", Collection.Num(), Source.Num());

        TArray<UMockReader*> Result;
        Collection.ToArray(Result);
        TestEqual("Result.Num()", Result.Num(), Collection.Num());

        for (int32 Index = 0; Index < Result.Num(); ++Index)
        {
            TestEqual<UObject*>(FString::Printf(TEXT("Result[%d]"), Index), Result[Index], Source[Index]);
        }
    });

    It("Should fill Array with TScriptInterface", [this]()
    {
        TArrayView<UObject*> Source = CreateSourceData();

        TObjectsCollection<IReader> Collection(Source.GetData(), Source.Num());
        TestEqual("Collection.Num()", Collection.Num(), Source.Num());

        TArray<TScriptInterface<IReader>> Result;
        Collection.ToArray(Result);
        TestEqual("Result.Num()", Result.Num(), Collection.Num());

        for (int32 Index = 0; Index < Result.Num(); ++Index)
        {
            TestEqual<UObject*>(FString::Printf(TEXT("Result[%d]"), Index), Result[Index].GetObject(), Source[Index]);
        }
    });
}


TArrayView<UObject*> FObjectsCollectionSpec::CreateSourceData() const
{
    const int32 NumElements = 3;

    // TObjectCollection will free memory in its destructor so we must allocate it properly
    UObject** Buffer = (UObject**)FMemory::Malloc(sizeof(UObject*) * NumElements, __alignof(UObject*));

    for (int32 Index = 0; Index < NumElements; ++Index)
    {
        Buffer[Index] = NewObject<UMockReader>();
    }

    return TArrayView<UObject*>((UObject**)Buffer, 3);
}