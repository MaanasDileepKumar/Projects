#include <stdio.h>
#include <string.h>

// To use system() when running in windows
#include <stdlib.h>
#include <time.h>

// Used for errno
#include <sys/errno.h>


/* Constants */

    // For the console

#define BUY_CAR 'n'
#define CARS_AVAILABLE 'b'
#define VIEW_FEEDBACKS 'v'
#define SALES_DATA 'c'
#define EXIT 'x'

    // The rest
// 0.05 = 5%
#define DISCOUNT_RATE 0.05f
#define TOTAL_CARS 69
#define TOTAL_MODELS 10
#define CSV_FILE "carSales.csv"
#define T 1
#define F 0
#define OPEN 0
#define CLOSE 1
#define ERROR 2





/* Initial sales data and inv */
// Using unsigned short as none of these can have a -ve value

    // Total cars available
unsigned short totalCarsInStock = 69;
    // To track how many cars were sold
unsigned short totalSalesCount = 0;
    // To  track no of feedbacks
int totalFeedbacks = 0;
    // Records no of cars per sale
unsigned short carsSoldPerSale[TOTAL_CARS];
    // Records what was sold
unsigned short carModelSoldPerSale[TOTAL_CARS];
    // If customer is an employee, records the discount data
    // including if applied and what the price is after the discount
unsigned short discountApplied[TOTAL_CARS];
    // To Store ratings
unsigned int ratings[TOTAL_CARS];
    // To store date of sale
    // 100 chars + 1 null terminator
char dateOfSaleRecord[TOTAL_CARS][101];
    // To store Feedback
char feedbacks[TOTAL_CARS][301];
    // Record the customer names
char custNames[TOTAL_CARS][201];
    // To track how many cars were sold per model
unsigned short carsSoldPerModel[TOTAL_CARS] = {0};
    // Total amount you got per sale from per car
float carsRevenuePerModel[TOTAL_CARS] = {0};


    // Car models, thier prices, year of manufacture and their availability
    // Car Models
char models[TOTAL_MODELS][20] = {
    "Toyota LandCruiser", "Toyota Aygos", "Toyota Corolla", "Toyota Camry", "Toyota Supra",
    "Lexus LX570", "Lexus Lc500", "Lexus RX350", "Lexus ES350", "Lexus LFA"
    };

    // Car Prices
float price[TOTAL_MODELS] = {47500.0f, 12000.0f, 23000.0f, 29000.0f, 52000.0f,
                            73500.0f, 45000.0f, 62200.0f, 45000.0f, 400000.0f};


    // The Respective years each car was manufactured
unsigned short manufacturedYear[TOTAL_MODELS] = {2024, 2013, 2019, 2021, 2000,
                                            2024, 2020, 2017, 2016, 2002};

    // Cars available in stock respectively
unsigned short carModelsInStock[TOTAL_MODELS] = {5,7,9,6,6,11,5,8,10,2};


    // Applying a discount
float priceAfterDiscount(float price) {
    // 1 - 0.05 = 0.95 remaining, 0.95 = 95%
    // Hence apply discount will return 95% the value of price
    return price * (1 - DISCOUNT_RATE);
}






/* Input words and Tnput numbers */
// char* used to pass a pointer to the string so that function can modify its contents

    // For inputting words
char userInputWords(char* words) {
    char input;
    printf("%s", words);
    scanf(" %c", &input);

    // To clear any buffer from invalid input
    while (getchar() != '\n');
    return input;
}

    // For inputting numbers
unsigned short userInputnum(char* num) {
    unsigned short input;
    printf("%s", num);
    scanf(" %hd", &input);
    while (getchar() != '\n');
    return input;
}






/* Sorting cars based on decending order of manufactured date */
void sortCarsInDescManufactureDate() {

//Using A Bubble sort algorithm
// Creating temp variables
    char tempModels[20];
    float tempPrice;
    unsigned short tempYear, tempStock;

// Looping from index 0 to index 2nd last ie 8 for MAX_CAR_MODELS - 1.
    // -1 makes it so that model in index 9 cant be compared with itself
    for (int i = 0; i < TOTAL_MODELS - 1; i++) {

        // Inner loop j starts from i + 1 and iterates over the values of i
        // This makes it so that each model is only compared once with every other model
        for (int j = i + 1; j < TOTAL_MODELS; j++) {

            // To chech if manufacture date of i models are <(older) than  j models
            if (manufacturedYear[i] < manufacturedYear[j]) {

                // Swapping Prices
                // Copying index i prices into the temp to store the original values of i
                tempPrice = price[i];
                // Replace index i prices with index j
                price[i] = price[j];
                // Now assign indix j prices with the original prices of i stored in the temp
                price[j] = tempPrice;

                // Swapping Manufactured Year
                tempYear = manufacturedYear[i];
                manufacturedYear[i] = manufacturedYear[j];
                manufacturedYear[j] = tempYear;

                // Swapping Available Stock
                tempStock = carModelsInStock[i];
                carModelsInStock[i] = carModelsInStock[j];
                carModelsInStock[j] = tempStock;

                // Swapping Models
                // Using strcpy instead here cause it's a string, and it can't be assigned the same way as integers
                strcpy(tempModels, models[i]);
                strcpy(models[i], models[j]);
                strcpy(models[j], tempModels);
            }
        }
    }
}





/* Saving Files */
FILE* file;
unsigned char fileStatus = CLOSE;

    // Create a file
FILE *createFile(const char* salesFile) {
    // open the file
    // w for writing
    file = fopen(salesFile, "w");

    // Then close it
    if (file!=NULL) {
        fclose (file);
    }
    return file;
}

    // mode: w for writing, r for reading and a for appending
    // r+ to read and write and file must exist
    // w+ similar to r+ but if the file doesn't exist it creates it
    // a+ to read and append
void open(char* salesFile, char* mode) {

    // Open File
    file = fopen(salesFile, mode);

    // if file is null
    if (file == NULL) {

        // create it and then check if It's still null
        if(createFile(salesFile) == NULL) {
            fileStatus = ERROR;

            // Print informing the error
            // errno to store error code
            printf("Error opening file %s: %s\n", salesFile, strerror(errno));
        }
        else {
            open(salesFile, mode);
        }
    }
    // if file is not null
    else {
        fileStatus = OPEN;
    }

}

    // Called to close when everything needed is concluded
void close() {

    // Making sure it only closes if its open
    if (fileStatus == OPEN) {
        fclose(file);
        fileStatus = CLOSE;
    }
}



    // Write it int the csv file
void write() {
    // loops through the sales and prints every sale saved
    for (int i = 0; i < totalSalesCount; i++) {
        fprintf(file, "%hd,%hd,%s,%s\n", carModelSoldPerSale[i], discountApplied[i], custNames[i], dateOfSaleRecord[i]);
    }
}



    // Save the file
void save() {
    open(CSV_FILE, "w");

    if (fileStatus == OPEN) {
        write();
    }
    else if (fileStatus == ERROR) {
        printf("Error writing file %s\n", CSV_FILE);
    }

    close();
}






/* Payment methods */

    // Collect card details

void collectCardDetails(char paymentMethod) {

    unsigned int cardNumber, cvc, year, month;

    // Collect card details based on payment method
    // Adding loops so that it loops till thw information is valid

    if (paymentMethod == 'd' || paymentMethod == 'D' ||
        paymentMethod == 'c' || paymentMethod == 'C') {

        // Input card number
        while (T) {
            printf("Card number: ");

            // To check if scanf can read Card number as an integer
            // Thus confirming Card number is a number
            if (scanf("%d", &cardNumber) == T) {

                break;
            }

            else{
                while (getchar() != '\n');
                printf("Invalid card number. Enter a valid card number \n\n");
            }
        }

        // Input Security Code
        while (T) {
            printf("CVC: ");

            if (scanf("%d", &cvc) == T) {
                break;
            }

            else {

                // Used to clear any buffers
                while(getchar() != '\n');
                printf("Invalid cvc number\n");
            }
        }

        while (T) {
            printf("Expiry Month: ");

            if (scanf("%d", &month) == T && month <= 12) {
                break;
            }

            else {
                while(getchar() != '\n');
                printf("Invalid Month\n");
            }
        }

        while (T) {
            printf("Expiry Year: ");

            if (scanf("%d", &year) == T) {
                break;
            }

            else {
                while(getchar() != '\n')
                printf("Invalid Year\n");
            }
        }
        printf("\n");
        printf("Card Number: %d\n", cardNumber);
        printf("CVC: %d\n", cvc);
        printf("Expiry Month: %d\n", month);
        printf("Expiry Year: %d\n", year);

    }

}





/* To deliver or not to deliver */

    // To add a delivery address
void deliveryAddress() {

    printf("Delivery with in 14 Days\n\n");

    // Adding multiple line of addresses and a postcode
    userInputWords("ADDRESS LINE 1: ");
    userInputWords("ADDRESS LINE 2: ");
    userInputWords("POSTCODE: ");

}

    // If u choose to pick up it gives you the address of the dealership
void pickupAddress() {
    printf("Pickup\n\n");
    printf("You can pick the car up at:\n");
    printf("321 Kings Street\n");
    printf("Eastminster\n");
    printf("London\n");
    printf("SW1P 2DA\n");
    printf("United Kingdom\n\n");

}






/* Leave a Feedback */

void feedback() {
    // Leave a rating
    unsigned int rating = 0;
    char feedback[300];

    // Loop to make sure the rating is within the max
    while (T) {
        printf("How would you rate this [out of 5]: ");
        if (scanf("%d", &rating) == T && rating <= 5) {
            break;
        }
        else {
            printf("Invalid Rating. Please enter a number out of 5\n\n");
            while(getchar() != '\n');
        }
    }


    // Leave a feedback
    printf("Enter your feedback: \n");
    getchar();
    scanf("%[^\n]", feedback);


    // To store ratings and feedbacks
    ratings[totalFeedbacks] = rating;
    strcpy(feedbacks[totalFeedbacks], feedback);


    totalFeedbacks++;
    printf("Feedback recorded! \n\n");
    while(getchar() != '\n');

}






/* Functions for the main console options*/

    // Option V, to display cars available and their information
    // Added Option V out of order so that it can be displayed in the buying process (Option B)
void displayCarModels() {
    sortCarsInDescManufactureDate();
    printf ("Cars in Stock:\n");

    for (int i = 0 ; i < TOTAL_MODELS ; i++) {
        if (carModelsInStock[i] > 0) {

            // %.2f to show the value with 2 decimal places
            printf("%d - %s - £%.2f - %d - %d available\n", i, models[i], price[i], manufacturedYear[i], carModelsInStock[i]);
        }
        else {

            printf("%d - %s - £%.2f - %d - OUT OF STOCK\n", i, models[i], price[i], manufacturedYear[i]);

        }
    }
}



    //Option B, to buy car
void buyingProcess() {
    // Displaying the models available and its information
    displayCarModels();

    // Specifying selected model from stock
    // -1 to show no selection has been made
    unsigned int selectedModel = -1;

    // Customer input ie selecting a model
    // Adding a loop to prevent invalid selection or if the car is out of stock
    while (selectedModel < 0 || selectedModel >= TOTAL_MODELS || carModelsInStock[selectedModel] == 0) {

        //Input car model based on its index
        selectedModel = userInputnum("\n Select Car Model (0-9): ");

        if (selectedModel < 0 || selectedModel >= TOTAL_MODELS) {
            printf("Invalid Selection. Please enter an option given above: \n");
        }
        else if ( carModelsInStock[selectedModel] == 0) {
            printf("Sorry, we don't have any of this model available \n");
        }
        else{
            break;
        }
    }

    // Collecting customer information, card details and present thier receipt

    // Needed to show customer buying one car
    // ALso check if you are eligible for a discunt
    unsigned short carQuantitySold = 1;

    // Select the price the customer pays as per the model selected
    float receiptPrice = price[selectedModel];


    // Collect Customer information
    printf("Customer Information: \n\n");
    printf("Name: ");


    scanf("%[^\n]s", custNames[totalSalesCount]);
    getchar();

    // Ask and verify valid input
    unsigned int age;

    while (T) {
        printf("age: ");

        // This checks if scanf can read age as an integer
        if (scanf("%d", &age) == T) {
            break;
        }

        else {
            // Clear invalid input from the buffer
            while(getchar() != '\n');
            printf("Invalid input. Please enter your age in numbers:\n");
        }

    }

    if (age < 18) {
        printf("Sorry, you must be at least 18 years old to purchase a car.\n");
        return;
    }



    // Check for discount eligibility
    unsigned short eligibleForDiscount = 0;

    // Creating a loop to handle invalid outputs
    while (T) {

        eligibleForDiscount = userInputWords("\nAre you an employee at Toyota? Answer 'y' or 'n' : ");;

        if (eligibleForDiscount != 'y' && eligibleForDiscount != 'Y' && eligibleForDiscount != 'n' && eligibleForDiscount != 'N' ) {
            printf("invalid Option, Enter 'y' for yes or 'n' for no \n");
        }
        else {
            break;
        }
    }

    if (eligibleForDiscount == 'y' || eligibleForDiscount == 'Y') {
        printf("Employee Discount Applied\n");
        receiptPrice = priceAfterDiscount(receiptPrice);
    }



    // Creating a loop so that if any option other tha D or C is given
    // It will show as invalid and loop back

    char paymentMethod = '\0';
    while (T)  {
        paymentMethod = userInputWords("\nHow would you like to pay? 'd' for Debit or 'c' for Credit: ");

        if (paymentMethod != 'd' && paymentMethod != 'D' && paymentMethod != 'c' && paymentMethod != 'C') {
            printf("Invalid option. Enter 'd' for Debit or 'c' for Credit:\n");
        }
        else {
            collectCardDetails(paymentMethod);
            break;
        }
    }


    // Creating a loop to prevent invalid inputs for delivery
    char delivery;
    while (T) {
        delivery = userInputWords("\nWould you like free delivery (Within 14 days) [D] or Pick-up [P] :  ");

        if (delivery == 'd' || delivery == 'D') {
            deliveryAddress();
            break;
        }
        else if ( delivery == 'p' || delivery == 'P') {
            pickupAddress();
            break;
        }
        else{
            printf("Invalid option. Enter 'd' for delivery or 'p' for pickup : ");
        }


    }

    // Collect and store date

    // Getting Date as per the computer
    // time_t datatype used to represent the no of seconds elapsed since Jan 1 1970
    time_t dateAndTime = time(NULL);

    // Converting it into local time
    // sturct tm contains local calendar and time info
    struct tm *currDate = localtime(&dateAndTime);

    // Print Date
    // Creating a char to store date
    char dateOfSale[100];

    // stftime formats and stores the date as a sting based on a custom format
    // %d to display day of month
    // %B used to display month name in full
    // %Y used to display Year in full
    // %I used to display hour
    // %M used to display minutes
    // %p used to diplay am and pm
    strftime(dateOfSale, 100, "%d %B %Y %I:%M%p", currDate);

    // Store date for sales record
    strcpy(dateOfSaleRecord[totalSalesCount], dateOfSale);

    // Removing the car bought from the stock
    carModelsInStock[selectedModel] -= 1;

    // Removing the car from the total stock
    totalCarsInStock -= 1;

    // Receipt for the car
    printf("-------------Toyota Horizon, LONDON-------------------\n");
    printf("                     SALE RECEIPT             \n\n");
    printf("Customer Name:%s.\n\n", custNames[totalSalesCount]);

    // %.2f to show the value with 2 decimal places
    printf("%s                         | %hd | £%.2f \n\n",
        models[selectedModel], carQuantitySold, receiptPrice );

    if (delivery == 'd' || delivery == 'D') {
        printf("Delivery\n");
    }
    else {
        printf("Pickup\n");
    }

    if (eligibleForDiscount == 'y') {
        printf("Employee Discount: Yes\n");
    }
    else {
        printf("Employee Discount: No\n");
        }

    printf("Total amount: %.2f GBP\n\n", receiptPrice);
    printf("-----------------------Thank You----------------------\n\n");


    char leaveFeedback;
    while (T) {

        leaveFeedback = userInputWords("Would you like to leave a review (y/n): ");
        if (leaveFeedback == 'y' || leaveFeedback == 'Y') {
            feedback();
            break;
        }
        else if (leaveFeedback == 'n' || leaveFeedback == 'N') {
            break;
        }
        else {
            printf("Invalid option. Enter 'y' for yes or 'n' for no \n\n");
        }
    }


    // Using Ascii to write thank you
    printf("TTTTTTT  H   H      A    N   N  K   K       Y   Y   OOO   U   U\n");
    printf("   T     H   H     A A   NN  N  K  K         Y Y   O   O  U   U\n");
    printf("   T     HHHHH    AAAAA  N N N  KKK           Y    O   O  U   U\n");
    printf("   T     H   H    A   A  N  NN  K  K          Y    O   O  U   U\n");
    printf("   T     H   H    A   A  N   N  K   K         Y     OOO    UUU\n");





    // Storing the sale info
    // Store the car model
    carModelSoldPerSale[totalSalesCount] = selectedModel;

    // Store the no of cars sold
    carsSoldPerSale[totalSalesCount] = carQuantitySold;

    //To update how much each model was sold
    carsSoldPerModel[selectedModel] += carQuantitySold;

    // Store discount if applied
    discountApplied[totalSalesCount] = eligibleForDiscount =='y';

    // Increase the sale count by 1
    totalSalesCount++;




}





    // Option C, Sales Data
void salesData() {
    float totalRevenue = 0;

    printf("\nSales Overview\n");

    // Display sales for each car model
    for (int i = 0; i < TOTAL_MODELS; i++) {

        if (carsSoldPerModel[i] > 0) {

            printf("%s sold %d car(s)\n\n",models[i], carsSoldPerModel[i]);

        }
    }

    // Going through All records
    for (int i = 0 ; i < totalSalesCount ; i++) {
        //Getting Model for records
        int carModelForRecords = carModelSoldPerSale[i];

        // Getting how much was sold
        unsigned short amountsoldForRecords = carsSoldPerSale[i];

        // Price for Recording purpose
        float priceForRecords = price[carModelForRecords];

        // Discounted Price for Recording
        float discountPriceForRecords = priceAfterDiscount(priceForRecords);

        // Output details
        // Check if discount was applied to state in records

        if (discountApplied[i] == T) {
            printf("Sale #%d | Car Model: %s | Quantity: %hd | Price: £%.2f | Discount: Yes | Discounted Price: £%.2f | Customer: %s | Date of Sale: %s\n\n",
                   i + 1, models[carModelForRecords], amountsoldForRecords, priceForRecords, discountPriceForRecords, custNames[i], dateOfSaleRecord[i]);
        }

        else {
            printf("Sale #%d | Car Model: %s | Quantity: %hd | Price: £%.2f | Discount: No | Discounted Price: N/A | Customer: %s | Date of Sale: %s\n\n",
                  i + 1, models[carModelForRecords], amountsoldForRecords, priceForRecords, custNames[i], dateOfSaleRecord[i]);
        }

        if (discountApplied[i] == T) {
            totalRevenue += discountPriceForRecords;
        }
        else {
            totalRevenue += priceForRecords;
        }

    }


    // Print the total sales data
    printf("\n %hd Cars left in stock",totalCarsInStock );
    printf("\nTotal Cars Sold: %d\n", totalSalesCount);
    printf("Total Revenue: £%.2f\n", totalRevenue);



}




    // Option Z to display the feedback
void viewFeedback() {
    if (totalFeedbacks == 0) {
        printf("No feedbacks given.\n");
    }
    else {
        printf("Display all feedbacks: \n\n");
    }

    for (int i = 0; i < totalFeedbacks; i++) {

        printf("Feedback %d:\n", i + 1);
        printf("%s\n\n", dateOfSaleRecord[i]);
        printf("rating: %d\n",ratings[i]);
        printf("Feedback Text: %s\n\n", feedbacks[i]);
    }

}





    // Options X, Exit the program
void exitProgram() {
    printf("Thank you for using the Toyota Sales System\n");

    // Save Data
    save();
}





/* The Ending input option to press enter either loop back or end */
void endingInput(char custChoice) {
    if(custChoice == EXIT) {
        printf("\nPress Enter to Exit");
    }

    else {
        printf("\nPress Enter to return to the Menu");
    }
    // Waits till the user presses enter
    getchar();
    getchar();
}






/* To clear the screen */

// Using the code given by to tutor to clear screen on MACOS
void clearScreen() {

#ifdef _WIN32
    system("cls");
#else
    printf("\e[1;1H\e[2J");
#endif
}






/* Starting the Program */

int main() {
    char custChoice;

    // Greeting the customer
    printf("Welcome to Toyota Horizon, LONDON\n\n");

    // Adding loop so that once an option is completed it will loop back
    do {
        clearScreen();
        printf("Menu:\n");
        printf("n. Choose Your New Car\n");
        printf("b. Cars in Stock\n");
        printf("v. Customer Feedbacks\n");
        printf("c. Sales Data\n");
        printf("x. Exit\n\n");

        custChoice = userInputWords("Choose an option: ");

        clearScreen();
        switch (custChoice) {

            case BUY_CAR:
                buyingProcess();
            break;

            case CARS_AVAILABLE:
                displayCarModels();
            break;

            case VIEW_FEEDBACKS:
                viewFeedback();
            break;

            case SALES_DATA:
                salesData();
            break;

            case EXIT:
                exitProgram();
            break;

            default:
                printf("\nInvalid Choice. Please choose from the options given.\n");
            break;
        }

        endingInput(custChoice);
    } while (custChoice != EXIT);

    clearScreen();
    printf("\nHave a good day\n\n");

    return 0;

}