#include <iostream>

int main(void) {
    int password;
    int counter = 0;
    do{
        printf("Enter password:");
        scanf("%d", &password);
        if(password == 1234) {
            printf("Access granted!\n");
            break;
        } else {
            printf("Access denied!\n");
        }
        counter++;
    }while(counter < 3);
    if (counter == 3) {
    printf("Too many failed attempts. Exiting...\n");
    }
    return 0;
}