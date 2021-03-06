// MarkovChain.cpp : Defines the entry point for the console application.

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include "Word.h"

#define SENTENCELENGTH 8

void removeSymbols(std::vector<char>& cV) {
	for (int x = 0; x < cV.size(); x++) {
		//If not letter, hyphen or apostrophe, erase
		if (!((cV.at(x) <= 'Z' && cV.at(x) >= 'A') || (cV.at(x) >= 'a' && cV.at(x) <= 'z') || (cV.at(x) == 39) || (cV.at(x) == '-'))) {
			cV.erase(cV.begin() + x);
		}
	}
}
void convertLowerCase(std::string& s) {
	for (int i = 0; i < s.length(); i++) {
		if (s[i] <= 'Z' && s[i] >= 'A') {
			s[i] += 32;
		}
	}
}
void storeWord(std::vector<char>& cV, std::vector<Word>& wV, std::vector<std::string>& sV) {
	//Check if char array has content
	if (cV.size() > 0) {
		//Initialize string and copy char vector to string
		std::string tempstring(cV.begin(), cV.end());
		//Clear char vector
		cV.clear();
		try {
			//Convert to lower case
			convertLowerCase(tempstring);
			//Check if word object with such a string exists
			if (wV.size() > 0) {
				bool objectexists = false;
				for (int i = 0; i < wV.size(); i++) {
					try {
						if (wV.at(i).getString() == tempstring) {
							objectexists = true;
							goto DOESNOTEXIST;
							
						}
					}
					catch (const std::exception& e) {
						std::cout << "Error : Could not store string into object" << std::endl;
					}
				}
				DOESNOTEXIST:
				if (objectexists == false) {
					wV.push_back(Word(tempstring));
				}
			}
			else {
				wV.push_back(Word(tempstring));
			}
			sV.push_back(tempstring);
		}
		catch (const std::exception& e) {
			std::cout << "Error : Could not copy chars to string" << std::endl;
		}
		//Clear string
		tempstring.clear();
	}
}
int findWordObj(std::string s, std::vector<Word> wV) {
	for (int w = 0; w < wV.size(); w++) {
		if (s == wV.at(w).getString()) {
			return w;
		}
	}
}

int main()
{
	//Create word object vector
	std::vector<Word> wordVec;
	wordVec.reserve(9999);

	//Create all words vector
	std::vector<std::string> allwordsVec;
	allwordsVec.reserve(99999);

	//Access dataset
	std::fstream datasetStream;
	datasetStream.open("dataset.txt", std::fstream::in | std::fstream::out | std::fstream::app);
	if (datasetStream) {
		std::cout << "Processing dataset..." << std::endl;
		//Read line-by-line
		std::string line;
		while (std::getline(datasetStream, line)) {
			std::vector<char> tempCharVec;
			for (int i = 0; i < line.size(); i++) {
				//Copy word to temp char array
				if (line[i] != ' ') {
					tempCharVec.push_back(line[i]);
					removeSymbols(tempCharVec);
				}
				//Store word as word object
				else {
					storeWord(tempCharVec, wordVec, allwordsVec);
				}
			}
		}
	}
	else {
		std::cout << "Error : could not access dataset file" << std::endl;
	}
	std::cout << "Dataset loaded" << std::endl << std::endl;
	datasetStream.close();

	//Assign word links
	std::cout << "Creating Markov chain..." << std::endl;
	std::cout << "Assinging word links" << std::endl;
	for (int a = 0; a < wordVec.size(); a++) {
		std::cout << a << " / " << wordVec.size() << std::endl;
		for (int b = 0; b < allwordsVec.size() - 1; b++) {
			//Word match
			if (wordVec.at(a).getString() == allwordsVec.at(b)) {
				//Add new choice
				if (wordVec.at(a).choiceVec.size() <= 0) {
					wordVec.at(a).addChoice(allwordsVec.at(b + 1), 1);
				}
				//If there are existing choices for the word
				else {
					bool stringexists = false;
					for (int c = 0; c < wordVec.at(a).choiceVec.size(); c++) {
						//Increment weight if choice repeats
						if (wordVec.at(a).choiceVec.at(c).strstring == allwordsVec.at(b + 1)) {
							wordVec.at(a).choiceVec.at(c).weight++;
							stringexists = true;
						}
					}
					if (stringexists == false) {
						//Add new choice
						wordVec.at(a).addChoice(allwordsVec.at(b + 1), 1);
					}
				}
			}
		}
	}
	std::cout << "Word links assigned" << std::endl << std::endl;

	//Randomly pick a starting word
	std::random_device wseed;
	std::mt19937 wgenerator(wseed());
	std::uniform_int_distribution<int> sd(0, allwordsVec.size() - 1);
	int randpos = sd(wgenerator);
	Word nextWord = wordVec.at(findWordObj(allwordsVec.at(randpos), wordVec));

	//Output chain
	int nextpos;
	int wordcount = 0;
	while (1) {
		std::cout << nextWord.getString() << " ";
		nextpos = findWordObj(nextWord.getNextWord(wordVec), wordVec);
		nextWord = wordVec.at(nextpos);
		if (wordcount < SENTENCELENGTH) {
			wordcount++;
		}
		else if (wordcount == SENTENCELENGTH) {
			wordcount = 0;
			std::cout << std::endl;
		}
	}
    return 0;
}