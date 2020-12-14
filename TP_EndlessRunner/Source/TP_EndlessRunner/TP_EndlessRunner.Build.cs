// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TP_EndlessRunner : ModuleRules
{
	public TP_EndlessRunner(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "UMG" });

		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore", "OnlineSubsystem" });

		// Uncomment if you are using Wwise audio
		// PublicDependencyModuleNames.Add("AkAudio");

		if (Target.Platform == UnrealTargetPlatform.Android)
		{
			DynamicallyLoadedModuleNames.Add("OnlineSubsystemGooglePlay");
		}

		if (Target.Platform == UnrealTargetPlatform.IOS)
		{
			DynamicallyLoadedModuleNames.Add("OnlineSubsystemIOS");
		}
	}
}
