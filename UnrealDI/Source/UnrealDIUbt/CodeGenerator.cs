// Copyright Andrei Sudarikov. All Rights Reserved.

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using EpicGames.Core;
using EpicGames.UHT.Tables;
using EpicGames.UHT.Tokenizer;
using EpicGames.UHT.Types;
using EpicGames.UHT.Utils;
using FuzzySharp;

namespace UnrealDI;

[UnrealHeaderTool]
public static class CodeGenerator
{
    struct ClassEntry
    {
        public UhtClass Class;
        public List<UhtClass> Dependencies;
    };

    enum InitDependenciesFindResult
    {
        Found,
        FoundSimilar,
        FoundMultiple,
        NotFound,
    };

    private const string ModuleName = "UnrealDI";
    private const string ExporterName = "UnrealDIGenerator";

    static CodeGenerator()
    {
        AppDomain.CurrentDomain.AssemblyResolve += OnAssemblyResolve;

        // Debugging helper
        //AppDomain.CurrentDomain.FirstChanceException += (s, e) => { Log.WriteLine(LogEventType.Error, e.Exception.ToString()!); };
    }

    private static Assembly? OnAssemblyResolve(object? sender, ResolveEventArgs args)
    {
        var fileName = new AssemblyName(args.Name).Name + ".dll";
        if (fileName != null)
        {
            string assemblyPath = Path.Combine(Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location)!, fileName);
            return Assembly.LoadFile(assemblyPath);
        }

        return null;
    }

    [UhtExporter(Name = ExporterName, ModuleName = ModuleName, Options = UhtExporterOptions.Default)]
    public static void GenerateCode(IUhtExportFactory factory)
    {
        List<ClassEntry> affectedClasses = FindClasses(factory);

        WriteGeneratedCode(factory, affectedClasses);
    }

    private static List<ClassEntry> FindClasses(IUhtExportFactory factory)
    {
        List<ClassEntry> result = new();

        foreach (UhtHeaderFile header in factory.Session.HeaderFiles)
        {
#if UE_5_5_OR_LATER
            if (header.Module.IsPartOfEngine)
#else
            if (header.Package.IsPartOfEngine)
#endif
            {
                // skip Engine packages, no need to generate anything for them
                continue;
            }

            foreach (UhtClass uhtClass in header.Children.OfType<UhtClass>())
            {
                if (uhtClass.MetaData.ContainsKey("NoInitDependencies"))
                {
                    // skip classes marked with NoInitDependencies metadata
                    continue;
                }

                var dependencies = FindClassDependencies(uhtClass, factory.Session);

                if (dependencies != null)
                {
                    result.Add(new ClassEntry { Class = uhtClass, Dependencies = dependencies });
                }
            }
        }

        return result;
    }

    private static void WriteGeneratedCode(IUhtExportFactory factory, List<ClassEntry> affectedClasses)
    {
        foreach (var classesPerModule in affectedClasses.GroupBy(c => c.Class.Package.Module))
        {
            using BorrowStringBuilder borrower = new(StringBuilderCache.Big);
            var sb = borrower.StringBuilder;

            sb.Append("// Copyright Andrei Sudarikov. All Rights Reserved.\n\n");
            sb.Append("/* ---------------------------------------------- *\n");
            sb.Append("     This file was generated by UnrealDI plugin\n");
            sb.Append("     DO NOT modify it manually!\n");
            sb.Append(" * ---------------------------------------------- */\n\n");

            sb.Append("#include \"DI/Impl/ExposeDependencies.h\"\n\n");

            var allIncludeFiles = classesPerModule
                .SelectMany(entry => entry.Dependencies.Select(d => d.HeaderFile).Prepend(entry.Class.HeaderFile))
                .Distinct();

            foreach (UhtHeaderFile? headerFile in allIncludeFiles)
            {
                sb.Append("#include \"");
                sb.Append(GetProperIncludePath(headerFile));
                sb.Append("\"\n");
            }

            borrower.StringBuilder.Append('\n');

            foreach (ClassEntry entry in classesPerModule)
            {
                sb.Append("EXPOSE_DEPENDENCIES_INTERNAL(");
                sb.Append(entry.Class.SourceName);
                sb.Append(");\n");
            }

#if UE_5_5_OR_LATER
            UhtModule module = classesPerModule.Key;
            var outputPath = Path.Combine(module.Module.OutputDirectory, $"{module.Module.Name}.DI.gen.cpp");
#else
            UHTManifest.Module module = classesPerModule.Key;
            var outputPath = Path.Combine(module.OutputDirectory, $"{module.Name}.DI.gen.cpp");
#endif

            factory.CommitOutput(outputPath, sb.ToString());
        }
    }

    private static List<UhtClass>? FindClassDependencies(UhtClass uhtClass, UhtSession session)
    {
        InitDependenciesFindResult findResult = FindInitDependenciesDeclaration(uhtClass, out UhtDeclaration declaration);
        if (findResult != InitDependenciesFindResult.Found)
        {
            switch (findResult)
            {
                case InitDependenciesFindResult.FoundSimilar:
                    uhtClass.LogError(declaration.Tokens[0].InputLine, "InitDependencies method has incorrect name. Check spelling. You can exclude this class from checks using UCLASS(meta=(NoInitDependencies))");
                    break;
                case InitDependenciesFindResult.FoundMultiple:
                    uhtClass.LogError(declaration.Tokens[0].InputLine, "Class has multiple InitDependencies methods. You can exclude this class from checks using UCLASS(meta=(NoInitDependencies))");
                    break;
            }

            return null;
        }

        UhtTokenReplayReader tokenReader = UhtTokenReplayReader.GetThreadInstance(uhtClass, uhtClass.HeaderFile.Data.Memory, declaration.Tokens, UhtTokenType.EndOfDeclaration);
        if (!IsValidDeclaration(uhtClass, tokenReader))
        {
            return null;
        }

        List<UhtClass> result = new();

        // try find all UClasses or IInterfaces among the rest of the tokens
        while (!tokenReader.IsEOF)
        {
            ref UhtToken token = ref tokenReader.PeekToken();
            if (token.IsIdentifier())
            {
                // quick filter for tokens that may not be Class or Interface names
                if (token.Value.Span[0] == 'U' || token.Value.Span[0] == 'I')
                {
                    UhtType? type = session.FindType(null, UhtFindOptions.Class | UhtFindOptions.SourceName, token.Value.ToString());

                    if (type is UhtClass dependencyClass)
                    {
                        if (!result.Contains(dependencyClass))
                        {
                            result.Add(dependencyClass);
                        }
                    }
                }
            }

            tokenReader.ConsumeToken();
        }

        return result;
    }

    private static bool IsValidDeclaration(UhtClass uhtClass, IUhtTokenReader tokenReader)
    {
        var errorLine = tokenReader.InputLine;

        if (tokenReader.TryOptional("virtual"))
        {
            uhtClass.LogError(errorLine, "InitDependencies must not be virtual");
            return false;
        }

        if (tokenReader.TryOptional("static"))
        {
            uhtClass.LogError(errorLine, "InitDependencies must not be static");
            return false;
        }

        if (!tokenReader.TryOptional("void"))
        {
            uhtClass.LogError(errorLine, "InitDependencies must return void");
            return false;
        }

        if (!tokenReader.TryOptional("InitDependencies"))
        {
            uhtClass.LogError(errorLine, "InitDependencies has invalid signature");
            return false;
        }

        if (!tokenReader.TryOptional('('))
        {
            uhtClass.LogError(errorLine, "InitDependencies must be a function");
            return false;
        }

        return true;
    }

    private static InitDependenciesFindResult FindInitDependenciesDeclaration(UhtClass uhtClass, out UhtDeclaration foundDeclaration)
    {
        if (uhtClass.Declarations == null)
        {
            foundDeclaration = new();
            return InitDependenciesFindResult.NotFound;
        }

        int bestDeclarationIndex = -1;
        int bestDeclarationScore = 0;
        bool foundMatchingDeclaration = false;
        const int ScoreThreshold = 90;

        //Log.WriteLine(LogEventType.Warning, $"Found declarations in class {uhtClass.SourceName}");

        for (int declarationIndex = 0; declarationIndex < uhtClass.Declarations.Count; ++declarationIndex)
        {
            UhtDeclaration declaration = uhtClass.Declarations[declarationIndex];
            if (declaration.Tokens.Length == 0)
                continue;

            // Debug. Dumps all declarations to log
            //Log.WriteLine(LogEventType.Warning, ".   " + string.Join(" ", declaration.Tokens.Select(t => t.Value.ToString())));

            for (int tokenIndex = 0; tokenIndex < declaration.Tokens.Length; ++tokenIndex)
            {
                UhtToken token = declaration.Tokens[tokenIndex];

                if (token.IsIdentifier())
                {
                    if (token.IsValue("InitDependencies"))
                    {
                        if (foundMatchingDeclaration)
                        {
                            foundDeclaration = declaration;
                            return InitDependenciesFindResult.FoundMultiple;
                        }

                        foundMatchingDeclaration = true;
                        bestDeclarationIndex = declarationIndex;
                    }

                    if (!foundMatchingDeclaration)
                    {
                        int score = Fuzz.Ratio("InitDependencies", token.Value.ToString());

                        if (score >= ScoreThreshold)
                        {
                            if (score > bestDeclarationScore)
                            {
                                bestDeclarationScore = score;
                                bestDeclarationIndex = declarationIndex;
                            }
                        }
                    }
                }

                if (token.IsSymbol('('))
                {
                    break;
                }
            }
        }

        if (foundMatchingDeclaration)
        {
            foundDeclaration = uhtClass.Declarations[bestDeclarationIndex];
            return InitDependenciesFindResult.Found;
        }

        if (bestDeclarationScore > 0)
        {
            foundDeclaration = uhtClass.Declarations[bestDeclarationIndex];
            return InitDependenciesFindResult.FoundSimilar;
        }

        // TODO: Do a fulltext search of InitDependencies inside the class. It may be inside UHT ignored preprocessor block

        foundDeclaration = new();
        return InitDependenciesFindResult.NotFound;
    }

    private static string GetProperIncludePath(UhtHeaderFile headerFile)
    {
        if (headerFile.ModuleRelativeFilePath.StartsWith("Public", StringComparison.OrdinalIgnoreCase))
        {
            // output relative path for files in the "Public" folder
            return headerFile.IncludeFilePath;
        }
        else
        {
            // otherwise output full path with module Name
#if UE_5_5_OR_LATER
            return headerFile.Module.Module.Name + "/" + headerFile.ModuleRelativeFilePath;
#else
            return headerFile.Package.Module.Name + "/" + headerFile.ModuleRelativeFilePath;
#endif
        }
    }
}
