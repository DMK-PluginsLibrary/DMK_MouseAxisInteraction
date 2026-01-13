// All rights reserved for Perspective Games


#include "DMK_InteractionBoxComponent.h"

#include "Engine/EngineCustomTimeStep.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"

UDMK_InteractionBoxComponent::UDMK_InteractionBoxComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UDMK_InteractionBoxComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UDMK_InteractionBoxComponent::HoldInteraction_Implementation(AActor* Issuer)
{
	HoldInteractionEvent.Broadcast(Issuer);
}

void UDMK_InteractionBoxComponent::HoldInteractionAxis_Implementation(AActor* Issuer, const FVector2D& Axis)
{
	HoldAxisInteractionEvent.Broadcast(Issuer,Axis);
}

bool UDMK_InteractionBoxComponent::HasBlockingMapping_Implementation()
{
	return bHaveAxisInteraction;
}

void UDMK_InteractionBoxComponent::HoldInteractionStop_Implementation(AActor* Issuer, float TimeElapsed)
{
	if (TimeElapsed < MaximumTimeElapsedToBeFastInteraction)
	{
		PressInteraction.Broadcast(Issuer);
	}
	
	HoldInteractionStopEvent.Broadcast(Issuer, TimeElapsed);
}

void UDMK_InteractionBoxComponent::SetNewConstrainRotation(const FRotator& NewRotator,
	UPhysicsConstraintComponent* PhysicsConstraintComponent)
{
	if (PhysicsConstraintComponent != nullptr)
	{
		PhysicsConstraintComponent->ConstraintInstance.AngularRotationOffset = NewRotator;	

		PhysicsConstraintComponent->SetAngularSwing1Limit(ACM_Limited, NewRotator.Yaw);
		PhysicsConstraintComponent->InitComponentConstraint();
	}
}

FVector UDMK_InteractionBoxComponent::ClampVectorByConstrainAxis(const FVector& OneConstrain,
	const FVector& SecondConstrain, const FVector& VectorToClamp)
{
	FVector Result{};
	
	Result.X = FMath::Clamp(VectorToClamp.X, FMath::Min(OneConstrain.X, SecondConstrain.X), FMath::Max(OneConstrain.X, SecondConstrain.X));
	Result.Y = FMath::Clamp(VectorToClamp.Y, FMath::Min(OneConstrain.Y, SecondConstrain.Y), FMath::Max(OneConstrain.Y, SecondConstrain.Y));
	Result.Z = FMath::Clamp(VectorToClamp.Z, FMath::Min(OneConstrain.Z, SecondConstrain.Z), FMath::Max(OneConstrain.Z, SecondConstrain.Z));

	return Result;
}

FRotator UDMK_InteractionBoxComponent::ClampRotatorByConstrainAxis(const FRotator& OneConstrain,
	const FRotator& SecondConstrain, const FRotator& RotatorToClamp)
{
	FRotator Result{};

	Result.Roll = FMath::ClampAngle(RotatorToClamp.Roll, FMath::Min(OneConstrain.Roll, SecondConstrain.Roll), FMath::Max(OneConstrain.Roll, SecondConstrain.Roll));
	Result.Yaw = FMath::ClampAngle(RotatorToClamp.Yaw, FMath::Min(OneConstrain.Yaw, SecondConstrain.Yaw), FMath::Max(OneConstrain.Yaw, SecondConstrain.Yaw));
	Result.Pitch = FMath::ClampAngle(RotatorToClamp.Pitch, FMath::Min(OneConstrain.Pitch, SecondConstrain.Pitch), FMath::Max(OneConstrain.Pitch, SecondConstrain.Pitch));

	return Result;
}

bool UDMK_InteractionBoxComponent::AreRotatorsNearlyEqual(const FRotator& OneRotator, const FRotator& SecondRotator,
	float AcceptableDistance)
{
	FRotator FirstRotationClamped = OneRotator;
	FRotator SecondRotationClamped = SecondRotator;
	FVector DeltaVector{FirstRotationClamped.Pitch - SecondRotationClamped.Pitch, FirstRotationClamped.Yaw - SecondRotationClamped.Yaw, FirstRotationClamped.Roll - SecondRotationClamped.Roll};
	
	return DeltaVector.Length() < AcceptableDistance;
}

