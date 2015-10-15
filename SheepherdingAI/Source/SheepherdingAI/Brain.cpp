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

	float savedWeights[8 * 4] = { -0.971191f, -0.435713f, 0.103305f, 0.907848f, 0.013398f,
		0.904172f, -0.692312f, -0.408124f, 0.751945f, -0.251645f,
		0.396253f, 0.491375f, -0.873898f, 0.379559f, 0.925474f,
		-0.816645f, -0.930395f, 0.29731f, 0.81225f, -0.014801f,
		0.504051f, 0.341801f, -0.651357f, -0.296364f, 0.640154f,
		-0.008679f, 0.156167f, 0.339674f, -0.02877f, 0.828425f,
		0.259186f, -0.506021f };

	for (int i = 0; i < numWeights; i++) {
		weights[i] = -1.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 2.0f)); // Random value between -1.0 and 1.0
	}
}

Brain::~Brain() {
}

// Set inputs of the neural network. They should be scaled to be in the [-1, 1] range.
void Brain::SetCurrentInput(float sheepPosX, float sheepPosY, float sheepVelX, float sheepVelY, float goalPosX, float goalPosY, float dogPosX, float dogPosY) {
	currentInput[0] = sheepPosX;
	currentInput[1] = sheepPosY;
	currentInput[2] = sheepVelX;
	currentInput[3] = sheepVelY;
	currentInput[4] = goalPosX;
	currentInput[5] = goalPosY;
	currentInput[6] = dogPosX;
	currentInput[7] = dogPosY;

	currentScaledInput[0] = (sheepPosX - 30.0f) / 2000.0f; // Fenced area is 2000 wide
	currentScaledInput[1] = sheepPosY / 2000.0f;
	currentScaledInput[2] = sheepVelX / 200.0f; // Sheep's max speed
	currentScaledInput[3] = sheepVelY / 200.0f;
	currentScaledInput[4] = (goalPosX - 30.0f) / 2000.0f;
	currentScaledInput[5] = goalPosY / 2000.0f;
	currentScaledInput[6] = (dogPosX - 30.0f) / 2000.0f;
	currentScaledInput[7] = dogPosY / 2000.0f;
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
	fitness = dist;
	//UE_LOG(LogTemp, Warning, TEXT("Fitness: %f"), fitness);
}

float Brain::GetFitness() {
	return fitness;
}

void Brain::PrintBrainStuff(int index) {
	UE_LOG(LogTemp, Warning, TEXT("Brain: %d: LeftOutput: %f"), index, GetLeftOutput());
	UE_LOG(LogTemp, Warning, TEXT("Brain: %d: RightOutput: %f"), index, GetRightOutput());
	UE_LOG(LogTemp, Warning, TEXT("Brain: %d: UpOutput: %f"), index, GetUpOutput());
	UE_LOG(LogTemp, Warning, TEXT("Brain: %d: DownOutput: %f"), index, GetDownOutput());
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

void Brain::Crossover(Brain* parent1, Brain* parent2) {
	for (int i = 0; i < numWeights; i++) {
		float coin = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		if (coin < 0.5f) {
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