// Fill out your copyright notice in the Description page of Project Settings.

#include "SheepherdingAI.h"
#include "HerdActor.h"

#include "SheepPawn.h"


// Sets default values
AHerdActor::AHerdActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AHerdActor::BeginPlay()
{
	Super::BeginPlay();
	
}

FVector Avoid(FVector diff){
	return FVector(1.f, 1.f, 1.f)/(diff*diff);
}

// Called every frame
void AHerdActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	// Variables
	std::vector<FVector> speedDiff;
	speedDiff.reserve(sheepArray.Num());
	std::vector<FVector> averagePos;
	averagePos.reserve(sheepArray.Num());
	std::vector<FVector> avoidanceVec;
	avoidanceVec.reserve(sheepArray.Num());

	UE_LOG(LogTemp, Warning, TEXT("Number of sheepos: %d"), sheepArray.Num());

	//For each sheep in sheepArray
	for (int i = 0; i < sheepArray.Num(); i++) {
		int count = 0;
		speedDiff[i] = FVector(0.f, 0.f, 0.f);
		averagePos[i] = FVector(0.f, 0.f, 0.f);
		avoidanceVec[i] = FVector(0.f, 0.f, 0.f);

		// Our precious sheep
		ASheepPawn* sheep = sheepArray[i];
		
		// Calculate nearby sheeps
		for (int j = 0; j < sheepArray.Num(); j++) {
			// Compare with another sheep
			ASheepPawn* otherSheep = sheepArray[j];

			if (i != j) {
				FVector diff = otherSheep->GetActorLocation() - sheep->GetActorLocation();
				if (diff.Size() < maxDistance) {
					speedDiff[i] += otherSheep->GetVelocity();
					averagePos[i] += diff;
					avoidanceVec[i] += Avoid(diff);
					count++;
				}
			}

			if (count > 0) {
				speedDiff[i] /= count;
				averagePos[i] /= count;
				avoidanceVec[i] /= count;
			}
		}
	}

	for (int i = 0; i < sheepArray.Num(); i++) {
		ASheepPawn* sheep = sheepArray[i];

		FVector oldVelocity = sheep->GetVelocity();
		FVector oldLocation = sheep->GetActorLocation();
		FVector newVelocity = oldVelocity 
			+ speedDiff[i] * alignmentWeight
			+ averagePos[i] * cohesionWeight
			+ avoidanceVec[i] * avoidanceWeight;
		FVector newLocation = oldLocation + newVelocity * DeltaTime;
		sheepArray[i]->SetActorLocation(newLocation);
		// UE_LOG(LogTemp, Warning, TEXT("Sheep's Location is %s"), *(newLocation.ToString()));

	}
}

void AHerdActor::SetSheepArray(TArray<class ASheepPawn*>& sheep){

	sheepArray = sheep;
	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Entered AllActors"));
	//for (int i = 0; i < sheepArray.Num(); i++) {
	//	ASheepPawn* sheep = sheepArray[i];
	//	FVector pos = sheep->GetActorLocation();

	//	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("THIS IS AN ON SCREEN MESSAEG!"));
	//	UE_LOG(LogTemp, Warning, TEXT("Sheep's Location is %s"), *(pos.ToString()));
	//}
}