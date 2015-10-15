// Fill out your copyright notice in the Description page of Project Settings.

#include "SheepherdingAI.h"
#include "HerdActor.h"
#include "Brain.h"

#include <algorithm>

bool CompareBrain(Brain* b1, Brain* b2)
{
	return b1->GetFitness() < b2->GetFitness();
}

// Sets default values
AHerdActor::AHerdActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a dummy root component we can attach things to.
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	fenceBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	fenceBox->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	fenceBox->AttachTo(RootComponent);

	goalBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent2"));
	goalBox->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	goalBox->AttachTo(RootComponent);

	// Flocking parameters
	separationWeight = 150.0f;
	alignmentWeight = 0.5f;
	cohesionWeight = 0.5f;
	cohesionDistance = 600.0f;
	cohesionDamping = 1000.0f;
	desiredSeparation = 150.0f;
	alignmentDistance = 600.0f;
	maxForce = 5000.0f;
	maxSpeed = 200.0f;
	sheepFriction = 0.9f;
	dogDistance = 600.0f;
	dogSeparationWeight = 100.0f;

	// Evolution parameters
	maxGenerations = 5;
	showGenerationInterval = 50;
	population = 1;
	mutationRate = 0.1f;
	elitePercentage = 0.2f;
	mutationSize = 0.02f;

	currentGeneration = 1;
	isTraining = true;
	currentTime = 0.0f;
	maxTime = 10.0f;
	fakeDeltaTime = 1.0f / 30.0f;

	dog = NULL;
	isSheepArraySet = false;

	UE_LOG(LogTemp, Warning, TEXT("HerdActor::HerdActor()"));
}

// Called when the game starts or when spawned
void AHerdActor::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("HerdActor::BeginPlay()"));
	
	// Set center of goal
	SetGoalCenter();

	for (int i = 0; i < population; i++) {
		brains.push_back(new Brain());
	}
}

// Called every frame
void AHerdActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	// Check if pointers are set
	bool error = false;
	if (!dog) {
		UE_LOG(LogTemp, Error, TEXT("HerdActor: Dog not set"));
		error = true;
	}

	if (!isSheepArraySet) {
		UE_LOG(LogTemp, Error, TEXT("HerdActor: Sheeparray not set"));
		error = true;
	}

	if (sheepArray.Num() == 0) {
		UE_LOG(LogTemp, Warning, TEXT("HerdActor: No sheep!"));
		//error = true;
	}

	if (error) return;

	// Print stuff
	GEngine->ClearOnScreenDebugMessages();

	if (!dog->useAI)
		isTraining = false;

	FString trainString = isTraining ? "true" : "false";

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("DeltaTime: " + FString::SanitizeFloat(fakeDeltaTime)));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Is training: " + trainString));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Current generation: " + FString::FromInt(currentGeneration)));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Max generations: " + FString::FromInt(maxGenerations)));

	if (dog->brain)
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Best fitness: " + FString::SanitizeFloat(dog->brain->GetFitness())));

	// Training:
	//
	// Go through all population and update positions of dog and sheep for each (still in the same game loop "tick",
	// so nothing will be seen on the screen.
	//
	// Calculate fitness value for all dogs in the population.
	//
	// Set the best dog/brain (with the best fitness value) and set isTraining = false
	// to show off the best from this generation in the game window.
	//
	// Crossover and mutations and other evolution stuff should be performed before moving to the next generation
	//

	//UE_LOG(LogTemp, Warning, TEXT("IM GONNA TRAIN!"));
	if (isTraining && dog->useAI) {
		for (int i = 0; i < population; i++) {
			//// Give the dog a bone... I mean brain
			dog->brain = brains[i];

			// Update until done
			while (!(AreAllSheepInGoal() || currentTime > maxTime)) {
				//UE_LOG(LogTemp, Warning, TEXT("Training!"));
				//FVector dogPos = dog->GetActorLocation();
				//UE_LOG(LogTemp, Warning, TEXT("Dog's location is %s"), *(dogPos.ToString()));

				// Set neural network inputs
				FVector dogPos = dog->GetActorLocation();
				FVector herdPos = GetHerdCenter();
				FVector herdVelocity = GetHerdVelocity();
				brains[i]->SetCurrentInput(herdPos.X, herdPos.Y, herdVelocity.X, herdVelocity.Y, goalCenter.X, goalCenter.Y, dogPos.X, dogPos.Y);

				// Feed forward, i.e. calculate new outputs from the inputs
				brains[i]->Forward();

				// Update dog movement. Will use outputs from the neural network
				//UE_LOG(LogTemp, Warning, TEXT("UpdateAIMovement: %d"), i);
				dog->UpdateAIMovement(fakeDeltaTime);

				// Update sheep movement
				UpdateFlocking(fakeDeltaTime);

				currentTime += fakeDeltaTime;
			}

			FVector dogPos = dog->GetActorLocation();
			//UE_LOG(LogTemp, Warning, TEXT("Dog's location is %s"), *(dogPos.ToString()));
			FVector herdPos = GetHerdCenter();
			FVector herdVelocity = GetHerdVelocity();

			// Todo: Calculate fitness value
			//brains[i]->SetCurrentInput(herdPos.X - dogPos.X, herdPos.Y - dogPos.Y, herdVelocity.X, herdVelocity.Y, goalCenter.X - dogPos.X, goalCenter.Y - dogPos.Y);
			brains[i]->CalcFitness(herdPos, goalCenter);

			Reset();
		}

		// Sort the brain vector based on fitness
		std::sort(brains.begin(), brains.end(), CompareBrain);

		// Set the dog's brain to the one with the best fitness
		dog->brain = brains[0];

		if (currentGeneration % showGenerationInterval == 0) {
			for (int i = 0; i < brains.size(); i++) {
				UE_LOG(LogTemp, Warning, TEXT("Brain #%d fitness: %f sorted"), i, brains[i]->GetFitness());
			}

			isTraining = false;
		}

		// Keep the best brains and replace the remaing brains with a random copy of the best brains
		int selectedPop = floor((float)population * elitePercentage); // Calc selected pop
		
		for (int i = selectedPop; i < brains.size(); i++){
			// random int in range 0 -> selectedPop
			int randInRange = rand() % (int)(selectedPop + 1);
			brains[i]->CopyWeights(brains[randInRange]);
			// Mutate the new brain
			// float mutationRate; - The probability that a weight will get mutated
			// float mutationSize; - The stdev of the noise added when mutating
			brains[i]->Mutate(mutationRate, mutationSize);
		}

		if (isTraining) {
			//dog->SetRandomStartLocation(fenceBox->Bounds);
			currentGeneration++;
		}
	}
	// If not training:
	// Show off the best dog from the current generation. Then train some more!
	else {
		
		if (dog->useAI && dog->brain) {
			// Set neural network inputs
			FVector dogPos = dog->GetActorLocation();
			FVector herdPos = GetHerdCenter();
			FVector herdVelocity = GetHerdVelocity();
			dog->brain->SetCurrentInput(herdPos.X, herdPos.Y, herdVelocity.X, herdVelocity.Y, goalCenter.X, goalCenter.Y, dogPos.X, dogPos.Y);

			// Feed forward, i.e. calculate new outputs from the inputs
			dog->brain->Forward();
		}

		dog->UpdateAIMovement(fakeDeltaTime);
		UpdateFlocking(fakeDeltaTime);

		if (AreAllSheepInGoal() || currentTime > maxTime) {
			FVector herdPos = GetHerdCenter();
			if (dog->useAI && dog->brain) {
				dog->brain->CalcFitness(herdPos, goalCenter);
				UE_LOG(LogTemp, Warning, TEXT("Best brain fitness: %f"), dog->brain->GetFitness());
				//UE_LOG(LogTemp, Warning, TEXT("Best brain wieghts:"));
				//dog->brain->PrintWeights();
			}

			//dog->SetRandomStartLocation(fenceBox->Bounds);
			currentGeneration++;

			Reset();
			isTraining = true;
		}
		currentTime += fakeDeltaTime;
	}

	//UE_LOG(LogTemp, Warning, TEXT("Box: %s"), *(Box->GetScaledBoxExtent().ToString()));	
}

void AHerdActor::SetSheepArray(TArray<class ASheepPawn*>& sheep){
	UE_LOG(LogTemp, Warning, TEXT("AHerdActor::SetSheepArray"));
	sheepArray = sheep;
	isSheepArraySet = true;
}

void AHerdActor::SetDog(class ADogAIPawn* &d) {
	UE_LOG(LogTemp, Warning, TEXT("Set dog"));
	dog = d;
}

TArray<class ASheepPawn*> AHerdActor::GetSheepArray() {
	return sheepArray;
}

UBoxComponent* AHerdActor::GetFenceBox() {
	return fenceBox;
}

void AHerdActor::Reset() {
	//For each sheep in sheepArray
	for (int i = 0; i < sheepArray.Num(); i++) {
		sheepArray[i]->Reset();
	}

	if (!dog) {
		//UE_LOG(LogTemp, Error, TEXT("Dog not set"));
	}
	else {
		dog->Reset();
	}

	currentTime = 0.0f;
}

// Used for sheep flocking
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

	if (separationSum.Size() > 0.0f) {
		separationSum.Normalize();
		separationSum *= maxSpeed;
		separationSum -= sheep->GetSheepVelocity();
		separationSum = separationSum.GetClampedToMaxSize(maxForce);
	}
	
	return separationSum;
}

// Used for sheep flocking
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

	if (count > 0) {
		meanVelocity /= count;


		//meanVelocity.Normalize();
		//meanVelocity *= maxSpeed;
		FVector steer = meanVelocity - sheep->GetSheepVelocity();
		steer = steer.GetClampedToMaxSize(maxForce);
		return steer;
	}
	else {
		return FVector(0.0f, 0.0f, 0.0f);
	}
}

// Used for sheep flocking
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
		distanceSum /= count;

	distanceSum = distanceSum.GetClampedToMaxSize(maxForce);

	return distanceSum;

}

// Used for sheep flocking
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

// Used for sheep flocking
FVector AHerdActor::DogSeparate(int index) {
	ASheepPawn* sheep = sheepArray[index];
	FVector dist = sheep->GetActorLocation() - dog->GetActorLocation();
	dist.Z = 0.0;
	float d = dist.Size();
	dist.Normalize();

	FVector separation = FVector(0.0f, 0.0f, 0.0f);
	if (d < dogDistance)
		 separation = dogDistance * dogDistance * dist / (d * d);
	
	//separation = separation.GetClampedToMaxSize(maxForce);

	return separation;
}

// Update sheep flocking behaviour
void AHerdActor::UpdateFlocking(float DeltaTime) {
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

		acceleration = acceleration.GetClampedToMaxSize(maxForce);

		//UE_LOG(LogTemp, Warning, TEXT("Sheep %d acceleration: %s"), i, *(acceleration.ToString()));
		//UE_LOG(LogTemp, Warning, TEXT("Sheep %d alignment: %s"), i, *((speedDiff[i] * alignmentWeight).ToString()));
		//UE_LOG(LogTemp, Warning, TEXT("Sheep %d cohesion: %s"), i, *((averagePosition[i] * cohesionWeight).ToString()));
		//UE_LOG(LogTemp, Warning, TEXT("Sheep %d separataion: %s"), i, *((separationVector[i] * separationWeight).ToString()));
		//UE_LOG(LogTemp, Warning, TEXT("Sheep %d sdogsepat: %s"), i, *((dogSeparationVector[i] * dogSeparationWeight).ToString()));

		FVector oldVelocity = sheep->GetSheepVelocity();
		FVector newVelocity = (oldVelocity + acceleration * DeltaTime) * sheepFriction;
		newVelocity = newVelocity.GetClampedToMaxSize(maxSpeed);
		//UE_LOG(LogTemp, Warning, TEXT("Acceleration %s"), *acceleration.ToString());

		FVector oldLocation = sheep->GetActorLocation();
		FVector newLocation = oldLocation + newVelocity * DeltaTime;

		//if (IsSphereInBounds(newLocation, 50.0f, fenceBox->Bounds)) {
		sheepArray[i]->SetActorLocation(newLocation);
		//}
		sheepArray[i]->SetSheepVelocity(newVelocity);

		//UE_LOG(LogTemp, Warning, TEXT("Sheep's old location is %s"), *(oldLocation.ToString()));
		//UE_LOG(LogTemp, Warning, TEXT("Sheep's new location is %s"), *(newLocation.ToString()));
		//UE_LOG(LogTemp, Warning, TEXT("Sheep's avoidanceVec is %s"), *(separationVector[i].ToString()));
	}

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Alignment: " + (speedDiff[0] * alignmentWeight).ToString()));
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Cohesion: " + (averagePosition[0] * cohesionWeight).ToString()));
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Separation: " + (separationVector[0] * separationWeight).ToString()));
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Dog separation: " + (dogSeparationVector[0] * dogSeparationWeight).ToString()));
}

FVector AHerdActor::GetHerdCenter() {
	FVector sum = FVector(0.0f, 0.0f, 0.0f);

	for (int i = 0; i < sheepArray.Num(); i++) {
		sum += sheepArray[i]->GetActorLocation();
	}

	if (sheepArray.Num() > 0)
		return sum / sheepArray.Num();
	else
		return sum;
}

FVector AHerdActor::GetHerdVelocity() {
	FVector sum = FVector(0.0f, 0.0f, 0.0f);

	for (int i = 0; i < sheepArray.Num(); i++) {
		sum += sheepArray[i]->GetSheepVelocity();
	}

	if (sheepArray.Num() > 0)
		return sum / sheepArray.Num();
	else
		return sum;
}

// Get how much the herd is seperated
float AHerdActor::GetHerdSpread() {
	// Variables
	std::vector<float> distance(sheepArray.Num());
	//distance.reserve(sheepArray.Num());

	// Calculate distance from one sheep to all the others
	// Sum and return
	float dist = 0.0f;
	for (int i = 0; i < sheepArray.Num(); i++) {
		distance[i] = 0.0f;
		ASheepPawn* sheep = sheepArray[i];
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

	return distanceTotal;
}

bool AHerdActor::IsSphereInBounds(FVector position, float radius, FBoxSphereBounds bounds) {
	return ((position.X > (bounds.Origin.X - bounds.BoxExtent.X + radius)) && 
		(position.X < (bounds.Origin.X + bounds.BoxExtent.X - radius)) &&
		(position.Y > (bounds.Origin.Y - bounds.BoxExtent.Y + radius)) &&
		(position.Y < (bounds.Origin.Y + bounds.BoxExtent.X - radius)));			
}

bool AHerdActor::AreAllSheepInGoal() {
	for (int i = 0; i < sheepArray.Num(); i++) {
		ASheepPawn* sheep = sheepArray[i];
		if (!IsSphereInBounds(sheep->GetActorLocation(), 50.0f, goalBox->Bounds)) {
			return false;
		}
	}

	return true;
}

void AHerdActor::SetGoalCenter() {
	FBoxSphereBounds bounds = goalBox->Bounds;

	float centerX = bounds.Origin.X + bounds.BoxExtent.X / 2.0f;
	float centerY = bounds.Origin.Y + bounds.BoxExtent.Y / 2.0f;

	goalCenter = FVector(centerX, centerY, 0.0f);
}
