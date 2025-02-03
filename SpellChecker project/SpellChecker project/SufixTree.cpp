#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "SuffixTree.h" 

const int MAX_WORDS = 100000; // Maximum number of words in the suffix tree
const int MAX_LENGTH = 100; // Maximum length of a word

SuffixTree::Node::Node()
{
	Endofword = false;
	frequency = 0;
	for (int i = 0; i < C_size; i++) {     //Kol child yeshwer 3ala null 
		children[i] = NULL;
	}
}

SuffixTree::SuffixTree()
{
	root = new Node();

}

SuffixTree::Node* SuffixTree::getRoot() const
{
	return root;
}

SuffixTree::~SuffixTree()
{
	DeleteNodes(root);
}
void SuffixTree::createAndSaveSuffixTree() {
	SuffixTree suffixTree; // Instantiate your SuffixTree object

	ifstream file("dic.txt"); // Replace 'ss.txt' with the actual file path

	if (file.is_open()) {
		string line;


		while (getline(file, line)) {
			istringstream iss(line);
			string lemma, word;
			int frequency;

			// Attempt to extract data from the line
			if (iss >> lemma >> word >> frequency) {
				// Insert word with its frequency into the suffix tree
				suffixTree.Insert(word.c_str(), frequency);
			}
			else {
				cerr << "Error parsing line: " << line << endl;
			}
		}
		file.close();
	}
	else {
		cerr << "Unable to open file" << endl;
	}

	const char* filename = "suffix_tree_data.bin";
	if (suffixTree.saveToFile(filename)) {
		cout << "Suffix tree saved to file: " << filename << endl;
	}
	else {
		cerr << "Failed to save suffix tree to file." << endl;
	}
}

bool SuffixTree::saveToFile(const char* filename) {
	std::ofstream file(filename, std::ios::binary);
	if (!file.is_open()) {
		std::cerr << "Error opening file for writing.\n";
		return false;
	}

	saveNodeToFile(root, file);

	file.close();
	return true;
}

// Helper function to serialize a node and its children
void SuffixTree::saveNodeToFile(Node* node, std::ofstream& file) {
	if (node == nullptr) {
		return;
	}

	file.write(reinterpret_cast<char*>(&node->frequency), sizeof(int));

	int wordLength = node->word.size();
	file.write(reinterpret_cast<char*>(&wordLength), sizeof(int));
	file.write(node->word.c_str(), wordLength);

	for (int i = 0; i < C_size; ++i) {
		saveNodeToFile(node->children[i], file);
	}
}

// Load suffix tree from a file
bool SuffixTree::loadFromFile(const char* filename) {
	std::ifstream file(filename, std::ios::binary);
	if (!file.is_open()) {
		std::cerr << "Error opening file for reading.\n";
		return false;
	}

	root = loadNodeFromFile(file);

	file.close();
	return true;
}

// Helper function to deserialize a node and its children
SuffixTree::Node* SuffixTree::loadNodeFromFile(std::ifstream& file) {
	int frequency;
	file.read(reinterpret_cast<char*>(&frequency), sizeof(int));

	int wordLength;
	file.read(reinterpret_cast<char*>(&wordLength), sizeof(int));

	std::string word;
	word.resize(wordLength);
	file.read(&word[0], wordLength);

	Node* node = new Node();
	node->frequency = frequency;
	node->word = word;

	for (int i = 0; i < C_size; ++i) {
		node->children[i] = loadNodeFromFile(file);
	}

	return node;
}

void SuffixTree::Insert(const char* word, int frequency) {
	if (word == nullptr || word[0] == '\0') {
		// Invalid input: Empty word or null pointer
		return;
	}

	Node* curr = root;

	for (int i = 0; word[i] != '\0'; ++i) {
		int idx = word[i] - 'a';

		if (idx < 0 || idx >= C_size) {
			// Invalid character index, out of bounds
			return;
		}

		if (curr->children[idx] == nullptr) {
			// If the current node doesn't have a child for the character, create a new node
			curr->children[idx] = new Node();
		}

		curr = curr->children[idx];
	}

	// Update or set the frequency of the word in the leaf node
	curr->frequency = frequency;
	curr->Endofword = true;
}



void SuffixTree::Search(const char* word)
{

	if (root == NULL) {
		cout << "Tree is empty!" << endl;
		return;
	}

	Node* curr = root;
	for (int i = 0; word[i] != NULL; ++i) {
		int index = word[i] - 'a';
		if (!curr->children[index]) {
			cout << "Word '" << word << "' not found" << endl;
			return;
		}
		curr = curr->children[index];
	}

	if (curr->Endofword) {
		cout << "Word '" << word << "' is found" << endl;
	}
	else {
		cout << "Word '" << word << "' not found" << endl;
	}
}

void SuffixTree::Suggest(const char* prefix) {
	Node* curr = root;
	for (int i = 0; prefix[i] != '\0'; ++i) {
		int index = prefix[i] - 'a';
		if (!curr->children[index]) {
			cout << "No suggestions found for the prefix '" << prefix << "'" << endl;
			return;
		}
		curr = curr->children[index];
	}

	const int maxSuggestions = 1000; // Assuming maximum 100 suggestions
	string* suggestions = new string[maxSuggestions]; // Array to store words
	int* frequencies = new int[maxSuggestions]; // Array to store corresponding frequencies
	int count = 0;

	// Traverse from the node representing the prefix to collect suggestions
	SuggestFromNode(curr, prefix, suggestions, frequencies, count, maxSuggestions);

	// Sort the suggestions by frequency
	BubbleSortByFrequency(suggestions, frequencies, count);

	cout << "Words suggested for '" << prefix << "' sorted by frequency are:" << endl;
	for (int i = 0; i < count; ++i) {
		cout << suggestions[i] << " (Frequency: " << frequencies[i] << ")" << endl;
	}

	delete[] suggestions; // Free allocated memory
	delete[] frequencies; // Free allocated memory
}

void SuffixTree::SuggestFromNode(Node* curr, const string& prefix, string* suggestions, int* frequencies, int& count, const int maxSuggestions) {
	if (count >= maxSuggestions) {
		cout << "Exceeded maximum suggestions limit." << endl;
		return;
	}

	if (curr->Endofword && count < maxSuggestions) {
		suggestions[count] = prefix;
		frequencies[count] = curr->frequency;
		++count;
	}

	for (int i = 0; i < C_size; ++i) {
		if (curr->children[i]) {
			char ch = 'a' + i;
			string newWord = prefix + ch;
			SuggestFromNode(curr->children[i], newWord, suggestions, frequencies, count, maxSuggestions);
		}
	}
}


void SuffixTree::BubbleSortByFrequency(string* suggestions, int* frequencies, int n) {
	for (int i = 0; i < n - 1; ++i) {
		for (int j = 0; j < n - i - 1; ++j) {
			if (frequencies[j] < frequencies[j + 1]) {
				// Swap the elements based on frequency
				swap(suggestions[j], suggestions[j + 1]);
				swap(frequencies[j], frequencies[j + 1]);
			}
		}
	}
}



int SuffixTree::calculateASCIISum(const char* str) {
	int sum = 0;
	while (*str != '\0') {
		sum += *str;
		++str;
	}
	return sum;
}


void SuffixTree::findClosestWord(Node* curr, const char* input, char* closest, int& minDiff, int inputSum, int& closestFrequency) {
	if (curr->Endofword) {
		int wordSum = calculateASCIISum(input);
		int diff = (wordSum > inputSum) ? (wordSum - inputSum) : (inputSum - wordSum);

		if (diff < minDiff || (diff == minDiff && curr->frequency > closestFrequency)) {
			minDiff = diff;
			closestFrequency = curr->frequency;
			strcpy_s(closest, MAX_LENGTH, input);
		}
	}

	for (int i = 0; i < 26; ++i) {
		if (curr->children[i]) {
			char ch = 'a' + i;
			char newWord[MAX_LENGTH];
			strcpy_s(newWord, MAX_LENGTH, input);
			int len = strlen(newWord);
			newWord[len] = ch;
			newWord[len + 1] = '\0';

			findClosestWord(curr->children[i], newWord, closest, minDiff, inputSum, closestFrequency);
		}
	}
}

void SuffixTree::correctWord(const char* input) {
	int inputSum = calculateASCIISum(input);
	char closest[MAX_LENGTH] = "";
	int minDiff = INT_MAX;
	int closestFrequency = 0; // Store the frequency of the closest word found

	findClosestWord(root, input, closest, minDiff, inputSum, closestFrequency);

	if (strlen(closest) > 0) {
		std::cout << "Input: " << input << " Output: " << closest << " (Frequency: " << closestFrequency << ")" << std::endl;
	}
	else {
		std::cout << "No suggestion found for input: " << input << std::endl;
	}
}




void SuffixTree::suggestCorrections(const char* userInput, const Node* suffixTree) {
	const Node* current = suffixTree;

	while (*userInput) {
		if (current->children[*userInput - 'a']) {
			current = current->children[*userInput - 'a'];
		}
		else {
			// Handle potential corrections based on the current node
			for (int i = 0; i < 26; ++i) {
				if (current->children[i]) {
					cout << static_cast<char>('a' + i);
					suggestCorrections(userInput + 1, current->children[i]);
				}
			}
			return;
		}
		++userInput;
	}
}

void SuffixTree::DeleteNodes(Node* node)
{
	if (node == nullptr) {
		return;
	}

	for (int i = 0; i < C_size; ++i) {
		DeleteNodes(node->children[i]);
	}

	delete node;
}









//void SuffixTree::suggestCorrections(const char* userInput, const Node* suffixTree) {
//	const Node* current = suffixTree;
//
//	while (*userInput) {
//		if (current->children[*userInput - 'a']) {
//			current = current->children[*userInput - 'a'];
//		}
//		else {
//			// Handle potential corrections based on the current node
//			for (int i = 0; i < 26; ++i) {
//				if (current->children[i]) {
//					 cout << static_cast<char>('a' + i);
//					suggestCorrections(userInput + 1, current->children[i]);
//				}
//			}
//			return;
//		}
//		++userInput;
//	}
//}

void SuffixTree::findClosestWord(const char* inputWord, char* result) {
	Node* node = root;
	int i = 0;
	int lastMatchingIndex = 0;

	while (inputWord[i] != '\0' && node->children[inputWord[i] - 'a']) {
		result[i] = inputWord[i];
		if (node->Endofword) {
			lastMatchingIndex = i;
		}
		node = node->children[inputWord[i] - 'a'];
		++i;
	}

	// If the input word is not completely matched and has only one additional character
	if (inputWord[i] != '\0' && inputWord[i + 1] == '\0') {
		result[lastMatchingIndex + 1] = '\0';
	}
	else {
		result[i] = '\0';
	}
}

void SuffixTree::spellCheck(const char* inputWord, SuffixTree& suffixTree, char* suggestion) {
	findClosestWord(inputWord, suggestion);
}




