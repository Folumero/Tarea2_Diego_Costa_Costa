// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomActionBase.h"
#include "CustomActionComponent.h"

void UCustomActionBase::ExecuteAction(UCustomActionComponent* TargetComponent)
{
    if (TargetComponent)
    {
        UE_LOG(LogTemp, Log, TEXT("Ejecutando acción en: %s"), *TargetComponent->GetName());
        // Aquí puedes definir comportamientos específicos
    }
}

