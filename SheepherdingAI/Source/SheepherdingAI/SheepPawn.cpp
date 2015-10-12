// Fill out your copyright notice in the Description page of Project Settings.

#include "SheepherdingAI.h"
#include "SheepPawn.h"


// Sets default values
ASheepPawn::ASheepPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a dummy root component we can attach things to.
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	OurVisibleComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OurVisibleComponent"));
	//OurVisibleComponent->bGenerateOverlapEvents = true;
	OurVisibleComponent->AttachTo(RootComponent);

	velocity = FVector(0.f, 0.f, 0.f);
}

FVector ASheepPawn::GetSheepVelocity() {
	return velocity;
}

void ASheepPawn::SetSheepVelocity(FVector vel) {
	velocity = vel;
}

// Called when the game starts or when spawned
void ASheepPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASheepPawn::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

// Called to bind functionality to input
void ASheepPawn::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

}

