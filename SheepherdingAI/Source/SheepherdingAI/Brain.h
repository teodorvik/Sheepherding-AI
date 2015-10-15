#pragma once

class SHEEPHERDINGAI_API Brain
{
public:
	Brain();
	~Brain();

	void SetCurrentInput(float sheepPosX, float sheepPosY, float sheepVelX, float sheepVelY, float goalPosX, float goalPosY, float dogPosX, float dogPosY);
	float GetLeftOutput();
	float GetRightOutput();
	float GetUpOutput();
	float GetDownOutput();

	// Calculate outputs from the inputs
	void Forward();
	
	void CalcFitness(FVector sheepPos, FVector goalPos);
	float GetFitness();
	void PrintBrainStuff(int index);
	void Mutate(float mutationRate, float mutationSize);
	void CopyWeights(Brain* brain);
	void Crossover(Brain* parent1, Brain* parent2);
	float GetWeight(int index);

	void PrintWeights();

private:
	static const int numOutput = 4; //Output: left, right, top, down
	static const int numInput = 8; //Input: sheep xpos, sheep ypos, sheep xvel, sheep yvel, goal xpos, goal ypos, dog xpos, dog ypos
	static const int numWeights = numInput * numOutput;
	
	float currentOutput[numOutput];
	float currentInput[numInput];
	float currentScaledInput[numInput];
	float weights[numWeights];

	float fitness;

	// Activation function used in the neural network. Returns a value in the range [-1, 1]
	float Activation(float val);
};