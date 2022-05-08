//
// Created by PC_volt on 01/05/2022.
//
#include <stdio.h>
#include <string.h>
#include "Menu.h"

void GetMenuPointer()
{

}

void AddMenuElement(MeltyLib::MenuSet *trainingMenuSet, const char *label, const char *name)
{
    MeltyLib::MenuElement *customElement = new MeltyLib::MenuElement(MeltyLib::NORMAL_ELEMENT, label, name);

    *trainingMenuSet->pElementsListEnd = customElement;
    trainingMenuSet->pElementsListEnd = trainingMenuSet->pElementsListEnd + 1; //increments the size of the list. pElementsListEnd doesn't need to be incremented for this to take effect.
}

void EditTrainingMenuElementsOrder()
{
    MeltyLib::Menu *&trainingMenu = *reinterpret_cast<MeltyLib::Menu **>(MeltyLib::PTR_TRAININGMENU);
    MeltyLib::MenuSet *trainingMenuSet = *trainingMenu->ppMenuSet;

    AddMenuElement(trainingMenuSet, "CUSTOM 1", "CUSTOM_1");
    AddMenuElement(trainingMenuSet, "CUSTOM 2", "CUSTOM_2");
}