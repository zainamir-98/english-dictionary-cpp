#include <iostream>
#include <string> // For unlocking string variables
#include <windows.h> // For coloured text
#include <conio.h> // For _getch() function
#include <fstream> // For file reading

using namespace std;

#define DICTIONARY_HASHTABLE_SIZE 5021

#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_LEFT 75
#define KEY_RIGHT 77
#define KEY_SPACE 32
#define KEY_BACKSPACE 8
#define KEY_ESCAPE 27

#define MAIN_COLOUR 11
#define SECONDARY_COLOUR 224
#define COLOUR_WHITE 15
#define COLOUR_GREY 7
#define COLOUR_YELLOW 14

#define SCREEN_MAIN 0
#define SCREEN_SEARCH 1
#define SCREEN_BOOKMARKS 2
#define SCREEN_HISTORY 3

/*
176 Blue BG
160 Green BG
14 Yellow text
224 Yellow BG
*/

// ** HASH TABLE FOR DICTIONARY ** //

// Dictionary Hash Table Node
class DHT_Node {
public:
	string word;
	string def;
	DHT_Node* next;

	DHT_Node() : word(""), def(""), next(NULL) {};
	DHT_Node(string w, string df) : word(w), def(df), next(NULL) {};
};

// Dictionary Hash Table
class DHT {
	DHT_Node* dictArray[DICTIONARY_HASHTABLE_SIZE]; // Array of DHT node pointers
	unsigned int words_entered; // Number of words entered
	 
public:
	DHT();
	void insert(string w, string df); // Insert a word
	int hashFunction(string s); // Hash function
	string getDefinition(string w); // Returns the definition associated with a given word
	void removeWord(string w); // Removes a word from the dictionary
	void loadWordsFromFile(void); // Load words from .txt file
	void display(void); // DEBUG
};

// ** TRIE FOR AUTOCOMPLETE / AUTOCORRECT ** //

class TrieNode
{
public:
	int value;
	TrieNode* children[26];               //For All 26 alphabets
	TrieNode* pNext;
	char node_char;

	TrieNode()
	{
		value = 0;
		for (int i = 0; i < 26; i++)            //default constructor
		{
			children[i] = NULL;
		}
	}
	TrieNode(char Letter)                        //parametried constructor
		: node_char(Letter) {
		for (int i = 0; i < 26; i++)
		{
			children[i] = NULL;
		}
	}
	~TrieNode() {};
};

class Trie
{
	friend class TrieNode;              //made class node friend to access variables
private:
	TrieNode* root;
	int Counter;
public:
	Trie()
	{
		root = new TrieNode();
		Counter = 0;
	}
	~Trie() {};

	void import_txt()          //function for imporitng dictionary file
	{
		int Counter = 0;
		string term;

		ifstream get_file("EnglishDictionary.txt");

		if (!get_file)
		{
			cout << "Couldn't open file" << endl;
			return;
		}

		int i = 0;

		while (!get_file.eof())
		{
			string word;
			getline(get_file, term);
			//cout << term << "\n";

			if (term != "") {
				string::iterator j;
				j = term.begin();

				while (j != term.end() && *j != ' ') {
					word.push_back(*j);
					++j;
				}

				++i;
			}

			insert(word);
			Counter++;


			if (Counter >= 28102)
			{
				break;
			}
		}

		//cout << "Inserted " << Counter << " words\n" << endl;
		get_file.close();
	}


	void insert(string term)        //For inserting data from text file into trie class
	{
		TrieNode *p = root;
		long int len = term.size();
		for (int x = 0; x < len; x++)
		{
			int i = term[x] - 'a';
			//cout << i << endl;
			if (i < 0)
			{
				//	cout << "Term " << term << " cannot be inserted.  No numbers or symbols allowed!" << endl;
				return;
			}

			if (!p->children[i])
			{
				p->children[i] = new TrieNode(term[x]);
			}
			p = p->children[i];
		}
		Counter++;
		p->value = Counter;
	}

	bool search(string key)    //function for searching a word 
	{
		TrieNode *p = root;
		long int len = key.size();
		for (int x = 0; x < len; x++)
		{
			int i = key[x] - 'a';
			if (p->children[i])                  //if words matched
			{
				p = p->children[i];
			}
			else
			{
				//	cout << "String not found!\n" << endl;            //if words didnt match
				return false;
			}
		}
		//cout << "String " << key << " successfully found\n" << endl;
		if (p->value != 0)
		{
			return true;
		}
		else
		{
			return false;
		}

	}

	bool get_prefix(string prefix) {              //For finding possible combinations
		TrieNode *p = root;
		long int len = prefix.size();
		for (int x = 0; x < len; x++) {
			int i = prefix[x] - 'a';
			if (p->children[i]) {
				p = p->children[i];
			}

			else {
				string prefix1 = prefix.substr(0, prefix.length() - 1);            //If entered wrong word then remove last alphabet and check again and repeat this until it finds a word.
				if (!prefix1.empty())                  //if string is not empty
				{
					if (get_prefix(prefix1))
						return true;
				}
				else {
					//cout << "Prefix invalid!" << endl;
					return false;

				}
			}
		}
		bool y = true;
		for (int z = 0; z < 26; ++z)
			if (p->children[z] != NULL) y = false;

		if (!y) cout << " Do you mean..." << endl << endl;
		dfs(p, prefix);
		return true;
	}

	void dfs(TrieNode* p, string prefix)      //traversing common path
	{
		int y = 0;
		while (y < 26)
		{
			if (p->children[y] != NULL)
			{
				dfs2(p->children[y], prefix);
			}
			y++;
		}
		cout << endl;
	}

	void dfs2(TrieNode* p, string prefix)    //printing all combinations
	{

		cout << " " << prefix;
		cout << p->node_char;

		while (p != NULL)
		{
			int y = 0;
			while (p->children[y] == NULL && y < 26)
			{
				y++;
			}
			p = p->children[y];

			if (p != NULL)
			{
				cout << p->node_char;
			}
		}
		cout << endl;
	}
};

// ** User interface ** //

class UI {
	int selection;
	int max_selections;
	int key_input;
	string str_input;
	bool stop_taking_str_input;
	unsigned int screen = SCREEN_MAIN;
	HANDLE hConsole;
	bool loop = true;

	DHT dhtObj;
	Trie autoCompleteTrieObj;

public:
	UI();
	void process(void);
	void setTextColour(int);
	void changeScreen(int);
	void showMenu(string* strArr);
	bool validEntry(void);
};

// ** Bookmarks ** //

// Common node for both bookmark and history classes
struct BH_Node    // Structure to declare node
{
	string data;
	BH_Node *link;                               //  Pointer of structure type to make another node for storing data and address of node
};

class BookmarkList
{
	BH_Node * headptr;                 // Head pointer for the first node
public:
	BookmarkList()
	{
		headptr = 0;
	}

	void insert_word_at_last(string a);
	void Display();

};

// History Class 
class HistoryList
{
	BH_Node * headptr;                 // Head pointer for the first node
public:
	HistoryList()
	{
		headptr = 0;
	}
	void insert_word_at_last(string a);
	bool search(string a);
	void Display();
};