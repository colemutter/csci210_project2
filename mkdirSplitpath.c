#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"

extern struct NODE* root;
extern struct NODE* cwd;

//make directory
void mkdir(char pathName[]){

    if (pathName == NULL || strlen(pathName) == 0) {
        printf("MKDIR ERROR: no path provided\n");
        return;
    }

    char baseName[64], dirName[128];
    struct NODE* parent = splitPath(pathName, baseName, dirName);

    if (!parent) {
        // splitPath already printed error message
        return;
    }

    if (strlen(baseName) == 0) {
        printf("MKDIR ERROR: no directory name provided\n");
        return;
    }

    // Check if directory already exists
    struct NODE* check = parent->childPtr;
    while (check) {
        if (strcmp(check->name, baseName) == 0 && check->fileType == 'D') {
            printf("MKDIR ERROR: directory %s already exists\n", pathName);
            return;
        }
        check = check->siblingPtr;
    }

    // Allocate and initialize new directory node
    struct NODE* newNode = (struct NODE*)malloc(sizeof(struct NODE));
    strcpy(newNode->name, baseName);
    newNode->fileType = 'D';
    newNode->childPtr = NULL;
    newNode->siblingPtr = NULL;
    newNode->parentPtr = parent;

    // Insert into parent's child list
    if (parent->childPtr == NULL) {
        parent->childPtr = newNode;
    } else {
        struct NODE* temp = parent->childPtr;
        while (temp->siblingPtr)
            temp = temp->siblingPtr;
        temp->siblingPtr = newNode;
    }

    printf("MKDIR SUCCESS: node %s successfully created\n", pathName);
}

struct NODE* findChild(struct NODE* parent, char* name) {
    struct NODE* curr = parent->childPtr;
    while (curr) {
        if (strcmp(curr->name, name) == 0 && curr->fileType == 'D') {
            return curr;
        }
        curr = curr->siblingPtr;
    }
    return NULL;
}

//handles tokenizing and absolute/relative pathing options
struct NODE* splitPath(char* pathName, char* baseName, char* dirName){

    char temp[128];
    char* token;
    struct NODE* start;

    // Case: path is root "/"
    if (strcmp(pathName, "/") == 0) {
        strcpy(dirName, "/");
        strcpy(baseName, "");
        return root;
    }

    // Copy into temp to avoid modifying original
    strcpy(temp, pathName);

    // Determine whether path is absolute or relative
    if (pathName[0] == '/') {
        start = root;
        token = strtok(temp + 1, "/"); // skip leading '/'
    } else {
        start = cwd;
        token = strtok(temp, "/");
    }

    // No '/' in path → no directory portion
    // Example: "file.txt" or "dir"
    if (!strchr(pathName, '/')) {
        strcpy(dirName, "");
        strcpy(baseName, pathName);
        return cwd;  // parent is current working directory
    }

    struct NODE* current = start;
    struct NODE* nextDir = NULL;

    // Traverse through tokens until the last one
    // Every token except final one must be an existing directory
    char dirPath[128] = "";
    while (token) {
        // Peek if this is the last token
        char* nextToken = strtok(NULL, "/");

        if (nextToken == NULL) {
            // This token is the baseName
            strcpy(baseName, token);

            // dirName = everything before baseName
            if (pathName[0] == '/')
                strcpy(dirName, "/");
            else
                strcpy(dirName, "");

            // Reconstruct directory name string
            // For absolute path p="/a/b/c", dirName="/a/b"
            // For relative path="a/b/c", dirName="a/b"
            char tmp2[128];
            strcpy(tmp2, pathName);
            tmp2[strlen(pathName) - strlen(baseName)] = '\0';

            // Remove trailing '/' from dirName
            if (tmp2[strlen(tmp2) - 1] == '/')
                tmp2[strlen(tmp2) - 1] = '\0';

            strcpy(dirName, tmp2);
            return current;
        }

        // Still traversing directories → must exist
        nextDir = findChild(current, token);
        if (!nextDir) {
            printf("ERROR: directory %s does not exist\n", token);
            return NULL;
        }

        current = nextDir;
        token = nextToken;
    }

    return current;
}
