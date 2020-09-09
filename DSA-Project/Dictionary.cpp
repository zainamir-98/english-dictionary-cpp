#include "Header.h"

// Becomes true when program has to be exited
bool exit_program = false;

// Bookmarks and history objects
BookmarkList b;
HistoryList h;

// Main menu item array
string itemArray_DictMain[] {
	"Search for a word",
	"Bookmarks",
	"History",
	"Exit"
};

// ** CLASS FUNCTIONS ** //

// ** Dictionary class functions ** //

// Default constructor
DHT::DHT() {
	words_entered = 0;

	// Initialize dictionary array with empty DHT nodes

	DHT_Node* n = new DHT_Node;

	for (int i = 0; i < DICTIONARY_HASHTABLE_SIZE; ++i)
		dictArray[i] = n;
}

// This hash function adds up the unicode values of each letter of
// the provided key string. Each unicode value is also multiplied by
// the positional weight of the character in order to avoid duplicate
// hash values when two strings with the same characters are hashed

int DHT::hashFunction(string s) {
	int g = 2;
	int sum = 0;

	for (int i = 0; i < s.length(); ++i)
		sum += (int)s[i] * pow(g, i);

	return sum % DICTIONARY_HASHTABLE_SIZE;
}

// Insert function
void DHT::insert(string w, string df) {
	// Generate hash index
	int hash_index = hashFunction(w);

	DHT_Node* n = new DHT_Node(w, df);

	// If there's already a word at the hash index, traverse linked list of nodes
	if (dictArray[hash_index]->word != "") {
		DHT_Node* travNode = new DHT_Node;
		travNode = dictArray[hash_index];

		// Traverse linked list at hash index till the end
		while (travNode->next != NULL)
			travNode = travNode->next;

		travNode->next = n;
	}
	else dictArray[hash_index] = n;

	++words_entered;
}

// Display function (DEBUG)
void DHT::display(void) {
	for (int i = 0; i < DICTIONARY_HASHTABLE_SIZE; ++i) {
		DHT_Node* travNode = new DHT_Node;
		travNode = dictArray[i];

		do {
			cout << i << ": " << travNode->word;
			if (travNode->word != "") cout << " [" << travNode->def << "]";
			travNode = travNode->next;

			if (travNode != NULL) cout << " --> ";
		} while (travNode != NULL);

		cout << endl;
	}

	cout << "Words: " << words_entered << endl;
}

// Get word definition 
string DHT::getDefinition(string w) {
	bool returned = false;
	int hash_index = hashFunction(w);

	// If word is found at hash index...
	if (dictArray[hash_index]->word == w) {
		return dictArray[hash_index]->def;
		returned = true;
	}
	else { // If word is not found at hash index, traverse linked list
		DHT_Node* travNode = new DHT_Node;
		travNode = dictArray[hash_index];

		while (travNode->next != NULL) {
			travNode = travNode->next;
			if (travNode->word == w) {
				return travNode->def;
				returned = true;
				break;
			}
		}
	}

	if (returned == false) return "Uh Oh! Word doesn't exist :(";
}
//
//// Remove a word
//void DHT::removeWord(string w) {
//	int hash_index = hashFunction(w);
//
//	if (dictArray[hash_index]->word == w) {
//		// Lazy deletion
//		dictArray[hash_index]->word = "";
//		dictArray[hash_index]->def = "";
//	}
//	else {
//		DHT_Node* travNode = new DHT_Node;
//		travNode = dictArray[hash_index];
//
//		do {
//			travNode = travNode->next;
//			if (travNode->word == w) {
//				// Lazy deletion
//				travNode->word = "";
//				travNode->def = "";
//				break;
//			}
//		} while (travNode != NULL);
//	}
//
//	--words_entered;
//}

// Load words and definitions from .txt file
void DHT::loadWordsFromFile(void) {
	ifstream textFile;
	// Open text file containing words and their definitions
	textFile.open("EnglishDictionary.txt");

	int i = 0;
	string line;

	// Read each line
	while (getline(textFile, line) && i < DICTIONARY_HASHTABLE_SIZE) {
		string word;
		string def;

		if (line != "") {
			string::iterator j;
			j = line.begin();

			// Extracts word
			while (j != line.end() && *j != ' ') {
				word.push_back(*j);
				++j;
			}

			// Extract definition
			while (j != line.end()) {
				def.push_back(*j);
				++j;
			}

			// Insert word and definition into dictionary hash table
			this->insert(word, def);
			++i;
		}
	}

	// Close file
	textFile.close();
}

// ** UI Class Functions ** //

// Default contructor
UI::UI(void) : selection(0), max_selections(sizeof(itemArray_DictMain) / sizeof(*itemArray_DictMain)), stop_taking_str_input(false) {
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	dhtObj.loadWordsFromFile();
	autoCompleteTrieObj.import_txt();
}

// Change text colour
void UI::setTextColour(int c) {
	SetConsoleTextAttribute(hConsole, c);
}

// Display menu from string array
void UI::showMenu(string* strArray) {
	for (int i = 0; i < max_selections; ++i) {
		if (i == selection) {
			setTextColour(SECONDARY_COLOUR);
			cout << " -> " << strArray[i] << " " << endl;
		}
		else {
			setTextColour(COLOUR_WHITE);
			cout << "    " << strArray[i] << " " << endl;
		}
	}
}

// Process menus and UI elements
void UI::process(void) {
	while (!exit_program) {
		switch (screen) {
		case SCREEN_MAIN:

			setTextColour(COLOUR_WHITE);
			cout << endl << "    NUST SEECS BEE-9C THIRD SEMESTER 2018" << endl;
			cout << "    DATA STRUCTURES AND ALGORITHMS END-OF-SEMESTER PROJECT" << endl;

			cout << endl << "    By: Zain Amir Zaman, Azhan Naqeeb, Shanawar Ali" << endl;
			cout << endl;

			setTextColour(MAIN_COLOUR);
			cout << " + -------------------------------------------------------- +" << endl;
			cout << "    DICTIONARY WITH AUTOCOMPLETE AND AUTOCORRECT             " << endl;
			cout << " + -------------------------------------------------------- +" << endl << endl;

			showMenu(itemArray_DictMain);

			setTextColour(COLOUR_GREY);
			cout << endl << " Use UP and DOWN arrow keys to navigate." << endl;
			cout << " Press SPACE to select." << endl;

			// Controls input key commands

			switch (key_input = _getch()) {
			case KEY_UP:
				--selection;
				break;
			case KEY_DOWN:
				++selection;
				break;
			case KEY_SPACE:
				if (selection == 0) changeScreen(SCREEN_SEARCH);
				if (selection == 1) changeScreen(SCREEN_BOOKMARKS);
				if (selection == 2) changeScreen(SCREEN_HISTORY);
				if (selection == 3) exit_program = true;
				break;
			}

			// Controls selection counter

			if (selection > max_selections - 1) selection = 0;
			if (selection < 0) selection = max_selections - 1;

			break;

		case SCREEN_SEARCH:
			setTextColour(MAIN_COLOUR);
			cout << " + -------------------------------------------------------- +" << endl;
			cout << "    DICTIONARY WITH AUTOCOMPLETE AND AUTOCORRECT             " << endl;
			cout << " + -------------------------------------------------------- +" << endl << endl;

			setTextColour(COLOUR_GREY);
			cout << " Press SPACE to enter." << endl;
			cout << " Press ESC to go back." << endl << endl << endl;

			// Take string input

			setTextColour(COLOUR_YELLOW);
			cout << " Enter word to search: ";
			setTextColour(COLOUR_WHITE);
			cout << " ";
			cout << str_input;

			// Run autocomplete / autocorrect as long as string input is provided
			if (str_input != "") {
				cout << endl << endl;
				autoCompleteTrieObj.get_prefix(str_input);
			}

			//if (!stop_taking_str_input) key_input = _getch();

			// Take keyboard input
			if (!stop_taking_str_input) {
				key_input = _getch();
				if (key_input == KEY_SPACE) stop_taking_str_input = true;
				else if (str_input != "" && key_input == KEY_BACKSPACE) str_input.pop_back(); // Add character to string input
				else if (key_input != NULL && key_input != 13 && key_input != KEY_BACKSPACE && validEntry()) str_input.push_back(key_input); // Remove character when BACKSPACE is pressed

				if (key_input == KEY_ESCAPE) changeScreen(SCREEN_MAIN);
			}
			
			// Once word is entered...
			if (stop_taking_str_input) {
				setTextColour(SECONDARY_COLOUR);
				cout << endl << endl << " " << str_input << " ";
				setTextColour(COLOUR_WHITE);
				// Get definition
				cout << " Definition: " << dhtObj.getDefinition(str_input) << endl << endl;
				setTextColour(COLOUR_GREY);
				cout << endl << " Press SPACE to search again." << endl;
				cout << endl << " Press B to bookmark word." << endl;
				h.insert_word_at_last(str_input); // Add searched word to history
				key_input = _getch();
				if (key_input == KEY_SPACE) {
					str_input = ""; // Reset string input
					stop_taking_str_input = false;
				} else if (key_input == KEY_ESCAPE) changeScreen(SCREEN_MAIN); // Go back to main menu
				else if (key_input == 66 || key_input == 98) b.insert_word_at_last(str_input); // Add to bookmarks
			}

			break;

		case SCREEN_BOOKMARKS:
			setTextColour(MAIN_COLOUR);
			cout << " + -------------------------------------------------------- +" << endl;
			cout << "    DICTIONARY WITH AUTOCOMPLETE AND AUTOCORRECT             " << endl;
			cout << " + -------------------------------------------------------- +" << endl << endl;

			setTextColour(SECONDARY_COLOUR);
			cout << " Bookmarks " << endl << endl;

			setTextColour(COLOUR_WHITE);

			// Show bookmarks
			b.Display();

			setTextColour(COLOUR_GREY);
			cout << endl << " Press ESC to go back." << endl;
			
			key_input = _getch();
			if (key_input == KEY_ESCAPE) changeScreen(SCREEN_MAIN);
			
			break;

		case SCREEN_HISTORY:
			setTextColour(MAIN_COLOUR);
			cout << " + -------------------------------------------------------- +" << endl;
			cout << "    DICTIONARY WITH AUTOCOMPLETE AND AUTOCORRECT             " << endl;
			cout << " + -------------------------------------------------------- +" << endl << endl;

			setTextColour(SECONDARY_COLOUR);
			cout << " History " << endl << endl;

			setTextColour(COLOUR_WHITE);

			// Show history
			h.Display();

			setTextColour(COLOUR_GREY);
			cout << endl << " Press ESC to go back." << endl;

			key_input = _getch();
			if (key_input == KEY_ESCAPE) changeScreen(SCREEN_MAIN);
			break;
		}

		// Reset keyboard input and clear screen

		key_input = 0;
		system("cls");
	}
}

void UI::changeScreen(int id) {
	switch (id) {
	case SCREEN_MAIN:
		max_selections = sizeof(itemArray_DictMain) / sizeof(*itemArray_DictMain);
		selection = 0;
		screen = SCREEN_MAIN;
		break;

	case SCREEN_SEARCH:
	case SCREEN_BOOKMARKS:
	case SCREEN_HISTORY:
		screen = id;
		break;
	}
}

// Returns true when input is a-z, A-Z or BACKSPACE, SPACE or ESC
bool UI::validEntry(void) {
	if ((key_input >= 97 && key_input <= 122) || key_input == KEY_BACKSPACE || key_input == KEY_SPACE || key_input == KEY_ESCAPE) return true;
	else return false;
}

// ** Bookmark and history class functions ** //

void BookmarkList::insert_word_at_last(string a)
{

	BH_Node* newNode, *last;
	newNode = new BH_Node;   // New node using Dynamic memory location
	newNode->data = a;
	newNode->link = NULL;
	last = headptr;
	if (!headptr)
		headptr = newNode;
	else
	{
		while (last->link)
		{
			last = last->link;
		}
		last->link = newNode;
	}

	ofstream i;
	i.open("Bookmark.txt", ios::app);
	if (i.fail())
	{
		cerr << "Error opening File" << endl;
	}
	i << a << endl;
	i.close();
}

void BookmarkList::Display()
{
	BH_Node *temp = new BH_Node;
	temp = headptr;

	while (temp != NULL)
	{
		cout << " " << temp->data << endl;
		temp = temp->link;
	}


}

// HistoryList Class Functions

bool HistoryList::search(string a)
{
	struct BH_Node* current = headptr;  // Initialize current 
	while (current != NULL)
	{
		if (current->data == a)
			return true;
		current = current->link;
	}
	return false;
}

void HistoryList::insert_word_at_last(string a)
{
	if (h.search(a) == true)return;
	else
	{
		BH_Node *newNode, *last;
		newNode = new BH_Node;     // New node using Dynamic memory location
		newNode->data = a;
		newNode->link = NULL;
		last = headptr;
		BH_Node*temp = new BH_Node;
		temp = headptr;

		if (!headptr)
			headptr = newNode;
		else
		{
			while (last->link)
			{
				last = last->link;
			}
			last->link = newNode;
		}
		ofstream i;
		i.open("History.txt", ios::app);
		if (i.fail())
		{
			cerr << "Error opening File" << endl;
		}
		i << a << endl;
		i.close();
	}
}

void HistoryList::Display()
{
	BH_Node *temp = new BH_Node;
	temp = headptr;

	// Displays data stored in linked list
	while (temp != NULL)
	{

		cout << " " << temp->data << endl;
		temp = temp->link;
	}

}