#include <iostream>
#include <fstream>
#include <sstream>
#include <cctype>
#include <cstring>

using namespace std;

/*
1. Read the name of the text file from the console. 
2. Read in a text file, not all at once. (This can be line by line, word by word or character by character.) 
3. The file content must be converted to a sequence of words, discarding punctuation and folding all letters into lower case.
4. Store the unique words and maintain a count of each different word. 
5. The words should be ordered by decreasing count and, if there are multiple words with the same count, alphabetically. (This ordering may be achieved as the words are read in, partially as the words are read or at the end of all input processing.)
6. Output the total number of words and the number of “unique words” 
7. Output the first fifteen words in the sorted list, along with their counts. 
8. Output the last fifteen words in the list, along with their counts.

REF QUICKSORT/PARTITION = https://www.geeksforgeeks.org/quick-sort/
*/

string lower(string word);
string removePunct(string line);

//Word class
class Word{
  private:
    string word;
    int count=1;
  public:
    Word(){this->count=0;};
    Word(string word){this->word = word;};
    bool isValid();
    void increaseCount(){this->count++;};
    string getString(){return this->word;};
    int getCount(){return this->count;};
};

//UniqueWords
class UniqueWords{
  private:
    Word words[50000];
    int currentIndex;
    int totalWordsAttempted;
  public:
    UniqueWords(){this->currentIndex = 0; this->totalWordsAttempted = 0;};
    bool isUnique(Word word);
    void addUnique(Word word);
    void quickSort(int low, int high);
    void quickSortAlphabetical(int low, int high);
    int partition(int low, int high); 
    int partitionAlphabetical(int low, int high);
    void sort();
    void reverse();
    void printResult();
    void displayAsc();
    void displayDesc();
    void swap(int n, int m);
    int getTotalWords(){return totalWordsAttempted;};
    int getTotalUniqueWords(){return currentIndex;};
};

int main () {
    string filename;
    string line;
    string word;
    UniqueWords u;
    
    cout << "Enter Filename: " << endl;
    cin >> filename;
    //ifstream myfile ("sample-short.txt");
    ifstream myfile (filename);
    if (myfile.is_open())
    {
        while ( getline (myfile, line) )
        {
            stringstream ss(removePunct(line));
            while(getline(ss, word, ' ')){
                //cout << lower(word) << " ";
                Word w(lower(word));
                u.addUnique(w);
            }
        }
        myfile.close();
    }
    else cout << "Unable to open file";

    //u.printResult();
    //cout << "----------------------" << endl;

    u.sort();
    u.printResult();

    return 0;
}

//Sort the words array by count then alphabetically
void UniqueWords::sort(){
  this->quickSort(0, this->currentIndex-1);
  int change = 1;
  int left = 0;
  int right = 0;
  for(int i = 0; i< this->currentIndex; i++){
    if(change != this->words[i].getCount()){
      //cout << " Change:"  << change << " words[i].getCount() "  << words[i].getCount() << ", i: " << i << endl;
      change = this->words[this->currentIndex].getCount();
      left = right;
      right = i;
      this -> quickSortAlphabetical(left, right-1);
    }
  }
  this->reverse();
}

//Reverse the order of the array
void UniqueWords::reverse(){
  Word temp;
  for(int i = 0, j = this->getTotalUniqueWords()-1; i < this->getTotalUniqueWords()/2; i++, j--){
    temp = words[i];
    words[i] = words[j];
    words[j] = temp;
  }
  return;
}

//Quicksort for count
void UniqueWords::quickSort(int low, int high){
  if (low < high)
    {
      int p = this->partition(low, high);
      this->quickSort(low, p - 1);
      this->quickSort(p + 1, high);
    }
}

//Partion on count
int UniqueWords::partition(int low, int high) 
{ 
  int pivot = this->words[high].getCount();
  int i = (low - 1);
  for (int j = low; j <= high - 1; j++) 
  { 
    if (this->words[j].getCount() < pivot) 
    { 
        i++;
        this->swap(i, j); 
    } 
  } 
  this->swap(i + 1, high); 
  return (i + 1); 
} 

//quicksort for alphabetical words
void UniqueWords::quickSortAlphabetical(int low, int high){
  if (low < high)
    {
      int p = this->partitionAlphabetical(low, high);
      this->quickSortAlphabetical(low, p - 1);
      this->quickSortAlphabetical(p + 1, high);
    }
}

//Partition alphabetically used in quicksort
int UniqueWords::partitionAlphabetical(int low, int high) 
{ 
  string pivot = this->words[high].getString(); 
  int i = (low - 1); 
  for (int j = low; j <= high - 1; j++) 
  { 
    if (this->words[j].getString() < pivot) 
    { 
        i++; 
        this->swap(i, j); 
    } 
  } 
  this->swap(i + 1, high); 
  return (i + 1); 
} 

//Adds unique, valid words to the array and increases the currentIndex
void UniqueWords::addUnique(Word word){
    if(!word.isValid()){
      return;
    }
      this->totalWordsAttempted++;
    if(this->isUnique(word)){
      this->words[this->currentIndex] = word;
      this->currentIndex++;
    }
}

//Checks if the word is unique and increases the count if it isnt
bool UniqueWords::isUnique(Word word){
    for(int i=0; i<this->currentIndex; i++){
        if(this->words[i].getString() == word.getString()){
            this->words[i].increaseCount();
            return false;
        }
    }
    return true;
}

//Checks if a word is valid
bool Word::isValid(){
  if(this->getString().length() > 0){
    return true;
  }
  return false;
}

//Prints the words and count from the words array
void UniqueWords::printResult(){
    //print all
    /*for(int i = 0 ; i < this->currentIndex ; i++ ){
      cout << this->words[i].getString() << ":" << this->words[i].getCount() << endl;
    }*/
    cout << "Total Words Attempted: " << this->getTotalWords() << ", Total Unique Words: " << this->getTotalUniqueWords() << endl; 
    cout << "\n First Fifteen---------" << endl;
    for(int i = 0 ; i < 15 ; i++ ){
      cout << this->words[i].getString() << ":" << this->words[i].getCount() << endl;
    }
    cout << "\n Last Fifteen------------ " << endl;
    for(int i = this->currentIndex ; i >= this->currentIndex-15 ; i-- ){
      cout << this->words[i].getString() << ":" << this->words[i].getCount() << endl;
    }
}

//Used to swap words within the array for sorting
void UniqueWords::swap(int n, int m){
  Word temp = this->words[n];
  this->words[n] = this->words[m];
  this->words[m] = temp;
}

//Used to convert to lowercase
string lower(string word){
    const char* temp = word.c_str();
    char newWord[30];
    strcpy(newWord, temp);
    for(int i=0; i<strlen(newWord);i++){
        //cout << (int)newWord[i] << endl; 
        if((int)newWord[i]>=65 && (int)newWord[i]<=90){
            newWord[i] += 32;
        }
    }
    return newWord;
}

//Remove punctuation
string removePunct(string line){
  for (int i = 0, length = line.size(); i < length; i++)
    {
      if (ispunct(line[i])) //|| (int)line[i] < 65 )
      {
        line.erase(i--, 1);
        length = line.size();
      }
    }
    return line;
}




