// Copyright (c) 2024 PullsarDev - GitHub: https://github.com/PullsarDev


#pragma once

#include "Styling/SlateStyle.h"

#if (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 1)
using EditorStyle = FAppStyle;
#else
using EditorStyle = FEditorStyle;
#endif

/**
 * FBrightEyeStyle manages the visual styling of the BrightEye tool using Slate UI framework.
 * It initializes, applies, and cleans up the tool's custom styles, providing access to the defined Slate style set.
 */
class FBrightEyeStyle
{
public:
	static void InitializeToolStyle();
	static void ShutDownStyle();

private:
	static FName ToolStyleName;

	static TSharedRef<FSlateStyleSet> CreateToolSlateStyleSet();
	static TSharedPtr<FSlateStyleSet> CreatedToolSlateStyleSet;

public:
	static FName GetToolStyleName(){return ToolStyleName;}

	static TSharedRef<FSlateStyleSet> GetCreatedToolSlateStyleSet() {return CreatedToolSlateStyleSet.ToSharedRef();}
};