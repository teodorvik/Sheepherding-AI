// Fill out your copyright notice in the Description page of Project Settings.

#include "SheepherdingAI.h"
#include "HerdActor.h"


// Sets default values
AHerdActor::AHerdActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a dummy root component we can attach things to.
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	Box->SetRelativeScale3D(FVector(5.0f, 5.0f, 5.0f));
	//Box->bGenerateOverlapEvents = true;
	//Box->OnComponentEndOverlap.AddDynamic(this, &AHerdActor::TriggerExit);
	//Box->OnComponentBeginOverlap.AddDynamic(this, &AHerdActor::TriggerEnter);
	Box->AttachTo(RootComponent);
}

// Called when the game starts or when spawned
void AHerdActor::BeginPlay()
{
	Super::BeginPlay();
	
}

FVector AHerdActor::Separate(int index) {
	FVector separationSum = FVector(0.0f, 0.0f, 0.0f);
	ASheepPawn* sheep = sheepArray[index];

	int count = 0;
	for (int i = 0; i < sheepArray.Num(); i++) {
		if (i != index) {
			// Distance between this sheep and other sheep
			FVector dist = sheep->GetActorLocation() - sheepArray[i]->GetActorLocation();
			float d = dist.Size();
			if (d > 0 && d < desiredSeparation) {
				dist.Normalize();
				separationSum += dist / (d / desiredSeparation);
				count++;
			}
		}
	}
	
	if (count > 0)
		separationSum /= count;

	return separationSum;
}

FVector AHerdActor::Align(int index) {
	FVector meanVelocity = FVector(0.0f, 0.0f, 0.0f);
	ASheepPawn* sheep = sheepArray[index];

	int count = 0;
	for (int i = 0; i < sheepArray.Num(); i++) {
		if (i != index) {
			// Distance between this sheep and other sheep
			FVector dist = sheepArray[i]->GetActorLocation() - sheep->GetActorLocation();
			float d = dist.Size();
			if (d > 0 && d < alignmentDistance) {
				meanVelocity += sheepArray[i]->GetSheepVelocity();
				//UE_LOG(LogTemp, Warning, TEXT("Meanvelocity is %s"), *(meanVelocity.ToString()));
				count++;
			}
		}
	}

	if (count > 0)
		meanVelocity /= count;

	meanVelocity = meanVelocity.GetClampedToMaxSize(maxForce);

	return meanVelocity;

}

FVector AHerdActor::Cohere(int index) {
	FVector distanceSum = FVector(0.0f, 0.0f, 0.0f);
	ASheepPawn* sheep = sheepArray[index];
	int count = 0;

	for (int i = 0; i < sheepArray.Num(); i++) {
		if (i != index) {
			FVector dist = sheepArray[i]->GetActorLocation() - sheep->GetActorLocation();
			float d = dist.Size();
			if (d < cohesionDistance) {
				distanceSum += dist;
				count++;
			}
		}
	}
	
	if (count > 0)
		//return SteerTo(index, distanceSum /= count);
		return distanceSum /= count;
	else
		return distanceSum;
}

FVector AHerdActor::SteerTo(int index, FVector target) {
	FVector pos = sheepArray[index]->GetActorLocation();
	FVector velocity = sheepArray[index]->GetSheepVelocity();

	FVector desired = target - pos;
	float d = desired.Size();
	
	FVector steer;
	if (d > 0) {
		desired.Z = 0.0f;
		desired.Normalize();

		if (d < cohesionDamping) {
			desired *= maxSpeed * (d / cohesionDamping);
		}
		else {
			desired *= maxSpeed;
		}

		steer = desired - velocity;
		steer = steer.GetClampedToMaxSize(maxForce);
	}
	else
		steer = FVector(0.0f, 0.0f, 0.0f);

	return steer;		
}

FVector AHerdActor::DogSeparate(int index) {
	ASheepPawn* sheep = sheepArray[index];
	FVector dist = sheep->GetActorLocation() - dog->GetActorLocation();
	dist.Z = 0.0;
	float d = dist.Size();
	dist.Normalize();

	if (d < dogDistance)
		return dogDistance * dogDistance * dist / (d * d);
	else
		return FVector(0.0f, 0.0f, 0.0f);
}

// Called every frame
void AHerdActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	// Variables
	std::vector<FVector> speedDiff;
	speedDiff.reserve(sheepArray.Num());
	std::vector<FVector> averagePosition;
	averagePosition.reserve(sheepArray.Num());
	std::vector<FVector> separationVector;
	separationVector.reserve(sheepArray.Num());
	std::vector<FVector> dogSeparationVector;
	dogSeparationVector.reserve(sheepArray.Num());

	//For each sheep in sheepArray
	for (int i = 0; i < sheepArray.Num(); i++) {
		ASheepPawn* sheep = sheepArray[i];
		speedDiff[i] = Align(i);
		averagePosition[i] = Cohere(i);
		separationVector[i] = Separate(i);
		dogSeparationVector[i] = DogSeparate(i);
	}

	for (int i = 0; i < sheepArray.Num(); i++) {
		ASheepPawn* sheep = sheepArray[i];

		FVector acceleration = speedDiff[i] * alignmentWeight // Alignment
			+ averagePosition[i] * cohesionWeight // Cohesion
			+ separationVector[i] * separationWeight // Separation
			+ dogSeparationVector[i] * dogSeparationWeight;

		FVector oldVelocity = sheep->GetSheepVelocity();
		FVector newVelocity = oldVelocity + acceleration * DeltaTime;
		newVelocity = newVelocity.GetClampedToMaxSize(maxSpeed);
		//UE_LOG(LogTemp, Warning, TEXT("Acceleration %s"), *acceleration.ToString());

		FVector oldLocation = sheep->GetActorLocation();
		FVector newLocation = oldLocation + newVelocity * DeltaTime;
			
		if (IsSphereInBounds(newLocation, 50.0f, Box->Bounds)) {
			sheepArray[i]->SetActorLocation(newLocation);
		}
		sheepArray[i]->SetSheepVelocity(newVelocity);

		//UE_LOG(LogTemp, Warning, TEXT("Sheep's old location is %s"), *(oldLocation.ToString()));
		//UE_LOG(LogTemp, Warning, TEXT("Sheep's new location is %s"), *(newLocation.ToString()));
		//UE_LOG(LogTemp, Warning, TEXT("Sheep's avoidanceVec is %s"), *(separationVector[i].ToString()));
	}

	GEngine->ClearOnScreenDebugMessages();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Alignment: " + (speedDiff[0] * alignmentWeight).ToString()));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Cohesion: " + (averagePosition[0] * cohesionWeight).ToString()));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Separation: " + (separationVector[0] * separationWeight).ToString()));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Dog separation: " + (dogSeparationVector[0] * dogSeparationWeight).ToString()));

	//UE_LOG(LogTemp, Warning, TEXT("Box: %s"), *(Box->GetScaledBoxExtent().ToString()));
}

void AHerdActor::SetSheepArray(TArray<class ASheepPawn*>& sheep){
	sheepArray = sheep;
}

void AHerdActor::SetDog(class ADogAIPawn* &d) {
	dog = d;
}

bool AHerdActor::IsSphereInBounds(FVector position, float radius, FBoxSphereBounds bounds) {
	return ((position.X > (bounds.Origin.X - bounds.BoxExtent.X + radius)) && 
		(position.X < (bounds.Origin.X + bounds.BoxExtent.X - radius)) &&
		(position.Y > (bounds.Origin.Y - bounds.BoxExtent.Y + radius)) &&
		(position.Y < (bounds.Origin.Y + bounds.BoxExtent.X - radius)));			
}

//void AHerdActor::TriggerEnter(class AActor* otherActor, class UPrimitiveComponent* otherComp, int32 otherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) {
//	UE_LOG(LogTemp, Warning, TEXT("TriggerEnter!!"));
//}
//
//void AHerdActor::TriggerExit(class AActor* otherActor, class UPrimitiveComponent* otherComp, int32 otherBodyIndex) {
//	UE_LOG(LogTemp, Warning, TEXT("TriggerExit!!"));
//}

