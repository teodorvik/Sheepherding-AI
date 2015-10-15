// Fill out your copyright notice in the Description page of Project Settings.

#include "SheepherdingAI.h"
#include "DogAIPawn.h"

// Sets default values
ADogAIPawn::ADogAIPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessPlayer = EAutoReceiveInput::Player0;
	
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	OurVisibleComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OurVisibleComponent"));
	OurVisibleComponent->AttachTo(RootComponent);

	useAI = false;
	brain = NULL;

	startLocation = FVector(0.0f, 0.0f, 0.0f);

	UE_LOG(LogTemp, Warning, TEXT("ADogAIPawn::ADogAIPawn()"));
}

// Called when the game starts or when spawned
void ADogAIPawn::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("ADogAIPawn::BeginPlay()"));

	startLocation = GetActorLocation();	

	UE_LOG(LogTemp, Warning, TEXT("ADogAIPawn::BeginPlay() end"));
}

// Called every frame
void ADogAIPawn::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	
	if (!useAI) {
		if (!CurrentVelocity.IsZero())
		{
			//UE_LOG(LogTemp, Warning, TEXT("Delta position: %s"), *((CurrentVelocity * DeltaTime).ToString()));
			FVector NewLocation = GetActorLocation() + (CurrentVelocity * DeltaTime);

			if (!box) {
				//UE_LOG(LogTemp, Warning, TEXT("DogAIPawn: No bounds set"));
				SetActorLocation(NewLocation);
			}
			else if (IsSphereInBounds(NewLocation, 50.0f, box->Bounds)) {
				SetActorLocation(NewLocation);
			}
		}
	}
}

void ADogAIPawn::SetupPlayerInputComponent(class UInputComponent* InputComponent) {
	Super::SetupPlayerInputComponent(InputComponent);

	if (!useAI) {
		InputComponent->BindAxis("MoveX", this, &ADogAIPawn::Move_XAxis);
		InputComponent->BindAxis("MoveY", this, &ADogAIPawn::Move_YAxis);
	}
}

void ADogAIPawn::SetSheepArray(TArray<class ASheepPawn*>& sheep) {
	sheepArray = sheep;
}

void ADogAIPawn::SetBounds(UBoxComponent* &b) {
	box = b;
}

void ADogAIPawn::Move_XAxis(float AxisValue) {
	CurrentVelocity.X = FMath::Clamp(AxisValue, -1.0f, 1.0f) * speed;
}

void ADogAIPawn::Move_YAxis(float AxisValue) {
	CurrentVelocity.Y = FMath::Clamp(AxisValue, -1.0f, 1.0f) * speed;
}

bool ADogAIPawn::IsSphereInBounds(FVector position, float radius, FBoxSphereBounds bounds) {
	return ((position.X > (bounds.Origin.X - bounds.BoxExtent.X + radius)) &&
		(position.X < (bounds.Origin.X + bounds.BoxExtent.X - radius)) &&
		(position.Y >(bounds.Origin.Y - bounds.BoxExtent.Y + radius)) &&
		(position.Y < (bounds.Origin.Y + bounds.BoxExtent.X - radius)));
}

void ADogAIPawn::UpdateAIMovement(float DeltaTime) {
	// Get input
	// All sheep
	// goal
	// distance from goal
	//float spread;
	//spread = HerdSpread();

	float thresh = 0.75;
	float input;

	if (brain) {
		input = brain->GetRightOutput() > thresh ? 1.0f : 0.0f;
		//UE_LOG(LogTemp, Warning, TEXT("GetRightOutput(): %f"), brain->GetRightOutput());
		CurrentVelocity.X = input * speed;
		input = brain->GetLeftOutput() > thresh ? 1.0f : 0.0f;
		//UE_LOG(LogTemp, Warning, TEXT("GetLeftOutput(): %f"), brain->GetLeftOutput());
		CurrentVelocity.X -= input * speed;
		input = brain->GetUpOutput() > thresh ? 1.0f : 0.0f;
		//UE_LOG(LogTemp, Warning, TEXT("GetUpOutput(): %f"), brain->GetUpOutput());
		CurrentVelocity.Y = input * speed;
		input = brain->GetDownOutput() > thresh ? 1.0f : 0.0f;
		//UE_LOG(LogTemp, Warning, TEXT("GetDownOutput(): %f"), brain->GetDownOutput());
		CurrentVelocity.Y -= input * speed;
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("ADogAIPawn: No brain!"));
	}

	//UE_LOG(LogTemp, Warning, TEXT("Delta position: %s"), *((CurrentVelocity * DeltaTime).ToString()));
	FVector NewLocation = GetActorLocation() + (CurrentVelocity * DeltaTime);

	//if (IsSphereInBounds(NewLocation, 50.0f, box->Bounds))
	SetActorLocation(NewLocation);
}

void ADogAIPawn::Reset() {
	SetActorLocation(startLocation);
}

void ADogAIPawn::SetRandomStartLocation(FBoxSphereBounds bounds) {
	float randX = -bounds.Origin.X - bounds.BoxExtent.X + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2.0f * bounds.BoxExtent.X)));
	float randY = -bounds.Origin.Y - bounds.BoxExtent.Y + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2.0f * bounds.BoxExtent.Y)));
	startLocation = FVector(randX, randY, startLocation.Z);
}