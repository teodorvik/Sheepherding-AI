#include "SheepherdingAI.h"
#include "Brain.h"

Brain::Brain() {
	for (int i = 0; i < numOutput; i++) {
		currentOutput[i] = -1.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 2.0f)); // Random value between -1 and 1
	}

	for (int i = 0; i < numInput; i++) {
		currentInput[i] = 0.0f;
	}

	for (int i = 0; i < numWeights; i++) {
		weights[i] = 0.0f;
	}
}

Brain::~Brain() {
}

void Brain::SetCurrentInput(float sheepPosX, float sheepPosY, float sheepVelX, float sheepVelY, float goalPosX, float goalPosY) {
	currentInput[0] = sheepPosX;
	currentInput[1] = sheepPosY;
	currentInput[2] = sheepVelX;
	currentInput[3] = sheepVelY;
	currentInput[4] = goalPosX;
	currentInput[5] = goalPosY;
}

float Brain::GetLeftOutput() {
	return currentOutput[0];
}

float Brain::GetRightOutput() {
	return currentOutput[1];
}

float Brain::GetUpOutput() {
	return currentOutput[2];
}

float Brain::GetDownOutput() {
	return currentOutput[3];
}

void Brain::Forward() {
	// Calc new outputs based on input and weights
	for (int i = 0; i < numOutput; i++) {
		float sum = 0.0f;
		for (int j = 0; j < numInput; j++) {
			sum += weights[i*numInput + j] * currentInput[j];
		}

		currentOutput[i] = Activation(sum);
	}
}

float Brain::Activation(float val) {
	return tanh(val);
}

void Brain::CalcFitness() {
	// Calculate distance between goal and herd center
	float sheepPosX = currentInput[0]; //  = sheepPosX;
	float sheepPosY = currentInput[1]; // = sheepPosY;
	float sheepVelX = currentInput[2]; // = sheepVelX;
	float sheepVelY = currentInput[3]; // = sheepVelY;
	float goalPosX = currentInput[4]; // = goalPosX;
	float goalPosY = currentInput[5]; // = goalPosY;

	float dx = goalPosX - sheepPosX;
	float dy = goalPosY - sheepPosY;
	float dist = sqrt(dx*dx + dy*dy);

	// fitness = -1.0f * static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 100.0f));
	fitness = dist*dist;
	UE_LOG(LogTemp, Warning, TEXT("Fitness: %f"), fitness);
}

float Brain::GetFitness() {
	return fitness;
}