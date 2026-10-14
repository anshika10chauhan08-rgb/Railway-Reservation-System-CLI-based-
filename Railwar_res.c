#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <conio.h>
#include <ctype.h>
#define hash_size 100

void to_lowercase(char *str) {
    for (int i = 0; str[i]; i++)
        str[i] = tolower(str[i]);
}

typedef struct train {
    int Train_Num;
    char Train_Name[100];
    int Seats;
    float Sleeper_Cost;
    float AC_Cost;
    char Departure_Time[10];
    char Arrival_Time[10];
    float General_Cost;
    struct train* next;
} train;

typedef struct ticket {
    int Ticket_Num;
    char Name[100];
    int Age;
    char gender[10];
    char Address[100];
    char Email[100];
    char Train_Class[20];
    int Train_Num;
    char PNR[20];
    int Seat_Num;
    float cost;
    char Departure_Time[10];
    char Arrival_Time[10];
    struct ticket* next;
} ticket;



ticket* front = NULL;
ticket* rear = NULL;
ticket* waiting_front = NULL;
ticket* waiting_rear = NULL;
train* Train_Head = NULL;
ticket* hash_table[hash_size] = { NULL };
int Ticket_Count = 1;

int hash(int ticket_num) {
    return ticket_num % hash_size;
}

void register_user();
int login_user();
void Available_Train(int, char*, char*, char*, int, char*, char*);
train* find_train(int);
void View_Train();
void Book_Ticket();
void View_Ticket();
void View_Waiting_Tickets();
void generate_pnr(char* pnr, int ticket_num, int train_num);
void Cancel_ticket();
void greetings();
void reg();
void reservation_menu();
void trains();
void delete_ticket_from_file(int cancel_ticket_no);


void register_user() {
    char username[20];
    char FirstName[20];
    char LastName[20];
    char email[40];
    char password[20];
    int phone[10];
    char agreement[10];
    int atFound = 0, dotFound = 0;
    FILE* file = fopen("register_data.txt", "a");

    if (file == NULL) {
        printf("\n\033[31mError: Unable to open registration file!\033[0m\n");
        return;
    }

    system("cls");
    printf("\033[1;36m==========================================================\033[0m\n");
    printf("                   NEW USER REGISTRATION                  \n");
    printf("\033[1;36m==========================================================\033[0m\n");

    printf("\n\tFirstName : ");
    scanf("%s", FirstName);

    printf("\n\tLastName : ");
    scanf("%s", LastName);

    // Email with validation loop
    do {
        atFound = dotFound = 0;
        printf("\n\tEmail ID : ");
        scanf("%s", email);

        for (int i = 0; email[i] != '\0'; i++) {
            if (email[i] == '@') {
                atFound = 1;
            } else if (atFound && email[i] == '.') {
                dotFound = 1;
            }
        }

        if (!(atFound && dotFound)) {
            printf("\033[31m\tInvalid email address. Please enter again.\033[0m\n");
        }
    } while (!(atFound && dotFound));

    printf("\n\tPhone No : ");
    scanf("%d", phone);

    printf("\n\tSet Username : ");
    scanf("%s", username);

    printf("\n\tSet Password : ");
    scanf("%s", password);

    // Ask for agreement
    printf("\n\tDo you agree to all terms & conditions? (yes/no): ");
    scanf("%s", agreement);
    to_lowercase(agreement);

    if (strcmp(agreement, "yes") != 0) {
        printf("\n\033[33mRegistration cancelled. You must agree to the terms.\033[0m\n");
        fclose(file);
        return;
    }

    printf("\033[1;36m\n============================================================\033[0m\n");

     fprintf(file, "FirstName: %s\nLastName: %s\nEmail: %s\nPhone: %d\nUsername: %s\nPassword: %s\n",
        FirstName, LastName, email, phone, username, password);

    // Write to Excel-compatible CSV file
    FILE* csv = fopen("registration_data.csv", "a");
    if (csv == NULL) {
        printf("\n\033[31mError: Unable to open CSV file!\033[0m\n");
    } else {
        fprintf(csv, "%s,%s,%s,%s,%s\n", FirstName, LastName, email, username, password);
        fclose(csv);
        printf("\033[32mRegistration also saved to Excel-compatible CSV file.\033[0m\n");
    }

    fclose(file);
    printf("\n\033[32mRegistration Successful!\033[0m\n");
}


int login_user() {
    char username[20], password[20];
    char fileUsername[20], filePassword[20];
    char line[100];
    FILE* file = fopen("register_data.txt", "r");

    if (file == NULL) {
        printf("\n\033[31mError: Unable to open registration file!\033[0m\n");
        return 0;
    }

    system("cls");
    printf("\033[1;36m==========================================\033[0m\n");
    printf("               USER LOGIN                 \n");
    printf("\033[1;36m==========================================\033[0m\n");
    printf("\n\tUserName : ");
    scanf("%s", username);
    printf("\n\tPassword : ");
    scanf("%s", password);
    printf("\033[1;36m==========================================\033[0m\n");

    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "Username:", 9) == 0) {
            sscanf(line, "Username:%s", fileUsername);
        } else if (strncmp(line, "Password:", 9) == 0) {
            sscanf(line, "Password:%s", filePassword);

            // Now check credentials
            if (strcmp(username, fileUsername) == 0 && strcmp(password, filePassword) == 0) {
                fclose(file);
                printf("\n\033[32mLogin Successful!\033[0m\n");
                return 1;
            }
        }
    }

    fclose(file);
    printf("\n\033[31mLogin Failed! Invalid credentials.\033[0m\n");
    return 0;
}


void Available_Train(int num, char* name, char* from, char* to, int seats, char* dep_time, char* arr_time) {
    train* t1 = (train*)malloc(sizeof(train));
    t1->Train_Num = num;
    snprintf(t1->Train_Name, sizeof(t1->Train_Name), "%s (%s to %s)", name, from, to);
    t1->Seats = seats;
    // Assign class-wise ticket cost
    t1->Sleeper_Cost = 500.0;
    t1->AC_Cost = 1000.0;
    t1->General_Cost = 250.0;
    // Store train timings
    snprintf(t1->Departure_Time, sizeof(t1->Departure_Time), "%s", dep_time);
    snprintf(t1->Arrival_Time, sizeof(t1->Arrival_Time), "%s", arr_time);
    t1->next = Train_Head;
    Train_Head = t1;
}


train* find_train(int Train_Num) {
    train* temp = Train_Head;
    while (temp) {
        if (temp->Train_Num == Train_Num)
            return temp;
        temp = temp->next;
    }
    return NULL;
}


void View_Train() {
    train* temp = Train_Head;

    system("cls");
    printf("\n\033[1;34m====================== Available Trains ======================\033[0m\n");
    while (temp) {
        printf("\n\tTrain Number: \033[1;33m%d\033[0m", temp->Train_Num);
        printf("\n\tTrain Name: \033[1;33m%s\033[0m", temp->Train_Name);
        printf("\n\tSeats Available: \033[1;33m%d\033[0m", temp->Seats);
        printf("\n\tSleeper Cost: \033[1;32mRs.%.2f\033[0m", temp->Sleeper_Cost);
        printf("\n\tAC Cost: \033[1;32mRs.%.2f\033[0m", temp->AC_Cost);
        printf("\n\tGeneral Cost: \033[1;32mRs.%.2f\033[0m", temp->General_Cost);
        printf("\n\tDeparture Time: \033[1;32m%s\033[0m", temp->Departure_Time); // Display departure time
        printf("\n\tArrival Time: \033[1;32m%s\033[0m\n", temp->Arrival_Time); // Display arrival time
        temp = temp->next;
    }
    printf("\n\033[1;34m============================================================\033[0m\n");
}


void Book_Ticket() {
    int train_num, payment_option;
    printf("\nEnter Train Number: ");
    scanf("%d", &train_num);

    train* t1 = find_train(train_num);
    if (t1 == NULL) {
        printf("\n\033[31mError: Invalid Train Number!\033[0m\n");
        return;
    }

    ticket* ticket1 = (ticket*)malloc(sizeof(ticket));
    ticket1->Ticket_Num = Ticket_Count++;
    generate_pnr(ticket1->PNR, ticket1->Ticket_Num, train_num);
    ticket1->Train_Num = train_num;
    ticket1->next = NULL;
    getchar();

    printf("Enter Name of Passenger: ");
    scanf("%[^\n]", ticket1->Name);

    printf("Enter Email ID: ");
    scanf("%s", ticket1->Email);  

    printf("Enter Age of Passenger: ");
    scanf("%d", &ticket1->Age);

    printf("Enter Gender (M/F): ");
    scanf("%s", ticket1->gender);
    getchar();

    printf("Enter Address: ");
    scanf("%[^\n]", ticket1->Address);

    printf("Enter Train Class (AC/General/Sleeper): ");
    scanf("%s", ticket1->Train_Class);

    // Assign seat number based on remaining seats:
    ticket1->Seat_Num = (t1->Seats); // assigning seat number as current seat count
    // decrease seats available since seat gets assigned on booking

    snprintf(ticket1->Departure_Time, sizeof(ticket1->Departure_Time), "%s", t1->Departure_Time);
    snprintf(ticket1->Arrival_Time, sizeof(ticket1->Arrival_Time), "%s", t1->Arrival_Time);

    // Payment method section
    printf("\nChoose Payment Option:\n");
    printf("1. UPI\n");
    printf("2. Debit/Credit Card\n");
    printf("3. Net Banking\n");
    printf("4. Cash at Counter\n");
    printf("Enter your choice: ");
    scanf("%d", &payment_option);

    // Calculate cost
    if (strcmp(ticket1->Train_Class, "AC") == 0)
        ticket1->cost = t1->AC_Cost;
    else if (strcmp(ticket1->Train_Class, "Sleeper") == 0)
        ticket1->cost = t1->Sleeper_Cost;
    else
        ticket1->cost = t1->General_Cost;

    switch (payment_option) {
        case 1:
            printf("\nTicket cost is: \033[1;32mRs. %.2f\033[0m\n", ticket1->cost);
            printf("\nPlease pay the amount to UPI ID: \033[1;34m8126609074@fam\033[0m\n");
            printf("\nEnter your UPI ID (e.g., name@bank): ");
            {
                char upi_id[50];
                scanf("%s", upi_id);
                printf("\nVerifying payment from %s...\n", upi_id);
                printf("\033[32mPayment Successful via UPI!\033[0m\n");
            }
            break;
        case 2:
            printf("\nTicket cost is: \033[1;32mRs. %.2f\033[0m\n", ticket1->cost);
            printf("\nEnter Card Number (XXXX-XXXX-XXXX-XXXX): ");
            {
                char card_number[25];
                scanf("%s", card_number);
                printf("Processing Card Payment...\n");
                printf("\033[32mPayment Successful via Card!\033[0m\n");
            }
            break;
        case 3:
            printf("\nTicket cost is: \033[1;32mRs. %.2f\033[0m\n", ticket1->cost);
            printf("\nEnter Net Banking User ID: ");
            {
                char netbank_id[30];
                scanf("%s", netbank_id);
                printf("\nRedirecting to bank portal for %s...\n", netbank_id);
                printf("\033[32mPayment Successful via Net Banking!\033[0m\n");
            }
            break;
        case 4:
            printf("\nPay at counter selected. Payment of \033[1;32mRs. %.2f\033[0m will be collected at boarding.\n", ticket1->cost);
            break;
        default:
            printf("\n\033[31mError: Invalid payment method! Booking failed.\033[0m\n");
            free(ticket1);
            return;
    }


    if (t1->Seats > 0) {
        if (rear == NULL) {
            front = rear = ticket1;
        } else {
            rear->next = ticket1;
            rear = ticket1;
        }

        printf("\n\033[32mTicket is Booked Successfully!\033[0m\n");
        printf("Ticket Number: \033[1;33m%d\033[0m\n", ticket1->Ticket_Num);
        printf("PNR: \033[1;33m%s\033[0m\n", ticket1->PNR);
        printf("Seat Number: \033[1;33m%d\033[0m\n", ticket1->Seat_Num);
        printf("Departure Time: \033[1;32m%s\033[0m\n", ticket1->Departure_Time); // Display departure time
        printf("Arrival Time: \033[1;32m%s\033[0m\n", ticket1->Arrival_Time); // Display arrival time
        t1->Seats--;

        int index = hash(ticket1->Ticket_Num);
        ticket1->next = hash_table[index];
        hash_table[index] = ticket1;

    } else {
        if (waiting_rear == NULL) {
            waiting_front = waiting_rear = ticket1;
        } else {
            waiting_rear->next = ticket1;
            waiting_rear = ticket1;
        }
        printf("\n\033[31mNo Seats Available. Added to Waiting List. Waiting Ticket Number: \033[1;33m%d\033[0m\n", ticket1->Ticket_Num);
    }
}


void View_Ticket() {
    ticket* temp = front;

    if (temp == NULL) {
        printf("\n\033[31mNo Tickets Booked Yet!\033[0m\n");
        return;
    }

    printf("\n\033[1;34m============ Booked Tickets ============\033[0m\n");
    ticket* current = temp;
    while (current) {
        printf("Ticket Number: \033[1;33m%d\033[0m\n", current->Ticket_Num);
        printf("Email: \033[1;33m%s\033[0m\n", current->Email);
        printf("Passenger Name: \033[1;33m%s\033[0m\n", current->Name);
        printf("Age: \033[1;33m%d\033[0m\n", current->Age);
        printf("Gender: \033[1;33m%s\033[0m\n", current->gender);
        printf("Address: \033[1;33m%s\033[0m\n", current->Address);
        printf("Class: \033[1;33m%s\033[0m\n", current->Train_Class);
        printf("Train Number: \033[1;33m%d\033[0m\n", current->Train_Num);
        printf("PNR: \033[1;33m%s\033[0m\n", current->PNR);
        printf("Seat Number: \033[1;33m%d\033[0m\n", current->Seat_Num);
        printf("Departure Time: \033[1;32m%s\033[0m\n", current->Departure_Time); 
        printf("Arrival Time: \033[1;32m%s\033[0m\n", current->Arrival_Time); 
        printf("Ticket Cost: \033[1;32mRs. %.2f\033[0m\n", current->cost);
        printf("==============================================\n");
        current = current->next;
    }

    // Write to file
    FILE* fp2 = fopen("BookedFile.txt", "w");
    if (fp2 == NULL) {
        printf("\n\033[31mError: File couldn't be created or opened!\033[0m\n");
        return;
    }

    current = temp;
    fprintf(fp2, "========== Booked Tickets ==========\n");
    while (current) {
        fprintf(fp2, "Ticket Number: %d\n", current->Ticket_Num);
        fprintf(fp2, "Passenger Name: %s\n", current->Name);
        fprintf(fp2, "Email: %s\n", current->Email);
        fprintf(fp2, "Age: %d\n", current->Age);
        fprintf(fp2, "Gender: %s\n", current->gender);
        fprintf(fp2, "Address: %s\n", current->Address);
        fprintf(fp2, "Class: %s\n", current->Train_Class);
        fprintf(fp2, "Train Number: %d\n", current->Train_Num);
        fprintf(fp2, "PNR: %s\n", current->PNR);
        fprintf(fp2, "Seat Number: %d\n", current->Seat_Num);
        fprintf(fp2, "Departure Time: %s\n", current->Departure_Time); 
        fprintf(fp2, "Arrival Time: %s\n", current->Arrival_Time); 
        fprintf(fp2, "Ticket Cost: Rs. %.2f\n", current->cost);
        fprintf(fp2, "=====================================\n");
        current = current->next;
    }




       // CSV export for Mail Merge
FILE* csv = fopen("BookedData.csv", "w");
if (csv == NULL) {
    printf("\n\033[31mError: Could not create BookedData.csv file for mail merge.\033[0m\n");
} else {
    // Write headers
    fprintf(csv, "Ticket_Number,Name,Age,Gender,Address,Email,Class,Train_Number,PNR,Seat_Number,Cost,Departure Time,Arrival Time\n");

    current = temp;
    while (current != NULL) {
        fprintf(csv, "%d,%s,%d,%s,%s,%s,%s,%d,%s,%d,%.2f,%s,%s\n",
                current->Ticket_Num,
                current->Name,
                current->Age,
                current->gender,
                current->Address,
                current->Email,
                current->Train_Class,
                current->Train_Num,
                current->PNR,
                current->Seat_Num,
                current->cost,
                current->Departure_Time,
                current->Arrival_Time);

        current = current->next;
    }

    fclose(csv);
    printf("\033[32mBooked ticket data also saved to BookedData.csv for Mail Merge.\033[0m\n");
}


    fclose(fp2);
    printf("\n\033[32mTicket details successfully saved to BookedFile.txt\033[0m\n");
}


void View_Waiting_Tickets() {
    ticket* temp = waiting_front;
    if (temp == NULL) {
        printf("\n\033[31mNo Waiting Tickets.\033[0m\n");
        return;
    }

    printf("\n\033[1;34m========== Waiting Tickets ==========\033[0m\n");
    while (temp) {
        printf("Waiting Ticket Number: \033[1;33m%d\033[0m\n", temp->Ticket_Num);
        printf("Passenger Name: \033[1;33m%s\033[0m\n", temp->Name);
        printf("Age: \033[1;33m%d\033[0m\n", temp->Age);
        printf("Gender: \033[1;33m%s\033[0m\n", temp->gender);
        printf("Address: \033[1;33m%s\033[0m\n", temp->Address);
        printf("Class: \033[1;33m%s\033[0m\n", temp->Train_Class);
        printf("Train Number: \033[1;33m%d\033[0m\n", temp->Train_Num);
        printf("PNR: \033[1;33m%s\033[0m\n", temp->PNR);
        printf("========================================\n");
        temp = temp->next;
    }
}


void Cancel_ticket()
{
    if (front == NULL) {
        printf("\n\033[31mNo tickets to cancel!\033[0m\n");
        return;
    }

    int cancel_ticket_no;
    printf("\nEnter the ticket number to cancel: ");
    scanf("%d", &cancel_ticket_no);

    ticket* temp = front;
    ticket* prev = NULL;
    while (temp != NULL && temp->Ticket_Num != cancel_ticket_no) {
        prev = temp;
        temp = temp->next;
    }

    if (temp == NULL) {
        printf("\n\033[31mError: Ticket number %d not found.\033[0m\n", cancel_ticket_no);
        return;
    }

    // Restore seat count
    train* t = find_train(temp->Train_Num);
    if (t != NULL) {
        t->Seats++;
    }

    // Remove ticket from linked list
    if (temp == front) {
        front = front->next;
        if (rear == temp) rear = NULL;
    } else {
        prev->next = temp->next;
        if (temp == rear) rear = prev;
    }

    int index = hash(cancel_ticket_no);
    ticket* curr = hash_table[index], * prev_hash = NULL;
    while (curr && curr->Ticket_Num != cancel_ticket_no) {
        prev_hash = curr;
        curr = curr->next;
    }
    if (curr) {
        if (prev_hash == NULL) {
            hash_table[index] = curr->next;
        } else {
            prev_hash->next = curr->next;
        }
    }

    free(temp);
    delete_ticket_from_file(cancel_ticket_no);
    printf("\n\033[32mTicket number %d cancelled successfully!\033[0m\n", cancel_ticket_no);

    if (waiting_front != NULL) {
        ticket* wait_ticket = waiting_front;
        waiting_front = waiting_front->next;
        if (waiting_front == NULL) waiting_rear = NULL;

        wait_ticket->next = NULL;
        if (rear == NULL) {
            front = rear = wait_ticket;
        } else {
            rear->next = wait_ticket;
            rear = wait_ticket;
        }

        if (t != NULL) t->Seats--;
        printf("\n\033[32mWaiting Ticket Number %d is now confirmed!\033[0m\n", wait_ticket->Ticket_Num);
    }
}

// Updated function to properly update booked tickets file upon cancellation
void delete_ticket_from_file(int cancel_ticket_no) {
    FILE *original = fopen("BookedFile.txt", "r");
    FILE *temp = fopen("temp.txt", "w");
    if (!original || !temp) {
        printf("\n\033[31mError opening file for cancellation update.\033[0m\n");
        if(original) fclose(original);
        if(temp) fclose(temp);
        return;
    }

    char line[256];
    int ticket_found = 0;

    while (fgets(line, sizeof(line), original)) {
        // Check for line containing ticket number
        if (strstr(line, "Ticket Number:") != NULL) {
            int ticket_num;
            sscanf(line, "Ticket Number: %d", &ticket_num);
            if (ticket_num == cancel_ticket_no) {
                ticket_found = 1;
                // Skip ticket details lines for this ticket
                for (int i = 0; i < 8; i++) {
                    fgets(line, sizeof(line), original);
                }
                continue; // Skip writing this ticket
            }
        }
        // Write the line to temp file if not canceled ticket lines
        fputs(line, temp);
    }

    fclose(original);
    fclose(temp);

    if (ticket_found) {
        remove("BookedFile.txt");
        rename("temp.txt", "BookedFile.txt");
        printf("\n\033[32mTicket number %d has been removed from the booked tickets file.\033[0m\n", cancel_ticket_no);
    } else {
        remove("temp.txt");
        printf("\n\033[31mNo ticket found with number %d in the booked tickets file.\033[0m\n", cancel_ticket_no);
    }
}


void generate_pnr(char* pnr, int ticket_num, int train_num)
{
    snprintf(pnr, 10, "T%d%03d", train_num, ticket_num);
}


void greetings() {
    printf("\n\n");
    printf("\033[1;36m");
    printf("\t\t\t\t\t     _________________________________________\n");
    printf("\t\t\t\t\t    |                                         |\n");
    printf("\t\t\t\t\t    |                                         |\n");
    printf("\t\t\t\t\t    |       ----------------------------      |\n");
    printf("\t\t\t\t\t    |          WELCOME TO SEAT SEEKER         |\n");
    printf("\t\t\t\t\t    |       ----------------------------      |\n");
    printf("\t\t\t\t\t    |                                         |\n");
    printf("\t\t\t\t\t    |                                         |\n");
    printf("\t\t\t\t\t    |_________________________________________|\n");
    printf("\n\n");
    printf("\t\t\t\t\t     _________________________________________ \n");
    printf("\t\t\t\t\t    |                                         |\n");
    printf("\t\t\t\t\t    |       --------------------------        |\n");
    printf("\t\t\t\t\t    |             BOOK YOUR TICKET            |\n");
    printf("\t\t\t\t\t    |       --------------------------        |\n");
    printf("\t\t\t\t\t    |                                         |\n");
    printf("\t\t\t\t\t    |          HAVE A GREAT JOURNEY           |\n");
    printf("\t\t\t\t\t    |                                         |\n");
    printf("\t\t\t\t\t    |_________________________________________|\n");
    printf("\033[0m");
    printf("\n\n\t\t\t\t             Press any key to continue: ");
    getch();
}

void reg() {
    int choice;
    int loginSuccess = 0;

    system("cls");
    do {
        printf("\033[1;36m\n\n\n\n\n\t\t\t-----------------------------------------------------------------\033[0m");
        printf("\033[1;37m\n\t\t\t                    GET REGISTER YOURSELF                 \033[0m");
        printf("\033[1;36m\n\t\t\t-----------------------------------------------------------------\033[0m\n");
        printf("\t\t\t\t\t       1. Register\n");
        printf("\t\t\t\t\t       2. Login\n");
        printf("\t\t\t\t\t       3. Exit\n");
        printf("\033[1;36m\n\t\t\t-----------------------------------------------------------------\033[0m\n");
        printf("\n\t\t\t\t\t Enter your choice: ");
        scanf("%d", &choice);

        if (choice == 1)
            register_user();
        else if (choice == 2)
            loginSuccess = login_user();
        else if (choice == 3)
            exit(0);
        else
            printf("\n\033[31mInvalid choice. Please try again.\033[0m\n");
    } while (!loginSuccess);
}

void reservation_menu() {
    int choice;
    system("cls");
    do {
        printf("\033[1;36m\n\n\t\t\t-----------------------------------------------------------------\033[0m");
        printf("\n\t\t\t                RAILWAY RESERVATION MENU               \033[0m");
        printf("\033[1;36m\n\t\t\t-----------------------------------------------------------------\033[0m\n");
        printf("\t\t\t\t\t1. View Trains\n");
        printf("\t\t\t\t\t2. Book Ticket\n");
        printf("\t\t\t\t\t3. View Tickets\n");
        printf("\t\t\t\t\t4. Cancel Tickets\n");
        printf("\t\t\t\t\t5. View Waiting Tickets\n");
        printf("\t\t\t\t\t6. Exit\n");
        printf("\033[1;36m\n\t\t\t-----------------------------------------------------------------\033[0m\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: View_Train(); break;
            case 2: Book_Ticket(); break;
            case 3: View_Ticket(); break;
            case 4: Cancel_ticket(); break;
            case 5: View_Waiting_Tickets(); break;
            case 6: printf("\n\033[32mHAVE A GREAT JOURNEY!\033[0m\n"); break;
            default: printf("\n\033[31mInvalid choice! Please try again.\033[0m\n");
        }
    } while (choice != 6);
}

void trains() 
{

    Available_Train(10001, "Rajdhani Express", "Delhi", "Hyderabad", 5, "10:00 AM", "18:00 PM");
    Available_Train(10002, "Shatabdi Express", "Delhi", "Patna", 6, "15:00 PM", "22:00 PM");
    Available_Train(10003, "Duronto Express", "Kolkata", "Pune", 2, "08:00 AM", "16:00 PM");
    Available_Train(10004, "Garib Rath", "Mumbai", "Rajasthan", 2, "12:00 AM", "20:00 AM");
    Available_Train(10005, "Superfast Express", "Bangalore", "Jaipur", 5, "09:00 PM", "17:00 AM");
}

int main()
{
    trains();
    greetings();
    reg();
    reservation_menu();

    return 0;
}
