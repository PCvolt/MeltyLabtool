//
// Created by PC_volt on 01/05/2022.
//
#include <stdio.h>
#include <string.h>
#include "Menu.h"

void GetMenuPointer()
{

}

void AddMenuElement(MeltyLib::MenuSet *trainingMenuSet, MeltyLib::ElementType type, const char *label, const char *name)
{
    MeltyLib::MenuElement *customElement = new MeltyLib::MenuElement(type, label, name);

    *trainingMenuSet->pElementsListEnd = customElement;
    trainingMenuSet->pElementsListEnd = trainingMenuSet->pElementsListEnd + 1; //increments the size of the list. pElementsListEnd doesn't need to be incremented for this to take effect.
}

void CreateCustomMenu()
{
    // I guess the Normal MenuElement has the same name as the Menu
}

void EditTrainingMenuElementsOrder()
{
    MeltyLib::Menu *&trainingMenu = *reinterpret_cast<MeltyLib::Menu **>(MeltyLib::PTR_TRAININGMENU);
    MeltyLib::MenuSet *trainingMenuSet = *trainingMenu->ppMenuSet;

    //AddMenuElement(trainingMenuSet, MeltyLib::SPACE_ELEMENT, "", ""); //crashes sometimes
    AddMenuElement(trainingMenuSet, MeltyLib::NORMAL_ELEMENT, "LABTOOL", "LABTOOL");
    //AddMenuElement(trainingMenuSet, MeltyLib::SELECT_ELEMENT, "SELECT ELEMENT", "SELECT_ELEMENT"); //crashes
}

/*
 * Move the attack display at the top of the screen, in columns: increase readability
 * Place Frame advantage and gaps between the two meter gauges
 * */