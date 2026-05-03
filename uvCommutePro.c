#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_SCHEDULES 8
#define MAX_SEATS_PER_SCHEDULE 15

typedef struct {
    char route[50];
    char time[30];
    int occupiedSeats;
} Schedule;

typedef struct {
    int seatNum;
    int isAvailable;
} Seat;

typedef struct Node {
    char name[50];
    int seatNo;
    char route[50];
    char time[30];
    float amountPaid;
    struct Node* next;
} Node;

typedef struct {
    Node* front;
    Node* rear;
} Queue;

typedef struct StackNode {
    int scheduleIndex;
    int seatNo;
    struct StackNode* next;
} StackNode;

int isValidName(char name[]);
void initQueue(Queue* q);
int isEmpty(Queue* q);
void enqueue(Queue* q, char name[], int seat, char route[], char time[], float pay);
void displayQueue(Queue* q);
void initSeats(Seat seats[]);
void displaySeats(Seat seats[]);
void processPayment(float* total, float* paid);
void cancelBooking(Schedule schedules[], int *selectedSchedule, char *selectedRoute, char *selectedTime, StackNode** undoStack);
void generateReceipt(char name[], int seat, char route[], char time[], float paid);
void pushUndo(StackNode** top, int scheduleIndex, int seatNo);
void popUndo(StackNode** top, Schedule schedules[], Seat seats[]);

int isValidName(char name[]) {
	int i;
    for(i=0; name[i] != '\0'; i++) {
        if(!isalpha(name[i]) && !isspace(name[i])) {
            return 0;
        }
    }
    return 1;
}

void initQueue(Queue* q) {
    q->front = NULL;
    q->rear = NULL;
}

int isEmpty(Queue* q) {
    return (q->front == NULL);
}

void enqueue(Queue* q, char name[], int seat, char route[], char time[], float pay) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    strcpy(newNode->name, name);
    newNode->seatNo = seat;
    strcpy(newNode->route, route);
    strcpy(newNode->time, time);
    newNode->amountPaid = pay;
    newNode->next = NULL;

    if(isEmpty(q)) {
        q->front = newNode;
        q->rear = newNode;
    } else {
        q->rear->next = newNode;
        q->rear = newNode;
    }
}

void displayQueue(Queue* q) {
    if(isEmpty(q)) {
        printf("No bookings yet!\n");
        return;
    }
    Node* temp = q->front;
    while(temp != NULL) {
        printf("Name: %-15s | Seat: %d | Time: %s | Paid: PHP %.2f\n",
               temp->name, temp->seatNo, temp->time, temp->amountPaid);
        temp = temp->next;
    }
}

void initSeats(Seat seats[]) {
    for(int i=0; i<MAX_SEATS_PER_SCHEDULE; i++) {
        seats[i].seatNum = i+1;
        seats[i].isAvailable = 1;
    }
}

void displaySeats(Seat seats[]) {
    printf("Available Seats:\n");
    for(int i=0; i<MAX_SEATS_PER_SCHEDULE; i++) {
        if(seats[i].isAvailable)
            printf("%d ", seats[i].seatNum);
    }
    printf("\n");
}

void processPayment(float* total, float* paid) {
    printf("Minimum down payment: PHP 50.00\n");
    printf("Enter amount to pay: ");
    scanf("%f", paid);
    getchar();
    if(*paid >= 50 && *paid <= *total) {
        printf("Payment accepted! Remaining balance: PHP %.2f\n", *total - *paid);
    } else {
        printf("Invalid amount! Must be at least 50 and not exceed total fare.\n");
    }
}

void pushUndo(StackNode** top, int scheduleIndex, int seatNo) {
    StackNode* newNode = (StackNode*)malloc(sizeof(StackNode));
    newNode->scheduleIndex = scheduleIndex;
    newNode->seatNo = seatNo;
    newNode->next = *top;
    *top = newNode;
}

void popUndo(StackNode** top, Schedule schedules[], Seat seats[]) {
    if(*top == NULL) {
        printf("No actions to undo!\n");
        return;
    }
    StackNode* temp = *top;
    schedules[temp->scheduleIndex].occupiedSeats++;
    seats[temp->seatNo-1].isAvailable = 0;
    printf("Undo successful! Seat %d restored in schedule %d.\n", temp->seatNo, temp->scheduleIndex+1);
    *top = temp->next;
    free(temp);
}

void cancelBooking(Schedule schedules[], int *selectedSchedule, char *selectedRoute, char *selectedTime, StackNode** undoStack) {
    char reason[100];
    int option;
    int oldSchedule = *selectedSchedule;
    printf("Enter reason for cancellation/rescheduling: ");
    fgets(reason, sizeof(reason), stdin);
    reason[strcspn(reason, "\n")] = '\0';

    printf("\nChoose action:\n");
    printf("1. Reschedule to another time\n");
    printf("2. Full Refund\n");
    printf("Enter choice: ");
    scanf("%d", &option);
    getchar();

    if(option == 1) {
        printf("\n--- AVAILABLE SCHEDULES ---\n");
        for(int i=0; i<MAX_SCHEDULES; i++) {
            printf("%d. %-20s %s | Seats Left: %d/%d\n",
                   i+1, schedules[i].route, schedules[i].time,
                   MAX_SEATS_PER_SCHEDULE - schedules[i].occupiedSeats,
                   MAX_SEATS_PER_SCHEDULE);
        }
        printf("\nEnter NEW schedule number you want: ");
        int newSched;
        scanf("%d", &newSched);
        getchar();

        if(newSched >=1 && newSched <= MAX_SCHEDULES) {
            if(schedules[newSched-1].occupiedSeats >= MAX_SEATS_PER_SCHEDULE) {
                printf("Sorry! New schedule is FULL. Cannot reschedule.\n");
            } else {
                schedules[oldSchedule-1].occupiedSeats--;
                schedules[newSched-1].occupiedSeats++;
                *selectedSchedule = newSched;
                strcpy(selectedRoute, schedules[newSched-1].route);
                strcpy(selectedTime, schedules[newSched-1].time);
                printf("\nRESCHEDULE SUCCESSFUL!\n");
                printf("New Schedule: %s | %s\n", selectedRoute, selectedTime);
            }
        } else {
            printf("Invalid schedule number!\n");
        }
    } else if(option == 2) {
        schedules[oldSchedule-1].occupiedSeats--;
        pushUndo(undoStack, oldSchedule-1, 1);
        *selectedSchedule = 0;
        strcpy(selectedRoute, "");
        strcpy(selectedTime, "");
        printf("Refund will be processed within 3-5 business days.\n");
    } else {
        printf("Invalid option!\n");
    }
}

void generateReceipt(char name[], int seat, char route[], char time[], float paid) {
    printf("\n========== RECEIPT ==========\n");
    printf("Name: %s\n", name);
    printf("Seat Number: %d\n", seat);
    printf("Route: %s\n", route);
    printf("Schedule: %s\n", time);
    printf("Amount Paid: PHP %.2f\n", paid);
    printf("=============================\n");
}

int main() {
    Queue bookingQueue;
    initQueue(&bookingQueue);

    Seat seats[MAX_SEATS_PER_SCHEDULE];
    initSeats(seats);

    Schedule schedules[MAX_SCHEDULES] = {
        {"Sipocot to Naga", "7:30 - 8:30", 0},
        {"Sipocot to Naga", "8:30 - 9:30", 0},
        {"Sipocot to Naga", "9:30 - 10:30", 0},
        {"Sipocot to Naga", "10:30 - 11:30", 0},
        {"Sipocot to Naga", "11:30 - 12:30", 0},
        {"Sipocot to Naga", "12:30 - 1:30", 0},
        {"Sipocot to Naga", "1:30 - 2:30", 0},
        {"Sipocot to Naga", "2:30 - 3:30", 0}
    };

    int choice;
    char userName[50] = "";
    int selectedSeat = 0;
    int selectedSchedule = 0;
    char selectedRoute[50] = "";
    char selectedTime[30] = "";
       float fare = 100.0; 
    float amountPaid = 0;
    StackNode* undoStack = NULL;

    do {
        printf("\n\n===== UV COMMUTE PRO =====\n");
        printf("1. Input Full Name\n");
        printf("2. Choose Schedule & Time\n");
        printf("3. Reserve Seat\n");
        printf("4. Pay for your Ride\n");
        printf("5. Cancel Booking\n");
        printf("6. Generate Receipt\n");
        printf("7. View Booking History\n");
        printf("8. Undo Last Cancellation\n");
        printf("9. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar();

        switch(choice) {
            case 1:
                printf("\n--- Input Full Name ---\n");
                while(1) {
                    printf("Enter your full name (letters only): ");
                    fgets(userName, sizeof(userName), stdin);
                    userName[strcspn(userName, "\n")] = '\0';
                    if(strlen(userName) == 0) {
                        printf("Name cannot be empty! Try again.\n");
                    } else if(!isValidName(userName)) {
                        printf("Invalid! Name can only contain letters and spaces.\n");
                    } else {
                        break;
                    }
                }
                printf("Name saved: %s\n", userName);
                break;

            case 2:
                printf("\n--- Available Schedules ---\n");
                int i;
                for(i=0; i<MAX_SCHEDULES; i++) {
                    printf("%d. %-20s %s | Seats Left: %d/%d\n",
                           i+1, schedules[i].route, schedules[i].time,
                           MAX_SEATS_PER_SCHEDULE - schedules[i].occupiedSeats,
                           MAX_SEATS_PER_SCHEDULE);
                }
                printf("\nEnter the NUMBER of the schedule you want: ");
                scanf("%d", &selectedSchedule);
                getchar();
                if(selectedSchedule >=1 && selectedSchedule <= MAX_SCHEDULES) {
                    if(schedules[selectedSchedule-1].occupiedSeats >= MAX_SEATS_PER_SCHEDULE) {
                        printf("SORRY! This schedule is FULL.\n");
                        selectedSchedule = 0;
                    } else {
                        strcpy(selectedRoute, schedules[selectedSchedule-1].route);
                        strcpy(selectedTime, schedules[selectedSchedule-1].time);
                        printf("Schedule selected: %s | %s\n", selectedRoute, selectedTime);
                    }
                } else {
                    printf("Invalid schedule number!\n");
                    selectedSchedule = 0;
                }
                break;

            case 3:
                printf("\n--- Seat Reservation ---\n");
                if(selectedSchedule == 0) {
                    printf("Please choose a schedule first!\n");
                    break;
                }
                displaySeats(seats);
                printf("\nEnter seat number to reserve (1-%d): ", MAX_SEATS_PER_SCHEDULE);
                scanf("%d", &selectedSeat);
                if(selectedSeat >=1 && selectedSeat <= MAX_SEATS_PER_SCHEDULE) {
                    if(seats[selectedSeat-1].isAvailable == 1) {
                        seats[selectedSeat-1].isAvailable = 0;
                        schedules[selectedSchedule-1].occupiedSeats++;
                        printf("Seat %d reserved successfully!\n", selectedSeat);
                    } else {
                        printf("Sorry, Seat %d is already taken!\n", selectedSeat);
                    }
                } else {
                    printf("Invalid seat number!\n");
                }
                break;

            case 4:
                printf("\n--- Payment ---\n");
                if(strlen(userName) == 0 || selectedSchedule == 0 || selectedSeat == 0) {
                    printf("Complete name, schedule, and seat first!\n");
                    break;
                }
                printf("Total Fare: PHP %.2f\n", fare);
                processPayment(&fare, &amountPaid);
                enqueue(&bookingQueue, userName, selectedSeat, selectedRoute, selectedTime, amountPaid);
                break;

            case 5:
                cancelBooking(schedules, &selectedSchedule, selectedRoute, selectedTime, &undoStack);
                break;

            case 6:
                if(strlen(userName) > 0 && selectedSchedule !=0 && selectedSeat !=0 && amountPaid > 0) {
                    generateReceipt(userName, selectedSeat, selectedRoute, selectedTime, amountPaid);
                } else {
                    printf("Complete all steps first!\n");
                }
                break;

            case 7:
                displayQueue(&bookingQueue);
                break;

            case 8:
                popUndo(&undoStack, schedules, seats);
                break;

            case 9:
                printf("Thank you for using UV Commute Pro!\n");
                break;

            default:
                printf("Invalid choice! Try again.\n");
        }
    } while(choice != 9);

    return 0;
}
