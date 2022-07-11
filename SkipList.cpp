#include "SkipList.h"
#include <iostream>

using namespace std;

SkipList::SkipList() : listHeads() {
    makeNewLevelList();
    makeNewLevelList();
}

// makes a new list on the top level of existing list.
// call only when top list is NULL or just the two sentinels. 
void SkipList::makeNewLevelList() {
    SkipList::Entry* minusInfinity = new Entry("!!", "");	// "!!" < any other string.
    SkipList::Entry* plusInfinity = new Entry("}}", "");	// "}}" > any other key.

    Quad* first = new Quad(minusInfinity);
    Quad* last = new Quad(plusInfinity);

    int numLists = listHeads.size();
    Quad* oldFirst = numLists == 0 ? NULL : listHeads[numLists - 1];
    Quad* oldLast  = numLists == 0 ? NULL : oldFirst->next;

    first->prev = NULL;
    first->next = last;
    first->above = NULL;
    first->below = oldFirst;

    last->prev = first;
    last->next = NULL;
    last->above = NULL;
    last->below = oldLast;

    if(oldFirst != NULL) {
        oldFirst->above = first;
        oldLast->above = last;
    }
    listHeads.push_back(first);

}

void SkipList::printOneList(int listNum) {
	Quad* bottomCurrent = listHeads[0];
	Quad* current = listHeads[listNum];

	while(bottomCurrent->next != NULL) {
		std::string toPrint;
		if(current->entry->getKey() == bottomCurrent->entry->getKey()) {
			toPrint = current->entry->getKey();
			current = current->next;
		}
		else {
			toPrint = "--";
		}
		cout << "--" << toPrint;
		bottomCurrent = bottomCurrent->next;
	}
	cout << "--" << bottomCurrent->entry->getKey() << "--" << endl;
}

void SkipList::print() {
	int numLists = listHeads.size();
	for(int i = numLists - 1; i >= 0; i--) {
		printOneList(i);
	}
} 

SkipList::Entry* SkipList::find(Key k) {
    int numLists = listHeads.size();
    Quad* current = listHeads[numLists - 1];

    while (current->below != NULL) {
        current = current->below;			// drop down
        while(k >= current->next->entry->getKey()) {	// scan forward
            current = current->next;
        }
    } 

    if(current->entry->getKey() == k) {
        return current->entry;
    }
    else {
        return NULL;
    }
}

// the "trail" is a vector of the last node visited on each list
// the last element of trail is on the lowest list; the first is on the highest.
std::vector<SkipList::Quad*>* SkipList::findWithTrail(Key k) {
    std::vector<SkipList::Quad*>* trail = new std::vector<Quad*>();

    int numLists = listHeads.size();
    Quad* current = listHeads[numLists - 1];

    while (current != NULL) {
        while(k >= current->next->entry->getKey()) {	// scan forward
            current = current->next;
        }
	trail->push_back(current);
        current = current->below;			// drop down
    } 
    return trail;
}


// fill in the assigned functions here.


void SkipList::insert(Key k, Value v){
    
    //check if key is already in the list


    Entry* targetEntry;
    targetEntry=find(k);
    std::vector<Quad*>* trail=findWithTrail(k);
    Quad* tempQuad;


    if(targetEntry!=NULL){

        tempQuad = trail->back();
        Entry* tempEntry = new Entry(k,v);
        while(tempQuad!=NULL){
            tempQuad->entry= tempEntry;
            tempQuad=tempQuad->above;
        }
        return;
    }

    else{

        //toss the coin
        int newHeight=0;
        int level=listHeads.size()-1;

        while(std::rand()%2==0){
            newHeight++;
        }

        //case (1) newHeight is less than level
        if(newHeight<level){
            targetEntry= new Entry(k,v);
            tempQuad= new Quad(targetEntry);
            int length=trail->size()-1;
            for(int i=0;i<=newHeight;i++){
                tempQuad= new Quad(targetEntry);
                tempQuad->prev=(*trail)[length-i];
                tempQuad->next=(*trail)[length-i]->next;
                (*trail)[length-i]->next->prev=tempQuad;
                (*trail)[length-i]->next=tempQuad;
            }

            (*trail)[length]->next->below=NULL;
            (*trail)[length-newHeight]->next->above=NULL;
            for(int i=0;i<newHeight;i++){
                (*trail)[length-i]->next->above=(*trail)[length-i-1]->next;
                (*trail)[length-i-1]->next->below=(*trail)[length-i]->next;
            }
        }
        //case (2) newHeigiht is no less than level
        else{

            int difference=newHeight-level;

            //add new levels
            for(int i=0;i<difference+1;i++){
                makeNewLevelList();
            }

            //insert
            trail=findWithTrail(k);
            int length=trail->size()-1;


            targetEntry= new Entry(k,v);

            for(int i=0;i<=newHeight;i++){
                tempQuad= new Quad(targetEntry);
                tempQuad->prev=(*trail)[length-i];
                tempQuad->next=(*trail)[length-i]->next;
                (*trail)[length-i]->next->prev=tempQuad;
                (*trail)[length-i]->next=tempQuad;
            }

            (*trail)[length]->next->below=NULL;
            (*trail)[length-newHeight]->next->above=NULL;

            for(int i=0;i<newHeight;i++){
                (*trail)[length-i]->next->above=(*trail)[length-i-1]->next;
                (*trail)[length-i-1]->next->below=(*trail)[length-i]->next;
            }

        }
        return;
    }


}


void SkipList::remove(Key k){
    //check k is in the list
    Entry* targetEntry;
    targetEntry=find(k);
    if(targetEntry==NULL){
        return;
    }

    else{
        std::vector<Quad*>* trail=findWithTrail(k);
        Quad* tempQuad=trail->back();
        while(tempQuad!=NULL){
            tempQuad->prev->next=tempQuad->next;
            tempQuad->next->prev=tempQuad->prev;
            tempQuad=tempQuad->above;
        }


        //remove extra empty levels
        while(listHeads.back()->next->entry->getKey()==listHeads.back()->below->next->entry->getKey()){
            listHeads.pop_back();
        }
        return;
    }

}


SkipList::Entry* SkipList::greaterEntry(Key k){
    std::vector<Quad*>* trail=findWithTrail(k);
    int lastInde=trail->size()-1;
    return (*trail)[lastInde]->next->entry;
}

SkipList::Entry* SkipList::lesserEntry(Key k){
    std::vector<Quad*>* trail=findWithTrail(k);
    int lastInde=trail->size()-1;
    Quad* tempQuad = (*trail)[lastInde];

    if(tempQuad->entry->getKey()==k){
        return tempQuad->prev->entry;
    }
    else{
        return tempQuad->entry;
    }    
}

SkipList::Entry* SkipList::ceilingEntry(Key k){
    std::vector<Quad*>* trail=findWithTrail(k);
    int lastInde=trail->size()-1;
    Quad* tempQuad = (*trail)[lastInde];

    if(tempQuad->entry->getKey()==k){
        return tempQuad->entry;
    }
    else{
        return tempQuad->next->entry;
    }

}

SkipList::Entry* SkipList::floorEntry(Key k){
    std::vector<Quad*>* trail=findWithTrail(k);
    int lastInde=trail->size()-1;
    Quad* tempQuad = (*trail)[lastInde];

    return tempQuad->entry;

}





















