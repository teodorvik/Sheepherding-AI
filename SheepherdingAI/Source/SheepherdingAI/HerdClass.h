// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "SheepPawn.h"
#include <vector>

/**
 * 
 */
class SHEEPHERDINGAI_API HerdClass
{
public:
	HerdClass();
	~HerdClass();
	FVector getCenter();

	/** Get names and positions of all actors */
	UFUNCTION(BlueprintCallable, Category = "Herd")
		void AllActors(UPARAM(ref) TArray<class ASheepPawn*> &sheep);

private:
	std::vector<ASheepPawn*> sheepVector;
};
