#define _CRT_SECURE_NO_WARNINGS_ 
#include<string>
#include<iostream>
#include<iomanip>
#include <Windows.h>
#include<locale>
#include <fstream>
#include"dirent.h"
using namespace std;

class CHRTimer
{
protected:
	LARGE_INTEGER m_liStart;
	LARGE_INTEGER m_liStop;
public:
	CHRTimer(void){
		m_liStart.QuadPart = m_liStop.QuadPart = 0;
	}

	~CHRTimer(void){}

	// Starts the timer
	void Start(){
		::QueryPerformanceCounter(&m_liStart);
	}

	// Stops the timer
	void Stop(){
		::QueryPerformanceCounter(&m_liStop);
	}

	// Returns the counter at the last Start()
	LONGLONG GetStartCounter(){
		return m_liStart.QuadPart;
	}

	// Returns the counter at the last Stop()
	LONGLONG GetStopCounter(){
		return m_liStop.QuadPart;
	}

	// Returns the interval between the last Start() and Stop()
	LONGLONG GetElapsed(){
		return (m_liStop.QuadPart - m_liStart.QuadPart);
	}

	// Returns the interval between the last Start() and Stop() in seconds
	double GetElapsedAsSeconds(){
		LARGE_INTEGER liFrequency;
		::QueryPerformanceFrequency(&liFrequency);
		return ((double)GetElapsed() / (double)liFrequency.QuadPart);
	}
};

//for print menu some other classes i have used
template<typename charT, typename traits = std::char_traits<charT> >
class center_helper {
	std::basic_string<charT, traits> str_;
public:
	center_helper(std::basic_string<charT, traits> str) : str_(str) {}
	template<typename a, typename b>
	friend std::basic_ostream<a, b>& operator<<(std::basic_ostream<a, b>& s, const center_helper<a, b>& c);
};

template<typename charT, typename traits = std::char_traits<charT> >
center_helper<charT, traits> centered(std::basic_string<charT, traits> str) {
	return center_helper<charT, traits>(str);
}
// redeclare for std::string directly so we can support anything that implicitly converts to std::string
center_helper<std::string::value_type, std::string::traits_type> centered(const std::string& str) {
	return center_helper<std::string::value_type, std::string::traits_type>(str);
}

template<typename charT, typename traits>
std::basic_ostream<charT, traits>& operator<<(std::basic_ostream<charT, traits>& s, const center_helper<charT, traits>& c) {
	std::streamsize w = s.width();
	if (w > c.str_.length()) {
		std::streamsize left = (w + c.str_.length()) / 2;
		s.width(left);
		s << c.str_;
		s.width(w - left);
		s << "";
	}
	else {
		s << c.str_;
	}
	return s;
}


class FileNameNode {
public:
	string fileName;
	FileNameNode *next;
	FileNameNode(const string fname) :fileName(fname), next(NULL) {}
};
class WordTypeNode {
public:
	string word;
	int wordFreq;
	FileNameNode *fileListHead;
	WordTypeNode *next;
	WordTypeNode():next(NULL),fileListHead(NULL){}
};

class IndexLinkedList {
private:
	WordTypeNode *head;
public:
	IndexLinkedList() :head(NULL) {}
	void addWord(const string &word, const string &fileName) {
		if (head == NULL) {//If list is empty the first word will be head, this will only happen once
			head = new WordTypeNode();
			head->word = word;
			head->fileListHead = new FileNameNode(fileName);
			head->wordFreq = 1;
			return;
		}
		WordTypeNode *walk = head, *prev = walk;
		FileNameNode *fileWalk, *filePrev;
		while (walk != NULL) {//If we find the word in this list add new nodes to filenames
			fileWalk = walk->fileListHead;
			filePrev = fileWalk;
			bool uniqueFile = true;
			if (walk->word == word) {//This equality can only happen once so if satisfied return
				walk->wordFreq++;
				while (fileWalk != NULL) {
					if (fileWalk->fileName == fileName) {//if the word is repeated in same file do not add it to fileList
						uniqueFile = false;
						break;
					}
					filePrev = fileWalk;
					fileWalk = fileWalk->next;
				}
				if(uniqueFile)
					filePrev->next = new FileNameNode(fileName);
				return;
			}
			prev = walk;
			walk = walk->next;
		}
		//If not found any equality create a new word node
		prev->next = new WordTypeNode();
		prev->next->word = word;
		prev->next->fileListHead = new FileNameNode(fileName);
		prev->next->wordFreq = 1;
	}

	void printListOfFiles(const string &singleWordQuery)const {
		WordTypeNode *walkWord = head;
		while (walkWord != NULL) {
			if (walkWord->word == singleWordQuery) {
				FileNameNode *walkFile = walkWord->fileListHead;
				cout << "\nList of files containing the word " << singleWordQuery << " is:" << endl;
				int i = 1;
				while (walkFile != NULL) {
					cout << i << ". " << walkFile->fileName << endl;
					i++; 
					walkFile = walkFile->next;
				}
				return;
			}
			walkWord = walkWord->next;
		}
		cout << "\nWord " << singleWordQuery << " does not exist in any file!" << endl;
	}

	void printMostFrequentWords(int topWhat) {
		cout << "Top " << topWhat << " most frequent words:" << endl;
		WordTypeNode *walk, *most_freq, *prev_most;
		walk = head;
		most_freq = head;
		while (walk != NULL) {//find the first most frequent one and print it 
			if (most_freq->wordFreq < walk->wordFreq) {
				most_freq = walk;
			}
			walk = walk->next;
		}
		prev_most = most_freq;
		cout << most_freq->word << " is repeated " << most_freq->wordFreq << " times." << endl;
		

		for (int i = 0; i < topWhat - 1; i++) {
			walk = head;
			most_freq = head;
			
			while (walk != NULL) {
				if ((most_freq->wordFreq < walk->wordFreq) && (walk->wordFreq < prev_most->wordFreq)) {
					most_freq = walk;
				}
				walk = walk->next;
			}
			prev_most = most_freq;
			cout << most_freq->word << " is repeated " << most_freq->wordFreq << " times." << endl;
		}
	}

	void printLeastFrequentWords(int topWhat) {
		cout << "Top " << topWhat << " least frequent words:" << endl;
		WordTypeNode *walk, *least_freq, *prev_least;
		walk = head;
		least_freq = head;
		while (walk != NULL) {//find the first least frequent one and print it 
			if (least_freq->wordFreq > walk->wordFreq) {
				least_freq = walk;
			}
			walk = walk->next;
		}
		prev_least = least_freq;
		cout << least_freq->word << " is repeated " << least_freq->wordFreq << " times." << endl;

		for (int i = 0; i < topWhat - 1; i++) {
			walk = head;
			least_freq = head;

			while (walk != NULL) {
				if ((least_freq->wordFreq > walk->wordFreq) && (walk->wordFreq > prev_least->wordFreq)) {
					least_freq = walk;
				}
				walk = walk->next;
			}
			prev_least = least_freq;
			cout << least_freq->word << " is repeated " << least_freq->wordFreq << " times." << endl;
		}


	}
	/* other private helper methos or public method that you may wish to add */
	bool isEmpty() const {
		return head == NULL;
	}

};

void getValidOption(int &option) {
	do {
		cin >> option;
	} while (!(option < 5 && option>0));
}

int  printDecoratedMenu(const string &indexCreationDuration) {
	string idxCreationDurationCopy = indexCreationDuration;
	idxCreationDurationCopy.append(" Minutes");

	for (int i = 0; i < 70; i++) {//top
		cout << "=";
	}
	cout << endl;
	cout << "=" << setw(50) << "Simple Document Retrieval System";
	cout << setw(19) << "=" << endl;
	cout << "=" << setw(68) << centered(idxCreationDurationCopy);
	cout << "=" << endl;

	for (int i = 0; i < 70; i++) {//middle
		cout << "=";
	}
	cout << endl;

	cout << "=" << "   " << "1. Enter a single keyword to list the document(s)(file names)" << "    " << "=" << endl;
	cout << "=" << "   " << "2. Print the top 10 words that appeared most frequently" << "          " << "=" << endl;
	cout << "=" << "   " << "3. Print the top 10 words that appeared least frequently" << "         " << "=" << endl;
	cout << "=" << "   " << "4. Exit" << setw(59) << "=" << endl;

	for (int i = 0; i < 70; i++) {//bottom
		cout << "=";
	}
	cout << endl;

	cout << "Option:";
	int option;
	getValidOption(option);
	return option;
}

void createIndex(IndexLinkedList &index, const string directoryOfFiles) {

	cout << "Processing..." << endl;
	const char *directoryOfFilesAsChar = directoryOfFiles.c_str();//dirent does not accept string argument
	struct dirent *entry;
	DIR *dir = opendir(directoryOfFilesAsChar);
	if (dir == NULL) {
		cout << "\nDirectory is empty." << endl;
		return;
	}

	while ((entry = readdir(dir)) != NULL) {
		string fileName = entry->d_name;//addWord accepts only string type argument
		if (fileName != "." && fileName != "..") {
			cout << "Currently reading " << fileName << endl;
			ifstream file;
			//string path = "C:/Users/CASPER/source/repos/CENG218_Project1/CENG218_Project1/AllDocs/" + fileName;//buna general çözüm
			string path = directoryOfFiles + "/" + fileName;
			file.open(path.c_str());
			string word;
			char ch;
			word.clear();
			while (file >> word) {
				bool flag = false;
				for (int j = 0; j < word.size(); j++) {
					char ch = word[j];
					if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'))
						flag = true;
					else
						word.erase(j);
				}
				if (flag) {
					if(!word.empty())
						index.addWord(word, fileName);
				}
			}
			file.close();
		}
	}
}

int main() {
	//string dirName = "C:\\Users\\CASPER\\source\\repos\\CENG218_Project1\\CENG218_Project1\\AllDocs\\";
	string dirName = "AllDocs";
	IndexLinkedList index;
	string idxCreationTime;
	CHRTimer timer;
	timer.Start();
	createIndex(index, dirName);
	timer.Stop();
	double timeAsMinute= timer.GetElapsedAsSeconds()/60;
	idxCreationTime = to_string(timeAsMinute);
	int opt;

	do {
		opt = printDecoratedMenu(idxCreationTime);
		switch (opt) {
		case 1: {
			string queryWord;
			//prompt user to enter single query word 
			cout << "Enter a keyword to search among all documents:";
			cin >> queryWord;
			index.printListOfFiles(queryWord);
			break;
		}
		case 2: index.printMostFrequentWords(10); break;
		case 3: index.printLeastFrequentWords(10); break;
		}
	} while (opt != 4);
	cout << "\nThanks for choosing our document retrieval system !" << endl;
	cout << "Goodbye..." << endl;

	return 0;
}