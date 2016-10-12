/**********************************************/

bool readTaggedSectionByLine (FILE* file, Article article, int index, int weight=1) {
    string str;
    int pos1, pos2, size;
    char word[20], line[200];
    
    str = string (fgets(line, 200, file));
    pos1 = str.find (openTags[index]);
    if (pos1 < 0) {
	return false;
    }
    else { //found openTag[i]
	pos1 = str.find (openTags[13]); //<D>
	while (pos1 > 0) { //found <D>
	    pos2 = str.find (closeTags[13], pos1);
	    size = str.copy (word, pos2-pos1, pos1);
	    word[size] = '\0';
	    if (index == 2) {
		topicsList.insert (string (word));
		article.addLabel (string (word));
	    }
	    wordList[string (word)] += weight;
	    pos1 = str.find (openTags[13], pos2);
	}
	pos1 = str.find (closeTags[index]);
	if (pos1 < 0) {
	    return false;
	}
    }
    return true; //success
}

/**********************************************/

bool readTaggedSectionByWords (FILE* file, Article article, int index, int weight=1) {
    string str;
    int pos1, i;
    char word[80];

    if (leftovers.size() > 0) {
	leftovers.swap(str);
    }
    else {
	fscanf (file, "%s", word);
	str = string (word);
    }
    //cout <<"initilized str" <<endl;
    pos1 = str.find (openTags[index]);
    while (pos1 < 0) {
	fscanf (file, "%s", word);
	str = string (word);
	pos1 = str.find (openTags[index]);
    }
    str.replace(pos1, openTags[index].size(), "");
    if (str.size() > 0) {
	pos1 = str.find (closeTags[index]);	
	if (pos1 < 0) {
	    addWord(article, str, weight);
	}else {
	    str.replace(pos1, closeTags[index].size(), "");
	    if (str.size() > 0) {
		//could be next Tag
		//fseek (file, -1*str.size(), SEEK_CUR);
		return false;
	    }
	    return true;
	}
    }
    //cout <<"found openTag[" <<index <<"]" <<endl;
    fscanf (file, "%s", word);
    str = string (word);
    pos1 = str.find (closeTags[index]);
    //cout <<str <<"\n" <<pos1 <<endl;
    while (pos1 < 0) {
	addWord(article, str, weight);
	fscanf (file, "%s", word);
	str = string (word);
	pos1 = str.find (closeTags[index]);
	//cout <<str <<"\n" <<pos1 <<endl;
    }
    if (pos1 > 0) {
	int len = str.copy(word, pos1, 0);
	word[len] = '\0';
	string tmp (word);
	addWord(article, tmp, weight);
	str.replace(0, pos1, "");
    }
    //cout <<str <<'\n' <<pos1 <<endl;
    str.replace(0, closeTags[index].size(), "");
    //cout <<"found closeTag" <<endl;
    str.swap(leftovers);
    return true; //success
}

/**********************************************/

int parseArticle (const char* fileName) {
    FILE* file;
    int bytesRead, pos1, pos2, size;
    char word[200];
    string str;
    //cout <<"trying to parse" <<endl;
  //open the file
    file = fopen ((string(dir)+fileName).c_str(), "rb");
    if (file == NULL) {
	return 1;
    }
    do {
	//find the first tag "<REUTERS"
	fscanf (file, "%s", word);
	str = string (word);
	pos1 = str.find (openTags[0]);
	//cout <<str <<'\n' <<pos1 <<endl;
	while (pos1 < 0) {
	    fscanf (file, "%s", word);
	    str = string (word);
	    pos1 = str.find (openTags[0]);
	    //cout <<str <<'\n' <<pos1 <<endl;
	}
	//found "<REUTERS"
	
	//find "NEWID"
	fscanf (file, "%s", word);
	str = string (word);
	pos1 = str.find (newid);
	//cout <<str <<'\n' <<pos1 <<endl;
	while (pos1 < 0) {
	    fscanf (file, "%s", word);
	    str = string (word);
	    pos1 = str.find (newid);
	    //cout <<str <<'\n' <<pos1 <<endl;
	}
	pos1 += newid.size();
	size = str.find ("\"", pos1) - pos1;
	char id[10];
	str.copy (id, size, pos1);
	id[size] = '\0';
	int uniqueId = atoi (id);
	Article article (uniqueId);
	cout <<uniqueId <<endl;
	//found NEWID and created article
	
	//find <TOPICS
	fscanf (file, "%s", word);
	str = string (word);
	pos1 = str.find (openTags[2]);
	//cout <<str <<'\n' <<pos1 <<endl;
	while (pos1 < 0) {
	    fscanf (file, "%s", word);
	    str = string (word);
	    pos1 = str.find (openTags[2]);
	    //cout <<str <<'\n' <<pos1 <<endl;
	}
	str.replace (0, pos1+openTags[2].size(), "");
	pos1 = str.find (openTags[13]);
	while (pos1 >= 0) {
	    str.replace (pos1, openTags[13].size(), " ");
	    pos1 = str.find (openTags[13]);
	}
	pos1 = str.find (closeTags[2]);
	if (pos1 >= 0) {
	    str.replace (pos1, closeTags[2].size(), " ");
	}
	pos1 = str.find (closeTags[13]);
	while (pos1 >= 0) {
	    str.replace (pos1, closeTags[13].size(), " ");
	    pos1 = str.find (closeTags[13]);
	}
	int i, lastSpace=0, len;
      	for (i=0; i<str.size(); i++) {
	    if (str[i] == ' ') {
		len = str.copy(word,i,lastSpace);
		word[len] = '\0';
		addWord(article, string (word));
		article.addLabel (string (word));
		topicsList.insert (string (word));
		lastSpace = i;
	    }
	}
	//found TOPICS

	//find BODY
	do {
	    fscanf (file, "%s", word);
	    str = string (word);
	    pos1 = str.find ('<');
	    cout <<str <<':' <<pos1 <<endl;
	    while (pos1 < 0) {
		fscanf (file, "%s", word);
		str = string (word);
		pos1 = str.find ('<');
		cout <<str <<':' <<pos1 <<endl;
	    }
	    pos1 = str.find (openTags[12]);
	    if (pos1 < 0) {
		pos1 = str.find (closeTags[0]);
		if (pos1 >= 0) {
		    break;
		}
	    }
	    else {
		pos1 = str.find (closeTags[12]);
		pos2 = str.find (closeTags[0]);
		cout <<str <<':' <<pos1 <<endl;
		while (pos1 < 0 && pos2 < 0) {
		    addWord(article, str);
		    fscanf (file, "%s", word);
		    str = string (word);
		    pos1 = str.find (closeTags[12]);
		    pos2 = str.find (closeTags[0]);
		    cout <<str <<'\n' <<pos1 <<endl;
		}
		break;
	    }
	} while (feof (file) == 0);

	articles.insert (articles.end(), article);
    } while(feof (file) == 0);


	
	
/****OLD CODE START****    
    do {
	//cout <<"finding <REUTERS" <<endl;
      //find the first tag "<REUTERS"
	fscanf (file, "%s", word);
	if( feof (file) != 0) {
	    break;
	}
	while (openTags[0].compare (word) != 0) {
	    fscanf (file, "%s", word);
	    if( feof (file) != 0) {
		break;
	    }
	}
	//cout <<"getting id" <<endl;
      //get the article id form "NEWID"
	string str (fgets (line, 200, file));
	pos1 = str.find (newid);
	if (pos1 < 0) {
	    return 1;
	}
	pos1 += newid.size();
	size = str.find ("\"", pos1) - pos1;
	char id[10];
	str.copy (id, size, pos1);
	id[size] = '\0';
	int uniqueId = atoi (id);
	Article article (uniqueId);
	cout <<uniqueId <<endl;
	//cout <<"read DATE through COMPANIES" <<endl;
      //read DATE through COMPANIES
	int i;
	for(i=1; i<8; i++) {
	    if (!readTaggedSectionByLine (file, article, i)) {
		return 1;
	    }
	}
	//cout <<"read UNKNOWN" <<endl;
      //read UNKNOWN
	if (!readTaggedSectionByWords (file, article, 8)) {
	    return 1;
	}
	//cout <<"read TEXT" <<endl;
      //read TEXT line
	fscanf (file, "%s", word);
	str = string (word);
	pos1 = str.find (openTags[9]);
	//cout <<str <<'\n' <<pos1 <<endl;
	while (pos1 < 0) {
	    fscanf (file, "%s", word);
	    str = string (word);
	    pos1 = str.find (openTags[9]);
	    //cout <<str <<'\n' <<pos1 <<endl;
	}
	//cout <<"read TITLE through BODY" <<endl;
      //read TITLE through BODY
	for(i=10; i<13; i++) {
	    if (!readTaggedSectionByWords (file, article, i)) {
		return 1;
	    }
	}
	
	articles.insert (articles.end(), article);
    } while (feof (file) == 0);
****OLD CODE END****/



	
    fclose(file);
    return 0;
}

/**********************************************/
