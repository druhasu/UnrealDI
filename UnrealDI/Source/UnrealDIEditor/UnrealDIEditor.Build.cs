// Copyright Andrei Sudarikov. All Rights Reserved.

using UnrealBuildTool;

public class UnrealDIEditor : ModuleRules
{
    public UnrealDIEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new []
            {
                "Core",
            });
        
        PrivateDependencyModuleNames.AddRange(
            new []
            {
                "BlueprintGraph",
                "CoreUObject",
                "Engine",
                "GraphEditor",
                "KismetCompiler",
                "Slate",
                "SlateCore",
                "UnrealEd",
                "UnrealDI",
                "PropertyEditor"
            });
    }
}
