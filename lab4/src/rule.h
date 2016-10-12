//Derek Boytim
//CSE 674
//Lab 4
//3-4-2012
//rule.h

#ifndef RULE_H
#define RULE_H

#include <iostream>
#include <set>
#include <vector>
#include <string>
#include <fstream>
#include <cstdlib>
#include <stdio.h>
#include <map>

using namespace std;

class Rule {
public:
    string consequent;
    set<string> antecedent;
    double support;
    double confidence;
    string toString ();
};

string Rule::toString () {
    string s = consequent;
    s += " <-";
    set<string>::iterator it = antecedent.begin();
    for ( ; it != antecedent.end(); it++) {
        s += " " + (*it);
    }
    s += " (";
    char val[13];
    sprintf(val, "%f", support);
    s += val;
    s += ", ";
    sprintf(val, "%f", confidence);
    s += val;
    s += ")";
    return s;
}

int loadRules (vector<Rule>& rules, set<string>& labelList, set<string>& wordList, string& best) {
    printf ("%s", "loading rules...");
    fstream fsRules ("rules.lst", fstream::in);
    if (fsRules == NULL) {
	printf ("%s\n", "error");
	return 1;
    }
    int pos1, pos2, posEnd;
    string line, label, word;
    set<string>::iterator it;
    map<string, int> labels;
    bool goodRule = true;
    getline (fsRules, line);
    while (!fsRules.eof()) {
        pos1 = line.find (" ");
        label = line.substr(0, pos1);
        it = labelList.find (label);
        if (it != labelList.end()) {
            //valid consequent
            labels[label]++;
            set<string> words;
            pos1 = line.find (" ", pos1+1) + 1;
            //pos1 now points to first char of first word in antecedent
            posEnd = line.find ("(", pos1);
            while (pos1 < posEnd) {
                pos2 = line.find (" ", pos1);
                word = line.substr (pos1, pos2-pos1);
                pos1 = pos2+1;
                it = wordList.find (word);
                if (it == wordList.end()) {
                    goodRule = false;
                    break;
                }
                words.insert (word);
            }
            posEnd++;
            pos1 = line.find (",", posEnd);
            double sup = atof (line.substr(posEnd, pos1-posEnd).c_str());
            pos1 += 2;
            pos2 = line.find (")", pos1);
            double con = atof (line.substr(pos1, pos2-pos1).c_str());
            if (goodRule) {
                Rule rule;
                rule.consequent = label;
                rule.antecedent = words;
                rule.support = sup;
                rule.confidence = con;
                rules.push_back (rule);
            }
            goodRule = true;
        }
        getline (fsRules, line);
    }
    map<string, int>::iterator it2 = labels.begin();
    int max=0;
    for ( ; it2 != labels.end(); it2++) {
        if ((*it2).second > max) {
            max = (*it2).second;
            best = (*it2).first;
        }
    }
    printf ("%d...", rules.size());
    printf ("%s\n", "done");
    fsRules.close();

    return 0;
}

void sortRules (vector<Rule>& rules) {
    printf ("%s", "sorting rules...");
    int i,j, maxIndex=0;
    double max;
    //sort by support
    for (i=0; i<rules.size()-1; i++) {
        max=0.0;
        for (j=i; j<rules.size(); j++) {
            if (rules[j].support > max) {
                max = rules[j].support;
                maxIndex = j;
            }
        }
        if (i != maxIndex) {
            Rule tmp = rules[i];
            rules[i] = rules[maxIndex];
            rules[maxIndex] = tmp;
        }
    }
    //sort equal supports by confidence
    for (i=0; i<rules.size(); ) {
        int beginSupIndex=i, endSupIndex;
        for (++i; rules[i].support == rules[beginSupIndex].support; i++) {}
        for (endSupIndex=i; beginSupIndex < endSupIndex; beginSupIndex++) {
            max=0.0;
            for (j=beginSupIndex; j<endSupIndex; j++) {
                if (rules[j].confidence > max) {
                    max = rules[j].confidence;
                    maxIndex = j;
                }
            }
            if (beginSupIndex != maxIndex) {
                Rule tmp = rules[beginSupIndex];
                rules[beginSupIndex] = rules[maxIndex];
                rules[maxIndex] = tmp;
            }
        }
    }

    printf ("%s\n", "done");
}

#endif
