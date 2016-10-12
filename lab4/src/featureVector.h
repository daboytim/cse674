//Derek Boytim
//CSE 674
//Lab 1
//1-21-2012
//featureVector.h
//updated 1-29-2012 for lab2
//   -added support for feature vectors to reside in memory
//   -fixed file output of Vector1: (<1234><, word(3)><>) no more ", "
//updated 2-13-2012 for lab3
//   -added support to read vectors from a file

#ifndef FEATURE_VECTOR_H
#define FEATURE_VECTOR_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <vector>
#include <string>

#include "article.h"

#define BUFFER_SIZE 8192 //8KB

using namespace std;

/**********************************************/

class V1Entry {
public:
    int articleId;
    vector<string> words;
    vector<int> wordCounts;
    vector<string> labels;
    string toString();
};

class Vector1 {
public:
    vector<V1Entry> entries;
    int write(string);
    int transactional(int, int);
};

int readVector1 (Vector1&, string, int, int);

/**********************************************/

class V2Entry {
public:
    int articleId;
    vector<int> wordCounts;
    vector<string> labels;
    string toString();
};

class Vector2 {
public:
    vector<V2Entry> entries;
    int write();
};

int readVector2 (Vector2&, string, int);

/**********************************************/

Vector1 createVector1 (map<string,int>& wordList, vector<Article>& articles) {
    cout <<"creating vector1...";
    Vector1 featureVector;
    int i, rtnval, count;
    string word;
    for (i=0; i<articles.size(); i++) {
	V1Entry entry;	
	entry.articleId = articles[i].getId();
	articles[i].setWordIteratorBegin();
	rtnval = articles[i].getNextEntry (word, count);
	while (rtnval == 0) {
	    if (wordList.find(word) != wordList.end()) {
		entry.words.insert (entry.words.end(), word);
		entry.wordCounts.insert (entry.wordCounts.end(), count);
	    }
	    rtnval = articles[i].getNextEntry (word, count);
	}	
	articles[i].setLabelIteratorBegin();
	rtnval = articles[i].getNextLabel (word);
	while (rtnval == 0) {
	    entry.labels.insert (entry.labels.end(), word);
	    rtnval = articles[i].getNextLabel (word);
	}
	featureVector.entries.insert (featureVector.entries.end(), entry);
    }
    cout <<"done\n";
    return featureVector;
}

/**********************************************/

Vector2 createVector2 (map<string,int>& wordList, vector<Article>& articles) {
    cout <<"creting vector2...";
    Vector2 featureVector;
    map<string,int>::iterator begin, end;
    string word;
    int i, rtnval, count;
    for (i=0; i<articles.size(); i++) {
	V2Entry entry;	
	entry.articleId = articles[i].getId();
	begin = wordList.begin();
	end = wordList.end();
       	for ( ; begin != end; begin++) {
	    word = (*begin).first;
	    count = articles[i].occurrences (word);
	    entry.wordCounts.insert (entry.wordCounts.end(), count);
	}
	articles[i].setLabelIteratorBegin();
	rtnval = articles[i].getNextLabel (word);
	while (rtnval == 0) {
	    entry.labels.insert (entry.labels.end(), word);
	    rtnval = articles[i].getNextLabel (word);
	}
	featureVector.entries.insert (featureVector.entries.end(), entry);
    }
    cout <<featureVector.entries.size() <<"...done\n";
    return featureVector;
}

/**********************************************/

int Vector1::write(string fileName="vector1") {
    cout <<"writing " <<fileName <<"...";
    FILE* v1f;
    v1f = fopen(fileName.c_str(), "w");
    if (v1f == NULL) {
	return 1;
    }
    string word;
    int i,j,count;
    for (i=0; i<entries.size(); i++) {
	V1Entry entry = entries[i];
	fprintf(v1f, "<%d><", entry.articleId);
	j=0;
	if (entry.words.size() > 0) {
	    word = entry.words[j];
	    count = entry.wordCounts[j];
	    fprintf (v1f, "%s(%d)", word.c_str(), count);
	    for (j++; j<entry.words.size(); j++) {
		word = entry.words[j];
		count = entry.wordCounts[j];
		fprintf (v1f, ", %s(%d)", word.c_str(), count);
	    }
	}
	fprintf (v1f, "%s", "><");
	j=0;
	if (entry.labels.size() > 0) {
	    word = entry.labels[j];
	    fprintf (v1f, "%s", word.c_str());
	    for (j++; j<entry.labels.size(); j++) {
		word = entry.labels[j];
		fprintf (v1f, ", %s", word.c_str());
	    }
	}
	fprintf (v1f, "%s", ">\n");
    }
    fclose(v1f);
    cout <<entries.size() <<"...done\n";
    return 0;
}

/**********************************************/

int Vector2::write() {
    cout <<"writing vector2...";
    FILE* v2f;
    v2f = fopen("vector2", "w");
    if (v2f == NULL) {
	return 1;
    }
    string word;
    int i,j,count;
    for (i=0; i<entries.size(); i++) {
	V2Entry entry = entries[i];
	fprintf(v2f, "<%d><", entry.articleId);
	j=0;
	if (entry.wordCounts.size() > 0) {
            count = entry.wordCounts[j];
	    fprintf (v2f, "%d", count);
	    for (j++; j<entry.wordCounts.size(); j++) {
		count = entry.wordCounts[j];
		fprintf (v2f, ", %d", count);
	    }
	}
	fprintf (v2f, "%s", "><");
	j=0;
	if (entry.labels.size() > 0) {
	    word = entry.labels[j];
	    fprintf (v2f, "%s", word.c_str());
	    for (j++; j<entry.labels.size(); j++) {
		word = entry.labels[j];
		fprintf (v2f, ", %s", word.c_str());
	    }
	}
	fprintf (v2f, "%s", ">\n");
    }
    fclose(v2f);
    cout <<entries.size() <<"...done\n";
    return 0;
}

/**********************************************/

int readVector1 (Vector1& vec1, string fileName, int lines=0) {
    cout <<"reading vector1 from file \"" <<fileName.c_str() <<"\"...";
    int cnt=0;
    FILE* v1f;
    v1f = fopen (fileName.c_str(), "r");
    if (v1f == NULL) {
	return 1;
    }
    char buffer[BUFFER_SIZE];
    if (lines == 0) {
	cnt = -1;
    }
    while (fgets (buffer, BUFFER_SIZE, v1f) != NULL && cnt < lines) {
        if (lines != 0) {
	    cnt++;
	}
	V1Entry entry;
	int i;
	//get article ID
	char id[11];
        for (i=1; buffer[i] != '>'; i++) {
	    id[i-1] = buffer[i];
	}
	id[i-1] = '\0';
	entry.articleId = atoi (id);
	i+=2; //skip '><'
	//get words and counts
	char word[40];
	char count[11];
	while (buffer[i] != '>') {
	    int j;
	    for (j=i; buffer[i] != '('; i++) {
		word[i-j] = buffer[i];
	    }
	    word[i-j] = '\0';
	    entry.words.insert (entry.words.end(), word);
	    i++; //skip '('
	    for (j=i; buffer[i] != ')'; i++) {
		count[i-j] = buffer[i];
	    }
	    count[i-j] = '\0';
	    entry.wordCounts.insert (entry.wordCounts.end(), atoi (count));
	    i++; //skip ')'
	    if (buffer[i] == ',') {
		i+=2; //skip ", "
	    }
	}
	i+=2; //skip '><'
	//get labels
	char label[40];
	while (buffer[i] != '>') {
	    int j;
	    for (j=i; buffer[i] != '>' && buffer[i] != ','; i++) {
		label[i-j] = buffer[i];
	    }
	    label[i-j] = '\0';
	    entry.labels.insert (entry.labels.end(), label);
	    if (buffer[i] == ',') {
		i+=2; //skip ", "
	    }
	}
	vec1.entries.insert (vec1.entries.end(), entry);
    }
    fclose (v1f);
    cout <<vec1.entries.size() <<"...done\n";
    return 0;
}

/**********************************************/

int readVector2 (Vector2& vec2, string fileName, int lines=0) {
    cout <<"reading vector2 from file \"" <<fileName.c_str() <<"\"...";
    int cnt=0;
    FILE* v2f;
    v2f = fopen (fileName.c_str(), "r");
    if (v2f == NULL) {
	return 1;
    }
    char buffer[BUFFER_SIZE];
    if (lines == 0) {
	cnt = -1;
    }
    while (fgets (buffer, BUFFER_SIZE, v2f) != NULL && cnt < lines) {
	if (lines != 0) {
	    cnt++;
	}
	V2Entry entry;
	int i;
	//get article ID
	char id[11];
        for (i=1; buffer[i] != '>'; i++) {
	    id[i-1] = buffer[i];
	}
	id[i-1] = '\0';
	entry.articleId = atoi (id);
	i+=2; //skip '><'
	//get word counts
      	char count[1];
	while (buffer[i] != '>') {
	    int j;
	    for (j=i; buffer[i] != '>' && buffer[i] != ','; i++) {
		count[i-j] = buffer[i];
	    }
	    count[i-j] = '\0';
	    entry.wordCounts.insert (entry.wordCounts.end(), atoi (count));
	    if (buffer[i] == ',') {
		i+=2; //skip ", "
	    }
	}
	i+=2; //skip '><'
	//get labels
	char label[40];
	while (buffer[i] != '>') {
	    int j;
	    for (j=i; buffer[i] != '>' && buffer[i] != ','; i++) {
		label[i-j] = buffer[i];
	    }
	    label[i-j] = '\0';
	    entry.labels.insert (entry.labels.end(), label);
	    if (buffer[i] == ',') {
		i+=2; //skip ", "
	    }
	}    
	vec2.entries.insert (vec2.entries.end(), entry);
    }
    fclose (v2f);
    cout <<vec2.entries.size() <<"...done\n";
    return 0;
}

/**********************************************/

string V1Entry::toString() {
    string s = "<";
    char id[6];
    sprintf(id, "%d", articleId);
    s += id;
    s += "><";
    int j=0;
    if (words.size() > 0) {
        s += words[j];
        for (j++; j<words.size(); j++) {
	    s += ", " + words[j];
            s += '(';
            sprintf(id, "%d", wordCounts[j]);
            s += id;
            s += ')';
	}
    }
    s += "><";
    j=0;
    if (labels.size() > 0) {
	s += labels[j];
	for (j++; j<labels.size(); j++) {
	    s += ", " + labels[j];
	}
    }
    s += ">";
    return s;
}
/**********************************************/

string V2Entry::toString() {
    string s = "<";
    char id[6];
    sprintf(id, "%d", articleId);
    s += id;
    s += "><";
    int j=0;
    if (labels.size() > 0) {
	s += labels[j];
	for (j++; j<labels.size(); j++) {
	    s += ", " + labels[j];
	}
    }
    s += ">";
    return s;
}

/**********************************************/

int Vector1::transactional(int begin, int end) {
    cout <<"writing vector 1 (transactional)...";
    FILE* v1f;
    v1f = fopen("vector1trans", "w");
    if (v1f == NULL) {
	return 1;
    }
    string word;
    int i,j;
    if (end > entries.size()) {
        end = entries.size();
    }
    else if (end == 0) {
        end = entries.size();
    }
    for (i=begin; i<end; i++) {
	V1Entry entry = entries[i];
	j=0;
	if (entry.words.size() > 0) {
	    word = entry.words[j];
	    fprintf (v1f, "%s", word.c_str());
	    for (j++; j<entry.words.size(); j++) {
		word = entry.words[j];
		fprintf (v1f, " %s", word.c_str());
	    }
	}
        for (j=0; j<entry.labels.size(); j++) {
            word = entry.labels[j];
            fprintf (v1f, " %s", word.c_str());
        }
	fprintf (v1f, "\n", NULL);
    }
    fclose(v1f);
    cout <<end-begin <<"...done\n";
    return 0;
}

#endif
