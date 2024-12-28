#include "CustomActionSubsystem.h"
#include "CustomActionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"


void UCustomActionSubsystem::RegisterComponent(UCustomActionComponent* Component)
{
    if (Component && !RegisteredComponents.Contains(Component))
    {
        RegisteredComponents.Add(Component);
        UE_LOG(LogTemp, Log, TEXT("Componente registrado: %s"), *Component->GetName());
    }
}

void UCustomActionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    DetectedActors.Empty();
    bIsPerceptionEnabled = false;

    // Inicializar los valores 
    PerceptionInfo.DetectionRadius;
    PerceptionInfo.DetectionInterval;


    GetWorld()->AddOnActorSpawnedHandler(FOnActorSpawned::FDelegate::CreateUObject(this, &ThisClass::RegisterActor));
    // Iniciar el escaneo 
    GetWorld()->GetTimerManager().SetTimer(ActorScanTimerHandle, this, &UCustomActionSubsystem::ScanForActors, 1.0f, true);
    UE_LOG(LogTemp, Log, TEXT("CustomActionSubsystem inicializado y escaneando actores."));

}


void UCustomActionSubsystem::Deinitialize()
{
    Super::Deinitialize();

    // Limpiar el temporizador
    GetWorld()->GetTimerManager().ClearTimer(ActorScanTimerHandle);
    UE_LOG(LogTemp, Log, TEXT("CustomActionSubsystem desinicializado."));


}

void UCustomActionSubsystem::RegisterActor(AActor* Actor)
{
    if (Actor)
    {
        UE_LOG(LogTemp, Log, TEXT("Actor detectado: %s"), *Actor->GetName());

        if (UCustomActionComponent* Component = Actor->FindComponentByClass<UCustomActionComponent>())
        {
            UE_LOG(LogTemp, Log, TEXT("Actor tiene componente: %s"), *Component->GetName());

            if (!RegisteredActors.Contains(Actor))
            {
                RegisteredActors.Add(Actor);
                UE_LOG(LogTemp, Log, TEXT("Actor registrado: %s"), *Actor->GetName());
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Actor %s no tiene UCustomActionComponent."), *Actor->GetName());
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Actor nulo pasado a RegisterActor."));
    }
}

void UCustomActionSubsystem::ScanForActors()
{
    TArray<AActor*> AllActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), AllActors);

    for (AActor* Actor : AllActors)
    {
        RegisterActor(Actor);
    }
}


void UCustomActionSubsystem::ConfigureDetection(float Radius, float Interval)
{
    DetectionRadius = Radius;
    DetectionInterval = Interval;
}

void UCustomActionSubsystem::ConfigurePerception(const FPerceptionInfo& NewPerceptionInfo)
{
    PerceptionInfo = NewPerceptionInfo;
    UE_LOG(LogTemp, Log, TEXT("Perception configurada: Radio = %f, Intervalo = %f"), PerceptionInfo.DetectionRadius, PerceptionInfo.DetectionInterval);
}

void UCustomActionSubsystem::StartDetection()
{

    if (!GetWorld()->GetTimerManager().IsTimerActive(DetectionTimerHandle))
    {
        UE_LOG(LogTemp, Log, TEXT("StartDetection: Iniciando detección periódica."));

        // Configurar temporizador global para `DetectActors`
        GetWorld()->GetTimerManager().SetTimer(DetectionTimerHandle, this, &UCustomActionSubsystem::DetectActors, PerceptionInfo.DetectionInterval, true);

        // Configurar temporizador individual basado en cada actor registrado
        for (AActor* ComponentOwner : RegisteredActors)
        {
            if (!ComponentOwner) continue;

            UCustomActionComponent* ActionComponent = ComponentOwner->FindComponentByClass<UCustomActionComponent>();
            if (!ActionComponent) continue;

            float DetectionIntervalFromComponent = ActionComponent->DetectionIntervalInfo;

            FTimerHandle ActorTimerHandle;
            GetWorld()->GetTimerManager().SetTimer(ActorTimerHandle,[this, ComponentOwner](){this->DetectActorsreplicate();},DetectionIntervalFromComponent,true);

            UE_LOG(LogTemp, Log, TEXT("Configurado temporizador para actor: %s con intervalo: %f"),*ComponentOwner->GetName(),DetectionIntervalFromComponent
            );
        }
    }

}

void UCustomActionSubsystem::StopDetection()
{
    if (GetWorld()->GetTimerManager().IsTimerActive(DetectionTimerHandle))
    {
        GetWorld()->GetTimerManager().ClearTimer(DetectionTimerHandle);
        UE_LOG(LogTemp, Log, TEXT("Detección detenida."));
    }
}

void UCustomActionSubsystem::EnablePerception()
{
    if (!bIsPerceptionEnabled)
    {
        bIsPerceptionEnabled = true;

        // Iniciar detección
        StartDetection();

        // Notificar cambio de estado
        OnPerceptionStateChanged.Broadcast(true);
        UE_LOG(LogTemp, Log, TEXT("Percepción activada."));
    }
}

void UCustomActionSubsystem::DisablePerception()
{
    if (!bIsPerceptionEnabled) return;

    bIsPerceptionEnabled = false;

    // Desactivar percepción para todos los actores
    for (AActor* Actor : ActorsWithPerceptionEnabled)
    {
        DisablePerceptionForActor(Actor);
    }

    StopDetection();

    DetectedActors.Empty();

    OnPerceptionStateChanged.Broadcast(false);
    UE_LOG(LogTemp, Log, TEXT("Percepción global desactivada."));
}

bool UCustomActionSubsystem::IsPerceptionEnabled() const
{
    return bIsPerceptionEnabled;
}

void UCustomActionSubsystem::InitPerceptionInfo(AActor* Actor, const FPerceptionInfo& Perception)
{

    if (!Actor)
    {
        UE_LOG(LogTemp, Warning, TEXT("Actor es nulo."));
        return;
    }

    UCustomActionComponent* Component = Actor->FindComponentByClass<UCustomActionComponent>();
    if (Component)
    {
        Component->SetPerceptionInfo(Perception);

        UE_LOG(LogTemp, Log, TEXT("PerceptionInfo inicializado para el actor: %s. Radio: %f, Intervalo: %f"),
            *Actor->GetName(),
            Perception.DetectionRadius,
            Perception.DetectionInterval);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("El actor %s no tiene un UCustomActionComponent."), *Actor->GetName());
    }
}

void UCustomActionSubsystem::DetectActors()
{

    if (!bIsPerceptionEnabled) return;
    UE_LOG(LogTemp, Log, TEXT("PerceptionActivate"));
    TArray<AActor*> CurrentlyDetectedActors;

    // Obtener todos los actores en el mundo
    TArray<AActor*> AllActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), AllActors);

    // Iterar sobre los actores registrados
    for (AActor* ComponentOwner : RegisteredActors)
    {
       
        if (!ComponentOwner) continue;DrawDebugSphere(GetWorld(),ComponentOwner->GetActorLocation(),PerceptionInfo.DetectionRadius,12,FColor::Green,false,0.1f);

        // Verificar actores dentro del radio de percepción
        for (AActor* Actor : AllActors)
        {
            if (Actor && Actor->FindComponentByClass<UCustomActionComponent>() && ComponentOwner != Actor)
            {
                float Distance = FVector::Dist(Actor->GetActorLocation(), ComponentOwner->GetActorLocation());

                if (Distance <= PerceptionInfo.DetectionRadius)
                {
                    CurrentlyDetectedActors.Add(Actor);

                    // Notificar si es un nuevo actor detectado
                    if (!DetectedActors.Contains(Actor))
                    {
                        OnActorDetected.Broadcast(Actor);
                        UE_LOG(LogTemp, Log, TEXT("Actor Detectado: %s por %s"),
                            *Actor->GetName(),
                            *ComponentOwner->GetName());
                    }
                }
            }
        }
    }

    // Notificar actores que ya no están dentro del área
    for (AActor* Actor : DetectedActors)
    {
        if (!CurrentlyDetectedActors.Contains(Actor))
        {
            OnActorLost.Broadcast(Actor);
            UE_LOG(LogTemp, Log, TEXT("Actor Perdido: %s"), *Actor->GetName());
        }
    }

    // Actualizar lista de actores detectados
    DetectedActors = CurrentlyDetectedActors;

    // Depuración adicional para actores detectados
    for (AActor* RegisteredActor : RegisteredActors)
    {
        if (!RegisteredActor) continue;

        FVector RegisteredActorLocation = RegisteredActor->GetActorLocation();

        for (AActor* DetectedActor : CurrentlyDetectedActors)
        {
            if (!DetectedActor || RegisteredActor == DetectedActor) continue;

            float Distance = FVector::Dist(DetectedActor->GetActorLocation(), RegisteredActorLocation);

            if (Distance <= PerceptionInfo.DetectionRadius)
            {
                UE_LOG(LogTemp, Log, TEXT("Actor %s detecta al actor %s dentro del radio."),
                    *RegisteredActor->GetName(),
                    *DetectedActor->GetName());
            }
        }
    }
}

const TArray<AActor*>& UCustomActionSubsystem::GetDetectedActors() const
{
    return DetectedActors;
}
void UCustomActionSubsystem::EnablePerceptionForActor(AActor* Actor)
{
    if (!bIsPerceptionEnabled)
    {
        bIsPerceptionEnabled = true;

        // Iniciar detección
        StartDetection();

        // Notificar cambio de estado
        OnPerceptionStateChanged.Broadcast(true);
        UE_LOG(LogTemp, Log, TEXT("Percepción activada."));

    }

}
void UCustomActionSubsystem::DisablePerceptionForActor(AActor* Actor)
{
    if (!bIsPerceptionEnabled) return;

    bIsPerceptionEnabled = false;

    // Desactivar percepción para todos los actores
    for (AActor* Actor : ActorsWithPerceptionEnabled)
    {
        DisablePerceptionForActor(Actor);
    }

    StopDetection();

    DetectedActors.Empty();

    OnPerceptionStateChanged.Broadcast(false);
    UE_LOG(LogTemp, Log, TEXT("Percepción global desactivada."));

    if (!Actor || !ActorsWithPerceptionEnabled.Contains(Actor)) return;

    ActorsWithPerceptionEnabled.Remove(Actor);

    UE_LOG(LogTemp, Log, TEXT("Percepción desactivada para el actor: %s"), *Actor->GetName());
}
void UCustomActionSubsystem::EnablePerceptionForActors(const TArray<AActor*>& Actors)
{
    for (AActor* Actor : Actors)
    {
        EnablePerceptionForActor(Actor);
    }
}
void UCustomActionSubsystem::DisablePerceptionForActors(const TArray<AActor*>& Actors)
{
    for (AActor* Actor : Actors)
    {
        DisablePerceptionForActor(Actor);
    }

}
bool UCustomActionSubsystem::IsActorPerceptionEnabled(AActor* Actor) const
{
    return Actor && ActorsWithPerceptionEnabled.Contains(Actor);
}

void UCustomActionSubsystem::DetectActorsreplicate()
{
    if (!bIsPerceptionEnabled) return;

    UE_LOG(LogTemp, Log, TEXT("PerceptionActivate"));
    TArray<AActor*> CurrentlyDetectedActors;

    // Obtener todos los actores en el mundo
    TArray<AActor*> AllActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), AllActors);

    // Iterar sobre los actores registrados
    for (AActor* ComponentOwner : RegisteredActors)
    {
        if (!ComponentOwner)
        {
            UE_LOG(LogTemp, Warning, TEXT("Actor registrado nulo."));
            continue;
        }

        // Buscar el componente UCustomActionComponent en el actor registrado
        UCustomActionComponent* ActionComponent = ComponentOwner->FindComponentByClass<UCustomActionComponent>();
        if (!ActionComponent)
        {
            UE_LOG(LogTemp, Warning, TEXT("El actor %s no tiene un componente CustomActionComponent."), *ComponentOwner->GetName());
            continue;
        }

        float CurrentRadius = ActionComponent->DetectionRadiusInfo;

        DrawDebugSphere(GetWorld(),ComponentOwner->GetActorLocation(),CurrentRadius,12,FColor::Green,false,0.1f);

        // Verificar actores dentro del radio de percepción
        for (AActor* Actor : AllActors)
        {
            if (Actor && Actor->FindComponentByClass<UCustomActionComponent>() && ComponentOwner != Actor)
            {
                float Distance = FVector::Dist(Actor->GetActorLocation(), ComponentOwner->GetActorLocation());

                if (Distance <= ActionComponent->DetectionRadiusInfo)
                {
                    CurrentlyDetectedActors.Add(Actor);

                    // Notificar si es un nuevo actor detectado
                    if (!DetectedActors.Contains(Actor))
                    {
                        OnActorDetected.Broadcast(Actor);
                        UE_LOG(LogTemp, Log, TEXT("Actor Detectado: %s por %s"),
                            *Actor->GetName(),
                            *ComponentOwner->GetName());
                    }
                }
            }
        }
    }

    // Notificar actores que ya no están dentro del área
    for (AActor* Actor : DetectedActors)
    {
        if (!CurrentlyDetectedActors.Contains(Actor))
        {
            OnActorLost.Broadcast(Actor);
            UE_LOG(LogTemp, Log, TEXT("Actor Perdido: %s"), *Actor->GetName());
        }
    }

    // Actualizar lista de actores detectados
    DetectedActors = CurrentlyDetectedActors;

    // Depuración adicional para actores detectados
    for (AActor* RegisteredActor : RegisteredActors)
    {
        if (!RegisteredActor) continue;

        // Obtener la posición del actor registrado
        FVector RegisteredActorLocation = RegisteredActor->GetActorLocation();

        // Buscar el componente UCustomActionComponent en el actor registrado
        UCustomActionComponent* ActionComponent = RegisteredActor->FindComponentByClass<UCustomActionComponent>();
        if (!ActionComponent)
        {
            UE_LOG(LogTemp, Warning, TEXT("El actor %s no tiene un componente CustomActionComponent."), *RegisteredActor->GetName());
            continue;
        }

        // Recorrer los actores detectados en esta iteración
        for (AActor* DetectedActor : CurrentlyDetectedActors)
        {
            if (!DetectedActor || RegisteredActor == DetectedActor) continue;

            // Calcular la distancia entre el actor registrado y el actor detectado
            float Distance = FVector::Dist(DetectedActor->GetActorLocation(), RegisteredActorLocation);

            if (Distance <= ActionComponent->DetectionRadiusInfo)
            {
                UE_LOG(LogTemp, Log, TEXT("Actor %s detecta al actor %s dentro del radio."),
                    *RegisteredActor->GetName(),
                    *DetectedActor->GetName());
            }
        }
    }
}
