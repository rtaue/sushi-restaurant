// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SushiRestaurant : ModuleRules
{
	public SushiRestaurant(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"SushiRestaurant/Public",
			"SushiRestaurant/Variant_Platforming",
			"SushiRestaurant/Variant_Combat",
			"SushiRestaurant/Variant_Combat/AI",
			"SushiRestaurant/Variant_SideScrolling",
			"SushiRestaurant/Variant_SideScrolling/Gameplay",
			"SushiRestaurant/Variant_SideScrolling/AI"
		});

        PrivateIncludePaths.AddRange(new string[] {
			"SushiRestaurant/Public"
		});

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
