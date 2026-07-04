// Copyright Andrei Sudarikov. All Rights Reserved.

using System.Collections.Generic;
using UnrealBuildTool;

public class UnrealDI : ModuleRules
{
    public static List<string> SetupModules { get; set; } = [];

    public UnrealDI(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
        [
            "Core",
            "UMG",
            // ... add other public dependencies that you statically link with here ...
        ]);

        PrivateDependencyModuleNames.AddRange(
        [
            "CoreUObject",
            "Engine",
            // ... add private dependencies that you statically link with here ...	
        ]);
    }

    public static void Setup(ModuleRules module)
    {
        string thisModuleName = nameof(UnrealDI);

        if (!module.PrivateDependencyModuleNames.Contains(thisModuleName) &&
            !module.PublicDependencyModuleNames.Contains(thisModuleName))
        {
            // add to Public dependencies by default for ease of use
            module.PublicDependencyModuleNames.Add(thisModuleName);
        }

        SetupModules.Add(module.Name);

#if UE_5_8_OR_LATER
        module.FilesToGenerate.Add(
            $"{module.Name}.DI.gen.cpp",
            [
                $"#if __has_include(\"{module.Name}.DI.gen.keep\")",
                $"#include \"{module.Name}.DI.gen.keep\"",
                "#endif",
            ]);
#endif
    }
}
