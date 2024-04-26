#include <iostream>
#include <fstream>
#include <cstring>
#include <sstream>
#include <ctime>
using namespace std;
struct date {
    int day, month, year;
    date *next;
};
struct car {
    string name;
    string plateNumber;
    string color;
    date *tDate;
    car *next;
};
struct user {
    int userID;
    string fname;
    string lname;
    car *rentedCar;
    user *next, *previous;
};
struct userList {
    user *head, *tail;
};
char* strtok_r(char *str, const char *delim, char **nextp) {
    if (str == NULL) {
        str = *nextp;
    }
    str += strspn(str, delim);
    if (*str == '\0') {
        return NULL;
    }
    char *ret = str;
    str += strcspn(str, delim);
    if (*str) {
        *str++ = '\0';
    }
    *nextp = str;
    return ret;
}
userList* parseFiles() {
    userList* uList = new userList();
    ifstream usersFile("users.txt");
    string line;
    while (getline(usersFile, line)) {
        char* next_token1 = NULL;
        char* token = strtok_r(&line[0], ",", &next_token1);
        user* newUser = new user();
        newUser->userID = stoi(token);
        token = strtok_r(NULL, ",", &next_token1);
        newUser->fname = token;
        token = strtok_r(NULL, ",", &next_token1);
        newUser->lname = token;
        // zid user
        if (uList->head == nullptr) {
            uList->head = newUser;
            uList->tail = newUser;
        } else {
            newUser->previous = uList->tail;
            uList->tail->next = newUser;
            uList->tail = newUser;
        }

        // open the rented_cars
        ifstream rentedCarsFile("rented_cars.txt");
        string rentedCarLine;
        while (getline(rentedCarsFile, rentedCarLine)) {
            char* next_token2 = NULL;
            char* rentedCarToken = strtok_r(&rentedCarLine[0], ":", &next_token2);
            int rentedUserID = stoi(rentedCarToken);
            if (rentedUserID == newUser->userID) {
                rentedCarToken = strtok_r(NULL, ":", &next_token2);
                char* next_token3 = NULL;
                char* carToken = strtok_r(rentedCarToken, ",", &next_token3);
                while (carToken != NULL) {
                    char* next_token4 = NULL;
                    char* carDetails = strtok_r(carToken, "#", &next_token4);
                    car* newCar = new car();
                    newCar->plateNumber = carDetails;
                    carDetails = strtok_r(NULL, "#", &next_token4);
                    char* next_token5 = NULL;
                    char* dateToken = strtok_r(carDetails, "/", &next_token5);
                    date* newDate = new date();
                    sscanf(dateToken, "%d-%d-%d", &newDate->day, &newDate->month, &newDate->year);
                    newCar->tDate = newDate;
                    dateToken = strtok_r(NULL, "/", &next_token5);
                    if (dateToken != NULL) {
                        date* returnDate = new date();
                        sscanf(dateToken, "%d-%d-%d", &returnDate->day, &returnDate->month, &returnDate->year);
                        newDate->next = returnDate;
                    }

                    // open the cars
                    ifstream carsFile("cars.txt");
                    string carLine;
                    while (getline(carsFile, carLine)) {
                        istringstream iss(carLine);
                        string carToken;
                        getline(iss, carToken, ',');
                        carToken.erase(0, carToken.find_first_not_of(' ')); // trim leading whitespace
                        carToken.erase(carToken.find_last_not_of(' ') + 1); // trim trailing whitespace
                        string plateNumber = newCar->plateNumber;
                        plateNumber.erase(0, plateNumber.find_first_not_of(' ')); // trim leading whitespace
                        plateNumber.erase(plateNumber.find_last_not_of(' ') + 1); // trim trailing whitespace
                        if (plateNumber == carToken) {
                            getline(iss, carToken, ',');
                            newCar->name = carToken;
                            getline(iss, carToken, ',');
                            newCar->color = carToken;
                            break;
                        }
                    }
                    carsFile.close();

                    // add car to list
                    if (newUser->rentedCar == nullptr) {
                        newUser->rentedCar = newCar;
                    } else {
                        car* lastCar = newUser->rentedCar;
                        while (lastCar->next != nullptr) {
                            lastCar = lastCar->next;
                        }
                        lastCar->next = newCar;
                    }
                    carToken = strtok_r(NULL, ",", &next_token3);
                }
            }
        }
        rentedCarsFile.close();
    }
    usersFile.close();

    return uList;
}
void printUserList(userList* uList) {
    user* currentUser = uList->head;
    while (currentUser != nullptr) {
        cout << "User ID: " << currentUser->userID << "\n";
        cout << "First Name: " << currentUser->fname << "\n";
        cout << "Last Name: " << currentUser->lname << "\n";
        car* currentCar = currentUser->rentedCar;
        while (currentCar != nullptr) {
            cout << "Car Name: " << currentCar->name << "\n";
            cout << "Car Color: " << currentCar->color << "\n";
            cout << "Car Plate Number: " << currentCar->plateNumber << "\n";
            date* currentDate = currentCar->tDate;
            while (currentDate != nullptr) {
                cout << "Date: " << currentDate->day << "-" << currentDate->month << "-" << currentDate->year << "\n";
                currentDate = currentDate->next;
            }
            currentCar = currentCar->next;
        }
        currentUser = currentUser->next;
    }
}
void addUser(userList* uList, user* newUser) {
    // if the user mawjoud
    user* currentUser = uList->head;
    while (currentUser != nullptr) {
        if (currentUser->userID == newUser->userID) {
            cout << "User with ID: " << newUser->userID << " already exists.\n";
            return;
        }
        currentUser = currentUser->next;
    }

    // If the user does not exist, add the user to the userList
    if (uList->head == nullptr) {
        uList->head = newUser;
        uList->tail = newUser;
    } else {
        newUser->previous = uList->tail;
        uList->tail->next = newUser;
        uList->tail = newUser;
    }
}
car* parseCarsFile() {
    ifstream carsFile("cars.txt");
    string line;
    car* head = nullptr;
    car* tail = nullptr;
    while (getline(carsFile, line)) {
        istringstream iss(line);
        string token;
        car* newCar = new car();
        getline(iss, token, ',');
        newCar->plateNumber = token;
        getline(iss, token, ',');
        newCar->name = token;
        getline(iss, token, ',');
        newCar->color = token;
        newCar->next = nullptr;
        if (head == nullptr) {
            head = newCar;
            tail = newCar;
        } else {
            tail->next = newCar;
            tail = newCar;
        }
    }
    carsFile.close();
    return head;
}
void printCarList(car* head) {
    car* currentCar = head;
    while (currentCar != nullptr) {
        cout << "Car Name: " << currentCar->name << "\n";
        cout << "Car Color: " << currentCar->color << "\n";
        cout << "Car Plate Number: " << currentCar->plateNumber << "\n";
        currentCar = currentCar->next;
    }
}
void writeBackToFiles(userList* uList) {
    // Open the users
    ofstream usersFile("users.txt");
    // Open the rented_cars
    ofstream rentedCarsFile("rented_cars.txt");

    user* currentUser = uList->head;
    while (currentUser != nullptr) {
        // write   user detail to the users
        usersFile << currentUser->userID << "," << currentUser->fname << "," << currentUser->lname << "\n";

        // Only write the user's rented cars to the rented_cars.txt file if the user has rented cars
        if (currentUser->rentedCar != nullptr) {
            rentedCarsFile << currentUser->userID << ":";
            car* currentCar = currentUser->rentedCar;
            while (currentCar != nullptr) {
                rentedCarsFile << currentCar->plateNumber << "#";
                date* currentDate = currentCar->tDate;
                while (currentDate != nullptr) {
                    rentedCarsFile << currentDate->day << "-" << currentDate->month << "-" << currentDate->year;
                    if (currentDate->next != nullptr) {
                        rentedCarsFile << "/";
                    }
                    currentDate = currentDate->next;
                }
                if (currentCar->next != nullptr) {
                    rentedCarsFile << ","; // remouve " " baad rl ,
                }
                currentCar = currentCar->next;
            }
            rentedCarsFile << "\n";
        }

        currentUser = currentUser->next;
    }

    // Close the files
    usersFile.close();
    rentedCarsFile.close();
}
void swap(user* a, user* b, userList* uList, user* previous) {
    if (previous != nullptr) {
        previous->next = b;
    } else {
        uList->head = b;
    }

    a->next = b->next;
    b->next = a;
}
void bubbleSort(userList* uList) {
    bool swapped;
    user *current;
    user *previous = nullptr;
    user *end = nullptr;

    if (uList->head == nullptr) {
        return;
    }

    do {
        swapped = false;
        current = uList->head;

        while (current->next != end) {
            if (current->lname > current->next->lname) {
                swap(current, current->next, uList, previous);
                swapped = true;
            }
            previous = current;
            current = current->next;
        }
        end = current;
    } while (swapped);
}
void rentCar(userList* uList, user* u, date* startDate, date* endDate) {
    // Parse the cars.txt file to get the list of all cars
    car* allCars = parseCarsFile();

    // cout <<carss
    cout << "Available cars:\n";
    car* availableCar = allCars;
    while (availableCar != nullptr) {
        // Check if the car hada hejiza
        bool isRented = false;
        user* checkUser = uList->head;
        while (checkUser != nullptr) {
            car* checkCar = checkUser->rentedCar;
            while (checkCar != nullptr) {
                if (checkCar->plateNumber == availableCar->plateNumber &&
                    ((checkCar->tDate->day >= startDate->day && checkCar->tDate->month >= startDate->month && checkCar->tDate->year >= startDate->year) &&
                    (checkCar->tDate->next->day <= endDate->day && checkCar->tDate->next->month <= endDate->month && checkCar->tDate->next->year <= endDate->year))) {
                    isRented = true;
                    break;
                }
                checkCar = checkCar->next;
            }
            if (isRented) {
                break;
            }
            checkUser = checkUser->next;
        }

        // If the car is not rented during the specified date range, print its details
        if (!isRented) {
            cout << "Car Name: " << availableCar->name << "\n";
            cout << "Car Color: " << availableCar->color << "\n";
            cout << "Car Plate Number: " << availableCar->plateNumber << "\n";
        }

        availableCar = availableCar->next;
    }

    // ask the user to select a car
    string plateNumber;
    cout << "Enter the plate number of the car you want to rent: ";
    cin >> plateNumber;

    // create a new car node and add it to the user's rentedCar list
    car* newCar = new car();
    newCar->plateNumber = plateNumber;
    newCar->tDate = startDate;
    startDate->next = endDate;
    if (u->rentedCar == nullptr) {
        u->rentedCar = newCar;
    } else {
        car* lastCar = u->rentedCar;
        while (lastCar->next != nullptr) {
            lastCar = lastCar->next;
        }
        lastCar->next = newCar;
    }


}
void addNewCar() {
    // Create a new car object
    car* newCar = new car;


    cout << "Enter the plate number of the new car: ";
    cin >> newCar->plateNumber;
    cout << "Enter the name of the new car: ";
    cin >> newCar->name;
    cout << "Enter the color of the new car: ";
    cin >> newCar->color;


    ofstream carsFile("cmake-build-debug/cars.txt", ios_base::app);


    carsFile << newCar->plateNumber << "," << newCar->name << "," << newCar->color << "\n";


    carsFile.close();
}
void deleteRentedCarsBeforeDate(userList* uList, date* specifiedDate) {
    // get the current date
    time_t t = time(0);   // get time now
    tm* now = localtime(&t);
    date* currentDate = new date();
    currentDate->day = now->tm_mday;
    currentDate->month = now->tm_mon + 1;
    currentDate->year = now->tm_year + 1900;

    // Iterate over the userList
    user* currentUser = uList->head;
    while (currentUser != nullptr) {
        // Iterate over the rented cars of the current user
        car* currentCar = currentUser->rentedCar;
        car* previousCar = nullptr;
        while (currentCar != nullptr) {
            // Check if the start date of the rented car is before the specified date and after the current date
            if ((currentCar->tDate->year < specifiedDate->year ||
                 (currentCar->tDate->year == specifiedDate->year && currentCar->tDate->month < specifiedDate->month) ||
                 (currentCar->tDate->year == specifiedDate->year && currentCar->tDate->month == specifiedDate->month && currentCar->tDate->day < specifiedDate->day)) &&
                (currentCar->tDate->year > currentDate->year ||
                 (currentCar->tDate->year == currentDate->year && currentCar->tDate->month > currentDate->month) ||
                 (currentCar->tDate->year == currentDate->year && currentCar->tDate->month == currentDate->month && currentCar->tDate->day > currentDate->day))) {
                // Delete the rented car
                if (previousCar != nullptr) {
                    previousCar->next = currentCar->next;
                } else {
                    currentUser->rentedCar = currentCar->next;
                }
                delete currentCar;
                currentCar = previousCar != nullptr ? previousCar->next : currentUser->rentedCar;
            } else {
                previousCar = currentCar;
                currentCar = currentCar->next;
            }
        }
        currentUser = currentUser->next;
    }
}
void printUsersFromFile() {

    ifstream usersFile("users.txt");
    string line;


    while (getline(usersFile, line)) {
        istringstream iss(line);
        string token;


        getline(iss, token, ',');
        int userID = stoi(token);


        getline(iss, token, ',');
        string firstName = token;


        getline(iss, token, ',');
        string lastName = token;


        cout << "User ID: " << userID << "\n";
        cout << "First Name: " << firstName << "\n";
        cout << "Last Name: " << lastName << "\n";
    }


    usersFile.close();
}
void rentCarProcess(userList* uList) {

    printUsersFromFile();


    int userID;
    user* u = nullptr;
    do {
        cout << "Enter user ID or -1 to exit: ";
        cin >> userID;


        if (userID == -1) {
            return;
        }

        u = uList->head;
        while (u != nullptr && u->userID != userID) {
            u = u->next;
        }
        if (u == nullptr) {
            cout << "User with ID: " << userID << " does not exist.\n";
        }
    } while (u == nullptr);


    date* startDate = new date();
    cout << "Enter start date (day month year): ";
    cin >> startDate->day >> startDate->month >> startDate->year;


    date* endDate = new date();
    cout << "Enter end date (day month year): ";
    cin >> endDate->day >> endDate->month >> endDate->year;


    rentCar(uList, u, startDate, endDate);


    printUserList(uList);
}
void addNewUserProcess(userList* uList) {

    user* newUser = new user;

    cout << "Enter the ID of the new user: ";
    cin >> newUser->userID;
    cout << "Enter the first name of the new user: ";
    cin >> newUser->fname;
    cout << "Enter the last name of the new user: ";
    cin >> newUser->lname;

    addUser(uList, newUser);

    writeBackToFiles(uList);
}
void displayAllCarsProcess() {
    car* allCars = parseCarsFile();

    printCarList(allCars);
}
void deleteRentedCarsProcess(userList* uList) {
    date* specifiedDate = new date();
    cout << "Enter date (day month year): ";
    cin >> specifiedDate->day >> specifiedDate->month >> specifiedDate->year;

    deleteRentedCarsBeforeDate(uList, specifiedDate);
}
void sortUserList(userList* uList) {
    if (uList->head == nullptr || uList->head->next == nullptr) {
        return;
    }

    bool swapped;
    do {
        swapped = false;
        user* current = uList->head;
        user* previous = nullptr;
        user* next = current->next;

        while (next != nullptr) {
            if (current->lname > next->lname) {
                // Swap current and next nodes
                swapped = true;
                if (previous != nullptr) {
                    // This is not the first node
                    previous->next = next;
                } else {
                    // This is the first node, update the head
                    uList->head = next;
                }
                // Swap next and current
                current->next = next->next;
                next->next = current;

                // Update previous and current
                previous = next;
                next = current->next;
            } else {
                // Move to the next pair
                previous = current;
                current = next;
                next = next->next;
            }
        }
    } while (swapped);
}
void displayUserInfo(userList* uList, int userID) {
    // Find the user with the given ID in the user list
    user* u = uList->head;
    while (u != nullptr && u->userID != userID) {
        u = u->next;
    }

    // If the user is found, print their details
    if (u != nullptr) {
        cout << "User ID: " << u->userID << "\n";
        cout << "First Name: " << u->fname << "\n";
        cout << "Last Name: " << u->lname << "\n";

        // Print the details of the user's rented cars
        car* currentCar = u->rentedCar;
        int carID = 1;
        while (currentCar != nullptr) {
            cout << "Car ID: " << carID << "\n";
            cout << "Car Name: " << currentCar->name << "\n";
            cout << "Car Color: " << currentCar->color << "\n";
            cout << "Car Plate Number: " << currentCar->plateNumber << "\n";
            date* currentDate = currentCar->tDate;
            while (currentDate != nullptr) {
                cout << "Date: " << currentDate->day << "-" << currentDate->month << "-" << currentDate->year << "\n";
                currentDate = currentDate->next;
            }
            currentCar = currentCar->next;
            carID++;
        }
    } else {
        cout << "User with ID: " << userID << " does not exist.\n";
    }
}
void filterCarsByColor(string color) {
    car* allCars = parseCarsFile();

    car* currentCar = allCars;
    while (currentCar != nullptr) {
        // If the car's color matches the specified color, print the car's details
        if (currentCar->color == color) {
            cout << "Car Name: " << currentCar->name << "\n";
            cout << "Car Color: " << currentCar->color << "\n";
            cout << "Car Plate Number: " << currentCar->plateNumber << "\n";
        }
        currentCar = currentCar->next;
    }
}
void searchRentedCarsOnDate(userList* uList, date* searchDate) {
    // Iterate over the userList
    user* currentUser = uList->head;
    while (currentUser != nullptr) {
        // Iterate over the rented cars of the current user
        car* currentCar = currentUser->rentedCar;
        while (currentCar != nullptr) {
            // Check if the rented car's date range includes the specified date
            if ((currentCar->tDate->year < searchDate->year ||
                 (currentCar->tDate->year == searchDate->year && currentCar->tDate->month < searchDate->month) ||
                 (currentCar->tDate->year == searchDate->year && currentCar->tDate->month == searchDate->month && currentCar->tDate->day <= searchDate->day)) &&
                (currentCar->tDate->next->year > searchDate->year ||
                 (currentCar->tDate->next->year == searchDate->year && currentCar->tDate->next->month > searchDate->month) ||
                 (currentCar->tDate->next->year == searchDate->year && currentCar->tDate->next->month == searchDate->month && currentCar->tDate->next->day >= searchDate->day))) {
                // Print the user's and car's details
                cout << "User ID: " << currentUser->userID << "\n";
                cout << "First Name: " << currentUser->fname << "\n";
                cout << "Last Name: " << currentUser->lname << "\n";
                cout << "Car Name: " << currentCar->name << "\n";
                cout << "Car Color: " << currentCar->color << "\n";
                cout << "Car Plate Number: " << currentCar->plateNumber << "\n";
            }
            currentCar = currentCar->next;
        }
        currentUser = currentUser->next;
    }
}
void deleteRentedCar(userList* uList, int userID) {
    // Find the user with the given ID in the user list
    user* u = uList->head;
    while (u != nullptr && u->userID != userID) {
        u = u->next;
    }

    // If the user is found, print their rented cars and ask which one to delete
    if (u != nullptr) {
        car* currentCar = u->rentedCar;
        int carID = 1;
        while (currentCar != nullptr) {
            cout << "Car ID: " << carID << "\n";
            cout << "Car Name: " << currentCar->name << "\n";
            cout << "Car Color: " << currentCar->color << "\n";
            cout << "Car Plate Number: " << currentCar->plateNumber << "\n";
            date* currentDate = currentCar->tDate;
            while (currentDate != nullptr) {
                cout << "Date: " << currentDate->day << "-" << currentDate->month << "-" << currentDate->year << "\n";
                currentDate = currentDate->next;
            }
            currentCar = currentCar->next;
            carID++;
        }

        int deleteCarID;
        cout << "Enter the car ID of the reservation you want to delete: ";
        cin >> deleteCarID;

        currentCar = u->rentedCar;
        car* previousCar = nullptr;
        int currentCarID = 1;
        while (currentCar != nullptr && currentCarID != deleteCarID) {
            previousCar = currentCar;
            currentCar = currentCar->next;
            currentCarID++;
        }
        if (currentCar != nullptr) {
            if (previousCar != nullptr) {
                previousCar->next = currentCar->next;
            } else {
                u->rentedCar = currentCar->next;
            }
            delete currentCar;
            cout << "Reservation with Car ID: " << deleteCarID << " has been deleted.\n";
        } else {
            cout << "Reservation with Car ID: " << deleteCarID << " does not exist.\n";
        }
    } else {
        cout << "User with ID: " << userID << " does not exist.\n";
    }
}
int main() {
    userList* uList = parseFiles();
    sortUserList(uList);
    int choice;
    string color;

    do {
        cout << "\n==================== MENU ====================\n";
        cout << " 1. Add a new user\n";
        cout << " 2. Display user info\n";
        cout << " 3. Add a new car\n";
        cout << " 4. Display all cars\n";
        cout << " 5. Filter cars by color\n";
        cout << " 6. Rent a car\n";
        cout << " 7. Delete a rented car\n";
        cout << " 8. Delete rented cars before a specified date\n";
        cout << " 9. Search for rented cars on a specific date\n";
        cout << "10. Save changes\n";
        cout << " 0. Exit\n";
        cout << "==============================================\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch(choice) {
            case 1:
                addNewUserProcess(uList);
                break;
            case 2:
                printUserList(uList);
                break;
            case 3:
                addNewCar();
                break;
            case 4:
                displayAllCarsProcess();
                break;
            case 5:
                cout << "Enter the color: ";
                cin >> color;
                filterCarsByColor(color);
                break;
            case 6:
                rentCarProcess(uList);
                break;
            case 7:
                int userID;
                cout << "Enter the user ID: ";
                cin >> userID;
                deleteRentedCar(uList, userID);
                break;
            case 8:
                deleteRentedCarsProcess(uList);
                break;
            case 9:
                date searchDate;
                cout << "Enter the date (day month year): ";
                cin >> searchDate.day >> searchDate.month >> searchDate.year;
                searchRentedCarsOnDate(uList, &searchDate);
                break;
            case 10:
                writeBackToFiles(uList);
                cout << "Changes saved successfully.\n";
                break;
            case 0:
                cout << "Exiting the program.\n";
                break;
            default:
                cout << "Invalid choice.\n";
                break;
        }
    } while (choice != 0);

    return 0;
}