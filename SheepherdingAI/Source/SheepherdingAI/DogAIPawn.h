// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <vector>
#include "Brain.h"
#include "SheepPawn.h"
#include "SheepherdingAI.h"
#include "GameFramework/Pawn.h"
#include "DogAIPawn.generated.h"

UCLASS()
class SHEEPHERDINGAI_API ADogAIPawn : public APawn
{
	GENERATED_BODY()

public:
	ADogAIPawn();

	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	void Move_XAxis(float AxisValue);
	void Move_YAxis(float AxisValue);

	void UpdateAIMovement(float DeltaTime);
	void Reset();

	UFUNCTION(BlueprintCallable, Category = "AIDog")
	void SetSheepArray(UPARAM(ref) TArray<class ASheepPawn*> &sheep);
	UFUNCTION(BlueprintCallable, Category = "AIDog")
	void SetBounds(UPARAM(ref) class UBoxComponent* &box);

	FVector CurrentVelocity;
	Brain* brain;
	bool isTraining;

	UPROPERTY(EditAnywhere)
	float speed;
	UPROPERTY(EditAnywhere)
	bool useAI;
	UPROPERTY(EditAnywhere)
	USceneComponent* OurVisibleComponent;

private:
	FVector startLocation;
	FVector herdCenter;
	float herdDistanceWeight = 0.5f;
	float herdSpreadWeight = 0.5f;
	TArray<class ASheepPawn*> sheepArray;
	UBoxComponent* box;

	float HerdSpread();
	float HerdDistanceToGoal();
	bool IsSphereInBounds(FVector position, float radius, FBoxSphereBounds bounds);
};
