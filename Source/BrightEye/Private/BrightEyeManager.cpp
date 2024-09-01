// Copyright (c) 2024 PullsarDev - GitHub: https://github.com/PullsarDev


#include "BrightEyeManager.h"
#include "System/Commands.h"
#include "UI/ControlPanel.h"
#include "System/InputProcessor.h"
#include "LevelEditor.h"
#include "SLevelViewport.h"
#include "Data/ToolAssetData.h"
#include "Components/SpotLightComponent.h"
#include "UnrealEdMisc.h"
#include "Blueprint/UserWidget.h"
#include "Data/BrightEyeSettings.h"
#include "Subsystems/EditorActorSubsystem.h"
#include "Data/ColorPicker.h"
#include "UI/ScalarEntry.h"

constexpr float MAX_DELAY_SPEED = 40.0f;
constexpr float MIN_DELAY_SPEED = 4.0f;

TSharedPtr<FBrightEyeManagerImp> FBrightEyeManager::BrightEyeManagerImp;

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
	
	if(!ActiveViewport.IsValid() || !IsValid(ToolSettings)){return;}
	
	if(InParamType == EBEScalarParamType::Brightness)
	{
		ToolSettings->Brightness = InNewParam;
		UpdateBrightness();
	}
	else if(InParamType == EBEScalarParamType::Radius)
	{
		ToolSettings->Radius = InNewParam;
		UpdateRadius();
	}
	else if(InParamType == EBEScalarParamType::Distance)
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

	if(!ActiveViewport.IsValid() || !IsValid(ToolSettings)){return;}

	ToolSettings->Color = InNewColor;
	
	UpdateColor();
	
	ForceViewportRedraw();
	
	ResetLightModificationState();
}

void FBrightEyeManagerImp::OnSmoothRotationToggled()
{
	UBESettings::GetInstance()->bSmoothCameraRotation = !UBESettings::GetInstance()->bSmoothCameraRotation;

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
		static const FName bSmoothCameraRotationName("bSmoothCameraRotation");

		const UBESettings* ToolSettings = UBESettings::GetInstance();
		if(!IsValid(ToolSettings)){return;}
		
		if (InPropertyChangedEvent.GetPropertyName() == BrightnessName)
		{
			UpdateBrightness();

			if(IsValid(ControlPanelWidget))
			{
				ControlPanelWidget->GetBrightnessEntry()->InitializeScalarValue(ToolSettings->Brightness);
			}
		}
		else if (InPropertyChangedEvent.GetPropertyName() == RadiusName)
		{
			UpdateRadius();
			
			if(IsValid(ControlPanelWidget))
			{
				ControlPanelWidget->GetRadiusEntry()->InitializeScalarValue(ToolSettings->Radius);
			}
		}
		else if (InPropertyChangedEvent.GetPropertyName() == DistanceName)
		{
			UpdateDistance();
			UpdateBrightness();
			
			if(IsValid(ControlPanelWidget))
			{
				ControlPanelWidget->GetDistanceEntry()->InitializeScalarValue(ToolSettings->Distance);
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
		}
		else if (InPropertyChangedEvent.GetPropertyName() == ActivateLightOnPressName)
		{
			if(ToolSettings->bActivateLightOnPress && IsValid(BrightEyeActor) && IsValid(BrightEyeComponent) && BrightEyeComponent->IsVisible())
			{
				BrightEyeComponent->SetVisibility(false);
			}
		}
		else if (InPropertyChangedEvent.GetPropertyName() == bHidePanelWhenIdleName)
		{
			if(IsValid(ControlPanelWidget))
			{
				ControlPanelWidget->ChangeHidePanelWhenIdle(ToolSettings->bHidePanelWhenIdle);
			}
		}
		else if (InPropertyChangedEvent.GetPropertyName() == bSmoothCameraRotationName)
		{
			ResetBrightEyeRotation();

			if(IsValid(ControlPanelWidget))
			{
				ControlPanelWidget->SmoothCameraRotationStateChanged();
			}
		}
	}
}

void FBrightEyeManagerImp::OnResetBrightEyeSettings() const
{
	if(IsValid(BrightEyeActor) && IsValid(BrightEyeComponent))
	
	UpdateBrightEyeSpecs();

	if(IsValid(ControlPanelWidget))
	{
		InitializePanelParams();
	}
}

void FBrightEyeManagerImp::UpdateBrightEyeSpecs() const
{
	UpdateBrightness();
	UpdateRadius();
	UpdateDistance();
	UpdateColor();
}

void FBrightEyeManagerImp::UpdateBrightness() const
{
	if(const UBESettings* ToolSettings = UBESettings::GetInstance())
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
	if(const UBESettings* ToolSettings = UBESettings::GetInstance())
	{
		float NewRadius = 1 + ToolSettings->Radius * 79;
		BrightEyeComponent->SetOuterConeAngle(NewRadius);
	}
}

void FBrightEyeManagerImp::UpdateDistance() const
{
	if(const UBESettings* ToolSettings = UBESettings::GetInstance())
	{
		float NewDistance = 3000 + ToolSettings->Distance * (ToolSettings->MaxDistance - 3000);
		BrightEyeComponent->SetAttenuationRadius(NewDistance);
	}
}

void FBrightEyeManagerImp::UpdateColor() const
{
	if(const UBESettings* ToolSettings = UBESettings::GetInstance())
	{
		BrightEyeComponent->SetLightColor(ToolSettings->Color);
	}
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

		if(BrightEyeManagerImp.IsValid())
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
			InLevelEditor->OnActiveViewportChanged().AddRaw(this,&FBrightEyeManagerImp::OnActiveViewportChanged);

			OnActiveViewportChanged(nullptr,InLevelEditor->GetActiveViewportInterface());
		}
	});

	LevelEditor.OnMapChanged().AddRaw(this, &FBrightEyeManagerImp::OnMapChanged);
	LevelEditor.OnActorSelectionChanged().AddRaw(this, &FBrightEyeManagerImp::OnActorSelectionChanged);

	TickHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateRaw(this, &FBrightEyeManagerImp::OnTick));

	if(IsValid(UBESettings::GetInstance()))
	{
		UBESettings::GetInstance()->OnBrightLightSettingsChanged.BindRaw(this,&FBrightEyeManagerImp::OnBrightEyeSettingsChangedOnEditorSettings);
		UBESettings::GetInstance()->OnResetBrightEyeSettings.BindRaw(this,&FBrightEyeManagerImp::OnResetBrightEyeSettings);
	}
}

void FBrightEyeManagerImp::RemoveDelegates() const
{
	FLevelEditorModule& LevelEditor = FModuleManager::GetModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));
	LevelEditor.OnMapChanged().RemoveAll(this);
	LevelEditor.OnActorSelectionChanged().RemoveAll(this);
	
	if (TickHandle.IsValid())
	{
		FTSTicker::GetCoreTicker().RemoveTicker(TickHandle);
	}
	
	if(LevelEditor.GetFirstLevelEditor().IsValid())
	{
		LevelEditor.GetFirstLevelEditor()->OnActiveViewportChanged().RemoveAll(this);
	}

	if(IsValid(UBESettings::GetInstance()))
	{
		UBESettings::GetInstance()->OnBrightLightSettingsChanged.Unbind();
	}
}


bool FBrightEyeManagerImp::OnTick(float DeltaTime)
{
	if (ActiveViewport.IsValid() && IsValid(BrightEyeActor)  && IsValid(BrightEyeComponent))
	{
		UpdateLightTransformWithViewport(DeltaTime);
	}
	
	if(bLightSettingsModified)
	{
		TimeSinceLastModification += DeltaTime;

		if(TimeSinceLastModification >= ConfigSaveInterval)
		{
			UBESettings::GetInstance()->SaveToolConfig();
			
			bLightSettingsModified = false;
		}
	}

	return true;
}

void FBrightEyeManagerImp::OnActiveViewportChanged(TSharedPtr<IAssetViewport> OldViewport, TSharedPtr<IAssetViewport> NewViewport)
{
	ActiveViewport = StaticCastSharedPtr<SLevelViewport>(NewViewport);

	if (ActiveViewport.IsValid())
	{
		ActiveActorLock = ActiveViewport.Pin().Get()->GetLevelViewportClient().GetActiveActorLock();
	}
}

void FBrightEyeManagerImp::OnMapChanged(UWorld* World, EMapChangeType MapChangeType)
{
	if(World && MapChangeType != EMapChangeType::SaveMap)
	{
		DestroyBrightLight();
		DestroyControlPanel();
	}
}

void FBrightEyeManagerImp::OnActorSelectionChanged(const TArray<UObject*>& InActors, bool bIsSelected) const
{
	if(!BrightEyeActor || !GEditor){return;}

	for(UObject* CurrentSelectedActor : InActors)
	{
		if(CurrentSelectedActor == BrightEyeActor)
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
	TSharedPtr<FLevelEditorViewportClient> ViewportClient = StaticCastSharedPtr<FLevelEditorViewportClient>(ActiveViewport.Pin()->GetViewportClient());
	if(ViewportClient.IsValid())
	{
		BrightEyeRotation = ViewportClient->GetViewRotation();
	}
}

#pragma region Input

void FBrightEyeManagerImp::ActivateInputProcessor()
{
	InputProcessor = MakeShared<FBEInputPreProcessor>();
	InputProcessor->OnKeySelected.BindRaw(this, &FBrightEyeManagerImp::HandleKeySelected);
	InputProcessor->OnKeReleased.BindRaw(this, &FBrightEyeManagerImp::HandleKeyReleased);
	
	FSlateApplication::Get().RegisterInputPreProcessor(InputProcessor, 0);
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
	bIsPressed = true;
	return CameraLevelCommands->ProcessCommandBindings(InKey);
}

bool FBrightEyeManagerImp::HandleKeyReleased(const FKeyEvent& InKey)
{
	bIsPressed = false;
	return CameraLevelCommands->ProcessCommandBindings(InKey);
}


void FBrightEyeManagerImp::InitCommands()
{
	CameraLevelCommands = MakeShareable(new FUICommandList());
	CameraLevelCommands->MapAction(FBECommands::Get().ToggleBrightEye,FExecuteAction::CreateRaw(this,&FBrightEyeManagerImp::OnToggleLight));
	CameraLevelCommands->MapAction(FBECommands::Get().ToggleControlPanel,FExecuteAction::CreateRaw(this,&FBrightEyeManagerImp::OnToggleControlPanel));
}

#pragma endregion Input


void FBrightEyeManagerImp::OnToggleLight()
{
	UBESettings* ToolSettings = UBESettings::GetInstance();
	if(!IsValid(ToolSettings)){return;}
	
	if(ActiveViewport.IsValid() && ActiveViewport.Pin().Get()->GetActiveViewport()->HasFocus())
	{
		if(!IsValid(BrightEyeActor))
		{
			CreateBrightLight();
		}
		if(bIsPressed)
		{
			if(ToolSettings->bActivateLightOnPress)
			{
				BrightEyeComponent->SetVisibility(true);
			}
		}
		else
		{
			if(ToolSettings->bActivateLightOnPress)
			{
				BrightEyeComponent->SetVisibility(false);
			}
			else
			{
				BrightEyeComponent->SetVisibility(!BrightEyeComponent->IsVisible());
			}
		}

		TSharedPtr<SLevelViewport> ViewportPtr = ActiveViewport.Pin();
		if (ViewportPtr.IsValid() && !ViewportPtr->IsRealtime())
		{
			ViewportPtr->GetLevelViewportClient().Invalidate();
		}
	}
}

void FBrightEyeManagerImp::OnToggleControlPanel()
{
	UBESettings* ToolSettings = UBESettings::GetInstance();
	if(!IsValid(ToolSettings)){return;}
	
	if(!bIsPressed && ActiveViewport.IsValid() && ActiveViewport.Pin().Get()->GetActiveViewport()->HasFocus())
	{
		if(!IsValid(ControlPanelWidget) || !ControlPanelWidget->GetIsOnTheScreen())
		{
			CreateAndShowControlPanel();
		}
		else
		{
			DestroyControlPanel();
		}
	}
}

void FBrightEyeManagerImp::CreateBrightLight()
{
	UWorld* EditorWorld = GEditor->GetEditorWorldContext().World();
	if (!EditorWorld) return;

	BrightEyeActor = EditorWorld->SpawnActor<AActor>(AActor::StaticClass());
	if (IsValid(BrightEyeActor))
	{
		BrightEyeActor->SetActorLabel(TEXT("Bright Eye Manager"));
		
		BrightEyeActor->SetFlags(RF_Transient);

		BrightEyeComponent = NewObject<USpotLightComponent>(BrightEyeActor);
		if(!IsValid(BrightEyeComponent)){return;}

		BrightEyeComponent->RegisterComponent();
		BrightEyeComponent->AttachToComponent(BrightEyeActor->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);

		const UBESettings* ToolSettings = UBESettings::GetInstance();
		if(!IsValid(ToolSettings)){return;}
		
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
	if(!IsValid(BrightEyeActor)){return;}

	BrightEyeActor->Destroy();
	BrightEyeActor = nullptr;
}

void FBrightEyeManagerImp::UpdateLightTransformWithViewport(const float& InDeltaTime)
{
	TSharedPtr<FLevelEditorViewportClient> ViewportClient = StaticCastSharedPtr<FLevelEditorViewportClient>(ActiveViewport.Pin()->GetViewportClient());
	
	if(ViewportClient.IsValid() && IsValid(BrightEyeComponent) && UBESettings::GetInstance())
	{
		FRotator CameraRotation = ViewportClient->GetViewRotation();
		FVector CameraLocation =  ViewportClient->GetViewLocation() + CameraRotation.Vector() * -30.0f;
		
		if(!UBESettings::GetInstance()->bSmoothCameraRotation)
		{
			BrightEyeComponent->SetWorldLocation(CameraLocation);
			BrightEyeComponent->SetWorldRotation(CameraRotation);
		}
		else
		{
			float DelaySpeed = FMath::Lerp(MAX_DELAY_SPEED,MIN_DELAY_SPEED,UBESettings::GetInstance()->RotationDelayFactor);
			BrightEyeRotation = FMath::RInterpTo(BrightEyeRotation, CameraRotation, InDeltaTime,DelaySpeed);
			BrightEyeComponent->SetWorldLocation(CameraLocation);
			BrightEyeComponent->SetWorldRotation(BrightEyeRotation);

			TSharedPtr<SLevelViewport> ViewportPtr = ActiveViewport.Pin();
			if (ViewportPtr.IsValid() && !ViewportPtr->IsRealtime())
			{
				ViewportPtr->GetLevelViewportClient().Invalidate();
			}
		}
	}
}

UUserWidget* FBrightEyeManagerImp::GetControlPanel()
{
	if(IsValid(ControlPanelWidget)){return ControlPanelWidget;}

	const TSoftClassPtr<UUserWidget> WidgetClassPtr(BEToolAssetData::ControlPanelPath);
	if(const auto ClassRef = WidgetClassPtr.LoadSynchronous())
	{
		if (const auto CreatedWidget = Cast<UBEControlPanel>(CreateWidget(GEditor->GetEditorWorldContext().World(), ClassRef)))
		{
			ControlPanelWidget = CreatedWidget;
			
			InitializePanel();
		}
	}

	return ControlPanelWidget;
}

void FBrightEyeManagerImp::InitializePanel()
{
	ControlPanelWidget->InitializePanel();
	
	BindPanelDelegates();
	
	InitializePanelParams();
}

void FBrightEyeManagerImp::BindPanelDelegates()
{
	if(!IsValid(ControlPanelWidget)){return;}
	
	ControlPanelWidget->GetBrightnessEntry()->GetOnParamChangedDelegate().BindRaw(this,&FBrightEyeManagerImp::OnScalarParamChanged,EBEScalarParamType::Brightness);
	ControlPanelWidget->GetDistanceEntry()->GetOnParamChangedDelegate().BindRaw(this,&FBrightEyeManagerImp::OnScalarParamChanged,EBEScalarParamType::Distance);
	ControlPanelWidget->GetRadiusEntry()->GetOnParamChangedDelegate().BindRaw(this,&FBrightEyeManagerImp::OnScalarParamChanged,EBEScalarParamType::Radius);
	ControlPanelWidget->GetColorPicker()->GetOnParamChangedDelegate().BindRaw(this,&FBrightEyeManagerImp::OnColorParamChanged);
	ControlPanelWidget->GetOnDelayStateChanged().BindRaw(this,&FBrightEyeManagerImp::OnSmoothRotationToggled);
}

void FBrightEyeManagerImp::InitializePanelParams() const
{
	if(const UBESettings* ToolSettings = UBESettings::GetInstance())
	{
		if(IsValid(ControlPanelWidget->GetBrightnessEntry()))
		{
			ControlPanelWidget->GetBrightnessEntry()->InitializeScalarValue(ToolSettings->Brightness);
		}
		if(IsValid(ControlPanelWidget->GetDistanceEntry()))
		{
			ControlPanelWidget->GetDistanceEntry()->InitializeScalarValue(ToolSettings->Distance);
		}
		if(IsValid(ControlPanelWidget->GetRadiusEntry()))
		{
			ControlPanelWidget->GetRadiusEntry()->InitializeScalarValue(ToolSettings->Radius);
		}
		if(IsValid(ControlPanelWidget->GetColorPicker()))
		{
			ControlPanelWidget->GetColorPicker()->InitializeColor(ToolSettings->Color);
		}
		ControlPanelWidget->ChangeHidePanelWhenIdle(ToolSettings->bHidePanelWhenIdle);
		ControlPanelWidget->ReInitializeCameraRotationState();
	}
}

void FBrightEyeManagerImp::UnbindPanelDelegates() const
{
	if(!IsValid(ControlPanelWidget)){return;}

	if(IsValid(ControlPanelWidget->GetBrightnessEntry()))
	{
		ControlPanelWidget->GetBrightnessEntry()->GetOnParamChangedDelegate().Unbind();
	}
	if(IsValid(ControlPanelWidget->GetDistanceEntry()))
	{
		ControlPanelWidget->GetDistanceEntry()->GetOnParamChangedDelegate().Unbind();
	}
	if(IsValid(ControlPanelWidget->GetRadiusEntry()))
	{
		ControlPanelWidget->GetRadiusEntry()->GetOnParamChangedDelegate().Unbind();
	}
	ControlPanelWidget->GetOnDelayStateChanged().Unbind();
}


void FBrightEyeManagerImp::CreateAndShowControlPanel()
{
	if(!IsValid(BrightEyeActor))
	{
		CreateBrightLight();
	}
	
	FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));
	TSharedPtr<IAssetViewport> ActiveLevelViewport = LevelEditorModule.GetFirstActiveViewport();
        
	if (ActiveLevelViewport.IsValid() && IsValid(GetControlPanel()))
	{
		ActiveLevelViewport->AddOverlayWidget(GetControlPanel()->TakeWidget());
		ControlPanelWidget->SetIsOnTheScreen(true);
	}
}

void FBrightEyeManagerImp::RemoveControlPanel() const
{
	if(!IsValid(ControlPanelWidget)){return;}
	
	FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));
	TSharedPtr<IAssetViewport> ActiveLevelViewport = LevelEditorModule.GetFirstActiveViewport();
        
	if (ActiveLevelViewport.IsValid())
	{
		ActiveLevelViewport->RemoveOverlayWidget(ControlPanelWidget->TakeWidget());
		ControlPanelWidget->SetIsOnTheScreen(false);
	}
}

void FBrightEyeManagerImp::DestroyControlPanel()
{
	if(!IsValid(ControlPanelWidget)){return;}
	
	FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));
	TSharedPtr<IAssetViewport> ActiveLevelViewport = LevelEditorModule.GetFirstActiveViewport();
        
	if (ActiveLevelViewport.IsValid())
	{
		ActiveLevelViewport->RemoveOverlayWidget(ControlPanelWidget->TakeWidget());
		ControlPanelWidget->SetIsOnTheScreen(false);
		ControlPanelWidget = nullptr;
	}
}
