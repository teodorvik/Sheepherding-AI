// Fill out your copyright notice in the Description page of Project Settings.

#include "SheepherdingAI.h"
#include "HerdClass.h"

HerdClass::HerdClass()
{
}

HerdClass::~HerdClass()
{
}


void HerdClass::AllActors(TArray<class ASheepPawn*>& sheepArray){
	UE_LOG(LogTemp, Warning, TEXT("ALLACTORS FOR FUCK SAKKEEEE!!!!!KK!KO!IKI"));

	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Entered AllActors"));
	for (int i = 0; i < sheepArray.Num(); i++) {
		ASheepPawn* sheep = sheepArray[i];
		FVector pos = sheep->GetActorLocation();

		// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("THIS IS AN ON SCREEN MESSAEG!"));
		UE_LOG(LogTemp, Warning, TEXT("Sheep's Location is df"));
	}
}

FVector HerdClass::getCenter(){
	FVector center = FVector(0,0,0);
	/*
	foreach(var sheep in sheepVector) {
		center += sheep.getPosition();
	}
	center = center / sheepVector.Length();*/
	return center;
}
