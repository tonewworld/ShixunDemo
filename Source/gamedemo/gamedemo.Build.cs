// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class gamedemo : ModuleRules
{
    public gamedemo(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "HeadMountedDisplay",
            "NavigationSystem",
            "AIModule",
            "UMG",          // 添加 UMG 支持（UUserWidget）
            "Slate",        // UMG 依赖的基础 UI 模块
            "SlateCore"     // Slate 核心
        });
    }
}