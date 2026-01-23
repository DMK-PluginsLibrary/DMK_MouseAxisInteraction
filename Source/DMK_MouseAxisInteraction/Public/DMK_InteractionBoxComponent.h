#pragma once

#include "CoreMinimal.h"
#include "DMK_InteractionTargetInterface.h"
#include "Components/BoxComponent.h"
#include "DMK_InteractionBoxComponent.generated.h"

UENUM()
enum class ELockState : uint8
{
	Lock,
	Unlock,
};

class UPhysicsConstraintComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDMKInteractionDelegate, AActor*, Issuer);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDMKInteractionStopDelegate, AActor*, Issuer, float, TimeElapsed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDMKInteractionAxisDelegate, AActor*, Issuer, const FVector2D&, Axis);
UCLASS(ClassGroup=(Custom), Blueprintable, meta=(BlueprintSpawnableComponent))
class DMK_MOUSEAXISINTERACTION_API UDMK_InteractionBoxComponent : public UBoxComponent, public IDMK_InteractionTargetInterface
{
	GENERATED_BODY()

public:
	UDMK_InteractionBoxComponent();

protected:

	virtual void BeginPlay() override;

public:
	
	// IDMK_InteractionTargetInterface
	virtual void HoldInteraction_Implementation(AActor* Issuer) override;
	virtual void HoldInteractionAxis_Implementation(AActor* Issuer, const FVector2D& Axis) override;
	virtual bool HasInputMapping_Implementation() override;
	virtual void HoldInteractionStop_Implementation(AActor* Issuer, const float TimeElapsed) override;
	virtual TSoftObjectPtr<UTexture2D> GetInteractionIcon_Implementation() override;
	virtual FText GetInteractionDescription_Implementation() override;
	virtual bool ShouldBlockMovement_Implementation() override;
	virtual TSoftObjectPtr<UInputMappingContext> GetInputMapping_Implementation() override;

	//--------------------------------

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category ="DMK_Interaction")
	bool bHaveAxisInteraction = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category ="DMK_Interaction")
	bool bShouldBlockMovement = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category ="DMK_Interaction")
	float MaximumTimeElapsedToBeFastInteraction = 0.2f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category ="DMK_Interaction")
	TSoftObjectPtr<UInputMappingContext> MappingContextToAdd;
	
	UPROPERTY(BlueprintAssignable, Category ="DMK_Interaction")
	FDMKInteractionDelegate PressInteraction;
	UPROPERTY(BlueprintAssignable, Category ="DMK_Interaction")
	FDMKInteractionDelegate HoldInteractionEvent;
	UPROPERTY(BlueprintAssignable, Category ="DMK_Interaction")
	FDMKInteractionStopDelegate HoldInteractionStopEvent;
	UPROPERTY(BlueprintAssignable, Category ="DMK_Interaction")
	FDMKInteractionAxisDelegate HoldAxisInteractionEvent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category ="DMK_Interaction|Visualisation", meta=( DisplayThumbnail="true", DisplayName="Image", AllowedClasses="/Script/Engine.Texture,/Script/Engine.MaterialInterface,/Script/Engine.SlateTextureAtlasInterface", DisallowedClasses = "/Script/MediaAssets.MediaTexture"))
	TSoftObjectPtr<UObject> InteractionIcon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category ="DMK_Interaction|Visualisation")
	FText InteractionDescription;
	
	UFUNCTION(BlueprintCallable, Category ="DMK_Interaction")
	static void SetNewConstrainRotation(const FRotator& NewRotator, UPhysicsConstraintComponent* PhysicsConstraintComponent);

	UFUNCTION(BlueprintCallable, Category ="DMK_Interaction")
	static FVector ClampVectorByConstrainAxis(const FVector& OneConstrain, const FVector& SecondConstrain, const FVector& VectorToClamp);
	UFUNCTION(BlueprintCallable, Category ="DMK_Interaction")
	static FRotator ClampRotatorByConstrainAxis(const FRotator& OneConstrain, const FRotator& SecondConstrain, const FRotator& RotatorToClamp);
	UFUNCTION(BlueprintCallable, Category ="DMK_Interaction")
	static bool AreRotatorsNearlyEqual(const FRotator& OneRotator, const FRotator& SecondRotator, float AcceptableDistance);

};
