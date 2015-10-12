// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <vector>
#include "SheepPawn.h"
#include "SheepherdingAI.h"
#include "GameFramework/Pawn.h"
#include "DogAIPawn.generated.h"

UCLASS()
class SHEEPHERDINGAI_API ADogAIPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ADogAIPawn();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	float HerdSpread();
	float HerdDistanceToGoal();

	UPROPERTY(EditAnywhere)
	USceneComponent* OurVisibleComponent;

	// Store references to all sheep (called from blueprint)
	UFUNCTION(BlueprintCallable, Category = "AIDog")
	void SetSheepArray(UPARAM(ref) TArray<class ASheepPawn*> &sheep);

	// Input functions
	void Move_XAxis(float AxisValue);
	void Move_YAxis(float AxisValue);

	// Input variables
	FVector CurrentVelocity;

	UPROPERTY(EditAnywhere)
	float speed;

	UPROPERTY(EditAnywhere)
	bool useAI;

private:
	FVector herdCenter;
	float herdDistanceWeight = 0.5f;
	float herdSpreadWeight = 0.5f;
	
	TArray<class ASheepPawn*> sheepArray;
};
