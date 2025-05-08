using UnrealBuildTool;
using System.IO;

public class EtherealKingdomsUE : ModuleRules
{
    public EtherealKingdomsUE(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        // Setup third-party path to SQLite
        string ThirdPartyPath = Path.GetFullPath(Path.Combine(ModuleDirectory, "../../ThirdParty"));
        string SQLitePath = Path.Combine(ThirdPartyPath, "SQLite");

        PublicIncludePaths.Add(SQLitePath);
        PrivateIncludePaths.Add(SQLitePath);

        PublicAdditionalLibraries.Add(Path.Combine(SQLitePath, "sqlite3.lib"));

        // Optional if DLL is needed at runtime
        RuntimeDependencies.Add("$(BinaryOutputDir)/sqlite3.dll", Path.Combine(SQLitePath, "sqlite3.dll"));

        // Required dependencies for UE modules
        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "UMG"
        });

        // Post-build DLL copy
        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            string DllSource = Path.Combine(SQLitePath, "sqlite3.dll");
            string DllTarget = Path.Combine("$(BinaryOutputDir)", "sqlite3.dll");

            RuntimeDependencies.Add(DllTarget, DllSource); // ensures it's copied
        }
    }
}
