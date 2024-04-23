#include <iostream>
#include <fstream>
#include <cstring>
#include <sstream>

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
    std::ifstream usersFile("users.txt");
    std::string line;
    while (std::getline(usersFile, line)) {
        char* next_token1 = NULL;
        char* token = strtok_r(&line[0], ",", &next_token1);
        user* newUser = new user();
        newUser->userID = std::stoi(token);
        token = strtok_r(NULL, ",", &next_token1);
        newUser->fname = token;
        token = strtok_r(NULL, ",", &next_token1);
        newUser->lname = token;
        // Add the new user to the userList
        if (uList->head == nullptr) {
            uList->head = newUser;
            uList->tail = newUser;
        } else {
            newUser->previous = uList->tail;
            uList->tail->next = newUser;
            uList->tail = newUser;
        }

        // Open the rented_cars.txt file and search for the current user's rented cars
        std::ifstream rentedCarsFile("rented_cars.txt");
        std::string rentedCarLine;
        while (std::getline(rentedCarsFile, rentedCarLine)) {
            char* next_token2 = NULL;
            char* rentedCarToken = strtok_r(&rentedCarLine[0], ":", &next_token2);
            int rentedUserID = std::stoi(rentedCarToken);
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

                    // Open the cars.txt file and search for the car details
                    std::ifstream carsFile("cars.txt");
                    std::string carLine;
                    while (std::getline(carsFile, carLine)) {
                        std::istringstream iss(carLine);
                        std::string carToken;
                        std::getline(iss, carToken, ',');
                        carToken.erase(0, carToken.find_first_not_of(' ')); // trim leading whitespace
                        carToken.erase(carToken.find_last_not_of(' ') + 1); // trim trailing whitespace
                        std::string plateNumber = newCar->plateNumber;
                        plateNumber.erase(0, plateNumber.find_first_not_of(' ')); // trim leading whitespace
                        plateNumber.erase(plateNumber.find_last_not_of(' ') + 1); // trim trailing whitespace
                        if (plateNumber == carToken) {
                            std::getline(iss, carToken, ',');
                            newCar->name = carToken;
                            std::getline(iss, carToken, ',');
                            newCar->color = carToken;
                            break;
                        }
                    }
                    carsFile.close();

                    // Add the new car to the user's rentedCar list
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
        std::cout << "User ID: " << currentUser->userID << "\n";
        std::cout << "First Name: " << currentUser->fname << "\n";
        std::cout << "Last Name: " << currentUser->lname << "\n";
        car* currentCar = currentUser->rentedCar;
        while (currentCar != nullptr) {
            std::cout << "Car Name: " << currentCar->name << "\n";
            std::cout << "Car Color: " << currentCar->color << "\n";
            std::cout << "Car Plate Number: " << currentCar->plateNumber << "\n";
            date* currentDate = currentCar->tDate;
            while (currentDate != nullptr) {
                std::cout << "Date: " << currentDate->day << "-" << currentDate->month << "-" << currentDate->year << "\n";
                currentDate = currentDate->next;
            }
            currentCar = currentCar->next;
        }
        currentUser = currentUser->next;
    }
}
void addUser(userList* uList, user* newUser) {
    // Check if the user already exists in the userList
    user* currentUser = uList->head;
    while (currentUser != nullptr) {
        if (currentUser->userID == newUser->userID) {
            std::cout << "User with ID: " << newUser->userID << " already exists.\n";
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
    std::ifstream carsFile("cars.txt");
    std::string line;
    car* head = nullptr;
    car* tail = nullptr;
    while (std::getline(carsFile, line)) {
        std::istringstream iss(line);
        std::string token;
        car* newCar = new car();
        std::getline(iss, token, ',');
        newCar->plateNumber = token;
        std::getline(iss, token, ',');
        newCar->name = token;
        std::getline(iss, token, ',');
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
        std::cout << "Car Name: " << currentCar->name << "\n";
        std::cout << "Car Color: " << currentCar->color << "\n";
        std::cout << "Car Plate Number: " << currentCar->plateNumber << "\n";
        currentCar = currentCar->next;
    }
}
void writeBackToFiles(userList* uList) {
    // Open the users.txt file in write mode
    std::ofstream usersFile("users.txt");
    // Open the rented_cars.txt file in write mode
    std::ofstream rentedCarsFile("rented_cars.txt");

    user* currentUser = uList->head;
    while (currentUser != nullptr) {
        // Write the user details to the users.txt file
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
                    rentedCarsFile << ","; // Removed the space after the comma
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
}// Function to find the middle of the linked list
user* getMiddle(user* head) {
    if (head == nullptr) {
        return head;
    }
    user* slow = head;
    user* fast = head;
    while (fast->next != nullptr && fast->next->next != nullptr) {
        slow = slow->next;
        fast = fast->next->next;
    }
    return slow;
}

// Function to merge two sorted linked lists
user* merge(user* first, user* second) {
    // If first linked list is empty
    if (first == nullptr) {
        return second;
    }

    // If second linked list is empty
    if (second == nullptr) {
        return first;
    }

    // Pick the smaller value and adjust the links
    if (first->lname < second->lname) {
        first->next = merge(first->next, second);
        first->next->previous = first;
        first->previous = nullptr;
        return first;
    } else {
        second->next = merge(first, second->next);
        second->next->previous = second;
        second->previous = nullptr;
        return second;
    }
}

// Function to implement merge sort
user* mergeSort(user* head) {
    if (head == nullptr || head->next == nullptr) {
        return head;
    }
    user* middle = getMiddle(head);
    user* nextOfMiddle = middle->next;

    middle->next = nullptr;
    nextOfMiddle->previous = nullptr;

    // Apply mergeSort on left list
    user* left = mergeSort(head);

    // Apply mergeSort on right list
    user* right = mergeSort(nextOfMiddle);

    // Merge the left and right lists
    user* sortedList = merge(left, right);

    return sortedList;
}

// Function to sort the user list
void sortUserList(userList* uList) {
    uList->head = mergeSort(uList->head);
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


int main() {
    // Parse the files and get the user list
    userList* uList = parseFiles();

    // Print the user list
    printUserList(uList);

    // Create a new user
    user* newUser = new user();
    newUser->userID = 5;
    newUser->fname = "New";
    newUser->lname = "User";

    // Add the new user to the user list
    addUser(uList, newUser);

    // Print the updated user list
    printUserList(uList);
    bubbleSort(uList);
    printUserList(uList);
    // Write the updated user list back to the files
    writeBackToFiles(uList);

    return 0;
}

