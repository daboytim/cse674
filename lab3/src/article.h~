//Derek Boytim
//CSE 674
//Lab 1
//1-18-2012
//article.h

#ifndef ARTICLE_H
#define ARTICLE_H

#include <map>
#include <set>
#include <string>
using namespace std;


class Article {
    int uniqueId;
    map<string,int> wordList;
    set<string> classLabels;
    map<string,int>::iterator wordIt;
    set<string>::iterator labelIt;
public:
    Article (int);
    int getId ();
    void addWord (string, int);
    void removeWord (string);
    void addLabel (string);
    int size ();
    int occurrences (string);
    int numLabels ();
    void setWordIteratorBegin ();
    void setLabelIteratorBegin ();
    int getNextEntry (string&, int&);
    int getNextLabel (string&);
};

Article::Article (int uId) {
    uniqueId = uId;
}

int Article::getId () {
    return uniqueId;
}

void Article::addWord (string word, int weight=1) {
    wordList[word]+=weight;
}

void Article::removeWord (string word) {
    wordList.erase (word);
}

void Article::addLabel (string label) {
    classLabels.insert (classLabels.end(), label);
}

int Article::size () {
    return (int) wordList.size();
}

int Article::occurrences (string word) {
    int occurrences;
    occurrences = wordList[word];
    if(occurrences == 0) {
	wordList.erase (word);
    }
    return occurrences;
}

int Article::numLabels () {
    return (int) classLabels.size();
}

void Article::setWordIteratorBegin () {
    wordIt = wordList.begin();
}

void Article::setLabelIteratorBegin () {
    labelIt = classLabels.begin();
}

int Article::getNextEntry (string& word, int& count) {
    if (wordIt != wordList.end()) {
	word = (*wordIt).first;
	count = (*wordIt).second;
	wordIt++;
	return 0;
    }
    return 1;
}

int Article::getNextLabel (string& word) {
    if (labelIt != classLabels.end()) {
	word = (*labelIt);
	labelIt++;
	return 0;
    }
    return 1;
}

#endif
