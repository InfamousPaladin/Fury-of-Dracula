// The following program file has adapted for use in this assignment from the
// COMP2521 labs. It was written by UNSW staff and for the purpose of the 
// assignment, and has been used to utilise.

// Item.h has been adapted from COMP2521 lab05 2020T2.

// Item.h  ... definition for items in Lists
// Written by John Shepherd, March 2013

#ifndef ITEM_H
#define ITEM_H

typedef int Item;
typedef int Key;

#define ItemCopy(i)     (i)
#define ItemKey(i)      (i)
#define ItemEQ(i1,i2)   ((i1) == (i2))
#define ItemLT(i1,i2)   ((i1) < (i2))
#define ItemGT(i1,i2)   ((i1) > (i2))
#define ItemShow(i)     printf("%d",(i))
#define ItemDrop(i)     ;

#endif

