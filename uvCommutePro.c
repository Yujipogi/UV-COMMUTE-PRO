#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// Constants for fixed sizes (array usage)
#define MAX_SCHEDULES 8
#define MAX_SEATS_PER_SCHEDULE 15

// Structure for each schedule (array of structures)
typedef struct {
    char route[50];
    char time[30];
    int occupiedSeats; // tracks how many seat are taken
} Schedule;

// Structure for seats
typedef struct {
    int seatNum;
    int isAvailable; // 1 for available, 0 for taken
} Seat;

// Node structure for booking queue (linked list)
typedef struct Node {
    char name[50];
    int seatNo;
    char route[50];
    char time[30];
    float amountPaid;
    struct Node* next; // pointer to next booking
} Node;

// Queue structure to manage bookings (first come first serve)
typedef struct {
    Node* front; //first element in the queue
    Node* rear; //last element in the queue
} Queue;

// Stack node for undo functionality (linked list)
typedef struct StackNode {
    int scheduleIndex;
    int seatNo;
    struct StackNode* next;
} StackNode;

// Function prototypes
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

// Function to validate that the name contains only letters and spaces
int isValidName(char name[]) {
	int i;
    for(i=0; name[i] != '\0'; i++) {
        if(!isalpha(name[i]) && !isspace(name[i])) {
            return 0;
        }
    }
    return 1;
}

// Queue functions for managing bookings
void initQueue(Queue* q) {
    q->front = NULL;
    q->rear = NULL;
}

// Check if the booking queue is empty
int isEmpty(Queue* q) {
    return (q->front == NULL);
}

// Enqueue a new booking into the queue
void enqueue(Queue* q, char name[], int seat, char route[], char time[], float pay) {
    Node* newNode = (Node*)malloc(sizeof(Node)); 
    strcpy(newNode->name, name);
    newNode->seatNo = seat;
    strcpy(newNode->route, route);
    strcpy(newNode->time, time);
    newNode->amountPaid = pay;
    newNode->next = NULL;

    // If queue is empty, new node becomes both front and rear
    if(isEmpty(q)) {
        q->front = newNode;
        q->rear = newNode;
    } else {// Otherwise, add to the end of the queue
        q->rear->next = newNode;
        q->rear = newNode;
    }
}

// Display all bookings in the queue
void displayQueue(Queue* q) {
    if(isEmpty(q)) {
        printf("No bookings yet!\n");
        return;
    }// Traverse the queue and print each booking's details
    Node* temp = q->front;
    while(temp != NULL) {// Print booking details
        printf("Name: %-15s | Seat: %d | Time: %s | Paid: PHP %.2f\n",
               temp->name, temp->seatNo, temp->time, temp->amountPaid);
        temp = temp->next;
    }
}

// Functions for seat management, payment processing, cancellation, receipt generation, and undo functionality
void initSeats(Seat seats[]) {
    for(int i=0; i<MAX_SEATS_PER_SCHEDULE; i++){
        seats[i].seatNum = i+1;
        seats[i].isAvailable = 1;
    }
}

// Display available seats for the selected schedule
void displaySeats(Seat seats[]) {
    printf("Available Seats:\n"); // Print seat numbers that are still available
    for(int i=0; i<MAX_SEATS_PER_SCHEDULE; i++) {
        if(seats[i].isAvailable)
            printf("%d ", seats[i].seatNum);
    }
    printf("\n");
}

// Process payment with validation for minimum down payment and maximum total fare
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

// Push a cancellation action onto the undo stack
void pushUndo(StackNode** top, int scheduleIndex, int seatNo) {
    StackNode* newNode = (StackNode*)malloc(sizeof(StackNode));
    newNode->scheduleIndex = scheduleIndex;
    newNode->seatNo = seatNo;
    newNode->next = *top;
    *top = newNode;
}

// Pop the last cancellation action from the undo stack and restore the seat and schedule
void popUndo(StackNode** top, Schedule schedules[], Seat seats[]) {
    if(*top == NULL) {
        printf("No actions to undo!\n");
        return;
    }// Restore the seat and update the schedule's occupied seats count
    StackNode* temp = *top;
    schedules[temp->scheduleIndex].occupiedSeats++;
    seats[temp->seatNo-1].isAvailable = 0;
    printf("Undo successful! Seat %d restored in schedule %d.\n", temp->seatNo, temp->scheduleIndex+1);
    *top = temp->next;
    free(temp);
}

// Handle booking cancellation with options for rescheduling or refund, and push cancellation actions onto the undo stack for potential reversal
void cancelBooking(Schedule schedules[], int *selectedSchedule, char *selectedRoute, char *selectedTime, StackNode** undoStack) {
    char reason[100];
    int option;
    int oldSchedule = *selectedSchedule;
    printf("Enter reason for cancellation/rescheduling: ");
    fgets(reason, sizeof(reason), stdin);
    reason[strcspn(reason, "\n")] = '\0';
// Provide options for rescheduling or refund
    printf("\nChoose action:\n");
    printf("1. Reschedule to another time\n");
    printf("2. Full Refund\n");
    printf("Enter choice: ");
    scanf("%d", &option);
    getchar();
// Handle rescheduling option
    if(option == 1) {
        printf("\n--- AVAILABLE SCHEDULES ---\n");
        for(int i=0; i<MAX_SCHEDULES; i++) {
            printf("%d. %-20s %s | Seats Left: %d/%d\n",
                   i+1, schedules[i].route, schedules[i].time,
                   MAX_SEATS_PER_SCHEDULE - schedules[i].occupiedSeats,
                   MAX_SEATS_PER_SCHEDULE);
        }// Prompt user to select a new schedule for rescheduling
        printf("\nEnter NEW schedule number you want: ");
        int newSched;
        scanf("%d", &newSched);
        getchar();
// Validate new schedule selection and update booking if valid, otherwise show error message
        if(newSched >=1 && newSched <= MAX_SCHEDULES) {
            if(schedules[newSched-1].occupiedSeats >= MAX_SEATS_PER_SCHEDULE) {
                printf("Sorry! New schedule is FULL. Cannot reschedule.\n");
            } else {// Update the occupied seats count for both old and new schedules, and update selected schedule details
                schedules[oldSchedule-1].occupiedSeats--;
                schedules[newSched-1].occupiedSeats++;
                *selectedSchedule = newSched;
                strcpy(selectedRoute, schedules[newSched-1].route);
                strcpy(selectedTime, schedules[newSched-1].time);
                printf("\nRESCHEDULE SUCCESSFUL!\n");
                printf("New Schedule: %s | %s\n", selectedRoute, selectedTime);
            }// Push the cancellation action onto the undo stack for potential reversal
        } else {
            printf("Invalid schedule number!\n");
        }// Handle refund option by updating schedule and clearing selected booking details, and push the cancellation action onto the undo stack for potential reversal
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

// Generate a receipt with booking details and payment information
void generateReceipt(char name[], int seat, char route[], char time[], float paid) {
    printf("\n========== RECEIPT ==========\n");
    printf("Name: %s\n", name);
    printf("Seat Number: %d\n", seat);
    printf("Route: %s\n", route);
    printf("Schedule: %s\n", time);
    printf("Amount Paid: PHP %.2f\n", paid);
    printf("=============================\n");
}

// Main function to run the UV Commute Pro application, providing a menu-driven interface for users to input their name, choose schedules, reserve seats, make payments, cancel bookings, generate receipts, view booking history, and undo cancellations.
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
// Variables to store user input and booking details
    int choice;
    char userName[50] = "";
    int selectedSeat = 0;
    int selectedSchedule = 0;
    char selectedRoute[50] = "";
    char selectedTime[30] = "";
       float fare = 100.0; 
    float amountPaid = 0;
    StackNode* undoStack = NULL;
// Main loop to display menu and handle user choices
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

        // Handle user choices with appropriate function calls and validations
        switch(choice) {
            case 1:// Input and validate user's full name, ensuring it contains only letters and spaces, and is not empty
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

            case 2:// Display available schedules with routes, times, and remaining seats, and allow user to select a schedule while validating the selection and checking for seat availability
                printf("\n--- Available Schedules ---\n");
                int i;
                for(i=0; i<MAX_SCHEDULES; i++) {
                    printf("%d. %-20s %s | Seats Left: %d/%d\n",
                           i+1, schedules[i].route, schedules[i].time,
                           MAX_SEATS_PER_SCHEDULE - schedules[i].occupiedSeats,
                           MAX_SEATS_PER_SCHEDULE);
                }// Prompt user to select a schedule and validate the selection, ensuring the schedule is not full before confirming the selection
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

            case 3:// Display available seats for the selected schedule and allow user to reserve a seat, validating the seat number and checking if the seat is still available before confirming the reservation
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

            case 4:// Display total fare and process payment, ensuring that the user has completed the previous steps of entering their name, selecting a schedule, and reserving a seat before allowing payment, and then enqueue the booking into the booking queue after successful payment
                printf("\n--- Payment ---\n");
                if(strlen(userName) == 0 || selectedSchedule == 0 || selectedSeat == 0) {
                    printf("Complete name, schedule, and seat first!\n");
                    break;
                }
                printf("Total Fare: PHP %.2f\n", fare);
                processPayment(&fare, &amountPaid);
                enqueue(&bookingQueue, userName, selectedSeat, selectedRoute, selectedTime, amountPaid);
                break;

            case 5:// Cancel a booking, providing options for rescheduling or refund, and pushing the cancellation action onto the undo stack
                cancelBooking(schedules, &selectedSchedule, selectedRoute, selectedTime, &undoStack);
                break;

            case 6:// Generate a receipt for the booking, ensuring that all necessary details (name, seat, route, time, and payment) are available before generating the receipt
                if(strlen(userName) > 0 && selectedSchedule !=0 && selectedSeat !=0 && amountPaid > 0) {
                    generateReceipt(userName, selectedSeat, selectedRoute, selectedTime, amountPaid);
                } else {
                    printf("Complete all steps first!\n");
                }
                break;

            case 7:// Display the booking history by showing all bookings in the queue, including details such as name, seat number, route, time, and amount paid
                displayQueue(&bookingQueue);
                break;

            case 8:// Undo the last cancellation action by popping from the undo stack and restoring the seat and schedule, allowing users to reverse a cancellation if they change their mind
                popUndo(&undoStack, schedules, seats);
                break;

            case 9:// Exit the application with a thank you message
                printf("Thank you for using UV Commute Pro!\n");
                break;

            default:// Handle invalid menu choices by displaying an error message and prompting the user to try again
                printf("Invalid choice! Try again.\n");
        }
    } while(choice != 9);// End of main loop

    return 0;// End of main function
}
