
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "SuffixTree.h" // Include your SuffixTree header file

int main() {

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

	/*suffixTree.createAndSaveSuffixTree();*/
  /*  suffixTree.loadFromFile("suffix_tree_data.bin");
	suffixTree.Suggest("ther"); // Suggest words starting with 't'*/
	while (true) {
		char inputWord[100];
		char suggestion[100];

		std::cout << "Enter a word for spell checking (or 'exit' to quit): ";
		std::cin >> inputWord;

		if (strcmp(inputWord, "exit") == 0) {
			break;
		}

		// Spell check and get suggestions
		suffixTree.spellCheck(inputWord, suffixTree, suggestion);

		cout << "Suggestions: " << suggestion << std::endl;
		suffixTree.Suggest(suggestion);
	}


	return 0;
}

