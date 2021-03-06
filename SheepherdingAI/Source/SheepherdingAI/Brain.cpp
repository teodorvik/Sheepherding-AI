#include "SheepherdingAI.h"
#include "Brain.h"

Brain::Brain() {
	for (int i = 0; i < numOutput; i++) {
		currentOutput[i] = 0.0f;
	}

	for (int i = 0; i < numInput; i++) {
		currentInput[i] = 0.0f;
		currentScaledInput[i] = 0.0f;
	}

	for (int i = 0; i < numWeights; i++) {
		//weights[i] = -1.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 2.0f)); // Random value between -1.0 and 1.0
		//weights[i] = savedWeights[i];
		weights[i] = -0.6f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 1.2f)); // Random value between -0.6 and 0.6
	}

	fitness = 0.0f;
}

Brain::~Brain() {
}

// Set inputs of the neural network. They should be scaled to be in the [-1, 1] range.
void Brain::SetCurrentInput(float sheepDist, float sheepAngle, float goalDist, float goalAngle) {
	currentScaledInput[0] = sheepDist / 2000.0f;
	currentScaledInput[1] = sheepAngle / PI;
	currentScaledInput[2] = goalDist / 2000.0f;
	currentScaledInput[3] = goalAngle / PI;
	currentScaledInput[4] = currentOutput[0];
	currentScaledInput[5] = currentOutput[1];
	//currentScaledInput[10] = currentOutput[2];
	//currentScaledInput[11] = currentOutput[3];
	//currentScaledInput[12] = currentOutput[4];
}

float Brain::GetTurnOutput() {
	return currentOutput[0];
}

float Brain::GetMoveForwardOutput() {
	return currentOutput[1];
}

void Brain::Forward() {
	// Calc new outputs based on input and weights
	for (int i = 0; i < numOutput; i++) {
		float sum = 0.0f;
		for (int j = 0; j < numInput; j++) {
			sum += weights[i*numInput + j] * currentScaledInput[j];
		}

		currentOutput[i] = Activation(sum);
	}
}

float Brain::Activation(float val) {
	return tanh(val);
}

void Brain::CalcFitness(FVector herdPos, FVector goalPos) {
	// Calculate distance between goal and herd center
	float dx = goalPos.X - herdPos.X;
	float dy = goalPos.Y - herdPos.Y;
	float dist = dx*dx + dy*dy;

	// fitness = -1.0f * static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 100.0f));
	fitness += dist;
	//UE_LOG(LogTemp, Warning, TEXT("Fitness: %f"), fitness);
}

void Brain::ResetFitness() {
	fitness = 0.0f;
}

float Brain::GetFitness() {
	return fitness;
}

void Brain::PrintBrainStuff(int index) {
	UE_LOG(LogTemp, Warning, TEXT("Brain: %d: GetTurnOutput: %f"), index, GetTurnOutput());
	UE_LOG(LogTemp, Warning, TEXT("Brain: %d: GetMoveForwardOutput: %f"), index, GetMoveForwardOutput());
}

void Brain::Mutate(float mutationRate, float mutationSize) {
	for (int i = 0; i < numWeights; i++) {
		// Range 0.0 -> 1.0 inclusive
		float randMutation = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		if (randMutation < mutationRate){
			// How much to mutate the selected weight. Range -mutationSize/2 -> mutationSize/2
			float randMutationInRange = (static_cast <float> (rand()) / (static_cast <float> (RAND_MAX)) - 0.5f) * mutationSize * 2.0f;
			//UE_LOG(LogTemp, Warning, TEXT("Mutating: weight before: %f"), weights[i]);
			weights[i] += randMutationInRange;
			//UE_LOG(LogTemp, Warning, TEXT("Mutating: weight after: %f"), weights[i]);

		}
	}
}

void Brain::Crossover(Brain* parent1, Brain* parent2, float cp) {
	int crossoverPoint = floor(cp * numWeights);
	for (int i = 0; i < numWeights; i++) {
		if (i < crossoverPoint) {
			weights[i] = parent1->GetWeight(i);
		}
		else {
			weights[i] = parent2->GetWeight(i);
		}
	}
}

void Brain::CopyWeights(Brain* brain) {
	for (int i = 0; i < numWeights; i++) {
		weights[i] = brain->GetWeight(i);
	}
}

float Brain::GetWeight(int index) {
	return weights[index];
}

void Brain::PrintWeights() {
	FString str = "";
	for (int i = 0; i < numWeights; i++) {
		str += FString::SanitizeFloat(weights[i]);
		if (i < numWeights - 1)
			str += ",";
	}

	UE_LOG(LogTemp, Warning, TEXT("Weight: %s"), *str);
}

void Brain::GenerateNewWeights() {
	for (int i = 0; i < numWeights; i++) {
		weights[i] = -1.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 2.0f)); // Random value between -1.0 and 1.0
	}
}

void Brain::UseSavedWeights() {
	//float savedWeights[numWeights] = { -0.234074, 0.426328, 0.129288, -0.324169, 0.467343, -0.008483, 0.387657, 0.581568, 0.745026, -0.485241, -0.516814, 0.872033 };
	float savedWeights[numWeights] = { -0.006187, 0.452003, -0.117858, -0.334867, 0.299361, 0.160287, 0.522738, 0.016084, 0.467007, 0.023428, -0.048278, 0.328429 };

	for (int i = 0; i < numWeights; i++) {
		weights[i] = savedWeights[i];
	}
}