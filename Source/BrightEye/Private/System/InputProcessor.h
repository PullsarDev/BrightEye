// Copyright (c) 2024 PullsarDev - GitHub: https://github.com/PullsarDev


#pragma once

#include "Framework/Application/IInputProcessor.h"
#include "Input/Events.h"


/**
 * FBEInputPreProcessor handles custom input processing for key and mouse events in the Slate UI system.
 * It allows capturing and processing specific input events like key presses, mouse clicks, and scrolls.
 */
class FBEInputPreProcessor : public IInputProcessor
{
public:
    FBEInputPreProcessor() {}

    virtual void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) override { }

    virtual bool HandleKeyUpEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override
    {
        const bool KeyReturn = HandleKey(InKeyEvent, false);
        return KeyReturn;
    }

    virtual bool HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override
    {
        const bool KeyReturn = HandleKey(InKeyEvent, true);
        return KeyReturn;
    }

    virtual bool HandleMouseButtonDoubleClickEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override
    {
        return false;
    }

    virtual bool HandleMouseButtonDownEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override
    {
        const FKeyEvent KeyEvent = CreateKeyEventFromPointerEvent(MouseEvent, true);
        const bool KeyReturn = HandleKey(KeyEvent, true);
        return KeyReturn;
    }

    virtual bool HandleMouseButtonUpEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override
    {
        const FKeyEvent KeyEvent = CreateKeyEventFromPointerEvent(MouseEvent, false);
        const bool KeyReturn = HandleKey(KeyEvent, false);
        return KeyReturn;
    }

    virtual bool HandleMouseWheelOrGestureEvent(FSlateApplication& SlateApp, const FPointerEvent& InWheelEvent, const FPointerEvent* InGestureEvent) override
    {
        if (InWheelEvent.GetWheelDelta() != 0)
        {
            const FKey Key = InWheelEvent.GetWheelDelta() < 0 ? EKeys::MouseScrollDown : EKeys::MouseScrollUp;
            const FModifierKeysState ModifierKeysState = InWheelEvent.GetModifierKeys();
            const FKeyEvent KeyEvent(Key, ModifierKeysState, 0, false, 0, 0);  // Create FKeyEvent
            const bool KeyReturn = HandleKey(KeyEvent, true);
            return KeyReturn;
        }
        return false;
    }

    DECLARE_DELEGATE_RetVal_OneParam(bool, FSettingsPressAnyKeyInputPreProcessorKeySelected, const FKeyEvent&);
    FSettingsPressAnyKeyInputPreProcessorKeySelected OnKeySelected;
    FSettingsPressAnyKeyInputPreProcessorKeySelected OnKeReleased;

private:
    bool HandleKey(const FKeyEvent& KeyEvent, const bool bIsPressed) const
    {
        if(bIsPressed)
        {
            return OnKeySelected.Execute(KeyEvent);
        }
        
        return OnKeReleased.Execute(KeyEvent);
    }

    static FKeyEvent CreateKeyEventFromPointerEvent(const FPointerEvent& MouseEvent, bool bIsPressed)
    {
        const FModifierKeysState ModifierKeysState = MouseEvent.GetModifierKeys();
        return FKeyEvent(MouseEvent.GetEffectingButton(), ModifierKeysState, 0, bIsPressed, 0, 0);
    }
};