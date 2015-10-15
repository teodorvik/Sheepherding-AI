// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <vector>

#include "SheepPawn.h"
#include "DogAIPawn.h"
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

	// Store references to all sheep (called from blueprint)
	UFUNCTION(BlueprintCallable, Category = "Herd")
	void SetSheepArray(UPARAM(ref) TArray<class ASheepPawn*> &sheep);

	UFUNCTION(BlueprintCallable, Category = "Herd")
	void SetDog(UPARAM(ref) class ADogAIPawn* &dog);

	// How can I get these pointers directly in the blueprint instead of having to call GetFenceBox()?
	UPROPERTY(EditAnywhere)
	UBoxComponent* fenceBox;

	UPROPERTY(EditAnywhere)
	UBoxComponent* goalBox;

	void SetGoalCenter();

	UFUNCTION(BlueprintCallable, Category = "Herd")
	UBoxComponent* GetFenceBox();

	TArray<class ASheepPawn*> GetSheepArray();

private:
	// Set flocking properties from the editor
	UPROPERTY(EditAnywhere)
	float separationWeight;
	UPROPERTY(EditAnywhere)
	float alignmentWeight;
	UPROPERTY(EditAnywhere)
	float cohesionWeight;
	UPROPERTY(EditAnywhere)
	float cohesionDistance;
	UPROPERTY(EditAnywhere)
	float cohesionDamping;
	UPROPERTY(EditAnywhere)
	float desiredSeparation;
	UPROPERTY(EditAnywhere)
	float alignmentDistance;
	UPROPERTY(EditAnywhere)
	float maxForce;
	UPROPERTY(EditAnywhere)
	float maxSpeed;
	UPROPERTY(EditAnywhere)
	float dogDistance;
	UPROPERTY(EditAnywhere)
	float dogSeparationWeight;

	FVector2D goalCenter;

	// Learning
	bool isTraining;
	int currentGeneration;
	UPROPERTY(EditAnywhere)
	int32 maxGenerations;
	UPROPERTY(EditAnywhere)
	int32 population; // The number of random networks to try out
	UPROPERTY(EditAnywhere)
	float mutationRate; // The probability that a weight will get mutated
	UPROPERTY(EditAnywhere)
	float elitePercentage; // How many percentage will be kept each generation
	UPROPERTY(EditAnywhere)
	float mutationSize; // The stdev of the noise added when mutating

	float currentTime;
	UPROPERTY(EditAnywhere)
	float maxTime;

	bool isSheepArraySet;
	TArray<class ASheepPawn*> sheepArray;
	ADogAIPawn* dog;
	std::vector<Brain*> brains;

	// Flocking stuff
	FVector Separate(int index);
	FVector Align(int index);
	FVector Cohere(int index);
	FVector DogSeparate(int index);
	FVector SteerTo(int index, FVector target);
	void UpdateFlocking(float DeltaTime);

	bool IsSphereInBounds(FVector position, float radius, FBoxSphereBounds bounds);
	bool AreAllSheepInGoal();
	void Reset();
};
