/*
 * ============================================================
 *  SMART VEHICLE FARE SYSTEM - Question 3
 *  BACSE104 - Structured and Object-Oriented Programming
 *  VIT - Winter Semester 2025-2026  |  Slot: D1
 * ============================================================
 *  OOP Concepts Demonstrated:
 *   - Abstract base class (Vehicle) with pure virtual function
 *   - Inheritance (Car, Bus, Bike, Auto, LuxuryCar <- Vehicle)
 *   - Runtime polymorphism via base-class pointers
 *   - Constructors & Destructors
 *   - Static members & functions
 *   - Friend function
 *   - Operator overloading (+)
 *   - Exception handling (custom exceptions)
 *   - Lambda expressions
 *   - File handling (trip_history.txt)
 *   - Menu-driven / multi-user system
 *   - Admin panel with fare multiplier (surge pricing)
 * ============================================================
 */

#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <stdexcept>
#include <ctime>
#include <functional>
#include <limits>

using namespace std;

// ============================================================
//  CUSTOM EXCEPTION CLASSES
// ============================================================

/* Exception thrown when distance is invalid (negative) */
class InvalidDistanceException : public exception {
    string msg;
public:
    explicit InvalidDistanceException(const string& m) : msg(m) {}
    const char* what() const noexcept override { return msg.c_str(); }
};

/* Exception thrown for bad menu / field input */
class InvalidInputException : public exception {
    string msg;
public:
    explicit InvalidInputException(const string& m) : msg(m) {}
    const char* what() const noexcept override { return msg.c_str(); }
};

/* Exception thrown for an unrecognised promo / coupon code */
class InvalidCouponException : public exception {
public:
    const char* what() const noexcept override {
        return "Invalid coupon code! Coupon will be ignored.";
    }
};

// ============================================================
//  TRIP RECORD (Plain Data Struct)
// ============================================================

struct TripRecord {
    string customerName;
    string vehicleType;
    double distance;
    double fare;
    string timestamp;

    TripRecord(const string& cn, const string& vt,
               double d, double f, const string& ts)
        : customerName(cn), vehicleType(vt), distance(d), fare(f), timestamp(ts) {}
};

// ============================================================
//  ABSTRACT BASE CLASS  –  Vehicle
// ============================================================

class Vehicle {
protected:
    int    vehicleID;
    string vehicleName;
    double ratePerKm;
    double minimumFare;

    static int    totalVehicles;   // tracks objects created
    static double totalEarnings;   // accumulated revenue

public:
    /* Constructor */
    Vehicle(int id, const string& name, double rate, double minFare)
        : vehicleID(id), vehicleName(name), ratePerKm(rate), minimumFare(minFare) {
        ++totalVehicles;
        cout << "  [CREATED] Vehicle object: " << vehicleName << "\n";
    }

    /* Virtual destructor – essential for correct polymorphic cleanup */
    virtual ~Vehicle() {
        cout << "  [DESTROYED] Vehicle object: " << vehicleName << "\n";
    }

    /* ---- Pure virtual function (makes Vehicle abstract) ---- */
    virtual double calculateFare(double distanceKm) = 0;

    /* Virtual display – overridden in each derived class */
    virtual void displayDetails() const {
        cout << fixed << setprecision(2)
             << "  ID: " << vehicleID
             << "  Name: " << vehicleName
             << "  Rate/km: Rs." << ratePerKm
             << "  Min Fare: Rs." << minimumFare << "\n";
    }

    /* Getters */
    string getVehicleName()  const { return vehicleName; }
    int    getVehicleID()    const { return vehicleID;   }
    double getRatePerKm()    const { return ratePerKm;   }
    double getMinimumFare()  const { return minimumFare; }

    /* Static interface */
    static int    getTotalVehicles()        { return totalVehicles; }
    static double getTotalEarnings()        { return totalEarnings; }
    static void   addEarnings(double amt)   { totalEarnings += amt; }

    /* Friend function declaration – defined after class */
    friend void compareFares(Vehicle& v1, Vehicle& v2, double distanceKm);
};

/* Static member definitions */
int    Vehicle::totalVehicles = 0;
double Vehicle::totalEarnings = 0.0;

/* Friend function – compares fare of two Vehicle objects for a given distance */
void compareFares(Vehicle& v1, Vehicle& v2, double distanceKm) {
    if (distanceKm < 0)
        throw InvalidDistanceException("Distance for comparison cannot be negative!");

    double fare1 = v1.calculateFare(distanceKm);
    double fare2 = v2.calculateFare(distanceKm);

    cout << "\n  ===== FARE COMPARISON  (" << distanceKm << " km) =====\n";
    cout << fixed << setprecision(2);
    cout << "  " << v1.vehicleName << "  :  Rs." << fare1 << "\n";
    cout << "  " << v2.vehicleName << "  :  Rs." << fare2 << "\n";
    if (fare1 < fare2)
        cout << "  >> " << v1.vehicleName << " is cheaper by Rs." << (fare2 - fare1) << "\n";
    else if (fare2 < fare1)
        cout << "  >> " << v2.vehicleName << " is cheaper by Rs." << (fare1 - fare2) << "\n";
    else
        cout << "  >> Both vehicles charge the same fare!\n";
    cout << "  ================================================\n";
}

// ============================================================
//  DERIVED CLASS 1  –  Car
// ============================================================

class Car : public Vehicle {
public:
    Car() : Vehicle(1, "Car", 15.0, 80.0) {}

    /* Slab-based pricing: 0-5 km min fare | 5-15 km Rs.15/km | >15 km Rs.12/km */
    double calculateFare(double dist) override {
        if (dist < 0) throw InvalidDistanceException("Distance cannot be negative!");
        double fare;
        if      (dist <= 5)  fare = minimumFare;
        else if (dist <= 15) fare = minimumFare + (dist - 5)  * ratePerKm;
        else                 fare = minimumFare + 10 * ratePerKm + (dist - 15) * 12.0;
        return max(fare, minimumFare);
    }

    void displayDetails() const override {
        cout << "  [CAR]        Rate: Rs.15/km | Min: Rs.80 | Slabs: 0-5(min), 5-15(Rs.15), 15+(Rs.12)\n";
    }
};

// ============================================================
//  DERIVED CLASS 2  –  Bus
// ============================================================

class Bus : public Vehicle {
    int seatingCapacity;
public:
    Bus() : Vehicle(2, "Bus", 5.0, 30.0), seatingCapacity(40) {}

    /* 0-10 km minimum fare | >10 km Rs.5/km */
    double calculateFare(double dist) override {
        if (dist < 0) throw InvalidDistanceException("Distance cannot be negative!");
        double fare = (dist <= 10) ? minimumFare : minimumFare + (dist - 10) * ratePerKm;
        return max(fare, minimumFare);
    }

    void displayDetails() const override {
        cout << "  [BUS]        Rate: Rs.5/km  | Min: Rs.30 | Capacity: " << seatingCapacity << " seats\n";
    }
};

// ============================================================
//  DERIVED CLASS 3  –  Bike
// ============================================================

class Bike : public Vehicle {
public:
    Bike() : Vehicle(3, "Bike", 8.0, 40.0) {}

    /* 0-3 km minimum fare | >3 km Rs.8/km */
    double calculateFare(double dist) override {
        if (dist < 0) throw InvalidDistanceException("Distance cannot be negative!");
        double fare = (dist <= 3) ? minimumFare : minimumFare + (dist - 3) * ratePerKm;
        return max(fare, minimumFare);
    }

    void displayDetails() const override {
        cout << "  [BIKE]       Rate: Rs.8/km  | Min: Rs.40 | Slabs: 0-3(min), 3+(Rs.8)\n";
    }
};

// ============================================================
//  DERIVED CLASS 4  –  Auto
// ============================================================

class Auto : public Vehicle {
public:
    Auto() : Vehicle(4, "Auto", 10.0, 50.0) {}

    /* 0-4 km minimum fare | >4 km Rs.10/km */
    double calculateFare(double dist) override {
        if (dist < 0) throw InvalidDistanceException("Distance cannot be negative!");
        double fare = (dist <= 4) ? minimumFare : minimumFare + (dist - 4) * ratePerKm;
        return max(fare, minimumFare);
    }

    void displayDetails() const override {
        cout << "  [AUTO]       Rate: Rs.10/km | Min: Rs.50 | Slabs: 0-4(min), 4+(Rs.10)\n";
    }
};

// ============================================================
//  DERIVED CLASS 5  –  LuxuryCar
// ============================================================

class LuxuryCar : public Vehicle {
    string carModel;
public:
    LuxuryCar() : Vehicle(5, "Luxury Car", 25.0, 200.0), carModel("Premium Sedan") {}

    /* Flat Rs.200 base + Rs.25/km for every kilometre */
    double calculateFare(double dist) override {
        if (dist < 0) throw InvalidDistanceException("Distance cannot be negative!");
        return max(minimumFare + dist * ratePerKm, minimumFare);
    }

    void displayDetails() const override {
        cout << "  [LUXURY CAR] Rate: Rs.25/km | Min: Rs.200 | Model: " << carModel << "\n";
    }
};

// ============================================================
//  CUSTOMER CLASS
// ============================================================

class Customer {
    string name;
    string customerType;   // regular | student | senior | member
    string couponCode;
    vector<TripRecord> tripHistory;

    static int totalCustomers;

public:
    /* Constructor */
    Customer(const string& n, const string& type = "regular")
        : name(n), customerType(type), couponCode("") {
        ++totalCustomers;
        cout << "  [REGISTERED] Customer: " << name << " (" << customerType << ")\n";
    }

    /* Destructor */
    ~Customer() {
        cout << "  [SESSION ENDED] Customer: " << name << "\n";
    }

    void setCoupon(const string& code) { couponCode = code; }

    string getName()   const { return name;         }
    string getType()   const { return customerType; }
    string getCoupon() const { return couponCode;   }

    void addTrip(const TripRecord& tr) { tripHistory.push_back(tr); }

    void showHistory() const {
        if (tripHistory.empty()) { cout << "  No trips yet.\n"; return; }
        cout << "\n  ===== TRIP HISTORY: " << name << " =====\n";
        for (size_t i = 0; i < tripHistory.size(); ++i) {
            cout << fixed << setprecision(2)
                 << "  Trip " << (i + 1) << " | " << tripHistory[i].vehicleType
                 << " | " << tripHistory[i].distance << " km"
                 << " | Rs." << tripHistory[i].fare
                 << " | " << tripHistory[i].timestamp << "\n";
        }
    }

    double totalSpent() const {
        double sum = 0;
        for (const auto& t : tripHistory) sum += t.fare;
        return sum;
    }

    /* Operator overloading: merge two customers into a combined view */
    Customer operator+(const Customer& other) const {
        Customer merged("Merged[" + name + "+" + other.name + "]", "regular");
        for (const auto& t : tripHistory)       merged.tripHistory.push_back(t);
        for (const auto& t : other.tripHistory) merged.tripHistory.push_back(t);
        return merged;
    }

    static int getTotalCustomers() { return totalCustomers; }
};

int Customer::totalCustomers = 0;

// ============================================================
//  FARE CALCULATOR  –  Extra Charges & Discounts
// ============================================================

class FareCalculator {
    double surgeMultiplier;   // admin-configurable surge factor (default 1.0)

public:
    FareCalculator() : surgeMultiplier(1.0) {}

    void setSurge(double m) {
        if (m <= 0) throw InvalidInputException("Surge multiplier must be positive!");
        surgeMultiplier = m;
        cout << "  [ADMIN] Surge pricing set to " << m << "x\n";
    }

    double getSurge() const { return surgeMultiplier; }

    /* Apply night / peak / waiting / luggage charges on top of base fare */
    double applyCharges(double baseFare, bool night, bool peak,
                        int waitMins, double luggageKg) {
        double fare = baseFare * surgeMultiplier;
        if (surgeMultiplier != 1.0)
            cout << "  [Surge " << surgeMultiplier << "x] Applied\n";

        if (night)  { fare *= 1.20; cout << "  [Night Charge +20%] Applied\n"; }
        if (peak)   { fare *= 1.15; cout << "  [Peak Hour +15%] Applied\n";    }

        if (waitMins > 0) {
            double wc = waitMins * 2.0;   // Rs.2 per waiting minute
            fare += wc;
            cout << "  [Waiting Rs." << fixed << setprecision(2) << wc
                 << " for " << waitMins << " min] Applied\n";
        }
        if (luggageKg > 0) {
            double lc = luggageKg * 10.0; // Rs.10 per kg
            fare += lc;
            cout << "  [Luggage Rs." << fixed << setprecision(2) << lc
                 << " for " << luggageKg << " kg] Applied\n";
        }
        return fare;
    }

    /* Apply customer-type discount and coupon discount */
    double applyDiscount(double fare, const Customer& cust) {
        double disc = 0.0;

        if      (cust.getType() == "student") { disc = 0.10; cout << "  [Student Discount 10%]\n"; }
        else if (cust.getType() == "senior")  { disc = 0.15; cout << "  [Senior Citizen 15%]\n";   }
        else if (cust.getType() == "member")  { disc = 0.20; cout << "  [Membership 20%]\n";       }

        /* Coupon system */
        string coupon = cust.getCoupon();
        if      (coupon == "SAVE10")  { disc += 0.10; cout << "  [Coupon SAVE10 +10%]\n";    }
        else if (coupon == "FIRST50") { disc += 0.50; cout << "  [Coupon FIRST50 +50%]\n";   }
        else if (coupon == "VIT25")   { disc += 0.25; cout << "  [Coupon VIT25 +25%]\n";     }
        else if (!coupon.empty())     { throw InvalidCouponException(); }

        disc = min(disc, 0.50); // cap total discount at 50 %
        return fare * (1.0 - disc);
    }

    /* Round to nearest whole rupee */
    static double roundFare(double fare) { return round(fare); }
};

// ============================================================
//  FILE HANDLER
// ============================================================

class FileHandler {
public:
    static void saveTrip(const TripRecord& tr) {
        ofstream f("trip_history.txt", ios::app);
        if (f.is_open()) {
            f << fixed << setprecision(2)
              << "Customer: " << tr.customerName
              << " | Vehicle: " << tr.vehicleType
              << " | Distance: " << tr.distance << " km"
              << " | Fare: Rs." << tr.fare
              << " | Time: " << tr.timestamp << "\n";
        }
    }

    static void showAllTrips() {
        ifstream f("trip_history.txt");
        if (!f.is_open()) { cout << "  No saved trip records found.\n"; return; }
        cout << "\n  ===== ALL SAVED TRIPS =====\n";
        string line;
        while (getline(f, line)) cout << "  " << line << "\n";
    }
};

// ============================================================
//  RECEIPT GENERATOR
// ============================================================

class ReceiptGenerator {
public:
    static void print(const string& custName, const string& vehicle,
                      double dist, double base, double final_fare,
                      const string& ts) {
        cout << "\n";
        cout << "  +------------------------------------------+\n";
        cout << "  |     SMART VEHICLE FARE  –  RECEIPT       |\n";
        cout << "  +------------------------------------------+\n";
        cout << fixed << setprecision(2);
        cout << "  Customer     : " << custName   << "\n";
        cout << "  Vehicle      : " << vehicle    << "\n";
        cout << "  Distance     : " << dist       << " km\n";
        cout << "  Base Fare    : Rs." << base     << "\n";
        cout << "  Final Fare   : Rs." << final_fare << "\n";
        cout << "  Date & Time  : " << ts         << "\n";
        cout << "  +------------------------------------------+\n";
        cout << "  |   Thank you for riding with SmartFare!   |\n";
        cout << "  +------------------------------------------+\n\n";
    }
};

// ============================================================
//  ADMIN PANEL
// ============================================================

class AdminPanel {
    FareCalculator& calc;
    string password;

public:
    AdminPanel(FareCalculator& c, const string& pwd = "admin123")
        : calc(c), password(pwd) {}

    void run() {
        string pwd;
        cout << "  Enter admin password: ";
        cin >> pwd;
        if (pwd != password) { cout << "  Access denied!\n"; return; }

        int ch;
        do {
            cout << "\n  ===== ADMIN PANEL =====\n"
                 << "  1. Total Vehicles Created\n"
                 << "  2. Total Earnings\n"
                 << "  3. Total Customers\n"
                 << "  4. View All Trip Records (file)\n"
                 << "  5. Set Surge Multiplier\n"
                 << "  6. Exit Admin\n"
                 << "  Choice: ";
            if (!(cin >> ch)) { cin.clear(); cin.ignore(1000,'\n'); ch = 0; }

            switch (ch) {
                case 1: cout << "  Total Vehicles: " << Vehicle::getTotalVehicles() << "\n"; break;
                case 2: cout << fixed << setprecision(2)
                             << "  Total Earnings: Rs." << Vehicle::getTotalEarnings() << "\n"; break;
                case 3: cout << "  Total Customers: " << Customer::getTotalCustomers() << "\n"; break;
                case 4: FileHandler::showAllTrips(); break;
                case 5: {
                    double m;
                    cout << "  Enter surge multiplier (e.g. 1.5): ";
                    cin >> m;
                    try { calc.setSurge(m); }
                    catch (InvalidInputException& e) { cout << "  ERROR: " << e.what() << "\n"; }
                    break;
                }
                case 6: cout << "  Exiting admin panel.\n"; break;
                default: cout << "  Invalid choice.\n";
            }
        } while (ch != 6);
    }
};

// ============================================================
//  HELPER – Current timestamp string
// ============================================================

string currentTime() {
    time_t now = time(nullptr);
    string s = ctime(&now);
    if (!s.empty() && s.back() == '\n') s.pop_back();
    return s;
}

// ============================================================
//  HELPER – Safe integer input
// ============================================================

int safeIntInput(const string& prompt) {
    int v;
    while (true) {
        cout << prompt;
        if (cin >> v) return v;
        cout << "  Invalid input. Try again.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

double safeDoubleInput(const string& prompt) {
    double v;
    while (true) {
        cout << prompt;
        if (cin >> v) return v;
        cout << "  Invalid input. Try again.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

// ============================================================
//  MAIN
// ============================================================

int main() {
    cout << "\n  ============================================\n";
    cout << "     SMART VEHICLE FARE SYSTEM  –  VIT 2026\n";
    cout << "  ============================================\n\n";

    /* ----- Create vehicle objects (runtime polymorphism via base pointer) ----- */
    cout << "  Initialising vehicles...\n";
    Vehicle* fleet[5];
    fleet[0] = new Car();
    fleet[1] = new Bus();
    fleet[2] = new Bike();
    fleet[3] = new Auto();
    fleet[4] = new LuxuryCar();
    cout << "\n";

    FareCalculator       calc;
    AdminPanel           admin(calc);
    vector<Customer*>    customers;
    vector<TripRecord>   allTrips;   // global trip log for lambda filtering
    Customer*            current = nullptr;

    int choice;
    do {
        cout << "  ============================\n";
        cout << "  MAIN MENU\n";
        cout << "  1.  Register / Switch Customer\n";
        cout << "  2.  Book a Ride\n";
        cout << "  3.  View Available Vehicles\n";
        cout << "  4.  My Trip History\n";
        cout << "  5.  Compare Two Vehicle Fares (Friend Function)\n";
        cout << "  6.  Filter Trips by Fare (Lambda)\n";
        cout << "  7.  Merge Two Customers (Operator +)\n";
        cout << "  8.  Admin Panel\n";
        cout << "  9.  Exit\n";
        cout << "  ============================\n";
        choice = safeIntInput("  Choice: ");

        switch (choice) {

        /* ---- 1. Register / Switch Customer ---- */
        case 1: {
            string name, type;
            cout << "  Enter name: ";
            cin.ignore(); getline(cin, name);
            cout << "  Customer type [regular / student / senior / member]: ";
            getline(cin, type);
            if (type != "student" && type != "senior" && type != "member") type = "regular";
            current = new Customer(name, type);
            customers.push_back(current);

            string coupon;
            cout << "  Coupon code (leave blank to skip): ";
            getline(cin, coupon);
            if (!coupon.empty()) current->setCoupon(coupon);
            break;
        }

        /* ---- 2. Book a Ride ---- */
        case 2: {
            if (!current) { cout << "  Please register a customer first!\n"; break; }
            try {
                /* Show fleet – runtime polymorphism */
                cout << "\n  ===== AVAILABLE VEHICLES =====\n";
                for (int i = 0; i < 5; ++i) {
                    cout << "  " << (i + 1) << ". ";
                    fleet[i]->displayDetails();
                }

                int vch = safeIntInput("  Select vehicle (1-5): ");
                if (vch < 1 || vch > 5)
                    throw InvalidInputException("Vehicle number must be between 1 and 5!");

                double dist = safeDoubleInput("  Enter distance (km): ");
                if (dist < 0)
                    throw InvalidDistanceException("Distance cannot be negative!");

                /* Extra charge inputs */
                char nChar, pChar;
                cout << "  Night time ride? (y/n): "; cin >> nChar;
                cout << "  Peak hour?       (y/n): "; cin >> pChar;
                int    waitMins  = safeIntInput("  Waiting minutes (0 if none): ");
                double luggageKg = safeDoubleInput("  Luggage kg (0 if none): ");

                if (waitMins < 0)  throw InvalidInputException("Waiting time cannot be negative!");
                if (luggageKg < 0) throw InvalidInputException("Luggage weight cannot be negative!");

                /* -- calculateFare() via base pointer = runtime polymorphism -- */
                double baseFare = fleet[vch - 1]->calculateFare(dist);

                cout << "\n  ===== FARE BREAKDOWN =====\n";
                cout << fixed << setprecision(2)
                     << "  Base Fare : Rs." << baseFare << "\n";

                double chargedFare = calc.applyCharges(baseFare,
                    nChar == 'y' || nChar == 'Y',
                    pChar == 'y' || pChar == 'Y',
                    waitMins, luggageKg);

                double finalFare;
                try {
                    finalFare = calc.applyDiscount(chargedFare, *current);
                } catch (InvalidCouponException& e) {
                    cout << "  WARNING: " << e.what() << "\n";
                    finalFare = chargedFare;
                }

                finalFare = FareCalculator::roundFare(finalFare);
                string ts  = currentTime();

                ReceiptGenerator::print(current->getName(),
                    fleet[vch - 1]->getVehicleName(),
                    dist, baseFare, finalFare, ts);

                TripRecord tr(current->getName(),
                              fleet[vch - 1]->getVehicleName(),
                              dist, finalFare, ts);
                current->addTrip(tr);
                allTrips.push_back(tr);
                FileHandler::saveTrip(tr);
                Vehicle::addEarnings(finalFare);

            } catch (const InvalidDistanceException& e) {
                cout << "  [EXCEPTION] " << e.what() << "\n";
            } catch (const InvalidInputException& e) {
                cout << "  [EXCEPTION] " << e.what() << "\n";
            } catch (const exception& e) {
                cout << "  [ERROR] " << e.what() << "\n";
            }
            break;
        }

        /* ---- 3. View Vehicles ---- */
        case 3: {
            cout << "\n  ===== ALL VEHICLES =====\n";
            for (int i = 0; i < 5; ++i) {
                cout << "  " << (i + 1) << ". ";
                fleet[i]->displayDetails();  // virtual dispatch
            }
            break;
        }

        /* ---- 4. Trip History ---- */
        case 4: {
            if (!current) { cout << "  No customer registered.\n"; break; }
            current->showHistory();
            cout << fixed << setprecision(2)
                 << "  Total Spent: Rs." << current->totalSpent() << "\n";
            break;
        }

        /* ---- 5. Compare Two Vehicles (Friend Function) ---- */
        case 5: {
            try {
                double dist = safeDoubleInput("  Distance for comparison (km): ");
                int v1 = safeIntInput("  First vehicle  (1-5): ");
                int v2 = safeIntInput("  Second vehicle (1-5): ");
                if (v1 < 1 || v1 > 5 || v2 < 1 || v2 > 5)
                    throw InvalidInputException("Vehicle numbers must be 1-5!");
                compareFares(*fleet[v1 - 1], *fleet[v2 - 1], dist); // friend function
            } catch (const exception& e) {
                cout << "  [EXCEPTION] " << e.what() << "\n";
            }
            break;
        }

        /* ---- 6. Filter Trips by Fare (Lambda Expression) ---- */
        case 6: {
            double threshold = safeDoubleInput("  Show trips with fare > Rs.: ");

            /* Lambda expression to filter trip records */
            auto isAboveThreshold = [threshold](const TripRecord& tr) {
                return tr.fare > threshold;
            };

            cout << "\n  ===== TRIPS WITH FARE > Rs." << fixed << setprecision(2)
                 << threshold << " =====\n";
            bool found = false;
            for (const auto& tr : allTrips) {
                if (isAboveThreshold(tr)) {
                    cout << "  " << tr.customerName << " | " << tr.vehicleType
                         << " | " << tr.distance << " km | Rs." << tr.fare << "\n";
                    found = true;
                }
            }
            if (!found) cout << "  No trips found above this threshold.\n";
            break;
        }

        /* ---- 7. Merge Two Customers (Operator Overloading) ---- */
        case 7: {
            if (customers.size() < 2) {
                cout << "  Need at least 2 registered customers.\n"; break;
            }
            cout << "  Registered customers:\n";
            for (size_t i = 0; i < customers.size(); ++i)
                cout << "  " << (i + 1) << ". " << customers[i]->getName() << "\n";
            int c1 = safeIntInput("  Select first customer:  ");
            int c2 = safeIntInput("  Select second customer: ");
            if (c1 < 1 || c2 < 1 ||
                c1 > (int)customers.size() || c2 > (int)customers.size()) {
                cout << "  Invalid selection.\n"; break;
            }
            Customer merged = *customers[c1-1] + *customers[c2-1]; // operator+
            merged.showHistory();
            cout << fixed << setprecision(2)
                 << "  Combined Total Spent: Rs." << merged.totalSpent() << "\n";
            break;
        }

        /* ---- 8. Admin Panel ---- */
        case 8:
            admin.run();
            break;

        case 9:
            cout << "  Goodbye! Thank you for using SmartFare.\n";
            break;

        default:
            cout << "  Invalid choice. Please enter 1-9.\n";
        }

    } while (choice != 9);

    /* ----- Session Summary ----- */
    cout << "\n  ===== SESSION SUMMARY =====\n";
    cout << "  Total Vehicle Types   : " << Vehicle::getTotalVehicles() << "\n";
    cout << "  Total Customers       : " << Customer::getTotalCustomers() << "\n";
    cout << fixed << setprecision(2)
         << "  Total Earnings (Rs.)  : " << Vehicle::getTotalEarnings() << "\n";
    cout << "  ===========================\n\n";
    cout << "  All trip records have been saved to 'trip_history.txt'.\n";
    cout 
    /* ----- Cleanup (destructors called here) ----- */
    cout << "  Releasing vehicle objects...\n";
    for (int i = 0; i < 5; ++i) delete fleet[i];

    cout << "  Releasing customer objects...\n";
    for (auto* c : customers) delete c;

    return 0;
}qq