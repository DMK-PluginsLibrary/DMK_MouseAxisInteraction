// 

#pragma once

#include "CoreMinimal.h"
#include "DMK_InteractionTargetInterface.h"
#include "Components/ActorComponent.h"
#include "DMK_InteractionComponent.generated.h"


class UInputMappingContext;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DMK_MOUSEAXISINTERACTION_API UDMK_InteractionComponent : public UActorComponent, public IDMK_InteractionTargetInterface
{
	GENERATED_BODY()

public:
	UDMK_InteractionComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	
	// IDMK_InteractionTargetInterface
	virtual void HoldInteraction_Implementation(AActor* Issuer) override;
	virtual void HoldInteractionAxis_Implementation(AActor* Issuer, const FVector2D& Axis) override;
	virtual TSoftObjectPtr<UInputMappingContext> GetInputMapping_Implementation() override;
	virtual void HoldInteractionStop_Implementation(AActor* Issuer, const float TimeElapsed) override;
	virtual TSoftObjectPtr<UTexture2D> GetInteractionIcon_Implementation() override;
	virtual FText GetInteractionDescription_Implementation() override;
	virtual bool ShouldBlockMovement_Implementation() override;
	//--------------------------------
	
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetComponentToInteract(UPrimitiveComponent* NewCachedComponent);
	
	void UpdateCachedComponentUsingTrace();
	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category ="DMK_Interaction")
	TScriptInterface<IDMK_InteractionTargetInterface> CachedComponent;

	//From this component trace will be launched
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category ="DMK_Interaction")
	FComponentReference InteractionBaseComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category ="DMK_Interaction")
	bool EnabledLineTrace = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category ="DMK_Interaction")
	bool ShouldCheckDirection = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category ="DMK_Interaction")
	bool DuringInteraction = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category ="DMK_Interaction")
	FVector HitLocation = FVector(0);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category ="DMK_Interaction")
	float CurrentTimeElapsed = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category ="DMK_Interaction")
	bool ShouldIgnoreOwner = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category ="DMK_Interaction")
	bool ShouldShowDebugTrace = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category ="DMK_Interaction")
	TObjectPtr<UInputMappingContext> MappingContextAdded;
};
