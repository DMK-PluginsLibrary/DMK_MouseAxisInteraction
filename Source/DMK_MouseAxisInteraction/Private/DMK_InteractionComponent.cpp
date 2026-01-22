// 


#include "DMK_InteractionComponent.h"

#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values for this component's properties
UDMK_InteractionComponent::UDMK_InteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UDMK_InteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
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
	if (CachedComponent->GetInputMapping().IsNull() == false)
	{
		MappingContextAdded = CachedComponent->GetInputMapping().LoadSynchronous();
		const APlayerController* PC=  UGameplayStatics::GetPlayerController(GetWorld(),0);
		UEnhancedInputLocalPlayerSubsystem* Input =  ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
		
		Input->AddMappingContext(MappingContextAdded, 1);
	}
	if (CachedComponent->ShouldBlockMovement())
	{
		UCharacterMovementComponent* CharacterMovementComponent =  Cast<UCharacterMovementComponent>(GetOwner()->GetComponentByClass(UCharacterMovementComponent::StaticClass()));
		if (CharacterMovementComponent != nullptr)
		{
			CharacterMovementComponent->SetMovementMode(MOVE_None);
		}
	}
	CachedComponent->HoldInteraction(Issuer);
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
		FVector OwnerRightVector = GetOwner()->GetActorRightVector();
		UActorComponent* ActorCachedComponent = Cast<UActorComponent>(CachedComponent.GetObject());
		if (ActorCachedComponent == nullptr)
		{
			return;
		}
		FVector ComponentOwnerForwardVector = ActorCachedComponent->GetOwner()->GetActorForwardVector();
		float Dot = OwnerRightVector.Dot(ComponentOwnerForwardVector);
		LocalAxis *= FMath::Sign(Dot);
	}
	CachedComponent->HoldInteractionAxis(GetOwner(), LocalAxis);
}

TSoftObjectPtr<UInputMappingContext> UDMK_InteractionComponent::GetInputMapping_Implementation()
{
	return IsValid(CachedComponent.GetObject()) ? CachedComponent->GetInputMapping() : TSoftObjectPtr<UInputMappingContext>();
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
	if (CachedComponent->ShouldBlockMovement())
	{
		UCharacterMovementComponent* CharacterMovementComponent =  Cast<UCharacterMovementComponent>(GetOwner()->GetComponentByClass(UCharacterMovementComponent::StaticClass()));
		if (CharacterMovementComponent != nullptr)
		{
			CharacterMovementComponent->SetMovementMode(MOVE_Walking);
		}
	}
	CachedComponent->HoldInteractionStop(GetOwner(), TimeElapsed);
	
}

TSoftObjectPtr<UTexture2D> UDMK_InteractionComponent::GetInteractionIcon_Implementation()
{
	return IsValid(CachedComponent.GetObject()) ? CachedComponent->GetInteractionIcon() : nullptr;
}

FText UDMK_InteractionComponent::GetInteractionDescription_Implementation()
{
	return IsValid(CachedComponent.GetObject()) ? CachedComponent->GetInteractionDescription() : FText();
}

bool UDMK_InteractionComponent::ShouldBlockMovement_Implementation()
{
	
	return IsValid(CachedComponent.GetObject()) ? CachedComponent->ShouldBlockMovement() : false;
}

void UDMK_InteractionComponent::SetComponentToInteract_Implementation(UPrimitiveComponent* NewCachedComponent)
{
	if (IsValid(CachedComponent.GetObject()))
	{
		HoldInteractionStop(GetOwner(), CurrentTimeElapsed);
	}
	if (NewCachedComponent->Implements<UDMK_InteractionTargetInterface>())
	{
		CachedComponent = TScriptInterface<IDMK_InteractionTargetInterface>(NewCachedComponent);
		HoldInteraction(GetOwner());
	}
}

void UDMK_InteractionComponent::UpdateCachedComponentUsingTrace()
{
	const USceneComponent* BaseComponent = Cast<USceneComponent>(InteractionBaseComponent.GetComponent(this->GetOwner()));
	if (IsValid(CachedComponent.GetObject()) == false || IsValid(BaseComponent))
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
	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartOfTrace, EndOfTrace, ECollisionChannel::ECC_Visibility, Params))
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
		const APlayerController* PC=  UGameplayStatics::GetPlayerController(GetWorld(),0);
		UEnhancedInputLocalPlayerSubsystem* Input =  ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
		
		Input->RemoveMappingContext(MappingContextAdded);
		MappingContextAdded = nullptr;
	}
}

