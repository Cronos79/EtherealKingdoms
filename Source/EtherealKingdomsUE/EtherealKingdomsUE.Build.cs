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

        PublicAdditionalLibraries.Add(Path.Combine(SQLitePath, "sqlite3.c"));

        // Required dependencies for UE modules
        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "UMG"
        });
    }
}
