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

	UPROPERTY(EditAnywhere)
	UBoxComponent* Box;

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

	TArray<class ASheepPawn*> sheepArray;
	ADogAIPawn* dog;

	FVector Separate(int index);
	FVector Align(int index);
	FVector Cohere(int index);
	FVector DogSeparate(int index);
	FVector SteerTo(int index, FVector target);

	bool IsSphereInBounds(FVector position, float radius, FBoxSphereBounds bounds);

	//void TriggerEnter(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	//void TriggerExit(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	//
};
