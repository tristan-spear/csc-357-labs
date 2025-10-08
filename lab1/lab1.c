#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// def for nodes in word linked list
typedef struct wordNode
{
    char text[1000];
    struct wordNode *next,*prev;
} wordNode;

// def for nodes in letter linked list
typedef struct letterNode
{ 
    char letter;
    wordNode *wordHead;
    struct letterNode *next,*prev;
} letterNode;

// adds letter node to end of linked list
void addLetter (char newLetter, letterNode **headNode)
{
    letterNode *newNode = (letterNode*)malloc(sizeof(letterNode));
    newNode -> wordHead = NULL;

    if (*headNode == NULL)
    {
        *headNode = newNode;
        (*headNode) -> prev = (*headNode) -> next = NULL;
        (*headNode) -> letter = newLetter;
    }
    else
    {
        letterNode *last;
        for(last = (*headNode); (last -> next) != NULL; last = (last -> next));
        last -> next = newNode;
        newNode -> prev = last;
        newNode -> next = NULL;
        newNode -> letter = newLetter;
    }
}

// adds word node to end of linked list
void addWordHelper(char newWord[1000], wordNode **headNode)
{
    wordNode *newNode = (wordNode*)malloc(sizeof(wordNode));
    strcpy(newNode -> text, newWord);

    if (*headNode == NULL)
    {
        *headNode = newNode;
        (*headNode) -> prev = (*headNode) -> next = NULL;
    }
    else
    {
        wordNode *last;
        for(last = (*headNode); (last -> next) != NULL; last = (last -> next));
        last -> next = newNode;
        newNode -> prev = last;
        newNode -> next = NULL;
    }
}

// finds node that matches the first letter of the text being added,
// then passes in that node's word linked list to helper method
void addWord(char newWord[1000], letterNode **headNode)
{
    // make sure word being added starts with a letter
    if(!isalpha(newWord[0]))
    {
        printf("Words must start with a letter!\n");
        return;
    }
    // do not add the word "print"
    else if(strcmp(newWord, "print") == 0)
        return;

    letterNode *itr = (letterNode*)malloc(sizeof(letterNode));
    
    // iterate to node with first letter of word
    for(itr = *headNode; itr -> letter != tolower(newWord[0]); itr = itr -> next);
    addWordHelper(newWord, &(itr -> wordHead));
}

// Iterates through word linked list, printing all words
void printHelper(wordNode **headNode)
{
    wordNode *itr;
    for(itr = *headNode; itr != NULL; itr = itr -> next)
        printf("%s ", itr -> text);
}

// Iterates through letter linked list, 
// passing word linked list to helper method when word linked lists exist
void print(letterNode **headNode)
{
    letterNode *itr;
    for(itr = *headNode; itr != NULL; itr = itr -> next)
        if (itr -> wordHead != NULL)
            printHelper(&(itr -> wordHead));   
}

// iterates through at deletes all nodes in a word linked list
void deleteAllHelper(wordNode **headNode)
{
    wordNode *curr = *headNode;
    wordNode *nextPtr;

    while(curr != NULL)
    {
        nextPtr = curr -> next;
        free(curr);
        curr = nextPtr;
    }
}

// iterates through letter linked list,
// passing through word linked lists to helper method where they exist,
// then deletes each letter node
void deleteAll(letterNode **headNode)
{
    letterNode *curr = *headNode;
    letterNode *nextPtr;
    while(curr != NULL)
    {
        if(curr -> wordHead != NULL)
            deleteAllHelper(&(curr -> wordHead));
        
        nextPtr = curr -> next;
        free(curr);
        curr = nextPtr;
    }
    *headNode = NULL;
}

// runs the program,
// initialize letter linked list to contain the 26 letters of the alphabet
// read input until 'print' is typed
// call print function
void run()
{
    letterNode *head = NULL;

    for(int i = 97; i < 123; ++i)
        addLetter(i, &head);

    char input[1000];

    printf("\nEnter words followed by the 'Enter' key to add to list\n");
    printf("Type 'print' to finish and print all words :\n");

    while(strcmp(input, "print") != 0)
    {
        scanf("%s", input);
        addWord(input, &head);
    }
    
    print(&head);
    deleteAll(&head);
}

int main()
{
    run();
    return 0;
}