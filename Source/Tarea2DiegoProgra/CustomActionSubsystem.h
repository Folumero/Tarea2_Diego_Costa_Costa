// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LocalPlayer.h"
#include "CustomActionSubsystem.generated.h"




USTRUCT(BlueprintType)
struct FPerceptionInfo
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
    float DetectionRadius;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
    float DetectionInterval;

    FPerceptionInfo(): DetectionRadius(), DetectionInterval() {}



};

class UCustomActionComponent;
//class UPerceptionComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActorDetected, AActor*, DetectedActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActorLost, AActor*, LostActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPerceptionStateChanged, bool, bIsPerceptionEnabled);

UCLASS()
class TAREA2DIEGOPROGRA_API UCustomActionSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

protected:

    UPROPERTY()
    TArray<AActor*> RegisteredActors;

    FTimerHandle ActorScanTimerHandle;


public:

    void RegisterComponent(UCustomActionComponent* Component);

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    UFUNCTION(BlueprintCallable)
    void RegisterActor(AActor* Actor);

    UFUNCTION(BlueprintCallable)
    void ScanForActors();


    UFUNCTION(BlueprintCallable)
    void ConfigureDetection(float DetectionRadius, float DetectionInterval);


    UFUNCTION(BlueprintCallable)
    void ConfigurePerception(const FPerceptionInfo& NewPerceptionInfo);


    UFUNCTION(BlueprintCallable)
    void StartDetection();

    UFUNCTION(BlueprintCallable)
    void StopDetection();


    UFUNCTION(BlueprintCallable)
    void EnablePerception();

    UFUNCTION(BlueprintCallable)
    void DisablePerception();


    UFUNCTION(BlueprintCallable, Category = "Perception")
    bool IsPerceptionEnabled() const;

    UFUNCTION(BlueprintCallable, Category = "Perception")
    void InitPerceptionInfo(AActor* Actor, const FPerceptionInfo& PerceptionInfo);


    UFUNCTION(BlueprintCallable, Category = "Detection")
    const TArray<AActor*>& GetDetectedActors() const; 


    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnActorDetected OnActorDetected;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnActorLost OnActorLost;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnPerceptionStateChanged OnPerceptionStateChanged;

    UFUNCTION(BlueprintCallable, Category = "Perception")
    void EnablePerceptionForActor(AActor* Actor);

    UFUNCTION(BlueprintCallable, Category = "Perception")
    void DisablePerceptionForActor(AActor* Actor);

    UFUNCTION(BlueprintCallable, Category = "Perception")
    void EnablePerceptionForActors(const TArray<AActor*>& Actors);

    UFUNCTION(BlueprintCallable, Category = "Perception")
    void DisablePerceptionForActors(const TArray<AActor*>& Actors);

    UFUNCTION(BlueprintCallable, Category = "Perception")
    bool IsActorPerceptionEnabled(AActor* Actor) const;

    float DetectionRadius;
    float DetectionInterval;
private:
    void  DetectActorsreplicate();

    UPROPERTY() TArray<UCustomActionComponent*> RegisteredComponents;

    void DetectActors(); 
    

    bool bIsPerceptionEnabled;

    UPROPERTY() TArray<AActor*> DetectedActors; 


    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception", meta = (AllowPrivateAccess = "true"))
    FPerceptionInfo PerceptionInfo;

    FTimerHandle DetectionTimerHandle; 

    private:
        UPROPERTY()
        TSet<AActor*> ActorsWithPerceptionEnabled;
};
