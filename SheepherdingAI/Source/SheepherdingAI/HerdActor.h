// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <vector>
#include "GameFramework/Actor.h"
#include "HerdActor.generated.h"

UCLASS()
class SHEEPHERDINGAI_API AHerdActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHerdActor();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	/** Get names and positions of all actors */
	UFUNCTION(BlueprintCallable, Category = "Herd")
		void SetSheepArray(UPARAM(ref) TArray<class ASheepPawn*> &sheep);

private:
	const float avoidanceWeight = 1.0;
	const float alignmentWeight = 0.0;
	const float cohesionWeight = 0.5;
	const float maxDistance = 500.0;

	TArray<class ASheepPawn*> sheepArray;
	
};
