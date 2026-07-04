// Copyright Andrei Sudarikov. All Rights Reserved.

using UnrealBuildTool;

public class UnrealDITests : ModuleRules
{
    public UnrealDITests(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        bUseUnity = false; // some tests may report false positive results without this

        PublicDependencyModuleNames.AddRange(
        [
            "Core",
            "UMG"
            // ... add other public dependencies that you statically link with here ...
        ]);

        PrivateDependencyModuleNames.AddRange(
        [
            "CoreUObject",
            "Engine",
            // ... add private dependencies that you statically link with here ...	
        ]);

        UnrealDI.Setup(this);
    }
}
