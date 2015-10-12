// Fill out your copyright notice in the Description page of Project Settings.

#include "SheepherdingAI.h"
#include "DogAIPawn.h"


// Sets default values
ADogAIPawn::ADogAIPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	OurVisibleComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OurVisibleComponent"));
	OurVisibleComponent->AttachTo(RootComponent);
}

// Called when the game starts or when spawned
void ADogAIPawn::BeginPlay()
{
	Super::BeginPlay();
	
	// random weights 
	// herdDistanceWeight;
	// herdSpreadWeight;
}

// Called every frame
void ADogAIPawn::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	
	// Get input
	// All sheep
	// goal
	// distance from goal
	float spread;	
	spread = HerdSpread();

	// UE_LOG(LogTemp, Warning, TEXT("penis: " + herdCenter.ToString()));

}

// Get how much the herd is seperated
float ADogAIPawn::HerdSpread() {	
	// Variables
	std::vector<float> distance(sheepArray.Num());
	//distance.reserve(sheepArray.Num());

	FVector center = FVector(0.0f, 0.0f, 0.0f);

	// Calculate distance from one sheep to all the others
	// Sum and return
	float dist = 0.0f;
	for (int i = 0; i < sheepArray.Num(); i++) {
		distance[i] = 0.0f;
		ASheepPawn* sheep = sheepArray[i];
		center += sheep->GetActorLocation();
		// 

		for (int j = 0; j < sheepArray.Num(); j++) {
			if (i != j) {
				FVector distVec = sheepArray[j]->GetActorLocation() - sheep->GetActorLocation();
				dist += distVec.Size();
			}			
		}
		distance[i] = dist;
	}
	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Sheeplocation: " + distance.size())); // FString::SanitizeFloat()

	//UE_LOG(LogTemp, Warning, TEXT("Dist: %d"), distance.size());
	float distanceTotal = 0.0f;
	for (int i = 0; i < distance.size(); i++) {
		distanceTotal += distance[i];

	}
	
	// Calculate center of the herd and update private variable. 
	herdCenter = center/sheepArray.Num();

	return distanceTotal;
}

// Herd distance from goal
float ADogAIPawn::HerdDistanceToGoal() {
	// Calculate center of herd to center of goal
	float dist = herdCenter.Size();
	return dist;
}

// Called to bind functionality to input
void ADogAIPawn::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

}

void ADogAIPawn::SetSheepArray(TArray<class ASheepPawn*>& sheep){
	sheepArray = sheep;
}
