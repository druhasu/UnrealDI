// Copyright Andrei Sudarikov. All Rights Reserved.

using UnrealBuildTool;

public class UnrealDIEditor : ModuleRules
{
    public UnrealDIEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
        [
            "Core",
        ]);

        PrivateDependencyModuleNames.AddRange(
        [
            "BlueprintGraph",
            "CoreUObject",
            "Engine",
            "GraphEditor",
            "KismetCompiler",
            "Slate",
            "SlateCore",
            "UnrealEd",
            "UnrealDI",
        ]);

        UnrealDI.Setup(this);
    }
}
