  // EtherealKingdomsClient.Target.cs
using UnrealBuildTool;
using System.Collections.Generic;

public class EtherealKingdomsClientTarget : TargetRules
{
    public EtherealKingdomsClientTarget(TargetInfo Target) : base(Target)
    {
		Type = TargetType.Client;
		DefaultBuildSettings = BuildSettingsVersion.V5;

		ExtraModuleNames.AddRange( new string[] { "EtherealKingdomsUE" } );
	}
}