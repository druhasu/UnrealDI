// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "IDetailCustomization.h"

class UK2Node_InitDependencies;
class UBlueprint;

class FInitDependenciesNodeDetails : public IDetailCustomization
{
public:
    using ThisClass = FInitDependenciesNodeDetails;

    static TSharedRef<IDetailCustomization> Make() { return MakeShared<FInitDependenciesNodeDetails>(); }

    void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

private:
    void OnDependenciesChanged();

    TWeakObjectPtr<UK2Node_InitDependencies> CurrentNode;
};
