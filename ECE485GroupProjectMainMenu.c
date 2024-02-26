//This is the Main Menu Program for ECE485 Group project

#include <stdio.h>

// Function prototypes
void mainMenu();
void option1();
void option2();
int exitProgram();

int main() {
    int choice;

    do {
        //Display the options for the mainMenu
        mainMenu();

        //User Input here
        printf("\nEnter your choice: ");
        scanf("%d", &choice);

        //Choices for the menu
        if(choice == 1){
            
            option1();
        }
        else if(choice == 2){
            option2();
        }
        else if(choice == 3){
    
            exitProgram();
        }
        else {
            
            printf("\nYou have not entered a valid input\n");
            printf("Please Try agian\n");
            continue;
        }
    } while (choice != 3);

}

//This function will display the menu for our program
void mainMenu() {
        
        printf("\n1: (Will place what it contains here)");
        printf("\n2: (Will place what it contains here)");
        printf("\n3 To exit the program");

}
// Function definitions for menu options
void option1() {
    printf("You selected Option 1.\n");
    // We will place the first Cache thing here
}

void option2() {//Choose this for 
    printf("You selected Option 2.\n");
    // We will place the second Cache thing here
}
int exitProgram() { //This is to exit the program
    printf("\nThe program will now exit");
    return(0);    
}




