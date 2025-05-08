  // EtherealKingdomsServer.Target.cs
using UnrealBuildTool;
using System.Collections.Generic;

[SupportedPlatforms(UnrealPlatformClass.Server)]
public class EtherealKingdomsServerTarget : TargetRules
{
    public EtherealKingdomsServerTarget(TargetInfo Target) : base(Target)
    {
		Type = TargetType.Server;
		DefaultBuildSettings = BuildSettingsVersion.V5;

		ExtraModuleNames.AddRange( new string[] { "EtherealKingdomsUE" } );
	}
}