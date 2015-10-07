// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "SheepController.generated.h"

/**
 * 
 */
UCLASS()
class SHEEPHERDINGAI_API ASheepController : public AAIController
{
	GENERATED_BODY()
public:
	FVector getPosition();

private:
	FVector position;
	FVector velocity;
};
