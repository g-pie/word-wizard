// Project Identifier: 50EB44D3F029ED934858FFFCEAC3547C68768FC9
#include <vector>
#include <deque>
#include <string>
#include <fstream>
#include <getopt.h>
#include <iostream>
using namespace std;
// a word
struct DictionaryEntry {

	// member variables for DictionaryEntry
	string word;
	char morphMethod;
	char letterChanged;
	char letterInserted;
	int changeIndex;
	int swapIndex;
	int deleteIndex;
	int insertIndex;
	int prevEntry;
	bool discovered = 0;
};
class Letterman {
public:
	// member functions for Letterman
	// getopt
	void getOptions(int argc, char** argv);

	//reading input
	void readDictionary();
	void search();


private:
	deque<int> searchContainer;
	vector<DictionaryEntry> Dictionary;
	string outputMode = "W";
	string startWord = "";
	string endWord = "";
	int numWords;
	int numWordsComplex;
	int wordsDiscovered = 0;
	int startIndex;
	int endIndex;
	char sqPolicy;
	bool startFound = 0;
	bool endFound = 0;
	bool solvable = 0;
	bool changeSwitch = 0;
	bool swapSwitch = 0;
	bool lengthSwitch = 0;
	//helper functions
	void morphTime(int currentWord);
	void changeMode(int currentWord, int compareWord);
	void swapMode(int currentWord, int compareWord);
	void deleteMode(int currentWord, int compareWord);
	void insertMode(int currentWord, int compareWord);
	void backtracker();
	void isStartEnd(string& word, int index);
};

//getopt 
void Letterman::getOptions(int argc, char** argv) {
	//getopt shit
	int option_index = 0, option = 0, qs = 0;
	bool changeFlagTrue = 0;
	bool swapFlagTrue = 0;
	bool lengthFlagTrue = 0;
	opterr = true;
	struct option longOpts[] = {
		{"stack", no_argument, nullptr, 's'},
		{"queue", no_argument, nullptr, 'q'},
		{"change", no_argument, nullptr, 'c'},
		{"swap", no_argument, nullptr, 'p'},
		{"length", no_argument, nullptr, 'l'},
		{"output", required_argument, nullptr, 'o'},
		{"begin", required_argument, nullptr, 'b'},
		{"end", required_argument, nullptr, 'e'},
		{"help", no_argument, nullptr, 'h'},
		{nullptr, 0, nullptr, '\0'} };
	while ((option = getopt_long
	(argc, argv, "sqcplo:b:e:h", longOpts, &option_index)) != -1){
		switch (option) {
			// stack mode
		case 's':
			sqPolicy = 's';
			++qs;
			break;
			// queue mode
		case 'q':
			sqPolicy = 'q';
			++qs;
			break;
			// change mode
		case 'c':
			changeSwitch = 1;
			changeFlagTrue = true;
			break;
			// swap mode
		case 'p':
			swapSwitch = 1;
			swapFlagTrue = true;
			break;
			// length mode
		case 'l':
			lengthSwitch = 1;
			lengthFlagTrue = true;
			break;
			// output mode word or modification?
		case 'o': {
			string temp = optarg;
			if (temp == "M"){
				outputMode = "M";
				break;
			}
			else if (temp == "W") {
				break;
			}
			else {
				cout << "Invalid output mode \n";
				exit(1);
			}
		}
			// beginning word?
		case 'b':
			startWord = optarg;
			break;
			// ending word?
		case 'e':
			endWord = optarg;
			break;
			// HELP!
		case 'h':
			cout << "Help Letterman stop the evil Word Wizard.\n Check the spec for usage\n";
			exit(0);
			break;
		default:
			// INVALID INPUT
			cout << "error: invalid input!\n";
			exit(1);
		}
	}
	// command line error checking
	if (qs != 1){
		cout << "Must have exactly one q or s.\n";
		exit(1);
	}
	if (changeFlagTrue == 0 && lengthFlagTrue == 0 && swapFlagTrue == 0) {
		cout << "Must have change length or swap.\n";
		exit(1);
	}
	// if change and/or swap is specified but length is not, and begin and end words have different length:
	if ((changeFlagTrue == 1 || swapFlagTrue == 1) && lengthFlagTrue == 0){
		if (startWord.length() != endWord.length()) {
			cout << "Start and end words have different length\n";
			exit(1);
		}
	}
}
void Letterman::readDictionary() {
	//helper variables
	char dictType;
	// read the input
	string input;
	// read dict type 
	cin >> dictType;

	// ignore endl
	cin >> ws;
	// num words
	cin >> numWords;

	Dictionary.reserve(numWords);
	// ignore another endl
	cin >> ws;
	// start reading dictionary words
	// Simple type dictionary
	if (dictType == 'S') {
		int i = 0;
		while (i < numWords){
			DictionaryEntry D;
			
			// read a line in
			//getline(cin, input);
			cin >> input;

			// if it's a comment...
			if (input[0] == '/' && input[1] == '/') {
				//delete input

				getline(cin, input);
			}
			// if the line is a word
			else {
				D.word = input;
				if (D.word == startWord) {
					startFound = 1;
					startIndex = i;
				}
				if (D.word == endWord) {
					endFound = 1;
					endIndex = i;
				}
				Dictionary.push_back(D);
				++i;
			}
		}

		// FOR TESTING PURPOSES: prints out your whole dictionary
		/*for (int i = 0; i < numWords; ++i) {
			cout << Dictionary[i].word << endl;
		}*/
	}
	// Complex dictionary inputs
	else {
		int i = 0;
		int offset = 0;
		while (i < numWords) {
			DictionaryEntry D;
			bool specialChar = 0;
			cin >> input;
			if (input[0] == '/' && input[1] == '/') {
				//delete input
				getline(cin, input);
			}
			else {
				int openBracketIndex;
				int closeBracketIndex;
				int j = 0;
				while (j < int(input.length()) && specialChar == 0) {
					if (input[j] == '[') {
						openBracketIndex = j;
						int spaceBtwnBrackets = 0;
						for (int k = j; j < int(input.length()); ++k) {
							if (input[k] == ']') {
								spaceBtwnBrackets = k - j - 1;
								closeBracketIndex = k;
								break;
							}
						}
						for (int k = 0; k < spaceBtwnBrackets; ++k) {
							DictionaryEntry temp;
							temp.word = input.substr(0, openBracketIndex) + input[openBracketIndex + 1 + k]
								+ input.substr(closeBracketIndex + 1);
							Dictionary.push_back(temp);
							isStartEnd(temp.word, i + k + offset);
							++offset;
						}
						--offset;
						specialChar = 1;
						break;
					}
					if (input[j] == '!') {
						D.word = input.substr(0, j) + input.substr(j + 1);
						Dictionary.push_back(D);
						isStartEnd(D.word, i + offset);
						++offset;
						DictionaryEntry temp;
						temp.word = D.word;
						char tempChar = temp.word[j - 1];
						temp.word[j - 1] = temp.word[(j - 2)];
						temp.word[j - 2] = tempChar;
						Dictionary.push_back(temp);
						isStartEnd(temp.word, i + offset);
						specialChar = 1;
						break;
					}
					if (j > 1) {
						if (input[j] == '?') {
							D.word = input.substr(0, j) + input.substr(j + 1);
							Dictionary.push_back(D);
							isStartEnd(D.word, i + offset);
							++offset;
							DictionaryEntry temp;
							temp.word = input;
							temp.word[j] = temp.word[j - 1];
							Dictionary.push_back(temp);
							isStartEnd(D.word, i + offset);
							specialChar = 1;
							break;
						}
					}
					if (j == int(input.length() - 1)) {
						if (input[j] == '&') {
							input.erase(j);
							D.word = input;
							Dictionary.push_back(D);
							isStartEnd(D.word, i + offset);
							++offset;
							DictionaryEntry reversed;
							reversed.word = input;
							int temp = j - 1;
							for (int k = 0; k < int(input.length()); ++k) {
								reversed.word[k] = input[temp];
								--temp;
							}
							Dictionary.push_back(reversed);
							isStartEnd(reversed.word, i + offset);
							specialChar = 1;
							break;
						}
					}
					++j;
				}
				if (specialChar == 0) {
					D.word = input;
					Dictionary.push_back(D);
					isStartEnd(D.word, i + offset);
				}
				++i;
			}
		}
		/*for (int j = 0; j < int(Dictionary.size()); ++j) {
			cout << Dictionary[j].word << endl;
		}*/
	}
}

void Letterman::search() {
	numWords = int(Dictionary.size());

	// error checking for start and end word
	if (startFound == 0) {
		cout << "Start word not found in dictionary\n";
		exit(1);
	}
	if (endFound == 0) {
		cout << "End word not found in dictionary\n";
		exit(1);
	}
	Dictionary[startIndex].discovered = 1;
	searchContainer.push_front(startIndex);
	++wordsDiscovered;
	while (!searchContainer.empty()) {
		int current;
		if (sqPolicy == 's') {
			// Investigate the current word
			current = searchContainer.back();
			// Remove it from the searchable container
			searchContainer.pop_back();
		}
		else {
			current = searchContainer.front();
			searchContainer.pop_front();
		}
		if (current == endIndex) {
			solvable = 1;
			break;
		}
		else {
			morphTime(current);
		}
	}
	// container empty
	if (solvable == 0) {
		cout << "No solution, " << wordsDiscovered << " words discovered.\n";
	}
	//solvable
	else {
		backtracker();
	}
}

void Letterman::morphTime(int currentWord) {
	// go through the dictionary
	for (int i = 0; i < int(Dictionary.size()); ++i) {
		// skip over discovered words
		if (Dictionary[i].discovered == 0) {
			// change active?
			if (changeSwitch == 1) {
				if (Dictionary[currentWord].word.length() == Dictionary[i].word.length()) {
					changeMode(currentWord, i);
				}
			}
			if (swapSwitch == 1) {
				if (Dictionary[currentWord].word.length() == Dictionary[i].word.length()) {
					swapMode(currentWord, i);
				}
			}
			if (lengthSwitch == 1) {
				if (Dictionary[i].word.length() == Dictionary[currentWord].word.length() - 1) {
					deleteMode(currentWord, i);
				}
				else if (Dictionary[i].word.length() == Dictionary[currentWord].word.length() + 1) {
					insertMode(currentWord, i);
				}
			}
		}
	}
}

void Letterman::changeMode(int currentWord, int compareWord) {

	// index through each character of the words
	int j = 0;
	// track the number of similar characters
	int numDifferent = 0;
	// track the index of the different char
	while (numDifferent < 2 && j < int(Dictionary[currentWord].word.length())) {
		if (Dictionary[currentWord].word[j] != Dictionary[compareWord].word[j]) {
			Dictionary[compareWord].changeIndex = j;
			Dictionary[compareWord].letterChanged = Dictionary[compareWord].word[j];
			++numDifferent;
		}
		++j;
	}
	// if exactly 1 char differs between the words, add compareWord to the searchable container.
	if (numDifferent == 1) {
		Dictionary[compareWord].morphMethod = 'c';
		++wordsDiscovered;
		Dictionary[compareWord].discovered = 1;
		Dictionary[compareWord].prevEntry = currentWord;
		searchContainer.push_back(compareWord);
	}
}

void Letterman::swapMode(int currentWord, int compareWord) {
	int diffIndex1 = 0;
	int diffIndex2 = 0;
	int numDifferences = 0;
	int j = 0;
	while (j < int(Dictionary[currentWord].word.length())) {
		if (numDifferences == 0) {
			if (Dictionary[currentWord].word[j] != Dictionary[compareWord].word[j]) {
				++numDifferences;
				diffIndex1 = j;
			}
		}
		else if (numDifferences == 1) {
			if (Dictionary[currentWord].word[j] != Dictionary[compareWord].word[j]) {
				++numDifferences;
				diffIndex2 = j;
			}
		}
		else {
			if (Dictionary[currentWord].word[j] != Dictionary[compareWord].word[j]) {
				++numDifferences;
			}
		}
		++j;
	}
	if (numDifferences == 2) {
		if (diffIndex2 - diffIndex1 == 1) {
			if (Dictionary[currentWord].word[diffIndex2] != Dictionary[compareWord].word[diffIndex2]) {
				if (Dictionary[currentWord].word[diffIndex1] != Dictionary[compareWord].word[diffIndex1]) {
					if (Dictionary[currentWord].word[diffIndex1] == Dictionary[compareWord].word[diffIndex2] &&
						Dictionary[currentWord].word[diffIndex2] == Dictionary[compareWord].word[diffIndex1]) {
						Dictionary[compareWord].morphMethod = 's';
						++wordsDiscovered;
						Dictionary[compareWord].discovered = 1;
						Dictionary[compareWord].prevEntry = currentWord;
						Dictionary[compareWord].swapIndex = diffIndex1;
						searchContainer.push_back(compareWord);
					}
				}
			}
		}
	}
}

void Letterman::deleteMode(int currentWord, int compareWord) {
	// compare word must be exactly 1 char longer than the current word
	if (Dictionary[compareWord].word.length() == (Dictionary[currentWord].word.length() - 1)) {
		int j = 0;
		bool validWord = 1;
		int indexDel = 0;
		while (j < int(Dictionary[currentWord].word.length()) && Dictionary[compareWord].word[j] == Dictionary[currentWord].word[j]) {
			indexDel = j;
			++j;
		}
		indexDel = j;
		if (j != int(Dictionary[currentWord].word.length())) {
			for (j = j; j < int(Dictionary[compareWord].word.length()); ++j) {
				if (Dictionary[currentWord].word[j + 1] != Dictionary[compareWord].word[j]) {
					validWord = 0;
					break;
				}
			}
		}
		if (validWord == 1) {
			Dictionary[compareWord].deleteIndex = indexDel;
			Dictionary[compareWord].morphMethod = 'd';
			++wordsDiscovered;
			Dictionary[compareWord].discovered = 1;
			Dictionary[compareWord].prevEntry = currentWord;
			searchContainer.push_back(compareWord);
		}
	}
}

void Letterman::insertMode(int currentWord, int compareWord) {
	if (Dictionary[compareWord].word.length() == (Dictionary[currentWord].word.length() + 1)) {
		int j = 0;
		bool validWord = 1;
		int insIndex = 0;
		while (j < int(Dictionary[compareWord].word.length()) && Dictionary[compareWord].word[j] == Dictionary[currentWord].word[j]) {
			insIndex = j;
			++j;
		}
		insIndex = j;
		if (j != int(Dictionary[compareWord].word.length())) {
			for (j = j; j < int(Dictionary[currentWord].word.length()); ++j) {
				if (Dictionary[currentWord].word[j] != Dictionary[compareWord].word[j + 1]) {
					validWord = 0;
					break;
				}
			}
		}
		if (validWord == 1) {
			Dictionary[compareWord].morphMethod = 'i';
			Dictionary[compareWord].insertIndex = insIndex;
			Dictionary[compareWord].letterInserted = Dictionary[compareWord].word[insIndex];
			Dictionary[compareWord].discovered = 1;
			++wordsDiscovered;
			Dictionary[compareWord].prevEntry = currentWord;
			searchContainer.push_back(compareWord);
		}
	}
}

void Letterman::backtracker() {
	// make a deque to backtrack
	deque<int> output;
	output.push_back(endIndex);
	int temp;
	int numMorphed = 1;
	while (output.front() != startIndex) {
		temp = Dictionary[output.front()].prevEntry;
		output.push_front(temp);
		++numMorphed;
	}
	if (outputMode == "W") {
		cout << "Words in morph: " << numMorphed << "\n";
		while (!output.empty()) {
			cout << Dictionary[output.front()].word << "\n";
			output.pop_front();
		}
	}
	else if(outputMode == "M"){
		cout << "Words in morph: " << numMorphed << "\n";
		cout << startWord << "\n";
		output.pop_front();
		while (!output.empty()) {
			switch (Dictionary[output.front()].morphMethod) {
			case 'c':
				cout << Dictionary[output.front()].morphMethod << ","; 
				cout << Dictionary[output.front()].changeIndex << ",";
				cout << Dictionary[output.front()].letterChanged << "\n";
				break;
			case 'i':
				cout << Dictionary[output.front()].morphMethod << "," <<
					Dictionary[output.front()].insertIndex << "," <<
					Dictionary[output.front()].letterInserted << "\n";
				break;
			case 'd':
				cout << Dictionary[output.front()].morphMethod << "," <<
					Dictionary[output.front()].deleteIndex << "\n";
				break;
			case 's':
				cout << Dictionary[output.front()].morphMethod << "," <<
					Dictionary[output.front()].swapIndex << "\n";
				break;
			}
			output.pop_front();
		}
	}
}

void Letterman::isStartEnd(string& word, int index) {
	if (word == startWord) {
		startFound = 1;
		startIndex = index;
	}
	if (word == endWord) {
		endFound = 1;
		endIndex = index;
	}
}