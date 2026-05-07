//Group 4 - UV Commute Pro
//Members:
//1. Alyssa Faye Bulante
//2. AVegail Golpe
//3. Kirby Ann Joven
//4. Neil Peladra

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// UV Commute Pro - A simple bus reservation system for UV students
#define MAX_SCHEDULES 8
#define MAX_SEATS_PER_SCHEDULE 15

// Structure for schedules
typedef struct {
    char route[50];
    char time[30];
    int occupiedSeats;
} Schedule;

// Structure for seats
typedef struct {
    int seatNum;
    int isAvailable;
} Seat;

// Node structure for booking history
typedef struct Node {
    char name[50];
    int seatNo;
    char route[50];
    char time[30];
    float amountPaid;
    char status[20];
    struct Node* next;
} Node;

// Queue structure
typedef struct {
    Node* front;
    Node* rear;
} Queue;

// Function prototypes
int isValidName(char name[]);
void initQueue(Queue* q);
int isEmpty(Queue* q);

void enqueue(
    Queue* q,
    char name[],
    int seat,
    char route[],
    char time[],
    float pay
);
// Display booking history
void displayQueue(Queue* q);
// Seat functions
void initSeats(Seat seats[]);
void displaySeats(Seat seats[]);
// Payment
int processPayment(float* total, float* paid);
// Cancel booking
void cancelBooking(
    Queue* bookingQueue,
    Schedule schedules[],
    Seat seats[][MAX_SEATS_PER_SCHEDULE],
    int *selectedSchedule,
    int *selectedSeat,
    char *selectedRoute,
    char *selectedTime
);
// Generate receipt
void generateReceipt(
    char name[],
    int seat,
    char route[],
    char time[],
    float paid
);

// Validate name
int isValidName(char name[]) {

    int i;

    for(i = 0; name[i] != '\0'; i++) {

        if(!isalpha(name[i]) && !isspace(name[i])) {
            return 0;
        }
    }

    return 1;
}

// Queue functions
void initQueue(Queue* q) {

    q->front = NULL;
    q->rear = NULL;
}
// Check if queue is empty
int isEmpty(Queue* q) {

    return (q->front == NULL);
}

// Add booking to history
void enqueue(
    Queue* q,
    char name[],
    int seat,
    char route[],
    char time[],
    float pay
) {// Create new node
    Node* newNode = (Node*)malloc(sizeof(Node));

    strcpy(newNode->name, name);
    newNode->seatNo = seat;

    strcpy(newNode->route, route);
    strcpy(newNode->time, time);

    newNode->amountPaid = pay;

    strcpy(newNode->status, "ACTIVE");

    newNode->next = NULL;
// If queue is empty, new node becomes front and rear
    if(isEmpty(q)) {

        q->front = newNode;
        q->rear = newNode;

    } else {
// Add to end of queue
        q->rear->next = newNode; 
        q->rear = newNode;
    }
}

// Display booking history
void displayQueue(Queue* q) {

    if(isEmpty(q)) {

        printf("No bookings yet!\n");
        return;
    }

    Node* temp = q->front;

    while(temp != NULL) {

        printf("Name: %-15s | Seat: %d | Time: %s | Paid: PHP %.2f | Status: %s\n",
               temp->name,
               temp->seatNo,
               temp->time,
               temp->amountPaid,
               temp->status);

        temp = temp->next;
    }
}

// Initialize seats for a schedule
void initSeats(Seat seats[]) {

    int i;

    for(i = 0; i < MAX_SEATS_PER_SCHEDULE; i++) {

        seats[i].seatNum = i + 1;
        seats[i].isAvailable = 1;
    }
}

// Display available seats
void displaySeats(Seat seats[]) {

    int i;

    printf("Available Seats:\n");

    for(i = 0; i < MAX_SEATS_PER_SCHEDULE; i++) {

        if(seats[i].isAvailable == 1) {

            printf("%d ", seats[i].seatNum);
        }
    }

    printf("\n");
}

// Payment processing
int processPayment(float* total, float* paid) {

    printf("Minimum down payment: PHP 50.00\n");
    printf("Enter amount to pay: ");

    scanf("%f", paid);
    getchar();

    if(*paid >= 50 && *paid <= *total) {

        printf("Payment accepted! Remaining balance: PHP %.2f\n", //display remaining balance
               *total - *paid);

        return 1;

    } else 

        printf("Invalid amount! Must be at least 50 and not exceed total fare.\n");

        return 0;
    }


// Cancel booking
void cancelBooking(
    Queue* bookingQueue,
    Schedule schedules[],
    Seat seats[][MAX_SEATS_PER_SCHEDULE],
    int *selectedSchedule,
    int *selectedSeat,
    char *selectedRoute,
    char *selectedTime
) {

    int oldSchedule = *selectedSchedule;// Store old schedule before resetting

    if(oldSchedule == 0 || *selectedSeat == 0) {

        printf("No booking found!\n");
        return;
    }

    // Decrease occupied seats
    schedules[oldSchedule - 1].occupiedSeats--;

    // Make seat available again
    seats[oldSchedule - 1][*selectedSeat - 1].isAvailable = 1;

    // Update booking status
    Node* temp = bookingQueue->front;

    while(temp != NULL) {// Find the matching booking

        if(strcmp(temp->route, selectedRoute) == 0 &&
           strcmp(temp->time, selectedTime) == 0 &&
           temp->seatNo == *selectedSeat) {

            strcpy(temp->status, "CANCELLED");
            break;
        }

        temp = temp->next;
    }

    // Reset booking details
    *selectedSchedule = 0;
    *selectedSeat = 0;

    strcpy(selectedRoute, "");
    strcpy(selectedTime, "");

    printf("Booking cancelled successfully!\n");
}

// Generate receipt
void generateReceipt(
    char name[],
    int seat,
    char route[],
    char time[],
    float paid
) {

    printf("\n========== RECEIPT ==========\n");

    printf("Name: %s\n", name);
    printf("Seat Number: %d\n", seat);
    printf("Route: %s\n", route);
    printf("Schedule: %s\n", time);
    printf("Amount Paid: PHP %.2f\n", paid);

    printf("=============================\n");
}

// Main function
int main() {

    Queue bookingQueue;
    initQueue(&bookingQueue);

    // Separate seats for every schedule
    Seat seats[MAX_SCHEDULES][MAX_SEATS_PER_SCHEDULE];

    int i;

    for(i = 0; i < MAX_SCHEDULES; i++) {

        initSeats(seats[i]);
    }

    Schedule schedules[MAX_SCHEDULES] = {// Predefined schedules

        {"Sipocot to Naga", "7:30 - 8:30", 0},
        {"Sipocot to Naga", "8:30 - 9:30", 0},
        {"Sipocot to Naga", "9:30 - 10:30", 0},
        {"Sipocot to Naga", "10:30 - 11:30", 0},
        {"Sipocot to Naga", "11:30 - 12:30", 0},
        {"Sipocot to Naga", "12:30 - 1:30", 0},
        {"Sipocot to Naga", "1:30 - 2:30", 0},
        {"Sipocot to Naga", "2:30 - 3:30", 0}
    };

    int choice;// User details and booking info

    char userName[50] = "";

    int selectedSeat = 0;
    int selectedSchedule = 0;

    char selectedRoute[50] = "";
    char selectedTime[30] = "";

    float fare = 100.0;
    float amountPaid = 0;

    do {

        printf("\n===== UV COMMUTE PRO =====\n");

        printf("1. Input Full Name\n");
        printf("2. Choose Schedule & Time\n");
        printf("3. Reserve Seat\n");
        printf("4. Pay for your Ride\n");
        printf("5. Cancel Booking\n");
        printf("6. Generate Receipt\n");
        printf("7. View Booking History\n");
        printf("8. Exit\n");

        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar();

        switch(choice) {

            case 1:// Input and validate full name

                printf("\n--- Input Full Name ---\n");

                while(1) {

                    printf("Enter your full name: ");

                    fgets(userName, sizeof(userName), stdin);

                    userName[strcspn(userName, "\n")] = '\0';

                    if(strlen(userName) == 0) {

                        printf("Name cannot be empty!\n");

                    } else if(!isValidName(userName)) {

                        printf("Invalid! Letters and spaces only.\n");

                    } else {

                        break;
                    }
                }

                printf("Name saved: %s\n", userName);

                break;

            case 2:// Show schedules and select

                printf("\n--- Available Schedules ---\n");

                for(i = 0; i < MAX_SCHEDULES; i++) {

                    printf("%d. %-20s %s | Seats Left: %d/%d\n",
                           i + 1,
                           schedules[i].route,
                           schedules[i].time,
                           MAX_SEATS_PER_SCHEDULE - schedules[i].occupiedSeats,
                           MAX_SEATS_PER_SCHEDULE);
                }

                printf("\nEnter schedule number: ");

                scanf("%d", &selectedSchedule);
                getchar();

                if(selectedSchedule >= 1 &&
                   selectedSchedule <= MAX_SCHEDULES) {

                    strcpy(selectedRoute,
                           schedules[selectedSchedule - 1].route);

                    strcpy(selectedTime,
                           schedules[selectedSchedule - 1].time);

                    printf("Schedule selected: %s | %s\n",
                           selectedRoute,
                           selectedTime);

                } else {

                    printf("Invalid schedule number!\n");
                    selectedSchedule = 0;
                }

                break;

            case 3:// Show seats and reserve

                printf("\n--- Seat Reservation ---\n");

                if(selectedSchedule == 0) {

                    printf("Please choose a schedule first!\n");
                    break;
                }

                // Show seats for selected schedule only
                displaySeats(seats[selectedSchedule - 1]);

                printf("\nEnter seat number to reserve (1-%d): ",
                       MAX_SEATS_PER_SCHEDULE);

                scanf("%d", &selectedSeat);
                getchar();

                if(selectedSeat >= 1 &&
                   selectedSeat <= MAX_SEATS_PER_SCHEDULE) {

                    if(seats[selectedSchedule - 1][selectedSeat - 1].isAvailable == 1) {

                        seats[selectedSchedule - 1][selectedSeat - 1].isAvailable = 0;

                        schedules[selectedSchedule - 1].occupiedSeats++;

                        printf("Seat %d reserved successfully!\n",
                               selectedSeat);

                    } else {

                        printf("Seat %d is already occupied in this schedule!\n",
                               selectedSeat);
                    }

                } else {

                    printf("Invalid seat number!\n");
                }

                break;

            case 4:// Process payment

                printf("\n--- Payment ---\n");

                if(strlen(userName) == 0 ||
                   selectedSchedule == 0 ||
                   selectedSeat == 0) {

                    printf("Complete name, schedule, and seat first!\n");
                    break;
                }

                printf("Total Fare: PHP %.2f\n", fare);

                if(processPayment(&fare, &amountPaid)) {

                    enqueue(
                        &bookingQueue,
                        userName,
                        selectedSeat,
                        selectedRoute,
                        selectedTime,
                        amountPaid
                    );
                }

                break;

            case 5: // Cancel booking

                cancelBooking(
                    &bookingQueue,
                    schedules,
                    seats,
                    &selectedSchedule,
                    &selectedSeat,
                    selectedRoute,
                    selectedTime
                );

                break;

            case 6:// Generate receipt if the process is complete

                if(strlen(userName) > 0 &&
                   selectedSchedule != 0 &&
                   selectedSeat != 0 &&
                   amountPaid > 0) {

                    generateReceipt(
                        userName,
                        selectedSeat,
                        selectedRoute,
                        selectedTime,
                        amountPaid
                    );

                } else {

                    printf("Complete all steps first!\n");
                }

                break;

            case 7: // View booking history

                displayQueue(&bookingQueue);

                break;

            case 8: // Exit

                printf("Thank you for using UV Commute Pro!\n");

                break;

            default:

                printf("Invalid choice! Try again.\n");
        }

    } while(choice != 8);

    return 0;
}
