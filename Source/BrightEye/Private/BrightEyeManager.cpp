// Copyright (c) 2024 PullsarDev - GitHub: https://github.com/PullsarDev


#include "BrightEyeManager.h"
#include "System/Commands.h"
#include "System/InputProcessor.h"
#include "LevelEditor.h"
#include "SLevelViewport.h"
#include "Components/SpotLightComponent.h"
#include "UnrealEdMisc.h"
#include "Data/BrightEyeSettings.h"
#include "Subsystems/EditorActorSubsystem.h"
#include "Data/ColorPicker.h"
#include "Helpers/GeometryUtils.h"
#include "UI/BrightEyePanel.h"


TSharedPtr<FBrightEyeManagerImp> FBrightEyeManager::BrightEyeManagerImp;

static constexpr float MAX_DELAY_SPEED = 40.0f;
static constexpr float MIN_DELAY_SPEED = 4.0f;

static constexpr float DROP_CHECK_TIME = 0.1f;

static constexpr float ConfigSaveInterval = 1.0f;

void FBrightEyeManagerImp::Initialize()
{
	ActivateInputProcessor();

	InitCommands();

	SetupDelegates();
}

void FBrightEyeManagerImp::Shutdown()
{
	DeactivateInputProcessor();

	RemoveDelegates();
}

void FBrightEyeManagerImp::OnScalarParamChanged(const float& InNewParam, EBEScalarParamType InParamType)
{
	UBESettings* ToolSettings = UBESettings::GetInstance();

	if (!ActiveViewport.IsValid() || !IsValid(ToolSettings)) { return; }

	if (InParamType == EBEScalarParamType::Brightness)
	{
		ToolSettings->Brightness = InNewParam;
		UpdateBrightness();
	}
	else if (InParamType == EBEScalarParamType::Radius)
	{
		ToolSettings->Radius = InNewParam;
		UpdateRadius();
	}
	else if (InParamType == EBEScalarParamType::Distance)
	{
		ToolSettings->Distance = InNewParam;
		UpdateDistance();
		UpdateBrightness();
	}

	ForceViewportRedraw();

	ResetLightModificationState();
}

void FBrightEyeManagerImp::OnColorParamChanged(const FLinearColor& InNewColor)
{
	UBESettings* ToolSettings = UBESettings::GetInstance();

	if (!ActiveViewport.IsValid() || !IsValid(ToolSettings)) { return; }

	ToolSettings->Color = InNewColor;

	UpdateColor();

	ForceViewportRedraw();

	ResetLightModificationState();
}

void FBrightEyeManagerImp::OnSmoothRotationToggled()
{
	UBESettings::GetInstance()->bSmoothLightRotation = !UBESettings::GetInstance()->bSmoothLightRotation;

	ResetBrightEyeRotation();

	ForceViewportRedraw();

	ResetLightModificationState();
}

void FBrightEyeManagerImp::OnBrightEyeSettingsChangedOnEditorSettings(const FPropertyChangedEvent& InPropertyChangedEvent)
{
	if (InPropertyChangedEvent.Property)
	{
		static const FName BrightnessName("Brightness");
		static const FName RadiusName("Radius");
		static const FName DistanceName("Distance");
		static const FName MaxBrightnessName("MaxBrightness");
		static const FName MaxDistanceName("MaxDistance");
		static const FName ColorName("Color");
		static const FName ActivateLightOnPressName("bActivateLightOnPress");
		static const FName bHidePanelWhenIdleName("bHidePanelWhenIdle");
		static const FName bSmoothCameraRotationName("bSmoothLightRotation");
		static const FName LightProfileName("LightProfile");

		const UBESettings* ToolSettings = UBESettings::GetInstance();
		if (!IsValid(ToolSettings)) { return; }

		if (InPropertyChangedEvent.GetPropertyName() == BrightnessName)
		{
			UpdateBrightness();

			if (BrightEyePanel.IsValid())
			{
				BrightEyePanel->RefreshBrightness();
			}
		}
		else if (InPropertyChangedEvent.GetPropertyName() == RadiusName)
		{
			UpdateRadius();

			if (BrightEyePanel.IsValid())
			{
				BrightEyePanel->RefreshRadius();
			}
		}
		else if (InPropertyChangedEvent.GetPropertyName() == DistanceName)
		{
			UpdateDistance();
			UpdateBrightness();

			if (BrightEyePanel.IsValid())
			{
				BrightEyePanel->RefreshDistance();
			}
		}
		else if (InPropertyChangedEvent.GetPropertyName() == MaxBrightnessName)
		{
			UpdateBrightness();
		}
		else if (InPropertyChangedEvent.GetPropertyName() == MaxDistanceName)
		{
			UpdateDistance();
			UpdateBrightness();
		}
		else if (InPropertyChangedEvent.GetPropertyName() == ColorName)
		{
			UpdateColor();
			if (BrightEyePanel.IsValid())
			{
				BrightEyePanel->RefreshColor();
			}
		}
		else if (InPropertyChangedEvent.GetPropertyName() == ActivateLightOnPressName)
		{
			if (ToolSettings->bActivateLightOnPress && IsValid(BrightEyeActor) && IsValid(BrightEyeComponent) &&
				BrightEyeComponent->IsVisible())
			{
				BrightEyeComponent->SetVisibility(false);
			}
		}
		else if (InPropertyChangedEvent.GetPropertyName() == bHidePanelWhenIdleName)
		{
			if (BrightEyePanel.IsValid())
			{
				BrightEyePanel->ChangeHidePanelWhenIdle(ToolSettings->bHidePanelWhenIdle);
			}
		}
		else if (InPropertyChangedEvent.GetPropertyName() == bSmoothCameraRotationName)
		{
			ResetBrightEyeRotation();

			if (BrightEyePanel.IsValid())
			{
				BrightEyePanel->RefreshCameraRotationState();
			}
		}
		else if (InPropertyChangedEvent.GetPropertyName() == LightProfileName)
		{
			UpdateLightProfile();
		}
		else
		{
			UpdateLightViewOffsetOnPanel();
		}

		ResetLightModificationState();
	}
}

void FBrightEyeManagerImp::OnResetBrightEyeSettings() const
{
	if (IsValid(BrightEyeActor) && IsValid(BrightEyeComponent))

		UpdateBrightEyeSpecs();
}

void FBrightEyeManagerImp::UpdateBrightEyeSpecs() const
{
	UpdateBrightness();
	UpdateRadius();
	UpdateDistance();
	UpdateColor();
	UpdateLightProfile();
}

void FBrightEyeManagerImp::UpdateBrightness() const
{
	if(!IsValid(BrightEyeComponent)){return;}
	
	if (const UBESettings* ToolSettings = UBESettings::GetInstance())
	{
		float t = ToolSettings->Distance;
		float b = ToolSettings->Brightness * ToolSettings->MaxBrightness * 0.01f;
		float c = ToolSettings->Brightness * ToolSettings->MaxBrightness * 0.99f;
		float d = 1.0f;

		auto EaseInQuart = [](float t, float b, float c, float d) -> float
		{
			t /= d;
			return c * t * t * t * t + b;
		};

		float CalculatedBrightness = EaseInQuart(t, b, c, d);
		float NewBrightness = CalculatedBrightness;
		BrightEyeComponent->SetIntensity(NewBrightness);
	}
}

void FBrightEyeManagerImp::UpdateRadius() const
{
	if(!IsValid(BrightEyeComponent)){return;}
	
	if (const UBESettings* ToolSettings = UBESettings::GetInstance())
	{
		float NewRadius = 1 + ToolSettings->Radius * 79;
		BrightEyeComponent->SetOuterConeAngle(NewRadius);
	}
}

void FBrightEyeManagerImp::UpdateDistance() const
{
	if(!IsValid(BrightEyeComponent)){return;}
	
	if (const UBESettings* ToolSettings = UBESettings::GetInstance())
	{
		float NewDistance = 3000 + ToolSettings->Distance * (ToolSettings->MaxDistance - 3000);
		BrightEyeComponent->SetAttenuationRadius(NewDistance);
	}
}

void FBrightEyeManagerImp::UpdateColor() const
{
	if(!IsValid(BrightEyeComponent)){return;}
	
	if (const UBESettings* ToolSettings = UBESettings::GetInstance())
	{
		BrightEyeComponent->SetLightColor(ToolSettings->Color);
	}
}

void FBrightEyeManagerImp::UpdateLightProfile() const
{
	if(!IsValid(BrightEyeComponent)){return;}
	
	if (const UBESettings* ToolSettings = UBESettings::GetInstance())
	{
		BrightEyeComponent->SetIESTexture(nullptr);
				
		if (ToolSettings->LightProfile.ToSoftObjectPath().IsValid())
		{
			BrightEyeComponent->SetIESTexture(ToolSettings->LightProfile.LoadSynchronous());
		}
	}
}

void FBrightEyeManagerImp::UpdateLightViewOffsetOnPanel() const
{
	if(!BrightEyePanel.IsValid()){return;}

	BrightEyePanel->UpdateViewCoords();
}


void FBrightEyeManagerImp::ResetLightModificationState()
{
	TimeSinceLastModification = 0.0f;
	bLightSettingsModified = true;
}

void FBrightEyeManagerImp::ForceViewportRedraw() const
{
	TSharedPtr<SLevelViewport> ViewportPtr = ActiveViewport.Pin();
	if (ViewportPtr.IsValid())
	{
		ViewportPtr->GetLevelViewportClient().Invalidate();
	}
}

void FBrightEyeManager::Initialize()
{
	FBECommands::Register();

	if (!BrightEyeManagerImp.IsValid())
	{
		BrightEyeManagerImp = MakeShareable(new FBrightEyeManagerImp);

		if (BrightEyeManagerImp.IsValid())
		{
			BrightEyeManagerImp->Initialize();
		}
	}
}

void FBrightEyeManager::Shutdown()
{
	FBECommands::Unregister();

	if (BrightEyeManagerImp.IsValid())
	{
		BrightEyeManagerImp->Shutdown();
		BrightEyeManagerImp.Reset();
	}
}

void FBrightEyeManagerImp::SetupDelegates()
{
	FLevelEditorModule& LevelEditor = FModuleManager::GetModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));

	LevelEditor.OnLevelEditorCreated().AddLambda([this](const TSharedPtr<ILevelEditor>& InLevelEditor)
	{
		if (InLevelEditor.IsValid())
		{
			InLevelEditor->OnActiveViewportChanged().AddRaw(this, &FBrightEyeManagerImp::OnActiveViewportChanged);

			OnActiveViewportChanged(nullptr, InLevelEditor->GetActiveViewportInterface());
		}
	});

	LevelEditor.OnMapChanged().AddRaw(this, &FBrightEyeManagerImp::OnMapChanged);
	LevelEditor.OnActorSelectionChanged().AddRaw(this, &FBrightEyeManagerImp::OnActorSelectionChanged);

	FEditorDelegates::BeginPIE.AddRaw(this, &FBrightEyeManagerImp::HandleBeginPIE);
	
	TickHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateRaw(this, &FBrightEyeManagerImp::OnTick));

	if (IsValid(UBESettings::GetInstance()))
	{
		UBESettings::GetInstance()->OnBrightLightSettingsChanged.BindRaw(this, &FBrightEyeManagerImp::OnBrightEyeSettingsChangedOnEditorSettings);
		UBESettings::GetInstance()->OnResetBrightEyeSettings.BindRaw(this, &FBrightEyeManagerImp::OnResetBrightEyeSettings);
	}
}

void FBrightEyeManagerImp::RemoveDelegates() const
{
	FEditorDelegates::BeginPIE.RemoveAll(this);
	
	FLevelEditorModule& LevelEditor = FModuleManager::GetModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));
	LevelEditor.OnMapChanged().RemoveAll(this);
	LevelEditor.OnActorSelectionChanged().RemoveAll(this);

	if (TickHandle.IsValid())
	{
		FTSTicker::GetCoreTicker().RemoveTicker(TickHandle);
	}

	if (LevelEditor.GetFirstLevelEditor().IsValid())
	{
		LevelEditor.GetFirstLevelEditor()->OnActiveViewportChanged().RemoveAll(this);
	}

	if (IsValid(UBESettings::GetInstance()))
	{
		UBESettings::GetInstance()->OnBrightLightSettingsChanged.Unbind();
	}
}

void FBrightEyeManagerImp::HandleBeginPIE(const bool bIsSimulating)
{	
	TryHideBrightEyePanel();
}


bool FBrightEyeManagerImp::OnTick(float InDeltaTime)
{
	if (ActiveViewport.IsValid() && IsValid(BrightEyeActor) && IsValid(BrightEyeComponent))
	{
		UpdateLightTransformWithViewport(InDeltaTime);
	}

	if (bLightSettingsModified)
	{
		TimeSinceLastModification += InDeltaTime;

		if (TimeSinceLastModification >= ConfigSaveInterval)
		{
			UBESettings::GetInstance()->SaveToolConfig();

			bLightSettingsModified = false;
		}
	}

	if(bCheckingForDrop)
	{
		CheckForOutOfBoundDropping(InDeltaTime);
	}
	
	return true;
}

void FBrightEyeManagerImp::OnActiveViewportChanged(TSharedPtr<IAssetViewport> OldViewport, TSharedPtr<IAssetViewport> NewViewport)
{
	if (bIsPanelOnWindow)
	{
		TSharedPtr<SLevelViewport> NewActiveViewport = StaticCastSharedPtr<SLevelViewport>(NewViewport);;

		TryHideBrightEyePanel();

		ActiveViewport = NewActiveViewport;

		ResetPanelLocation();

		TryRevealBrightEyePanel();
	}
	else
	{
		ActiveViewport = StaticCastSharedPtr<SLevelViewport>(NewViewport);
	}
}

void FBrightEyeManagerImp::OnMapChanged(UWorld* World, EMapChangeType MapChangeType)
{
	if (World && MapChangeType != EMapChangeType::SaveMap)
	{
		DestroyBrightLight();
		DestroyBrightEyePanel();
	}
}

void FBrightEyeManagerImp::OnActorSelectionChanged(const TArray<UObject*>& InActors, bool bIsSelected) const
{
	if (!BrightEyeActor || !GEditor) { return; }

	for (UObject* CurrentSelectedActor : InActors)
	{
		if (CurrentSelectedActor == BrightEyeActor)
		{
			if (UEditorActorSubsystem* EditorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>())
			{
				return EditorActorSubsystem->SetActorSelectionState(BrightEyeActor, false);
			}
		}
	}
}

void FBrightEyeManagerImp::ResetBrightEyeRotation()
{
	TSharedPtr<FLevelEditorViewportClient> ViewportClient = StaticCastSharedPtr<FLevelEditorViewportClient>(
		ActiveViewport.Pin()->GetViewportClient());
	if (ViewportClient.IsValid())
	{
		BrightEyeRotation = ViewportClient->GetViewRotation();
	}
}

#pragma region Input

void FBrightEyeManagerImp::ActivateInputProcessor()
{
	if (FSlateApplication::IsInitialized())
	{
		InputProcessor = MakeShared<FBEInputPreProcessor>();
		InputProcessor->OnKeySelected.BindRaw(this, &FBrightEyeManagerImp::HandleKeySelected);
		InputProcessor->OnKeReleased.BindRaw(this, &FBrightEyeManagerImp::HandleKeyReleased);

		FSlateApplication::Get().RegisterInputPreProcessor(InputProcessor, 0);
	}
}

void FBrightEyeManagerImp::DeactivateInputProcessor()
{
	if (FSlateApplication::IsInitialized() && InputProcessor.IsValid())
	{
		FSlateApplication::Get().UnregisterInputPreProcessor(InputProcessor);
		InputProcessor.Reset();
	}
}

bool FBrightEyeManagerImp::HandleKeySelected(const FKeyEvent& InKey)
{
	if(IsInPie()){return false;}
	
	bIsAnyControlKeyPressed = true;
	
	if (ActiveViewport.IsValid() && ActiveViewport.Pin()->GetActiveViewport()->HasFocus())
	{
		return CameraLevelCommands->ProcessCommandBindings(InKey);
	}
	return false;
}


bool IsAnyMouseButtonDown(const FKeyEvent& InKey)
{
	if(InKey.GetKey() == EKeys::LeftMouseButton || InKey.GetKey() == EKeys::RightMouseButton || InKey.GetKey() == EKeys::MiddleMouseButton)
	{
		return true;
	}
	return false;
}

bool FBrightEyeManagerImp::HandleKeyReleased(const FKeyEvent& InKey)
{
	if(IsInPie()){return false;}
	
	bIsAnyControlKeyPressed = false;

	if(InKey.GetKey() == EKeys::LeftMouseButton)
	{
		bCheckingForDrop = true;
	}
	
	if(bIsAimLighting)
	{
		bIsAimLighting = false;

		if(!bIsLightActiveBeforeAiming)
		{
			if(IsValid(BrightEyeComponent))
			{
				BrightEyeComponent->SetVisibility(false);
			}
			
			bIsLightActiveBeforeAiming = false;
				
			TSharedPtr<SLevelViewport> ViewportPtr = ActiveViewport.Pin();
			if (ViewportPtr.IsValid() && !ViewportPtr->IsRealtime())
			{
				ViewportPtr->GetLevelViewportClient().Invalidate();
			}
		}
	}
	if (CameraLevelCommands.IsValid() && ActiveViewport.IsValid() && ActiveViewport.Pin()->GetActiveViewport()->HasFocus())
	{
		return CameraLevelCommands->ProcessCommandBindings(InKey);
	}
	return false;
}


void FBrightEyeManagerImp::InitCommands()
{
	CameraLevelCommands = MakeShareable(new FUICommandList());
	CameraLevelCommands->MapAction(FBECommands::Get().ToggleBrightEye,FExecuteAction::CreateRaw(this, &FBrightEyeManagerImp::OnToggleLight));
	CameraLevelCommands->MapAction(FBECommands::Get().ToggleControlPanel,FExecuteAction::CreateRaw(this, &FBrightEyeManagerImp::OnToggleBrightEyePanel));
	CameraLevelCommands->MapAction(FBECommands::Get().AimBrightEye,FExecuteAction::CreateRaw(this, &FBrightEyeManagerImp::OnAimBrightEye));
}

#pragma endregion Input


void FBrightEyeManagerImp::OnToggleLight()
{
	UBESettings* ToolSettings = UBESettings::GetInstance();
	if (!IsValid(ToolSettings)) { return; }
	
	if (ActiveViewport.IsValid() && ActiveViewport.Pin().Get()->GetActiveViewport()->HasFocus())
	{
		if (!IsValid(BrightEyeActor))
		{
			CreateBrightEyeLight();
		}
		if (bIsAnyControlKeyPressed)
		{
			if (ToolSettings->bActivateLightOnPress)
			{
				if(IsValid(BrightEyeComponent)){BrightEyeComponent->SetVisibility(true);}
			}
		}
		else
		{
			if (ToolSettings->bActivateLightOnPress)
			{
				if(IsValid(BrightEyeComponent)){BrightEyeComponent->SetVisibility(false);}
			}
			else
			{
				if(IsValid(BrightEyeComponent)){BrightEyeComponent->SetVisibility(!BrightEyeComponent->IsVisible());}
			}
		}

		InvalidateViewport();
	}
}

void FBrightEyeManagerImp::OnToggleBrightEyePanel()
{
	UBESettings* ToolSettings = UBESettings::GetInstance();
	if (!IsValid(ToolSettings)) { return; }

	if (!bIsAnyControlKeyPressed && ActiveViewport.IsValid() && ActiveViewport.Pin().Get()->GetActiveViewport()->HasFocus())
	{
		if (!bIsPanelOnWindow)
		{
			if (!BrightEyePanelParent.IsValid())
			{
				CreateBrightEyePanel();
			}
			TryRevealBrightEyePanel();
		}
		else
		{
			TryHideBrightEyePanel();
		}
	}
}

void FBrightEyeManagerImp::InvalidateViewport() const
{
	TSharedPtr<SLevelViewport> ViewportPtr = ActiveViewport.Pin();
	if (ViewportPtr.IsValid() && !ViewportPtr->IsRealtime())
	{
		ViewportPtr->GetLevelViewportClient().Invalidate();
	}
}

void FBrightEyeManagerImp::SetLightVisibility(bool bVisible) const
{
	if (IsValid(BrightEyeComponent))
	{
		BrightEyeComponent->SetVisibility(bVisible);
		InvalidateViewport();
	}
}

void FBrightEyeManagerImp::OnAimBrightEye()
{
	bIsAimLighting = bIsAnyControlKeyPressed;

	TSharedPtr<SLevelViewport> ViewportPtr = ActiveViewport.Pin();
	if (!ViewportPtr.IsValid() || !ViewportPtr->GetActiveViewport()->HasFocus()) { return; }

	if (bIsAimLighting)
	{
		bIsLightActiveBeforeAiming = IsValid(BrightEyeComponent) && BrightEyeComponent->IsVisible();

		if (!bIsLightActiveBeforeAiming)
		{
			if (!IsValid(BrightEyeActor))
			{
				CreateBrightEyeLight();
			}

			SetLightVisibility(true);
		}
	}
	else
	{
		if (!bIsLightActiveBeforeAiming)
		{
			SetLightVisibility(false);
			bIsLightActiveBeforeAiming = false;
		}
	}
}


void FBrightEyeManagerImp::OnPanelDragStarted()
{
	bIsDragging = true;
	TryHideBrightEyePanel();
}

void FBrightEyeManagerImp::CheckForOutOfBoundDropping(const float& InDeltaTime)
{
	PanelDropTimer  += InDeltaTime;
	if(PanelDropTimer  >= DROP_CHECK_TIME)
	{
		bCheckingForDrop = false;
		if(bIsDragging)
		{
			OnPanelDragFinished(BEPanelLocation);
			bIsDragging = false;
		}
	}
}

void FBrightEyeManagerImp::OnCoordsChanged(const FVector2D& InNewCoords)
{
	UBESettings* ToolSettings = UBESettings::GetInstance();

	if (!ActiveViewport.IsValid() || !IsValid(ToolSettings)) { return; }

	ToolSettings->LightViewOffset = InNewCoords * 100.0f;
	
	ForceViewportRedraw();

	ResetLightModificationState();
}

void FBrightEyeManagerImp::OnPanelDragFinished(const FVector2D& InDropLocation)
{
	bIsDragging = false;
	UpdateBrightEyePanelLocation(InDropLocation);
	TryRevealBrightEyePanel();
}

void FBrightEyeManagerImp::CreateBrightEyeLight()
{
	UWorld* EditorWorld = GEditor->GetEditorWorldContext().World();
	if (!EditorWorld) return;

	BrightEyeActor = EditorWorld->SpawnActor<AActor>(AActor::StaticClass());
	if (IsValid(BrightEyeActor))
	{
		BrightEyeActor->SetActorLabel(TEXT("Bright Eye Manager"));

		BrightEyeActor->SetFlags(RF_Transient);

		BrightEyeComponent = NewObject<USpotLightComponent>(BrightEyeActor);
		if (!IsValid(BrightEyeComponent)) { return; }

		BrightEyeComponent->RegisterComponent();
		BrightEyeComponent->AttachToComponent(BrightEyeActor->GetRootComponent(),FAttachmentTransformRules::KeepWorldTransform);

		const UBESettings* ToolSettings = UBESettings::GetInstance();
		if (!IsValid(ToolSettings)) { return; }

		BrightEyeComponent->SetInnerConeAngle(0);
		BrightEyeComponent->SetCastShadows(false);

		ResetBrightEyeRotation();

		UpdateBrightEyeSpecs();

		BrightEyeComponent->SetVisibility(false);
		TArray<USceneComponent*> AttachedComponents;
		BrightEyeComponent->GetChildrenComponents(true, AttachedComponents);

		for (USceneComponent* Component : AttachedComponents)
		{
			if (UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(Component))
			{
				PrimitiveComponent->SetVisibility(false);
			}
		}
	}
}

void FBrightEyeManagerImp::DestroyBrightLight()
{
	if (!IsValid(BrightEyeActor)) { return; }

	BrightEyeActor->Destroy();
	BrightEyeActor = nullptr;
}



void FBrightEyeManagerImp::UpdateLightTransformWithViewport(const float& InDeltaTime)
{
	TSharedPtr<FLevelEditorViewportClient> ViewportClient = StaticCastSharedPtr<FLevelEditorViewportClient>(ActiveViewport.Pin()->GetViewportClient());

	if (ViewportClient.IsValid() && IsValid(BrightEyeComponent) && UBESettings::GetInstance())
	{
		FVector LightLocation = ViewportClient->GetViewLocation() + ViewportClient->GetViewRotation().Quaternion().GetUpVector() * UBESettings::GetInstance()->LightViewOffset.Y + ViewportClient->GetViewRotation().Quaternion().GetRightVector() * UBESettings::GetInstance()->LightViewOffset.X + ViewportClient->GetViewRotation().Vector() * -20.0f;
		if(bIsAimLighting)
		{
			FVector MouseHitLocation;
			FGeometryUtils::GetHitLocationFromCameraAndMouse(MouseHitLocation);

			FRotator LookRotation = (MouseHitLocation - LightLocation).Rotation();
			
			float DelaySpeed = FMath::Lerp(MIN_DELAY_SPEED, MAX_DELAY_SPEED,UBESettings::GetInstance()->RotationDelayFactor);
			BrightEyeRotation = FMath::RInterpTo(BrightEyeRotation, LookRotation, InDeltaTime, DelaySpeed);
			
			BrightEyeComponent->SetWorldLocation(LightLocation);
			BrightEyeComponent->SetWorldRotation(BrightEyeRotation);

			InvalidateViewport();
		}
		else
		{
			FRotator LightRotation = ViewportClient->GetViewRotation();
			if(!UBESettings::GetInstance()->LightViewOffset.IsZero())
			{
				LightRotation = FGeometryUtils::AdjustLightRotationFromTrace(ViewportClient->GetViewLocation(),ViewportClient->GetViewRotation(), LightLocation);
			}
			
			
			if (!UBESettings::GetInstance()->bSmoothLightRotation)
			{
				BrightEyeComponent->SetWorldLocation(LightLocation);

				BrightEyeComponent->SetWorldRotation(LightRotation);
			}
			else
			{
				float DelaySpeed = FMath::Lerp(MAX_DELAY_SPEED, MIN_DELAY_SPEED,UBESettings::GetInstance()->RotationDelayFactor);
				BrightEyeRotation = FMath::RInterpTo(BrightEyeRotation, LightRotation, InDeltaTime, DelaySpeed);
			
				BrightEyeComponent->SetWorldLocation(LightLocation);
				BrightEyeComponent->SetWorldRotation(BrightEyeRotation);

				InvalidateViewport();
			}
		}
	}
}

void FBrightEyeManagerImp::InitializePanel()
{
	RefreshColorPicker();

	InitializePanelParams();
}

void FBrightEyeManagerImp::ResetPanelLocation()
{
	if (BEPanelLocation.IsZero() && ActiveViewport.Pin().IsValid())
	{
		const FVector2D ActiveViewportSize = ActiveViewport.Pin()->GetActiveViewport()->GetSizeXY();
		BEPanelLocation.X = ActiveViewportSize.X / 2.0f;
		BEPanelLocation.Y = ActiveViewportSize.Y / 2.0f;
	}

	UpdateBrightEyePanelLocation(BEPanelLocation);
}

void FBrightEyeManagerImp::RefreshColorPicker()
{
	if (BrightEyePanel.IsValid())
	{
		if (const UBESettings* ToolSettings = UBESettings::GetInstance())
		{
			if (IsValid(BrightEyePanel->GetColorPicker()))
			{
				if (!BrightEyePanel->GetColorPicker()->GetOnParamChangedDelegate().IsBound())
				{
					BrightEyePanel->GetColorPicker()->GetOnParamChangedDelegate().BindRaw(this, &FBrightEyeManagerImp::OnColorParamChanged);
				}

				BrightEyePanel->GetColorPicker()->InitializeColor(ToolSettings->Color);
			}
		}
	}
}

void FBrightEyeManagerImp::InitializePanelParams() const
{
	if (BrightEyePanel.IsValid())
	{
		BrightEyePanel->RefreshPanel();
	}
}

void FBrightEyeManagerImp::CreateBrightEyePanel()
{
	if (!IsValid(BrightEyeActor))
	{
		CreateBrightEyeLight();
	}

	if (ActiveViewport.Pin().IsValid())
	{
		ResetPanelLocation();

		SAssignNew(BrightEyePanelParent, SHorizontalBox)
		+ SHorizontalBox::Slot()
		.FillWidth(1.0f)
		.VAlign(VAlign_Top)
		.HAlign(HAlign_Left)
		.Padding(TAttribute<FMargin>(this, &FBrightEyeManagerImp::GetBrightEyePanelPadding))
		[
			SAssignNew(BrightEyePanel, SBrightEyePanel)
			.Owner(BrightEyeActor)
			.OnPanelDragStarted_Raw(this, &FBrightEyeManagerImp::OnPanelDragStarted)
			.OnPanelDragFinished_Raw(this, &FBrightEyeManagerImp::OnPanelDragFinished)
			.OnBrightnessChanged_Raw(this, &FBrightEyeManagerImp::OnScalarParamChanged, EBEScalarParamType::Brightness)
			.OnRadiusChanged_Raw(this, &FBrightEyeManagerImp::OnScalarParamChanged, EBEScalarParamType::Radius)
			.OnDistanceChanged_Raw(this, &FBrightEyeManagerImp::OnScalarParamChanged, EBEScalarParamType::Distance)
			.OnSmoothRotationStateChanged_Raw(this, &FBrightEyeManagerImp::OnSmoothRotationToggled)
			.OnCoordsChanged_Raw(this,&FBrightEyeManagerImp::OnCoordsChanged)
			.Cursor(EMouseCursor::Type::Default)
		];

		InitializePanel();
	}
}

void FBrightEyeManagerImp::UpdateBrightEyePanelLocation(const FVector2D InLocation)
{
	if (!ActiveViewport.IsValid()) { return; }

	const FVector2D ActiveViewportSize = ActiveViewport.Pin()->GetActiveViewport()->GetSizeXY();
	FVector2D ScreenPos = InLocation;

	constexpr float EdgeFactor = 0.97f;
	const float MinX = ActiveViewportSize.X * (1 - EdgeFactor);
	const float MinY = ActiveViewportSize.Y * (1 - EdgeFactor);
	const float MaxX = ActiveViewportSize.X * EdgeFactor;
	const float MaxY = ActiveViewportSize.Y * EdgeFactor;
	if (ScreenPos.X < MinX || ScreenPos.X > MaxX || ScreenPos.Y < MinY || ScreenPos.Y > MaxY)
	{
		ScreenPos.X = ActiveViewportSize.X / 2.0f;
		ScreenPos.Y = ActiveViewportSize.Y / 2.0f;
	}
	BEPanelLocation = ScreenPos;
}

FMargin FBrightEyeManagerImp::GetBrightEyePanelPadding() const
{
	return FMargin(BEPanelLocation.X, BEPanelLocation.Y, 0, 0);
}

void FBrightEyeManagerImp::DestroyBrightEyePanel()
{
	if (!BrightEyePanelParent.IsValid() || !ActiveViewport.Pin().IsValid()) { return; }

	if (ActiveViewport.Pin().IsValid())
	{
		ActiveViewport.Pin()->RemoveOverlayWidget(BrightEyePanelParent.ToSharedRef());
		bIsPanelOnWindow = false;
		BrightEyePanelParent.Reset();
		BrightEyePanel.Reset();
	}
}

void FBrightEyeManagerImp::TryRevealBrightEyePanel()
{
	if (!BrightEyePanelParent.IsValid() || !ActiveViewport.Pin().IsValid()) { return; }
	ActiveViewport.Pin().Get()->AddOverlayWidget(BrightEyePanelParent.ToSharedRef());
	bIsPanelOnWindow = true;
}

void FBrightEyeManagerImp::TryHideBrightEyePanel()
{
	if (!BrightEyePanelParent.IsValid() || !ActiveViewport.Pin().IsValid()) { return; }
	ActiveViewport.Pin().Get()->RemoveOverlayWidget(BrightEyePanelParent.ToSharedRef());
	bIsPanelOnWindow = false;
}

bool FBrightEyeManagerImp::IsInPie()
{
	if (GEditor == nullptr){return true;}

	if(GEditor->GetPIEWorldContext())
	{
		return true;
	}
	return false;
}


