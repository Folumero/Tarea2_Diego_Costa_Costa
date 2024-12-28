#include "CustomActionComponent.h"
#include "CustomActionSubsystem.h"



UCustomActionComponent::UCustomActionComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    DetectionRadiusInfo;
    DetectionIntervalInfo;  
}

float UCustomActionComponent::GetDetectionRadius()const
{
    return DetectionRadiusInfo;
}

void UCustomActionComponent::BeginPlay()
{
    Super::BeginPlay();

    // Registrar el componente en el subsistema
    if (UWorld* World = GetWorld())
    {
        if (UCustomActionSubsystem* Subsystem = World->GetSubsystem<UCustomActionSubsystem>())
        {
            Subsystem->RegisterComponent(this);
        }
    }
}

void UCustomActionComponent::ReceiveParameter(const FString& ParameterName, const FString& ParameterValue)
{
    UE_LOG(LogTemp, Log, TEXT("Componente %s recibió: %s = %s"), *GetName(), *ParameterName, *ParameterValue);
    OnParameterReceived.Broadcast(ParameterName, ParameterValue);
}

void UCustomActionComponent::SetPerceptionInfo(const FPerceptionInfo& NewInfo)
{

    // Actualiza los valores de percepción
    DetectionRadiusInfo = NewInfo.DetectionRadius;
    DetectionIntervalInfo = NewInfo.DetectionInterval;

    UE_LOG(LogTemp, Log, TEXT("PerceptionInfo aplicado. Radio: %f, Intervalo: %f"),
        DetectionRadiusInfo,
        DetectionIntervalInfo);

    
}
