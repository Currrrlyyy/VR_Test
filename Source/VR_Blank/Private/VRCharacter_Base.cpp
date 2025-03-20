// Fill out your copyright notice in the Description page of Project Settings.


#include "VRCharacter_Base.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Logging/LoggingHelper.h"

AVRCharacter_Base::AVRCharacter_Base()
{
	PrimaryActorTick.bCanEverTick = true;
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	RootComponent = CapsuleComponent;
	
	VRCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("VRCameraComponent"));
	VRCameraComponent->SetupAttachment(CapsuleComponent);

	MovementComponent = CreateDefaultSubobject<UPawnMovementComponent, UFloatingPawnMovement>(TEXT("VRCharMovementComp"));
	MovementComponent->UpdatedComponent = CapsuleComponent;
}

// Called when the game starts or when spawned
void AVRCharacter_Base::BeginPlay()
{
	Super::BeginPlay();
	AddDefaultInputMappingContext();
}

// Called every frame
void AVRCharacter_Base::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AVRCharacter_Base::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	UE_LOG(LogVRCharacter, Warning, TEXT("Setting InputComp..."));
	
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EnhancedInputComponent)
	{
		UE_LOG(LogVRCharacter, Warning, TEXT("[%hs]: PlayerInputComponent was not valid"), __FUNCTION__);
		return;
	}
		
	if (MovementInputAction)
	{
		EnhancedInputComponent->BindAction(MovementInputAction, ETriggerEvent::Triggered, this, &AVRCharacter_Base::OnMovementActionTriggered);		
	}
}

UPawnMovementComponent* AVRCharacter_Base::GetMovementComponent() const
{
	if (UPawnMovementComponent* OutComponent = Super::GetMovementComponent())
	{
		return OutComponent;
	}
	return MovementComponent;
}

void AVRCharacter_Base::OnMovementActionTriggered(const FInputActionValue& Value)
{
	FVector2D VectorValue = Value.Get<FVector2D>();
	FVector InputDirection = FVector(VectorValue.Y, VectorValue.X, 0);

	FVector LineStart = GetRootComponent()->GetComponentLocation();
	FVector FV = VRCameraComponent->GetForwardVector();
	FVector RV = VRCameraComponent->GetRightVector();
	FV.Z = 0.0f;
	RV.Z = 0.0f;
	
	FV *= InputDirection.X;
	RV *= InputDirection.Y;

	DrawDebugLine(GetWorld(), LineStart, LineStart + FV * 500.0f, FColor::Red, false, 0.0f);
	DrawDebugLine(GetWorld(), LineStart, LineStart + RV * 500.0f, FColor::Green, false, 0.0f);

	FVector LineEnd = LineStart + InputDirection * 200.0f;
	DrawDebugLine(GetWorld(), LineStart, LineEnd, FColor::Blue, false, 0.0f);
	AddMovementInput(FV);
	AddMovementInput(RV);
}

void AVRCharacter_Base::AddDefaultInputMappingContext() const
{
	if (const APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem =
			PlayerController->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			InputSubsystem->AddMappingContext(DefaultIMC, 0);
		}
	}
}

