//
// Created by PC_volt on 01/05/2022.
//
#include <stdio.h>
#include <string.h>
#include "Menu.h"

void GetMenuPointer()
{

}

void EditTrainingMenuElementsOrder()
{
    MeltyLib::Menu *&trainingMenu = *reinterpret_cast<MeltyLib::Menu **>(MeltyLib::PTR_TRAININGMENU);
    MeltyLib::MenuSet *trainingMenuSet = *trainingMenu->ppMenuSet;


    // At creation, the pElementsList points the end of the list (= pElementsListEnd), which is zero'd
    // We could completely reorder the pElementsList
    MeltyLib::MenuElement **beginning = trainingMenuSet->pElementsList - 20; //0x50 bytes = 80 bytes = 20 int, pre-existing MenuElement array

    *trainingMenuSet->pElementsList = *(trainingMenuSet->pElementsList - 1); //copies the last element
    trainingMenuSet->pElementsList = trainingMenuSet->pElementsList + 1; //increments the size of the list. pElementsListEnd doesn't need to be incremented for this to take effect.
}