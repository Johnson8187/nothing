#include <iostream>

int main(void) {
    int option;
    int amount;
    int balance = 1000;
    
    while(1){ 
        printf("Enter option:");
        scanf("%d", &option);
        switch(option) {
            case 1:
                printf("Your balance is: %d\n", balance);
                break;
            case 2:
                printf("Enter amount to deposit:");
                scanf("%d", &amount);
                balance += amount;
                printf("Deposit successful!\nYour balance is: %d\n", balance);
                break;
            case 3:
                printf("Enter amount to withdraw:");
                scanf("%d", &amount);
                if(amount > balance) {
                    printf("Insufficient balance!\n");
                } else {
                    balance -= amount;
                    printf("Withdrawal successful!\nYour balance is: %d\n", balance);
                }
                break;
            case 4:
                printf("Thank you for using our ATM service!\n");
                return 0;
            }
    }
    return 0;
}


