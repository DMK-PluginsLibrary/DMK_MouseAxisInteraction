#include "DMK_InteractionComponent.h"

#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "KismetTraceUtils.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"


UDMK_InteractionComponent::UDMK_InteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UDMK_InteractionComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UDMK_InteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                              FActorComponentTickFunction* ThisTickFunction)
{
	if (this->DuringInteraction)
	{
		CurrentTimeElapsed += DeltaTime;
	}
	else if (EnabledLineTrace)
	{
		UpdateCachedComponentUsingTrace();
	}

}

void UDMK_InteractionComponent::HoldInteraction_Implementation(AActor* Issuer)
{
	if (IsValid(CachedComponent.GetObject()) == false)
	{
		return;
	}
	DuringInteraction = true;
	CurrentTimeElapsed = 0.f;
	if (IDMK_InteractionTargetInterface::Execute_GetInputMapping(CachedComponent.GetObject()).IsNull() == false)
	{
		MappingContextAdded = Cast<UInputMappingContext>(IDMK_InteractionTargetInterface::Execute_GetInputMapping(CachedComponent.GetObject()).LoadSynchronous());
		const APlayerController* PC=  UGameplayStatics::GetPlayerController(GetWorld(),0);
		UEnhancedInputLocalPlayerSubsystem* Input =  ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
		
		Input->AddMappingContext(MappingContextAdded, 1);
	}
	if (IDMK_InteractionTargetInterface::Execute_ShouldBlockMovement(CachedComponent.GetObject()))
	{
		UCharacterMovementComponent* CharacterMovementComponent =  Cast<UCharacterMovementComponent>(GetOwner()->GetComponentByClass(UCharacterMovementComponent::StaticClass()));
		if (CharacterMovementComponent != nullptr)
		{
			CharacterMovementComponent->SetMovementMode(MOVE_None);
		}
	}
	IDMK_InteractionTargetInterface::Execute_HoldInteraction(CachedComponent.GetObject(), Issuer);
}

void UDMK_InteractionComponent::HoldInteractionAxis_Implementation(AActor* Issuer, const FVector2D& Axis)
{
	if (IsValid(CachedComponent.GetObject()) == false)
	{
		return;
	}
	FVector2D LocalAxis = Axis;

	if (ShouldCheckDirection)
	{
		FVector OwnerRightVector = GetOwner()->GetActorForwardVector();
		UActorComponent* ActorCachedComponent = Cast<UActorComponent>(CachedComponent.GetObject());
		if (ActorCachedComponent == nullptr)
		{
			return;
		}
		FVector ComponentOwnerForwardVector = ActorCachedComponent->GetOwner()->GetActorRightVector();
		float Dot = OwnerRightVector.Dot(ComponentOwnerForwardVector);
		LocalAxis *= FMath::Sign(Dot);
		if (Debug)
		{
			GEngine->bEnableOnScreenDebugMessages = true;
			DrawDebugDirectionalArrow(GetWorld(), GetOwner()->GetActorLocation(), GetOwner()->GetActorLocation() + OwnerRightVector* 300, 1.f, FColor::Cyan,false, 20.f);
			DrawDebugDirectionalArrow(GetWorld(), ActorCachedComponent->GetOwner()->GetActorLocation(), ActorCachedComponent->GetOwner()->GetActorLocation() + ComponentOwnerForwardVector* 300, 1.f, FColor::Purple,false, 20.f);
			GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Red, FString::Printf(TEXT("OwnerRightVector %s"), *OwnerRightVector.ToCompactString()));
			GEngine->AddOnScreenDebugMessage(2, 1.f, FColor::Red, FString::Printf(TEXT("ComponentOwnerForwardVector %s"), *ComponentOwnerForwardVector.ToCompactString()));
			GEngine->AddOnScreenDebugMessage(3, 1.f, FColor::Red, FString::Printf(TEXT(" OwnerRightVector Dot ComponentOwnerForwardVector = %f"), Dot));
		}
	}
	IDMK_InteractionTargetInterface::Execute_HoldInteractionAxis(CachedComponent.GetObject(), GetOwner(), LocalAxis);
}

TSoftObjectPtr<UInputMappingContext> UDMK_InteractionComponent::GetInputMapping_Implementation()
{
	return IsValid(CachedComponent.GetObject()) ? IDMK_InteractionTargetInterface::Execute_GetInputMapping(CachedComponent.GetObject()) : TSoftObjectPtr<UInputMappingContext>();
}

void UDMK_InteractionComponent::HoldInteractionStop_Implementation(AActor* Issuer, const float TimeElapsed)
{
	DuringInteraction = false;
	if (IsValid(CachedComponent.GetObject()) == false)
	{
		return;
	}
	if (MappingContextAdded)
	{
		const APlayerController* PC=  UGameplayStatics::GetPlayerController(GetWorld(),0);
		UEnhancedInputLocalPlayerSubsystem* Input =  ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
		
		Input->RemoveMappingContext(MappingContextAdded);
		MappingContextAdded = nullptr;
	}
	if (IDMK_InteractionTargetInterface::Execute_ShouldBlockMovement(CachedComponent.GetObject()))
	{
		UCharacterMovementComponent* CharacterMovementComponent =  Cast<UCharacterMovementComponent>(GetOwner()->GetComponentByClass(UCharacterMovementComponent::StaticClass()));
		if (CharacterMovementComponent != nullptr)
		{
			CharacterMovementComponent->SetMovementMode(ModeOfMovementToReturn);
		}
	}
	IDMK_InteractionTargetInterface::Execute_HoldInteractionStop(CachedComponent.GetObject(), GetOwner(), TimeElapsed);
}

TSoftObjectPtr<UTexture2D> UDMK_InteractionComponent::GetInteractionIcon_Implementation()
{
	return IsValid(CachedComponent.GetObject()) ? IDMK_InteractionTargetInterface::Execute_GetInteractionIcon(CachedComponent.GetObject()) : nullptr;
}

FText UDMK_InteractionComponent::GetInteractionDescription_Implementation()
{
	return IsValid(CachedComponent.GetObject()) ? IDMK_InteractionTargetInterface::Execute_GetInteractionDescription(CachedComponent.GetObject()) : FText();
}

bool UDMK_InteractionComponent::ShouldBlockMovement_Implementation()
{
	
	return IsValid(CachedComponent.GetObject()) ? IDMK_InteractionTargetInterface::Execute_ShouldBlockMovement(CachedComponent.GetObject()) : false;
}

void UDMK_InteractionComponent::SetComponentToInteract_Implementation(UPrimitiveComponent* NewCachedComponent)
{
	if (IsValid(CachedComponent.GetObject()))
	{
		IDMK_InteractionTargetInterface::Execute_HoldInteractionStop(this, GetOwner(), CurrentTimeElapsed);
	}
	if (NewCachedComponent->Implements<UDMK_InteractionTargetInterface>())
	{
		CachedComponent = TScriptInterface<IDMK_InteractionTargetInterface>(NewCachedComponent);
		IDMK_InteractionTargetInterface::Execute_HoldInteraction(this, GetOwner());
	}
}

void UDMK_InteractionComponent::UpdateCachedComponentUsingTrace()
{
	const USceneComponent* BaseComponent = Cast<USceneComponent>(InteractionBaseComponent.GetComponent(this->GetOwner()));
	if (IsValid(BaseComponent) == false)
	{
		return;
	}

	const FVector StartOfTrace =  BaseComponent->GetComponentLocation();
	const FVector EndOfTrace = BaseComponent->GetForwardVector() * 300 + StartOfTrace;
	FCollisionQueryParams Params;
	if (this->ShouldIgnoreOwner)
	{
		Params.AddIgnoredActor(GetOwner());
	}

	FHitResult HitResult;
	const bool bWasHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartOfTrace, EndOfTrace, ECollisionChannel::ECC_Visibility, Params);
	if (ShouldShowDebugTrace)
	{
		DrawDebugLineTraceSingle(GetWorld(), StartOfTrace, EndOfTrace, EDrawDebugTrace::ForOneFrame, bWasHit, HitResult, FColor::Red, FColor::Green, 5.f);
	}
	if (bWasHit)
	{
		UPrimitiveComponent* HitComponent = HitResult.GetComponent();
		
		if (HitComponent->Implements<UDMK_InteractionTargetInterface>())
		{
			CachedComponent = TScriptInterface<IDMK_InteractionTargetInterface>(HitComponent);
			HitLocation = HitResult.Location;
			return;
		}
	}
	
	CachedComponent = nullptr;
	HitLocation = FVector(0);
	
	if (MappingContextAdded)
	{
		const APlayerController* PlayerController=  UGameplayStatics::GetPlayerController(GetWorld(),0);
		UEnhancedInputLocalPlayerSubsystem* Input =  ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		
		Input->RemoveMappingContext(MappingContextAdded);
		MappingContextAdded = nullptr;
	}
}

