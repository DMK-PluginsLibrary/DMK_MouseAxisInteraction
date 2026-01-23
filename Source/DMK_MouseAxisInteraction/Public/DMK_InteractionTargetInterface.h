// All rights reserved for Perspective Games

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DMK_InteractionTargetInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UDMK_InteractionTargetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DMK_MOUSEAXISINTERACTION_API IDMK_InteractionTargetInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category ="DMK_Interaction")
	void HoldInteraction(AActor* Issuer);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category ="DMK_Interaction")
	void HoldInteractionStop(AActor* Issuer, const float TimeElapsed);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category ="DMK_Interaction")
	void HoldInteractionAxis(AActor* Issuer, const FVector2D& Axis);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category ="DMK_Interaction")
	bool HasInputMapping();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category ="DMK_Interaction")
	TSoftObjectPtr<UInputMappingContext> GetInputMapping();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category ="DMK_Interaction")
	bool ShouldBlockMovement();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category ="DMK_Interaction")
	TSoftObjectPtr<UTexture2D> GetInteractionIcon();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category ="DMK_Interaction")
	FText GetInteractionDescription();
};
