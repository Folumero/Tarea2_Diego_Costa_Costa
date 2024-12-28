#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

// Declaración anticipada para evitar dependencias circulares
class UCustomActionComponent;

// La inclusión generada SIEMPRE debe estar al final
#include "CustomActionBase.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class TAREA2DIEGOPROGRA_API UCustomActionBase : public UObject
{
    GENERATED_BODY()

public:
    virtual void ExecuteAction(UCustomActionComponent* TargetComponent);
};
