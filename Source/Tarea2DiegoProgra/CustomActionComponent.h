#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CustomActionSubsystem.h"
#include "CustomActionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnParameterReceived, const FString&, ParameterName, const FString&, ParameterValue);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TAREA2DIEGOPROGRA_API UCustomActionComponent : public UActorComponent
{
    GENERATED_BODY()



public:


    UCustomActionComponent();

    float GetDetectionRadius()const;
    float GetDetectionInterval() const;

    // Recibir parámetros desde el subsistema
    UFUNCTION(BlueprintCallable)
    void ReceiveParameter(const FString& ParameterName, const FString& ParameterValue);

    void SetPerceptionInfo(const FPerceptionInfo& NewInfo);



    //Notificar parámetros recibidos
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnParameterReceived OnParameterReceived;

    UPROPERTY(EditAnywhere, Category = "Events")
    bool Test;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
    float DetectionRadiusInfo;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
    float DetectionIntervalInfo;

private:




protected:
    virtual void BeginPlay() override;
};
