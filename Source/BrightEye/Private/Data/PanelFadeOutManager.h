// Copyright (c) 2024 PullsarDev - GitHub: https://github.com/PullsarDev


#pragma once

#include "CoreMinimal.h"
#include "Widgets/SWidget.h"
#include "Framework/Application/SlateApplication.h"

/**
 * Structure to manage fade-out and fade-in animation for a panel (SWidget).
 * Uses FTSTicker for real-time updates and allows setting the widget, fade-out amount, duration, and start delay dynamically.
 */
struct FPanelFadeOutManager
{
    FPanelFadeOutManager()
        : FadeOutAmount(0), FadeInDuration(0), FadeOutDuration(0), StartDelay(0), CurrentOpacity(1.0f), FadeStartTime(0), DelayStartTime(0),
          TotalElapsedTime(0.0f),
          bIsAnimating(false),
          bIsFadingIn(false),
          bFadeOutComplete(false)
    {
    }

    ~FPanelFadeOutManager()
    {
        if (TickerHandle.IsValid())
        {
            FTSTicker::GetCoreTicker().RemoveTicker(TickerHandle);
            TickerHandle.Reset();
        }
    }

    // Starts the fade-out animation with separate fade-out and fade-in durations
    void StartFadeOut(const TSharedPtr<SWidget>& InWidget, float InFadeOutAmount, float InFadeOutDuration, float InStartDelay)
    {
        if (!InWidget.IsValid()) { return; }

        Widget = InWidget;
        FadeOutAmount = FMath::Clamp(InFadeOutAmount, 0.0f, 1.0f);
        FadeOutDuration = FMath::Max(InFadeOutDuration, 0.01f);
        StartDelay = FMath::Max(InStartDelay, 0.0f);

        bIsAnimating = true;
        bIsFadingIn = false;
        bFadeOutComplete = false;
        TotalElapsedTime = 0.0f;

        if (StartDelay > 0.0f)
        {
            DelayStartTime = FSlateApplication::Get().GetCurrentTime();
            TickerHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateRaw(this, &FPanelFadeOutManager::HandleStartDelay), 0.01f);
        }
        else
        {
            FadeStartTime = FSlateApplication::Get().GetCurrentTime();
            TickerHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateRaw(this, &FPanelFadeOutManager::UpdateFadeOut), 0.01f);
        }
    }

    // Starts the fade-in animation
    void StartFadeIn(const TSharedPtr<SWidget>& InWidget, float InFadeInDuration)
    {
        if (!InWidget.IsValid()) { return; }

        Widget = InWidget;
        FadeInDuration = FMath::Max(InFadeInDuration, 0.01f);

        bIsAnimating = true;
        bIsFadingIn = true;
        TotalElapsedTime = 0.0f;

        FadeStartTime = FSlateApplication::Get().GetCurrentTime();
        TickerHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateRaw(this, &FPanelFadeOutManager::UpdateFadeIn), 0.01f);
    }

    // Stops the current animation and starts fade-in
    void StopAndStartFadeIn(const TSharedPtr<SWidget>& InWidget, float InFadeInDuration)
    {
        if (bIsAnimating)
        {
            // Stop the current animation and reset animating state
            bIsAnimating = false;
            FTSTicker::GetCoreTicker().RemoveTicker(TickerHandle);

            // Directly start fade-in
            StartFadeIn(InWidget, InFadeInDuration);
        }
        else if (bFadeOutComplete)  // If fade-out has completed, start fade-in
        {
            StartFadeIn(InWidget, InFadeInDuration);
        }
    }

private:
    // Handles the start delay before triggering the fade-out
    bool HandleStartDelay(float DeltaTime)
    {
        double CurrentTime = FSlateApplication::Get().GetCurrentTime();
        double ElapsedDelayTime = CurrentTime - DelayStartTime;

        if (ElapsedDelayTime >= StartDelay)
        {
            FadeStartTime = FSlateApplication::Get().GetCurrentTime();
            FTSTicker::GetCoreTicker().RemoveTicker(TickerHandle);

            TickerHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateRaw(this, &FPanelFadeOutManager::UpdateFadeOut), 0.01f);
            return false;
        }
        return true;
    }

    // Fade-out update function
    bool UpdateFadeOut(float DeltaTime)
    {
        TotalElapsedTime += DeltaTime;

        CurrentOpacity = FMath::Lerp(1.0f, FadeOutAmount, TotalElapsedTime / FadeOutDuration);
        SetWidgetOpacity(CurrentOpacity);

        if (TotalElapsedTime >= FadeOutDuration)
        {
            bIsAnimating = false;
            bFadeOutComplete = true;
            FTSTicker::GetCoreTicker().RemoveTicker(TickerHandle);
            return false;
        }

        return true;
    }

    // Fade-in update function
    bool UpdateFadeIn(float DeltaTime)
    {
        TotalElapsedTime += DeltaTime;

        CurrentOpacity = FMath::Lerp(FadeOutAmount, 1.0f, TotalElapsedTime / FadeInDuration);
        SetWidgetOpacity(CurrentOpacity);

        if (TotalElapsedTime >= FadeInDuration)
        {
            bIsAnimating = false;
            FTSTicker::GetCoreTicker().RemoveTicker(TickerHandle);
            return false;
        }

        return true;
    }

    // Function to apply the calculated opacity to the widget
    void SetWidgetOpacity(float InOpacity) const
    {
        if (Widget.IsValid())
        {
            Widget->SetRenderOpacity(InOpacity);
        }
    }

    TSharedPtr<SWidget> Widget;
    float FadeOutAmount;
    float FadeInDuration;
    float FadeOutDuration;
    float StartDelay;
    float CurrentOpacity;
    double FadeStartTime;
    double DelayStartTime;
    float TotalElapsedTime;
    bool bIsAnimating;
    bool bIsFadingIn;
    bool bFadeOutComplete; 
    FTSTicker::FDelegateHandle TickerHandle;
};
