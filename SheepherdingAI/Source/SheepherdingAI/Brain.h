#pragma once

class SHEEPHERDINGAI_API Brain
{
public:
	Brain();
	~Brain();

	void SetCurrentInput(float sheepDist, float sheepAngle, float goalDist, float goalAngle);
	float GetTurnOutput();
	float GetMoveForwardOutput();

	// Calculate outputs from the inputs
	void Forward();
	
	void CalcFitness(FVector sheepPos, FVector goalPos);
	void ResetFitness();
	float GetFitness();
	void PrintBrainStuff(int index);
	void Mutate(float mutationRate, float mutationSize);
	void CopyWeights(Brain* brain);
	void Crossover(Brain* parent1, Brain* parent2);
	float GetWeight(int index);

	void PrintWeights();
	void GenerateNewWeights();

private:
	static const int numOutput = 2; //Output: up/down, left/right
	//static const int numInput = 8 + numOutput; //Input: sheep xpos, sheep ypos, sheep xvel, sheep yvel, goal xpos, goal ypos, dog xpos, dog ypos
	static const int numInput = 4 + numOutput; // sheep distance, sheep angle, goal distance, goal angle. Angles relative to dog angle
	static const int numWeights = numInput * numOutput;
	
	float currentOutput[numOutput];
	float currentInput[numInput];
	float currentScaledInput[numInput];
	float weights[numWeights];

	float fitness;

	// Activation function used in the neural network. Returns a value in the range [-1, 1]
	float Activation(float val);
};