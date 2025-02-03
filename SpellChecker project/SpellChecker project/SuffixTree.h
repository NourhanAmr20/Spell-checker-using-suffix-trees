#pragma once
#include <cstring>
#include <string>
using namespace std;

int const C_size = 26;

class SuffixTree
{
private:
	class Node
	{
	public:
		int frequency = 0;
		Node* children[C_size];
		bool Endofword;
		string word;
		Node();
		void serializeNode(FILE* file);
		static Node* deserializeNode(FILE* file);
	};
	Node* root;
public:

	SuffixTree();
	Node* getRoot() const;
	~SuffixTree();
	void createAndSaveSuffixTree();
	bool saveToFile(const char* filename);
	bool loadFromFile(const char* filename);
	void saveNodeToFile(Node* node, std::ofstream& file);
	static Node* loadNodeFromFile(std::ifstream& file);
	void Insert(const char* word, int frequency);
	void Search(const char* word);
	void Suggest(const char* preFix); // Bta5od el prefix el da5lna w if found then call SuggestfromNode() 
	void SuggestFromNode(Node* curr, const std::string& prefix, std::string* suggestions, int* frequencies, int& count, const int maxSuggestions);	//void suggestCorrections(const char* userInput, const Node& suffixTree);
	void BubbleSortByFrequency(string* suggestions, int* frequencies, int n);
	void insertWord(const char* word);
	void suggestCorrections(const char* userInput, const Node* suffixTree);
	int calculateASCIISum(const char* str);
	void findClosestWord(Node* curr, const char* input, char* closest, int& minDiff, int inputSum, int& closestFrequency);
	void correctWord(const char* input);

	void findClosestWord(const char* inputWord, char* result);
	void spellCheck(const char* inputWord, SuffixTree& suffixTree, char* suggestion);
private:
	// Function to delete nodes recursively
	void DeleteNodes(Node* node);

	//void Display(const string& preFix = "");

	// find dictionary file Phase 2

};
#pragma once
