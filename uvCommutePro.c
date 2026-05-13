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
#include <time.h>

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
    char bookingDateTime[100];
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
    char scheduleTime[],
    float pay
);

void displayQueue(Queue* q);
void searchPassenger(Queue* q, char searchName[]);

void initSeats(Seat seats[]);
void displaySeats(Seat seats[]);

int processPayment(float* total, float* paid);

void cancelBooking(
    Queue* bookingQueue,
    Schedule schedules[],
    Seat seats[][MAX_SEATS_PER_SCHEDULE],
    int *selectedSchedule,
    int *selectedSeat,
    char *selectedRoute,
    char *selectedTime
);

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

int isEmpty(Queue* q) {
    return (q->front == NULL);
}

// Add booking to history
void enqueue(
    Queue* q,
    char name[],
    int seat,
    char route[],
    char scheduleTime[],
    float pay
) {

    Node* newNode = (Node*)malloc(sizeof(Node));

    strcpy(newNode->name, name);
    newNode->seatNo = seat;

    strcpy(newNode->route, route);
    strcpy(newNode->time, scheduleTime);

    newNode->amountPaid = pay;

    strcpy(newNode->status, "ACTIVE");

    time_t now;
    struct tm *local;
    time(&now);
    local = localtime(&now);
    strftime(newNode->bookingDateTime, sizeof(newNode->bookingDateTime), "%m-%d-%Y %I:%M %p", local);

    newNode->next = NULL;

    if(isEmpty(q)) {

        q->front = newNode;
        q->rear = newNode;

    } else {

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
    Node* arr[100]; 
    int count = 0;

    while(temp != NULL) {
        arr[count++] = temp;
        temp = temp->next;
    }

    // Bubble sort by date and time
    for(int i = 0; i < count - 1; i++) {
        for(int j = 0; j < count - i - 1; j++) {
            if(strcmp(arr[j]->bookingDateTime, arr[j+1]->bookingDateTime) > 0) {
                Node* swap = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = swap;
            }
        }
    }

    // Display sorted bookings
    printf("\n-----------------------------------------------------------------------------------\n");
    printf("%-20s | %-15s | %-15s | %-12s | %-15s\n",
           "Date & Time", "Passenger Name", "Route", "Seat Occupied", "Schedule");
    printf("-----------------------------------------------------------------------------------\n");
    for(int i = 0; i < count; i++) {
        printf("%-20s | %-15s | %-15s | %-12d | %-15s\n",
               arr[i]->bookingDateTime,
               arr[i]->name,
               arr[i]->route,
               arr[i]->seatNo,
               arr[i]->time);
    }
    printf("-----------------------------------------------------------------------------------\n");
}
//Linear search for passenger by name
void searchPassenger(Queue* q, char searchName[]) {
    Node* temp = q->front;
    int found = 0;

    while(temp != NULL) {
        if(strcmp(temp->name, searchName) == 0) {
            printf("\nPassenger Found!\n");
            printf("------------------------------------------------------------------------------------------------------------\n");
            printf("%-20s | %-15s | %-15s | %-12s | %-12s | %-10s\n",
                   "Date & Time", "Route", "Schedule", "Seat Occupied", "Paid Amount", "Status");
            printf("------------------------------------------------------------------------------------------------------------\n");
            printf("%-20s | %-15s | %-15s | %-12d | PHP %-7.2f | %-10s\n",
                   temp->bookingDateTime,
                   temp->route,
                   temp->time,
                   temp->seatNo,
                   temp->amountPaid,
                   temp->status);
            printf("------------------------------------------------------------------------------------------------------------\n");
            found = 1;
            break;
        }
        temp = temp->next;
    }

    if(!found) {
        printf("Search for '%s': No result found.\n", searchName);
    }
}

// Initialize seats
void initSeats(Seat seats[]) {
	int i;
    for(i = 0; i < MAX_SEATS_PER_SCHEDULE; i++) {

        seats[i].seatNum = i + 1;
        seats[i].isAvailable = 1;
    }
}

// Display available seats
void displaySeats(Seat seats[]) {

    printf("Available Seats:\n");

    for(int i = 0; i < MAX_SEATS_PER_SCHEDULE; i++) {

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
        printf("Payment accepted! Remaining balance: PHP %.2f\n",
               *total - *paid);
        return 1;

    } else {

        printf("Invalid amount! Must be at least 50 and not exceed total fare.\n");
        return 0;
    }
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

    int oldSchedule = *selectedSchedule;

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

    while(temp != NULL) {

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
    char schedule[],
    float paid
) {
    time_t now;
    struct tm *local;
    char dateTime[100];

    time(&now);
    local = localtime(&now);
    
    strftime(dateTime, sizeof(dateTime), "%m-%d-%Y %I:%M %p", local);

    printf("\n========== RECEIPT ==========\n");
    printf("Date & Time: %s\n", dateTime);
    printf("Name: %s\n", name);
    printf("Seat Number: %d\n", seat);
    printf("Route: %s\n", route);
    printf("Schedule: %s\n", schedule);
    printf("Amount Paid: PHP %.2f\n", paid);

    printf("=============================\n");
}

// Main function
int main() {

    Queue bookingQueue;
    initQueue(&bookingQueue);

    // Separate seats for every schedule
    Seat seats[MAX_SCHEDULES][MAX_SEATS_PER_SCHEDULE];

    for(int i = 0; i < MAX_SCHEDULES; i++) {
        initSeats(seats[i]);
    }

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
    int i;
    char userName[50] = "";
    char searchName[50] = "";

    int selectedSeat = 0;
    int selectedSchedule = 0;

    char selectedRoute[50] = "";
    char selectedTime[30] = "";

    float fare = 100.0;
    float amountPaid = 0;

    do {
        printf("\n=========================================\n");
        printf("       WELCOME TO UV COMMUTE PRO         \n");
        printf("=========================================\n");
        printf("1. Input Full Name\n");
        printf("2. Choose Schedule & Time\n");
        printf("3. Reserve Seat\n");
        printf("4. Pay for your Ride\n");
        printf("5. Cancel Booking\n");
        printf("6. Generate Receipt\n");
        printf("7. Search Passenger Name\n");
        printf("8. View Booking History\n");
        printf("9. Exit\n");
        printf("=========================================\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar();

        switch(choice) {

            case 1:

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

            case 2:

                printf("\n--- Available Schedules ---\n");

                for(int i = 0; i < MAX_SCHEDULES; i++) {

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

            case 3:

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

            case 4:

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

            case 5:

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

            case 6:

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

            case 7:

                printf("\n--- Search Passenger ---\n");
                printf("Enter full passenger name: ");
                fgets(searchName, sizeof(searchName), stdin);
                searchName[strcspn(searchName, "\n")] = '\0';
                searchPassenger(&bookingQueue, searchName);

                break;

            case 8:

                displayQueue(&bookingQueue);

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
