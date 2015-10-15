#pragma once

class SHEEPHERDINGAI_API Brain
{
public:
	Brain();
	~Brain();

	void SetCurrentInput(float sheepPosX, float sheepPosY, float sheepVelX, float sheepVelY, float goalPosX, float goalPosY);
	float GetLeftOutput();
	float GetRightOutput();
	float GetUpOutput();
	float GetDownOutput();

	// Calculate outputs from the inputs
	void Forward();
	
	void CalcFitness();
	float GetFitness();

	void PrintBrainStuff(int index);

private:
	static const int numOutput = 4; //Output: left, right, top, down
	static const int numInput = 6; //Input: sheep xpos, ypos, xvel, yvel, goal xpos, goal ypos
	static const int numWeights = numInput * numOutput;
	
	float currentOutput[numOutput];
	float currentInput[numInput];
	float currentScaledInput[numInput];
	float weights[numWeights];

	float fitness;

	// Activation function used in the neural network. Returns a value in the range [-1, 1]
	float Activation(float val);
};