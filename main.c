/**
 * NAME: Ajani Green
 * DESCRIPTION: This program will allow the user to fill out student's presence in their classes for the day. 
 *              It will also allow the supervisorMenu to accept/reject the slips posted
 * DATE: April 14, 2022
 **/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<time.h>
#include<unistd.h>
#include<conio.h>
#include<ctype.h>
#include<dirent.h>

FILE *fp, *fp2, *fp3, *fp4, *fp5;

typedef enum ArrayType { LATE, ABSENT } ArrayType;

const char END_OF_SLIP = '~';
const char START_OF_SLIP = '^';
char dirName[] = "data";
char unreviewedSlipsFileName[100], subjectsFileName[100], timePeriodsFileName[100], doneTimesFileName[100], beadleSlipFileName[100];

typedef struct Pair {
    int period;
    char* subject;
} Pair;

typedef struct SlipEntry {
    Pair subjectAndPeriod;
    bool teacherPresence;
    char** lateStudentList;
    char** absentStudentList;
    char* teacherArrival;
    int numOfLateStudents;
    int numOfAbsentStudents;
} SlipEntry;

typedef struct Slip {
    SlipEntry* entries;
    int numOfEntries;
    char* date;
} Slip;

typedef struct Queue {
    unsigned capacity; 
    int front, rear, size;
    Slip* arr;
} Queue;

// Function Protoypes
bool dirExists(const char* filen); // Function to check if a directory exists or not
bool emptyFile(const char* filen); // Function to check whether a file is empty or not
bool fileExists(const char* filen); // Function to check whether a file exists on the machine or not
bool subjectSearch(char const argv[]); // Function to search if a subject entered is valid
bool dateCompare(); // Function to compare today's date to the date that is at the top of a file
bool dateSearch(const char* filen); // Function to search if today's date is already in the file
bool studentEnteredAlready(SlipEntry* entry, int studentsEntered, char* studentName); // Checks if a students has been entered in the input stream already
const char* timeStrings(int key); // Function to assign certain time period to a key integer
const char* getDate(); // Function to make the current date into a string format
const char* toUpper(const char argv[]); // Function to convert a string into all uppercase
int getLineCount(const char* filen); // Function to count how many lines are in a file
int getUnreviewedSlipsCount(); // Function to count the number of unreviewed slips there are in the file
int getReviewedSlipsCount(); // Function to count the number of reviewed slips there are in the file
void dateFile(const char* filen); // Function to put the date string in a file
void menu();  // Function that will allow the user to enter the system as a student or supervisorMenu
void studentMenu(); // Function that will show all the times to be filled out
void supervisorMenu(); // Function that will show the unreviewed slips to be reviewed
void fillOut(int key); // Function that allows the input of student and subject data
void studentPresence(SlipEntry* entry, bool isAbsentStudent, bool teacherPresence); // Function to fill out the student's presence in the class
void teacherArrival(SlipEntry* entry, int key, FILE *slip); // Function that allows the accurate input of time that the teacher arrived to the class
void printSubjects(); // Function that prints all the valid subjects
void printSlip(); // Function that prints what is on the slip itself
void approveSlip(Slip slip); // Function to approve an unreviewed slip
void denySlip(); // Function to deny an unreviewed slip
void extraFiles(const char* name); // Function that creates the StudentNames.txt and SubjectNames.txt files on the machine
void fileChecks(); // Function that does all the necessary checks for files to see whether they should be created or new dates should be added to the file
void splashScreen(); // Function that prints the splashScreen
bool periodWasFilledOut(int key);
char getLastCharInFile(FILE* file);
char** getSubjects();
char** getTimes();
int getSubjectCount();
int getPeriodCount();
Pair generatePair(int period, const char* name);
SlipEntry generateSlipEntry(int key);
void addStudentToSlipArray(SlipEntry* entry, ArrayType arrayType, const char* name);
Slip generateSlip();
void addEntryToSlip(Slip* slip, SlipEntry entry);
void printEntryToFile(SlipEntry slipEntry);
void printEntryToStdout(SlipEntry entry);
Queue getUnreviewedSlipsFromFile();
void editSlip(Slip* slip);
void editSlipEntry(Slip slip, SlipEntry* entry);
void editSlipEntrySubject(Slip slip, SlipEntry* entry);
void editSlipEntryStudents(Slip slip, SlipEntry* entry, ArrayType type);
Queue createQueue(unsigned capacity);
bool queueIsFull(Queue* queue);
bool queueIsEmpty(Queue* queue);
void enqueue(Queue* queue, Slip slip);
Slip dequeue(Queue* queue);

void main() {
    fileChecks();
    splashScreen();
    menu();
}

void splashScreen() { // Function that prints the splashScreen
    system("cls");
    printf("                        @               &               .\n");
    printf("                        @&             & &             &&\n");
    printf("                        @  #&       && & & &&       &&  &\n");
    printf("                        @ &%%&&&  ,&&&%%%%& &%%%%%&&#  %%&&%%& &\n");
    printf("                        @ &%%%%%%%%%%%%%%%%%%%%%%%%& &%%%% %%%%%%%%%%  %%%%& &\n");
    printf("                        @ &%%%%%%%%%%%%. %%%%%%%%& &%%%%%%%% %. %%%%%%%%& &\n");
    printf("                        @ &%%#     %% %% %& &%%%% % *    %%%%& &\n");
    printf("                        @ &%%%%%%    %%%%%%%%%& &%%%% .%,/   %%%%& &\n");
    printf("                        @ &%%%%%%%%%%%%%%%%%%%%%%%%& &%%%%%%%%%%%%%%%%%%%%%%%%& &\n");
    printf("                        @ &&&&&&&&&&&&&& &&&&&&&&&&&&&& &\n");
    printf("                         & &%%%% %%%%%%%% %%%%%& &%%%%%%%%%%%%%%  %%%%%&%\n");
    printf("                          & &%%%%%%  %%%%%%%%%& &%%%%%%%%%%%%%%%% %%%%&,*\n");
    printf("                    %%%%     &  &%(/%%%%%%.%%& &%%%%%%%%%%%%%%%%%%& &  %%  %%\n");
    printf("                   %%%%%% %%     && &%%%%%%%% %%& &%%%%%%%% %&  &     %% %%%%%%\n");
    printf("                    %%%%%%%%%%%%%%     && &&%%%%& &%% %%& ,&     %%%%%%%%%%%%%%%%\n");
    printf("                    %%%%%%%%   .%%%%%%%%   &&/ , &  &&   %%%%%%%% %%  %%%%%%%%%%\n");
    printf("                          %%%%%% ,  %%%%%%    &    #%%%%%%%% %% %%%%%%\n");
    printf("                              %%%%%% %% %%%%%%%% %%%%%%%%%%/ %%%%%%.\n");
    printf("                                %%%%%%%%%%  %%%%  #%%%%%%\n");
    printf("                                      %%%%%%%%%%%%\n\n");
    printf(" _____          __  __ _____ _____ ____  _   _    _____ ____  _      _      ______ _____ ______\n"); 
    printf("/ ____|    /\\   |  \\/  |  __ \\_   _/ __ \\| \\ | |  / ____/ __ \\| |    | |    |  ____/ ____|  ____|\n");
    printf("| |       /  \\  | \\  / | |__) || || |  | |  \\| | | |   | |  | | |    | |    | |__ | |  __| |__   \n");
    printf("| |      / /\\ \\ | |\\/| |  ___/ | || |  | | . ` | | |   | |  | | |    | |    |  __|| | |_ |  __|  \n");
    printf("| |____ / ____ \\| |  | | |    _| || |__| | |\\  | | |___| |__| | |____| |____| |___| |__| | |____ \n");
    printf(" \\_____/_/    \\_\\_|  |_|_|   |_____\\____/|_| \\_|  \\_____\\____/|______|______|______\\_____|______|\n");
    sleep(3);                                                          
}

void menu() { // Function that will allow the user to enter the system as a student or supervisorMenu
    system("cls");

    printf("__________________________________________________________________________________________________\n");
    printf("\t\t\t[ WELCOME TO THE BEADLE SLIP SYSTEM ]\n");
    printf("\t\tWhich would you like to access the system as?:\n");
    printf("\n\t[1] Supervisor\n\t[2] Beadle\n\n\t[0] Exit\n");
    printf("__________________________________________________________________________________________________\n\n");
    int choice;
    do {
        printf("\n> ");
        scanf("%d", &choice);
        switch (choice) {
            case 0:
                exit(-1);
            case 1:
                supervisorMenu();
                break;
            case 2:
                studentMenu();
                break;
            default:
                printf("| Invalid option!");
                getchar();
                break;
        }
    } while (choice < 1 || choice > 2);
}

void supervisorMenu() { // Function that will show the unreviewed slips to be reviewed
    system("cls");
    printf("_________________________________________________\n");
    printf("\t[ BEADLE SLIP (SUPERVISOR) ]\n");
    printf("_________________________________________________\n\n");
    if (getUnreviewedSlipsCount() == 0) {
        printf("\n| You have %d unreviewed beadle slips\n\n| Press any key to go back to the main menu\n", getUnreviewedSlipsCount());
        getch();
        getchar();
        menu();
    } else {
        printf("\n| You have %d unreviewed beadle slips\n| Press 1 to view them\n| Press any other key to exit\n", getUnreviewedSlipsCount());
        int choice, temp;
        if ((choice = getch()) == 49) {
            Queue slips = getUnreviewedSlipsFromFile();
            Slip slip = dequeue(&slips);

            for (int i = 0; i < slip.numOfEntries; i++)
                printEntryToStdout(slip.entries[i]);
                
            printf("\n| Approve this slip? (y/N)\n| Edit this slip? (e/E)\n");
            char ch_choice;
            int cont = 0;
            while (!cont) {
                printf("\n> ");
                getchar();
                scanf("%c", &ch_choice);
                if (ch_choice == 'y' || ch_choice == 'Y') {
                    approveSlip(slip);
                    cont++;
                } else if (ch_choice == 'n' || ch_choice == 'N') {
                    denySlip();
                    cont++;
                } else if (ch_choice == 'e' || ch_choice == 'E') {
                    editSlip(&slip);
                } else printf("| Invalid option!\n");
            }
        } else menu();
    }
}

void studentMenu() { // Function that will show all the times to be filled out
    system("cls");
    const int periodCount = getPeriodCount();

    printf("_________________________________________________\n");
    printf("\t\t[ BEADLE SLIP ]\n");
    printf("\t       DATE: %s",getDate());
    printf("_________________________________________________\n\n");
    printf("\n| Hello!\n| Below are the following times you have not filled out as yet.\n\n");
    
    for(int i = 0; i < periodCount; i++)
        if (!periodWasFilledOut(i+1))
            printf("[%d] %s\n", i+1, timeStrings(i));
    
    if (getLineCount(doneTimesFileName) == periodCount)
        printf("| You have filled out everything for today!\n\n");
    else
        printf("\n| Which one would you like to fill out? [Enter 1-%d]\n", periodCount);    
    
    printf("| [NOTE: ENTER 0 TO EXIT THE PROGRAM]\n");
    printf("| [NOTE: ENTER %d TO VIEW THE INFORMATION ON THE SLIP SO FAR]\n", periodCount + 1);
    
    int choice;
    do {
        printf("> CHOICE: ");
        scanf("%d", &choice);

        if (choice < 0 || choice > periodCount + 1) {
            printf("Invalid choice! [Choose from 0-%d]\n", periodCount + 1);
            choice = -1;
            getchar();
        }

        if (choice == 0)
            exit(0);

        if (choice == periodCount + 1) {
            printSlip();
            system("cls");
            studentMenu();
            return;
        }

        if (periodWasFilledOut(choice)) {
            printf("| This period was already filled out! Please choose from the list available.\n");
            choice = -1;
            getchar();
        } else if (getLineCount(doneTimesFileName) < choice) {
            printf(("| You can't fill this out as yet!\n"));
            choice = -1;
            getchar();
        } else fillOut(choice);
    } while (choice > periodCount || choice < 0);
}

void fillOut(int key) { // Function that allows the input of student and subject data   
    char temp;
    getchar();
    SlipEntry entry = generateSlipEntry(key);

    system("cls");
    printf("_________________________________________________\n");
    printf("\t\t[ BEADLE SLIP ]\n");
    printf("\t       %s\n", timeStrings(key-1));
    printf("_________________________________________________\n\n");
    printf("| SUBJECT: \n");
    printf("| [NOTE: ENTER 'Break' IF YOU ARE ON A BREAK]\n");
    printf("| [NOTE: ENTER 'None' IF A CLASS HAS NOT STARTED YET]\n");
    printf("> ");

    char subject[50];
    gets(subject);
    if (!subjectSearch(toUpper(subject))) {
        char choice; 
        int cont = 0;
        do {
            cont = 0;
            printf("| Not a valid subject!\n");
            printf("| Would you like to see a list of valid subjects? (y/n)");
            do {
                printf("\n> ");
                scanf("%c", &choice);
                if (choice == 'y' || choice == 'Y') {
                    printSubjects();
                    cont++;
                } else if (choice == 'n' || choice == 'N') {
                    printf("| Okay!\n");
                    cont++;
                } else {
                    printf("| Invalid input!");
                    getchar();
                }
            } while (cont == 0);
            getchar();
            printf("\n| SUBJECT: \n");
            printf("| [NOTE: ENTER 'Break' IF YOU ARE ON A BREAK]\n");
            printf("| [NOTE: ENTER 'None' IF A CLASS HAS NOT STARTED YET]\n");
            printf("> ");
            gets(subject);
        } while (!subjectSearch(toUpper(subject)));
    }

    strcpy(entry.subjectAndPeriod.subject, subject);

    if (strcmp(toUpper(subject), "BREAK") == 0 || strcmp(toUpper(subject), "NONE") == 0) {
        printEntryToFile(entry);
        studentMenu();
        return;
    } else {
        int cont = 0;

        printf("\n| Is the teacher present? (Y/n)");
        bool loc_teacher_presence;
        do {
            printf("\n> ");
            scanf("%c", &temp);
            if (temp == 'y' || temp == 'Y') {
                loc_teacher_presence = true;
                cont++;
            } else if (temp == 'n' || temp == 'N') {
                loc_teacher_presence = false;
                cont++;
            } else {
                printf("| Invalid input!");
                getchar();
            }
        } while (cont == 0);
        getchar();
        
        studentPresence(&entry, false, loc_teacher_presence);
        getchar();
        studentPresence(&entry, true, loc_teacher_presence);
        
        if (entry.teacherPresence) {
            getchar();
            teacherArrival(&entry, key, fp);
        }
    }

    printEntryToFile(entry);

    fp4 = fopen(doneTimesFileName, "a");
    fprintf(fp4, "\nDone %d", key);
    fclose(fp4);
    system("cls");
    studentMenu();
}

void studentPresence(SlipEntry* entry, bool isAbsentStudent, bool teacherPresence) { // Fucntion to fill out the student's presence in the class
    if (isAbsentStudent) {
        printf("\n| Were there any absent students? (Y/n)");
    } else {
        printf("\n| Were there any late students? (Y/n)");
    }
    int cont4 = 0;
    char temp;

    do {
        printf("\n> ");
        scanf("%c", &temp);
        if (temp == 'y' || temp == 'Y') {
            int i = 1;
            char temp2;
            int cont = 1;
            cont4++;

            printf("| Who were they?\n");
            while (cont == 1) {
                char studentName[100];
                getchar();
                printf("\n[%d] NAME: ", i);
                gets(studentName);

                if (studentEnteredAlready(entry, isAbsentStudent ? entry->numOfAbsentStudents : entry->numOfLateStudents, studentName))
                    printf("| You've already entered this student!\n| Press enter to continue");
                else {
                    addStudentToSlipArray(entry, isAbsentStudent ? ABSENT : LATE, studentName);
                    i++;

                    entry->teacherPresence = teacherPresence;
                    printf("\n| Is that all? (Y/n)");
                    int cont2 = 0;
                    
                    do {
                        printf("\n> ");
                        scanf("%c", &temp2);
                        if (temp2 == 'Y' || temp2 == 'y') {
                            cont = 0;
                            cont2++;
                        } else if (temp2 == 'N' || temp2 == 'n') {
                            cont = 1;
                            cont2++;
                        } else {
                            printf("| Invalid input!");
                            getchar();
                        }
                    } while (cont2 == 0);
                }
            }
        } else if (temp == 'n' || temp == 'N') {
            if (isAbsentStudent) {
                strcpy(entry->absentStudentList[entry->numOfAbsentStudents++], "None");
            } else {
                strcpy(entry->lateStudentList[entry->numOfLateStudents++], "None");
            }
            entry->teacherPresence = teacherPresence;
            cont4++;
        } else {
            printf("| Invalid input!");
            getchar();
        }
    } while (cont4 == 0);
}

void teacherArrival(SlipEntry* entry, int key, FILE *slipFile) { // Function that allows the accurate input of time that the teacher arriived to the class
    printf("| TEACHER TIME OF ARRIVAL: ");
    gets(entry->teacherArrival);
}

bool dirExists(const char* filen) { // Function to check if a directory exists or not
    DIR *dir = opendir(dirName);
    return dir;
}

bool emptyFile(const char *filen) { // Function to check whether a file is empty or not
    long size = 0;
    fp = fopen(filen, "r");
    if (NULL != fp) {
        fseek (fp, 0, SEEK_END);
        size = ftell(fp);
    }
    fclose(fp);
    return size == 0;
}

void printSlip() { // Function that prints what is on the slip itself
    system("cls");
    printf("_________________________________________________\n");
    printf("\t\tWould you like to:\n");
    printf("\n\t[1] Print the entire beadle slip\n\t[2] Print a specific date in the beadle slip\n");
    printf("_________________________________________________");
    int choice;
    char date[20], str[512], ch, dateStr[100];
    int lineCount = 0, foundLine1, foundLine2, lineID = 1;
    bool found = false;
    bool oneSlip = (getReviewedSlipsCount() == 1) ? true : false;

    getchar();
    do {
        printf("\n> ");
        scanf("%d", &choice);
        switch (choice) {
            case 1:
                if (emptyFile(beadleSlipFileName)) printf("| There is nothing on the slip or the slip has not been approved as yet!\n");
                else {
                    fp5 = fopen(beadleSlipFileName, "r");
                    while (ch != EOF) {
                        ch = getc(fp5);
                        putchar(ch);
                    }
                    fclose(fp5);
                }

                printf(("| Press any key to go back to the student menu\n"));
                getch();
                break;
            case 2:
                if (emptyFile(beadleSlipFileName)) printf("| There is nothing on the slip or the slip has not been approved as yet!\n");
                else {
                    printf("\n| What is the date you would like to search for?\n[FORMAT: \"Month dd, yyyy\"]\n> ");
                    getchar();
                    gets(date);
                    system("cls");
                    sprintf(dateStr, "DATE: %s", date);
                    fp5 = fopen(beadleSlipFileName, "r");
                    while (fgets(str, 512, fp5)) {
                        if (strstr(str, dateStr) != NULL) {
                            if (lineID == 1) {
                                foundLine1 = lineCount;
                                lineID++;
                                found = true;
                                if (oneSlip) break;
                            } else {
                                foundLine2 = lineCount;
                                found = true;
                                break;
                            }
                        } else {
                            lineCount++;
                        }
                    }
                    if (!found) printf("| This date could not be found in the file!\n");
                    else {
                        printf("| Slip for %s found!\n\n", date);
                        rewind(fp5);
                        while (ch != EOF) {
                            ch = getc(fp5);
                            if (oneSlip) {
                                if (lineCount >= foundLine1)
                                    putchar(ch);
                            } else {
                                if (lineCount >= foundLine1 && lineCount < foundLine2)
                                    putchar(ch);
                                if (lineCount == foundLine2+1) break; 
                            }
                            if (ch == '\n') lineCount++; 
                        }

                    }
                    fclose(fp5);
                }

                printf(("| Press any key to go back to the student menu\n"));
                getch();
                break;
            default:
                printf("| Invalid option!");
                getchar();
        }
    } while (choice < 1 || choice > 2);    
}

void printEntryToStdout(SlipEntry entry) {
    printf("Subject: %s\n\n", entry.subjectAndPeriod.subject);
    if (entry.numOfLateStudents == 0)
            printf("Late Students: None\n\n");
    else {
        printf("Late Students: {\n");
        for (int j = 0; j < entry.numOfLateStudents; j++)
            printf("\t%s\n", entry.lateStudentList[j]);
        printf("}\n\n");
    }

    if (entry.numOfAbsentStudents == 0)
        printf("Absent Students: None\n\n");
    else {
        printf("Absent Students: {\n");
        for (int j = 0; j < entry.numOfAbsentStudents; j++)
            printf("\t%s\n", entry.absentStudentList[j]);
        printf("}\n\n");
    }

    printf("Teacher present: %s\n", entry.teacherPresence ? "Yes" : "No");
    if (entry.teacherPresence)
        printf("Teacher Time of Arrival: %s\n\n\n", entry.teacherArrival);
}

void approveSlip(Slip slip) { // Function to approve an unreviewed slip
    fp = fopen(unreviewedSlipsFileName, "r");
    if (!fp) {
        printf("| Could not open fp! [MODULE: approveSlip]");
        exit(-1);
    }

    // Finding where exactly the information to be updated is found in the original file
    char str[512];
    int lineCount = 0, foundLine1, foundLine2, lineID = 1;
    while(fgets(str, sizeof(str), fp)) {
        if (strstr(str, "DATE: ")) {
            if (lineID == 1 && strstr(str, slip.date) != NULL) {
                foundLine1 = lineCount;
                lineID++;
            } else if (lineID == 2) {
                foundLine2 = lineCount;
                break;
            }
        } else lineCount++;
    }

    // Adding the approved slip to the main file
    fp5 = fopen(beadleSlipFileName, "a");
    if (!fp5) {
        printf("| Could not open fp5! [MODULE approveSlip]");
    }

    fprintf(fp5, "Date: %s", slip.date);
    for (int i = 0; i < slip.numOfEntries; i++) {
        SlipEntry entry = slip.entries[i];
        
        fprintf(fp5, "\n\nPeriod: %s\n", timeStrings(entry.subjectAndPeriod.period));
        fprintf(fp5, "Subject: %s\n\n", entry.subjectAndPeriod.subject);
        
        if (entry.numOfLateStudents == 0)
            fprintf(fp5, "Late Students: None\n\n");
        else {
            fprintf(fp5, "Late Students: {\n");
            for (int j = 0; j < entry.numOfLateStudents; j++)
                fprintf(fp5, "\t%s\n", entry.lateStudentList[j]);
            fprintf(fp5, "}\n\n");
        }

        if (entry.numOfAbsentStudents == 0)
            fprintf(fp5, "Absent Students: None\n\n");
        else {
            fprintf(fp5, "Absent Students: {\n");
            for (int j = 0; j < entry.numOfAbsentStudents; j++)
                fprintf(fp5, "\t%s\n", entry.absentStudentList[j]);
            fprintf(fp5, "}\n\n");
        }

        fprintf(fp5, "Teacher present: %s\n", entry.teacherPresence ? "Yes" : "No");
        if (entry.teacherPresence)
            fprintf(fp5, "Teacher Time of Arrival: %s", entry.teacherArrival);
    }

    fputs("\n**********************************************************\n", fp5);
    fclose(fp5);

    // Creating a new file to write to
    FILE *temp;
    char *temp_fname = "temp.txt";
    temp = fopen(temp_fname, "w");

    char ch = 'A';
    lineCount = 0;
    // Resetting the cursor of fp to the very beginning. (Reading from the top of the file again)
    rewind(fp);
    while (ch != EOF) {
        ch = getc(fp);
        if (lineCount < foundLine1 || lineCount > foundLine2)
            putc(ch, temp);
        if (ch == '\n') lineCount++;
    }

    // Closing both files
    fclose(temp);
    fclose(fp);

    // Deleting original file
    remove(unreviewedSlipsFileName);

    // Renaming temp file to original file name, giving the impression that the file was just updated.
    rename(temp_fname, unreviewedSlipsFileName);

    printf("| You have approved this slip!\n| Enter any key to return to the supervisorMenu menu!");
    getch();
    supervisorMenu();
}

void denySlip() { // Function to deny an unreviewed slip
    fp = fopen(unreviewedSlipsFileName, "r");
    if (!fp) {
        printf("| Could not open fp! [MODULE: denySlip]");
        exit(-1);
    }
    char str[512];
    int lineCount = 0, foundLine1, foundLine2, lineID = 1;
    while(fgets(str, sizeof(str), fp)) {
        if (strstr(str, "DATE: ")) {
            if (lineID == 1) {
                foundLine1 = lineCount;
                lineID++;
            } else if (lineID == 2) {
                foundLine2 = lineCount;
                break;
            }
        } else lineCount++;
    }
    char ch;
    FILE *temp;
    char *temp_fname = "temp.txt";
    temp = fopen(temp_fname, "w");

    ch = 'A';
    lineCount = 0;
    rewind(fp);
    while (ch != EOF) {
        ch = getc(fp);
        if (lineCount < foundLine1 || lineCount > foundLine2)
            putc(ch, temp);
        if (ch == '\n') lineCount++;
    }

    fclose(temp);
    fclose(fp);
    remove(unreviewedSlipsFileName);
    rename(temp_fname, unreviewedSlipsFileName);

    printf("| You have denied this slip!\n| Enter any key to return to the supervisorMenu menu!");
    getch();
    supervisorMenu();
}

void printSubjects() { // Function that prints all the valid subjects
    printf("\n_______________________________________________\n");
    char ch;
    fp2 = fopen(subjectsFileName, "r");
    while (ch != EOF) {
        ch = getc(fp2);
        putchar(ch);
    }
    fclose(fp2);
    printf("\n_______________________________________________\n");
}

bool studentEnteredAlready(SlipEntry* entry, int studentsEntered, char* studentName) { // Checks if a students has been entered in the input stream already
    for (int i = 0; i < studentsEntered; i++) {
        if (strcmp(toUpper(entry->lateStudentList[i]), toUpper(studentName)) == 0) 
            return true;
        if (strcmp(toUpper(entry->absentStudentList[i]), toUpper(studentName)) == 0) 
            return true;
    }
    return false;
}

bool subjectSearch(char const argv[]) { // Function to search if a subject entered is valid
    char** subjects = getSubjects();

    for (int i = 0; i < getSubjectCount(); i++)
        if (strcmp(toUpper(argv), subjects[i]) == 0)
            return true;
    return false;
}

bool periodWasFilledOut(int key) {
    char keyStr[8], str[512];
    sprintf(keyStr, "Done %d", key);

    fp4 = fopen(doneTimesFileName, "r");
    while (fgets(str, sizeof(str), fp4))
        if (strstr(str, keyStr) != NULL) {
            fclose(fp4);
            return true;
        }
    fclose(fp4);
    return false;
}

bool dateSearch(const char* filen) { // Function to search if today's date is already in the file
    bool found;
    int num = 0;
    char string[100];
    if (strcmp(filen, unreviewedSlipsFileName) == 0) {
        fp = fopen(filen, "r");
        while (fgets(string, sizeof(string), fp) != NULL)
            if (strstr(string, getDate()) != NULL) num++;
        found = (num > 0) ? true : false;
        fclose(fp);
    } else if (strcmp(filen, doneTimesFileName) == 0) {
        fp4 = fopen(filen, "r");
        while (fgets(string, sizeof(string), fp4) != NULL)
            if (strstr(getDate(), string) != NULL) num++;
        found = (num > 0) ? true : false;
        fclose(fp4);
    }   
    return found;
}

bool dateCompare() { // Function to compare today's date to the date that is at the top of a file
    char str[100];
    fp = fopen(unreviewedSlipsFileName, "r");
    fgets(str, sizeof(str), fp);
    fclose(fp);
    return strstr(str, getDate()) != NULL;
}

bool fileExists(const char* filen) {
    return access(filen, F_OK) != -1;
} // Function to check whether a file exists on the machine or not

void fileChecks() { // Function that does all the necessary checks for files to see whether they should be created or new dates should be added to the file
    sprintf(unreviewedSlipsFileName, "%s/UnreviewedSlips.txt", dirName);
    sprintf(subjectsFileName, "%s/Subjects.txt", dirName);
    sprintf(timePeriodsFileName, "%s/TimePeriods.txt", dirName);
    sprintf(doneTimesFileName, "%s/DoneTimes.txt", dirName);
    sprintf(beadleSlipFileName, "%s/BeadleSlipData.txt", dirName);
    if (!dirExists(dirName)) system("mkdir data");
    if (!fileExists(unreviewedSlipsFileName)) {
        fp = fopen(unreviewedSlipsFileName, "w");
        if (!fp) {
            printf("| Error opening fp!");
            exit(-1);
        }
        dateFile(unreviewedSlipsFileName);
        fclose(fp);
    } else if (!dateCompare() && !dateSearch(unreviewedSlipsFileName)) {
        fopen(unreviewedSlipsFileName, "a");
        if (!fp) {
            printf("| Error opening fp!");
            exit(-1);
        }
        fputs("\n\n**********************************************************\n", fp);
        fclose(fp);
        dateFile(unreviewedSlipsFileName);
    }
    if (!fileExists(beadleSlipFileName)) {
        fp5 = fopen(beadleSlipFileName, "w");
        if (!fp5) {
            printf("| Could not open fp5!");
            exit(-1);
        }
        fclose(fp5);
    }

    if (!fileExists(subjectsFileName)) {
        extraFiles(subjectsFileName);
    } else if (emptyFile(subjectsFileName)) {
        extraFiles(subjectsFileName);
    } else if (getLastCharInFile(fopen(subjectsFileName, "r")) != '\n') {
        fp2 = fopen(subjectsFileName, "a");
        fprintf(fp2, "\n");
        fclose(fp2);
    }

    if (!fileExists(timePeriodsFileName)) {
        extraFiles(timePeriodsFileName);
        printf("%s didn't exist, so I created it for you. However, it is empty!\n"
               "Please add information in the file to run the program."
               "\n\nEach period MUST be in the format #:## - #:## AM|PM.\nFor example: 8:00 - 8:35 AM", timePeriodsFileName);
        sleep(20);
        exit(10);
    } else if (emptyFile(timePeriodsFileName)) {
        printf("%s was empty!\nPlease add information in the file before running the program."
               "\n\nEach period MUST be in the format #:## - #:## AM|PM.\nFor example: 8:00 - 8:35 AM", timePeriodsFileName);
        sleep(20);
        exit(11);
    }

    if (!fileExists(doneTimesFileName)) {
        fp4 = fopen(doneTimesFileName, "w");
        if (!fp4) {
            printf("| Error opening fp4!");
            exit(-1);
        }
        dateFile(doneTimesFileName);
        fclose(fp4);
    } else if (!dateCompare() && !dateSearch(doneTimesFileName)) {
        fp4 = fopen(doneTimesFileName, "w");
        if (!fp4) {
            printf("| Error opening fp4!");
            exit(-1);
        }
        dateFile(doneTimesFileName);
        fclose(fp4);
    }
}

char getLastCharInFile(FILE* file) {
    if (!file) {
        printf("GetLastCharInFile | There was an error reading from the file...");
        exit(-1);
    }
    
    char ch, lastChar;

    ch = getc(file);
    while (ch != EOF) {
        lastChar = ch;
        ch = getc(file);
    }

    fclose(file);
    return lastChar;
}

int getLineCount(const char* filen) { // Function to count how many lines are in a file
    int lineCount = 0;
    char ch;
     if (strcmp(filen, unreviewedSlipsFileName) == 0) {
         fp = fopen(unreviewedSlipsFileName, "r");
         ch = getc(fp);
         while (ch != EOF) {
            if (ch == '\n') lineCount++;
            ch = getc(fp);
         }
         fclose(fp);
     } else if (strcmp(filen, subjectsFileName) == 0) {
         fp2 = fopen(subjectsFileName, "r");
         ch = getc(fp2);
         while (ch != EOF) {
            if (ch == '\n') lineCount++;
            ch = getc(fp2);
         }
         fclose(fp2);
     } else if (strcmp(filen, timePeriodsFileName) == 0) {
         fp3 = fopen(timePeriodsFileName, "r");
         ch = getc(fp3);
         while (ch != EOF) {
            if (ch == '\n') lineCount++;
            ch = getc(fp3);
         }
         fclose(fp3);
     } else if (strcmp(filen, doneTimesFileName) == 0) {
         fp4 = fopen(doneTimesFileName, "r");
         ch = getc(fp4);
         while (ch != EOF) {
            if (ch == '\n') lineCount++;
            ch = getc(fp4);
         }
         fclose(fp4);
     }
    return lineCount+1;
}

int getUnreviewedSlipsCount() { // Function to count the number of unreviewed slips there are in the file
    fp = fopen(unreviewedSlipsFileName, "r");
    if (!fp) {
         printf("| Could not open fp! [MODULE: getUnreviewedSlipsCount]");
         exit(-1);
    }
    int count = 0;
    char str[512];
    while(fgets(str, sizeof(str), fp)) {
        if (strstr(str, "DATE: ") != NULL) count++;
    }
    fclose(fp);
    return count;
}

int getReviewedSlipsCount() { // Function to count the number of reviewed slips there are in the file
    fp5 = fopen(beadleSlipFileName, "r");
    if (!fp5) {
         printf("| Could not open fp5! [MODULE: getReviewedSlipsCount]");
         exit(-1);
    }
    int count = 0;
    char str[512];
    while(fgets(str, sizeof(str), fp5)) {
        if (strstr(str, "DATE: ") != NULL) count++;
    }
    fclose(fp5);
    return count;
}

void extraFiles(const char* name) { // Function that creates the StudentNames.txt and SubjectNames.txt files on the machine
    FILE *temp;

    if (strcmp(name, subjectsFileName) == 0) {
        temp = fp2;
    } else if (strcmp(name, timePeriodsFileName) == 0) {
        temp = fp3;
    }

    temp = fopen(name, "w");
    if (!temp) {
        printf("| Error opening temp file pointer!");
        exit(-1);
    }

    if (strcmp(name, subjectsFileName) == 0)
        fprintf(temp, "BREAK\nNONE\n");
    fclose(temp);
}

const char* timeStrings(int key) { // Function to assign certain time period to a key integer
    char** times = getTimes();

    if (key < 0 || key > getPeriodCount())
        return NULL;
    return times[key];
}

char** getSubjects() {
    int maxSubjects = getSubjectCount() + 3;
    char **ret = (char**)malloc(maxSubjects * sizeof(char*));
    for (int i = 0; i < maxSubjects; i++)
        ret[i] = (char*)malloc(128 * sizeof(char));

    fp2 = fopen(subjectsFileName, "r");
    if (!fp2) {
        printf("| Error opening fp2!");
        exit(-1);
    }

    char str[1024];
    int x = 0, y = 0;
    while (fgets(str, 1024, fp2)) {
        char subject[strlen(str)];
        for (int i = 0; i <= strlen(str); i++) {
            if (str[i] == '\n' || str[i] == '\t')
                continue;
            subject[y++] = str[i];
        }
        strcpy(ret[x++], subject);
        y = 0;
    }
    fclose(fp2);
    return ret;
}

char** getTimes() {
    int maxPeriods = getPeriodCount() + 3;
    char **ret = (char**)malloc(maxPeriods * sizeof(char*));
    for (int i = 0; i < maxPeriods; i++)
        ret[i] = (char*)malloc(128 * sizeof(char));

    fp3 = fopen(timePeriodsFileName, "r");
    if (!fp3) {
        printf("| Error opening fp3!");
        exit(-1);
    }

    char str[1024];
    int x = 0, y = 0;
    while (fgets(str, 1024, fp3)) {
        char time[strlen(str)];
        for (int i = 0; i <= strlen(str); i++) {
            if (str[i] == '\n' || str[i] == '\t' || str[i] == '"')
                continue;
            time[y++] = str[i];
        }
        strcpy(ret[x++], time);
        y = 0;
    }
    fclose(fp3);
    return ret;
}

int getSubjectCount() {
    return getLineCount(subjectsFileName);
}

int getPeriodCount() {
    return getLineCount(timePeriodsFileName);
}

const char* toUpper(const char argv[]) { // Function to convert a string into all uppercases
    char *ret = (char*)malloc(strlen(argv) * sizeof(char));
    strcpy(ret, argv);
    for (int i = 0; argv[i]; i++) 
        ret[i] = toupper(ret[i]);
    return ret;
}

const char* getDate() { // Function to make the current date into a string format
    char *cur_time = (char *)malloc(128 * sizeof(char *));
    time_t t;
    struct tm* ptm;
    t = time(NULL);
    ptm = localtime(&t);
    strftime(cur_time, 128, "%B %d, %Y\n", ptm);
    return cur_time;
}

void dateFile(const char* filen) { // Function to put the date string in a file
    char cur_time[128];
    time_t t;
    struct tm* ptm;
    t = time(NULL);
    ptm = localtime(&t);
    strftime(cur_time, 128, "%B %d, %Y", ptm);
    if (strcmp(filen, unreviewedSlipsFileName) == 0) {
        fp = fopen(unreviewedSlipsFileName, "a");
        fprintf(fp,"DATE: %s", cur_time);
        fclose(fp);
    } else if (strcmp(filen, doneTimesFileName) == 0) {
        fp4 = fopen(doneTimesFileName, "a");
        fprintf(fp4,"%s", cur_time);
        fclose(fp4);
    }  
}

Pair generatePair(int period, const char* name) {
    Pair pair;
    pair.period = period;
    pair.subject = (char*)malloc(50 * sizeof(char));

    if (name != NULL)
        strcpy(pair.subject, name);
    else
        strcpy(pair.subject, "");
    return pair;
}

SlipEntry generateSlipEntry(int key) {
    SlipEntry entry;
    const int studentCount = 34;

    entry.subjectAndPeriod = generatePair(key, NULL);

    entry.lateStudentList = (char**)malloc(studentCount * sizeof(char*));
    for (int i = 0; i < studentCount; i++)
        entry.lateStudentList[i] = (char*)malloc(128 * sizeof(char));
    
    entry.absentStudentList = (char**)malloc(studentCount * sizeof(char*));
    for (int i = 0; i < studentCount; i++)
        entry.absentStudentList[i] = (char*)malloc(128 * sizeof(char));
    
    entry.numOfLateStudents = 0;
    entry.numOfAbsentStudents = 0;

    entry.teacherArrival = (char*)malloc(100 * sizeof(char));
    return entry;
}

void addStudentToSlipArray(SlipEntry* entry, ArrayType arrayType, const char* name) {
    bool stringIsEmpty = true;
    while (*name != '\0') {
        if (!isspace((unsigned char) *name)) {
            stringIsEmpty = false;
            break;
        }
        name++;
    }

    if (strstr(name, "None") != NULL) return;

    if (stringIsEmpty) return;

    switch (arrayType) {
        case LATE: {
            sprintf(entry->lateStudentList[entry->numOfLateStudents++], "%s", name);
            break;
        }
        case ABSENT: {
            sprintf(entry->absentStudentList[entry->numOfAbsentStudents++], "%s", name);
            break;
        }
        default: {
            printf("Unexpected error: Invalid enum value!");
            exit(-1);
        }
    }
}

Slip generateSlip() {
    Slip slip;
    slip.entries = (SlipEntry*)malloc((getPeriodCount() + 3) * sizeof(SlipEntry));
    slip.numOfEntries = 0;
    slip.date = (char*)malloc(50 * sizeof(char));
    return slip;
}

void addEntryToSlip(Slip* slip, SlipEntry entry) {
    slip->entries[slip->numOfEntries++] = entry;
}

void printEntryToFile(SlipEntry slipEntry) {
    fp = fopen(unreviewedSlipsFileName, "a");
    if (!fp) {
        printf("There was an error opening fp...");
        exit(-1);
    }

    fprintf(fp, "\n%c\nPERIOD: %s\n", START_OF_SLIP, timeStrings(slipEntry.subjectAndPeriod.period));
    fprintf(fp, "SUBJECT: %s", slipEntry.subjectAndPeriod.subject);
    
    if (strcmp(toUpper(slipEntry.subjectAndPeriod.subject), "BREAK") == 0 || strcmp(toUpper(slipEntry.subjectAndPeriod.subject), "NONE") == 0) {
        fprintf(fp, "\n%c\n", END_OF_SLIP);
        return;
    }

    fputs("\n\nABSENT STUDENTS: {\n", fp);
    char **absentStudents = slipEntry.absentStudentList;
    for (int i = 0; i < slipEntry.numOfAbsentStudents; i++)
        fprintf(fp, "\t%s\n", absentStudents[i]);
    fputs("}\n\n", fp);

    fputs("LATE STUDENTS: {\n", fp);
    char **lateStudents = slipEntry.lateStudentList;
    for (int i = 0; i < slipEntry.numOfLateStudents; i++)
        fprintf(fp, "\t%s\n", lateStudents[i]);
    fputs("}\n", fp);

    bool teacherPresent = slipEntry.teacherPresence;
    fputs("TEACHER PRESENT: ", fp);
    fputs(teacherPresent ? "Yes\n" : "No\n", fp);

    if (teacherPresent)
        fprintf(fp, "TEACHER TIME OF ARRIVAL: %s\n%c\n", slipEntry.teacherArrival, END_OF_SLIP);
    else
        fprintf(fp, "%c\n", END_OF_SLIP);

    fclose(fp);
}

Queue getUnreviewedSlipsFromFile() {
    Queue queue = createQueue(50);

    if (emptyFile(unreviewedSlipsFileName))
        return queue;

    FILE *slipFile = fopen(unreviewedSlipsFileName, "r");
    
    char str[1024]; 
    bool readingLateStudents = false, readingAbsentStudents = false;
    
    Slip slip = generateSlip();
    SlipEntry entries[getPeriodCount()];
    for (int i = 0; i < getPeriodCount(); i++)
        entries[i] = generateSlipEntry(i + 1);
    
    int entryKey = 1;
    SlipEntry entry = entries[entryKey-1]; 
    while (fgets(str, sizeof(str), slipFile)) {
        if (strstr(str, "***") != NULL) {
            enqueue(&queue, slip);

            slip = generateSlip();
            for (int i = 0; i < getPeriodCount(); i++)
                entries[i] = generateSlipEntry(i + 1);
            entryKey = 1; entry = entries[entryKey-1]; 
            readingLateStudents = false, readingAbsentStudents = false;
        }

        // Start reading into slips once date is found;  
        if (readingLateStudents || readingAbsentStudents) {
            if (strstr(str, "}") != NULL) {
                readingAbsentStudents = false;
                readingLateStudents = false;
                continue;
            }
            
            char name[strlen(str)];
            strncpy(name, &str[1], strlen(str)-1);
            name[strlen(str) - 2] = '\0';

            ArrayType arrayType = readingLateStudents ? LATE : ABSENT;
            addStudentToSlipArray(&entry, arrayType, name);
        }

        if (strstr(str, "DATE:") != NULL) {
            char dateStr[50];
            strncpy(dateStr, &str[5], strlen(str) - 4);
            dateStr[strlen(str) - 6] = '\0';
            strcpy(slip.date, dateStr);
        } else if (strstr(str, "PERIOD:") != NULL) {
            entry.subjectAndPeriod.period = entryKey;
        } else if (strstr(str, "SUBJECT:") != NULL) {
            char subjectStr[150];
            strncpy(subjectStr, &str[9], strlen(str) - 10);
            subjectStr[strlen(str) - 9] = '\0';
            strcpy(entry.subjectAndPeriod.subject, subjectStr);
        } else if (strstr(str, "ABSENT STUDENTS:") != NULL) {
            readingAbsentStudents = true;
        } else if (strstr(str, "LATE STUDENTS:") != NULL) {
            readingLateStudents = true;
        } else if (strstr(str, "TEACHER PRESENT:") != NULL) {
            entry.teacherPresence = strstr(str, "Yes") != NULL ? true : false;
        } else if (strstr(str, "TEACHER TIME OF ARRIVAL:") != NULL) {
            char toaStr[strlen(str) - 26];
            strncpy(toaStr, &str[25], strlen(str) - 24);
            toaStr[strlen(str) - 26] = '\0';
            strcpy(entry.teacherArrival, toaStr);
        } else if (strstr(str, "~") != NULL) {
            addEntryToSlip(&slip, entry);
            entryKey++;
            entry = entries[entryKey-1];
        }
    }
    enqueue(&queue, slip);
    
    fclose(slipFile);
    return queue;
}

void editSlip(Slip* slip) {
    system("cls");
    printf("______________________________________\n");
    printf("\t\t[ EDITING ]\n");
    printf("\n______________________________________\n\n");
    printf("| There are %d periods to edit\n| Which period would you like to edit?\n\n", slip->numOfEntries);

    printf("[0] Approve slip\n");
    for (int i = 0; i < slip->numOfEntries; i++)
        printf("[%d] %s\n", i + 1, timeStrings(i));

    int choice = 0;
    printf("> CHOICE: ");

    do {
        getchar();
        scanf("%d", &choice);

        if (choice < 0 || choice > slip->numOfEntries) {
            printf("| Invalid choice!\n");
        } else if (choice == 0) {
            approveSlip(*slip);
        } else 
            editSlipEntry(*slip, &slip->entries[choice - 1]);
    } while (choice < 0 || choice > slip->numOfEntries);
}

void editSlipEntry(Slip slip, SlipEntry* entry) {
    system("cls");
    printf("______________________________________\n");
    printf("\t\t[ EDITING PERIOD %d]\n", entry->subjectAndPeriod.period);
    printf("\n______________________________________\n\n");

    printEntryToStdout(*entry);

    printf("\n| What would you like to edit?\n\n");
    printf("[0] Nothing\n[1] Subject\n[2] Late Students\n[3] Absent Students\n\n");
    
    int choice = 0;
    printf("> CHOICE: ");

    do {
        scanf("%d", &choice);

        if (choice < 0 || choice > 3) { 
            printf("| Invalid option!\n");
        } else {
            switch(choice) {
                case 0: {
                    slip.entries[entry->subjectAndPeriod.period] = *entry;
                    editSlip(&slip);
                }
                case 1: {
                    editSlipEntrySubject(slip, entry);
                    break;
                }
                case 2: {
                    editSlipEntryStudents(slip, entry, LATE);
                    break;
                }
                case 3: {
                    editSlipEntryStudents(slip, entry, ABSENT);
                    break;
                }
            }
        }
    } while (choice < 0 || choice > 3);
}

void editSlipEntrySubject(Slip slip, SlipEntry* entry) {

    printf("| What would you like to change the subject of this entry to?\n");
    printf("> SUBJECT: ");
    
    char subject[50];
    getchar();
    gets(subject);
    if (!subjectSearch(toUpper(subject))) {
        char choice; 
        int cont = 0;
        do {
            cont = 0;
            printf("| Not a valid subject!\n");
            printf("| Would you like to see a list of valid subjects? (y/n)");
            do {
                printf("\n> ");
                scanf("%c", &choice);
                if (choice == 'y' || choice == 'Y') {
                    printSubjects();
                    cont++;
                } else if (choice == 'n' || choice == 'N') {
                    printf("| Okay!\n");
                    cont++;
                } else {
                    printf("| Invalid input!");
                    getchar();
                }
            } while (cont == 0);
            getchar();
            printf("\n> SUBJECT: ");
            gets(subject);
        } while (!subjectSearch(toUpper(subject)));
    }

    strcpy(entry->subjectAndPeriod.subject, subject);
    editSlipEntry(slip, entry);
}

void editSlipEntryStudents(Slip slip, SlipEntry* entry, ArrayType type) {
    switch (type) { 
        case LATE: {
            if (entry->numOfLateStudents == 0) {
                entry->lateStudentList = (char**)malloc(34 * sizeof(char*));
                for (int i = 0; i < 34; i++)
                    entry->lateStudentList[i] = (char*)malloc(128 * sizeof(char));
            }

            int cont4 = 0;
            do {
                int i = 1;
                char temp2;
                int cont = 1;
                cont4++;

                printf("| Who were the late students?\n");
                while (cont == 1) {
                    char studentName[100];
                    getchar();
                    printf("\n[%d] NAME: ", i);
                    gets(studentName);

                    if (studentEnteredAlready(entry, entry->numOfLateStudents, studentName))
                        printf("| You've already entered this student!\n| Press enter to continue");
                    else {
                        addStudentToSlipArray(entry, LATE, studentName);
                        i++;
                        printf("\n| Is that all? (Y/n)");
                        int cont2 = 0;
                                
                        do {
                            printf("\n> ");
                            scanf("%c", &temp2);
                            if (temp2 == 'Y' || temp2 == 'y') {
                                cont = 0;
                                cont2++;
                            } else if (temp2 == 'N' || temp2 == 'n') {
                                cont = 1;
                                cont2++;
                            } else {
                                printf("| Invalid input!");
                                getchar();
                            }
                        } while (cont2 == 0);
                    }
                }
            } while (cont4 == 0);

            slip.entries[entry->subjectAndPeriod.period] = *entry;
            editSlip(&slip);

            break;
        }
        case ABSENT: {
            if (entry->numOfAbsentStudents == 0) {
                entry->absentStudentList = (char**)malloc(34 * sizeof(char*));
                for (int i = 0; i < 34; i++)
                    entry->absentStudentList[i] = (char*)malloc(128 * sizeof(char));
            }

            int cont4 = 0;
            do {
                int i = 1;
                char temp2;
                int cont = 1;
                cont4++;

                printf("| Who were the absent students?\n");
                while (cont == 1) {
                    char studentName[100];
                    getchar();
                    printf("\n[%d] NAME: ", i);
                    gets(studentName);

                    if (studentEnteredAlready(entry, entry->numOfAbsentStudents, studentName))
                        printf("| You've already entered this student!\n| Press enter to continue");
                    else {
                        addStudentToSlipArray(entry, ABSENT, studentName);
                        i++;
                        printf("\n| Is that all? (Y/n)");
                        int cont2 = 0;
                                
                        do {
                            printf("\n> ");
                            scanf("%c", &temp2);
                            if (temp2 == 'Y' || temp2 == 'y') {
                                cont = 0;
                                cont2++;
                            } else if (temp2 == 'N' || temp2 == 'n') {
                                cont = 1;
                                cont2++;
                            } else {
                                printf("| Invalid input!");
                                getchar();
                            }
                        } while (cont2 == 0);
                    }
                }
            } while (cont4 == 0);

            slip.entries[entry->subjectAndPeriod.period] = *entry;
            editSlip(&slip);

            break;
        }
    }
}

// Queue Methods
Queue createQueue(unsigned capacity) {
    Queue queue;
    queue.capacity = capacity;
    queue.front = queue.size = 0;
    queue.rear = capacity-1;
    queue.arr = (Slip*)malloc(queue.capacity * sizeof(Slip));
    return queue;
}

bool queueIsFull(Queue* queue) {
    return queue->size == queue->capacity;
}

bool queueIsEmpty(Queue* queue) {
    return queue->size == 0;
}

void enqueue(Queue* queue, Slip slip) {
    if (queueIsFull(queue)) {
        printf("Queue overflow!\n");
        return;
    }

    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->arr[queue->rear] = slip;
    queue->size++;
}

Slip dequeue(Queue* queue) {
    if (queueIsEmpty(queue)) {
        printf("Queue underflow!\n");
        exit(-1);
    }

    Slip item = queue->arr[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size--;
    return item;
}