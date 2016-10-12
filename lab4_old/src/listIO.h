//Derek Boytim
//CSE 674
//Lab1/2
//1-29-2012

#ifndef LISTIO_H
#define LISTIO_H

#include <map>
#include <set>
#include <string>

using namespace std;

/**********************************************/

int loadStopWords (set<string>& stopWords) {
    FILE* fdStopWords;
    char word[20];
    printf ("%s", "loading stop words...");
    fdStopWords = fopen ("stopWords.lst", "r");
    if (fdStopWords == NULL) {
	printf ("%s\n", "error");
	return 1;
    }
    
    fscanf(fdStopWords, "%s", word);
    while (feof(fdStopWords) == 0) {
	stopWords.insert ( string (word) );
        fscanf (fdStopWords, "%s", word);
    }
    printf ("%d...", stopWords.size());
    printf ("%s\n", "done");
    fclose (fdStopWords);
    
    return 0;
}

/**********************************************/

int loadIgnoreWords (set<string>& ignoreWords) {
    FILE* fdIgnoreWords;
    char word[20];
    printf ("%s", "loading ignore words...");
    fdIgnoreWords = fopen ("ignoreWords.lst", "r");
    if (fdIgnoreWords == NULL) {
	printf ("%s\n", "error");
	return 1;
    }
    
    fscanf(fdIgnoreWords, "%s", word);
    while (feof(fdIgnoreWords) == 0) {
	ignoreWords.insert ( string (word) );
        fscanf (fdIgnoreWords, "%s", word);
    }
    printf ("%d...", ignoreWords.size());
    printf ("%s\n", "done");
    fclose (fdIgnoreWords);
    
    return 0;
}

/**********************************************/

int writeIgnoreWords(set<string>& ignoreWords) {
    cout <<"writing ignoreWords...";
    FILE* iwf;
    iwf = fopen("ignoreWords.lst", "w");
    if (iwf == NULL) {
	cout <<"error\n";
	return 1;
    }
    int i;
    set<string>::iterator it = ignoreWords.begin();
    for (i=0; i<ignoreWords.size(); i++) {
        fprintf(iwf, "%s\n", (*it).c_str());
	it++;
    }
    fclose(iwf);
    cout <<"done\n";
}

/**********************************************/

int writeWordList(map<string,int>& wordList) {
    cout <<"writing wordList...";
    FILE* iwf;
    iwf = fopen("importantWords.lst", "w");
    if (iwf == NULL) {
	cout <<"error\n";
	return 1;
    }
    int i;
    map<string,int>::iterator it = wordList.begin();
    for (i=0; i<wordList.size(); i++) {
        fprintf(iwf, "%s\n", (*it).first.c_str());
	it++;
    }
    fclose(iwf);
    cout <<"done\n";
}

/**********************************************/

#endif
