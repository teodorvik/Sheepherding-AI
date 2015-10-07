// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "SheepPawn.generated.h"

UCLASS()
class SHEEPHERDINGAI_API ASheepPawn : public APawn
{
	GENERATED_BODY()
public:
	// Sets default values for this pawn's properties
	ASheepPawn();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	UPROPERTY(EditAnywhere)
	USceneComponent* OurVisibleComponent;

	void SetSheepVelocity(FVector vel);
	FVector GetSheepVelocity();

private:
	FVector velocity;
	
};
