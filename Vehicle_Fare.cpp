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

class InvalidDistanceException : public exception {
    string msg;
public:
    explicit InvalidDistanceException(const string& m) : msg(m) {}
    const char* what() const noexcept override { return msg.c_str(); }
};

class InvalidInputException : public exception {
    string msg;
public:
    explicit InvalidInputException(const string& m) : msg(m) {}
    const char* what() const noexcept override { return msg.c_str(); }
};

class InvalidCouponException : public exception {
public:
    const char* what() const noexcept override {
        return "Invalid coupon code! Coupon will be ignored.";
    }
};

class InvalidBaggageException : public exception {
    string msg;
public:
    explicit InvalidBaggageException(const string& m) : msg(m) {}
    const char* what() const noexcept override { return msg.c_str(); }
};

struct TripRecord {
    string customerName;
    string vehicleType;
    double distance;
    double fare;
    string timestamp;
    double baggageFee;
    int    rating;   // -1 = not rated, 1-5 = star rating

    TripRecord(const string& cn, const string& vt,
               double d, double f, const string& ts,
               double bagFee = 0.0, int rt = -1)
        : customerName(cn), vehicleType(vt), distance(d), fare(f),
          timestamp(ts), baggageFee(bagFee), rating(rt) {}
};

class Vehicle {
protected:
    int    vehicleID;
    string vehicleName;
    double ratePerKm;
    double minimumFare;

    static int    totalVehicles;
    static double totalEarnings;

public:
    Vehicle(int id, const string& name, double rate, double minFare)
        : vehicleID(id), vehicleName(name), ratePerKm(rate), minimumFare(minFare) {
        ++totalVehicles;
        cout << "  [CREATED] Vehicle object: " << vehicleName << "\n";
    }

    virtual ~Vehicle() {
        cout << "  [DESTROYED] Vehicle object: " << vehicleName << "\n";
    }

    virtual double calculateFare(double distanceKm) = 0;

    virtual void displayDetails() const {
        cout << fixed << setprecision(2)
             << "  ID: " << vehicleID
             << "  Name: " << vehicleName
             << "  Rate/km: Rs." << ratePerKm
             << "  Min Fare: Rs." << minimumFare << "\n";
    }

    virtual bool allowsBaggage() const { return true; }
    virtual int  freeBagAllowance() const { return 0; }

    string getVehicleName()  const { return vehicleName; }
    int    getVehicleID()    const { return vehicleID;   }
    double getRatePerKm()    const { return ratePerKm;   }
    double getMinimumFare()  const { return minimumFare; }

    static int    getTotalVehicles()      { return totalVehicles; }
    static double getTotalEarnings()      { return totalEarnings; }
    static void   addEarnings(double amt) { totalEarnings += amt; }

    friend void compareFares(Vehicle& v1, Vehicle& v2, double distanceKm);
};

int    Vehicle::totalVehicles = 0;
double Vehicle::totalEarnings = 0.0;

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

class Car : public Vehicle {
public:
    Car() : Vehicle(1, "Car", 15.0, 80.0) {}

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

class Bus : public Vehicle {
    int seatingCapacity;
public:
    Bus() : Vehicle(2, "Bus", 5.0, 30.0), seatingCapacity(40) {}

    double calculateFare(double dist) override {
        if (dist < 0) throw InvalidDistanceException("Distance cannot be negative!");
        double fare = (dist <= 10) ? minimumFare : minimumFare + (dist - 10) * ratePerKm;
        return max(fare, minimumFare);
    }

    void displayDetails() const override {
        cout << "  [BUS]        Rate: Rs.5/km  | Min: Rs.30 | Capacity: " << seatingCapacity << " seats\n";
    }
};

class Bike : public Vehicle {
public:
    Bike() : Vehicle(3, "Bike", 8.0, 40.0) {}

    double calculateFare(double dist) override {
        if (dist < 0) throw InvalidDistanceException("Distance cannot be negative!");
        double fare = (dist <= 3) ? minimumFare : minimumFare + (dist - 3) * ratePerKm;
        return max(fare, minimumFare);
    }

    void displayDetails() const override {
        cout << "  [BIKE]       Rate: Rs.8/km  | Min: Rs.40 | Slabs: 0-3(min), 3+(Rs.8)\n";
    }

    bool allowsBaggage() const override { return false; }
};

class Auto : public Vehicle {
public:
    Auto() : Vehicle(4, "Auto", 10.0, 50.0) {}

    double calculateFare(double dist) override {
        if (dist < 0) throw InvalidDistanceException("Distance cannot be negative!");
        double fare = (dist <= 4) ? minimumFare : minimumFare + (dist - 4) * ratePerKm;
        return max(fare, minimumFare);
    }

    void displayDetails() const override {
        cout << "  [AUTO]       Rate: Rs.10/km | Min: Rs.50 | Slabs: 0-4(min), 4+(Rs.10)\n";
    }
};

class LuxuryCar : public Vehicle {
    string carModel;
public:
    LuxuryCar() : Vehicle(5, "Luxury Car", 25.0, 200.0), carModel("Premium Sedan") {}

    double calculateFare(double dist) override {
        if (dist < 0) throw InvalidDistanceException("Distance cannot be negative!");
        return max(minimumFare + dist * ratePerKm, minimumFare);
    }

    void displayDetails() const override {
        cout << "  [LUXURY CAR] Rate: Rs.25/km | Min: Rs.200 | Model: " << carModel << " | 1 free bag\n";
    }

    int freeBagAllowance() const override { return 1; }
};

class SUV : public Vehicle {
    int seatingCapacity;
public:
    SUV() : Vehicle(6, "SUV", 20.0, 150.0), seatingCapacity(7) {}

    double calculateFare(double dist) override {
        if (dist < 0) throw InvalidDistanceException("Distance cannot be negative!");
        double fare = (dist <= 6) ? minimumFare : minimumFare + (dist - 6) * ratePerKm;
        return max(fare, minimumFare);
    }

    void displayDetails() const override {
        cout << "  [SUV]        Rate: Rs.20/km | Min: Rs.150 | Seats: " << seatingCapacity << " | 1 free bag\n";
    }

    int freeBagAllowance() const override { return 1; }
};

class MiniVan : public Vehicle {
    int seatingCapacity;
public:
    MiniVan() : Vehicle(7, "MiniVan", 18.0, 120.0), seatingCapacity(8) {}

    double calculateFare(double dist) override {
        if (dist < 0) throw InvalidDistanceException("Distance cannot be negative!");
        double fare = (dist <= 8) ? minimumFare : minimumFare + (dist - 8) * ratePerKm;
        return max(fare, minimumFare);
    }

    void displayDetails() const override {
        cout << "  [MINIVAN]    Rate: Rs.18/km | Min: Rs.120 | Seats: " << seatingCapacity << "\n";
    }
};

class Truck : public Vehicle {
    double capacityTons;
public:
    Truck() : Vehicle(8, "Truck", 22.0, 250.0), capacityTons(1.5) {}

    double calculateFare(double dist) override {
        if (dist < 0) throw InvalidDistanceException("Distance cannot be negative!");
        double fare = minimumFare + dist * ratePerKm;
        return max(fare, minimumFare);
    }

    void displayDetails() const override {
        cout << "  [TRUCK]      Rate: Rs.22/km | Min: Rs.250 | Capacity: " << capacityTons
             << " tons | 1 free bag\n";
    }

    int freeBagAllowance() const override { return 1; }
};

class Bicycle : public Vehicle {
public:
    Bicycle() : Vehicle(9, "Bicycle", 3.0, 10.0) {}

    double calculateFare(double dist) override {
        if (dist < 0) throw InvalidDistanceException("Distance cannot be negative!");
        double fare = (dist <= 2) ? minimumFare : minimumFare + (dist - 2) * ratePerKm;
        return max(fare, minimumFare);
    }

    void displayDetails() const override {
        cout << "  [BICYCLE]    Rate: Rs.3/km  | Min: Rs.10 | Eco-friendly | No baggage allowed\n";
    }

    bool allowsBaggage() const override { return false; }
};

const int NUM_VEHICLES = 9;

class Customer {
    string name;
    string customerType;
    string couponCode;
    vector<TripRecord> tripHistory;

    static int totalCustomers;

public:
    Customer(const string& n, const string& type = "regular")
        : name(n), customerType(type), couponCode("") {
        ++totalCustomers;
        cout << "  [REGISTERED] Customer: " << name << " (" << customerType << ")\n";
    }

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
                 << " | Bag Fee: Rs." << tripHistory[i].baggageFee
                 << " | " << tripHistory[i].timestamp;
            if (tripHistory[i].rating >= 1)
                cout << " | Rating: " << tripHistory[i].rating << "/5";
            else
                cout << " | Rating: Not rated";
            cout << "\n";
        }
    }

    double totalSpent() const {
        double sum = 0;
        for (const auto& t : tripHistory) sum += t.fare;
        return sum;
    }

    Customer operator+(const Customer& other) const {
        Customer merged("Merged[" + name + "+" + other.name + "]", "regular");
        for (const auto& t : tripHistory)       merged.tripHistory.push_back(t);
        for (const auto& t : other.tripHistory) merged.tripHistory.push_back(t);
        return merged;
    }

    static int getTotalCustomers() { return totalCustomers; }
};

int Customer::totalCustomers = 0;

class FareCalculator {
    double surgeMultiplier;

    static constexpr double SMALL_BAG_FEE  = 20.0;
    static constexpr double MEDIUM_BAG_FEE = 40.0;
    static constexpr double LARGE_BAG_FEE  = 70.0;

public:
    FareCalculator() : surgeMultiplier(1.0) {}

    void setSurge(double m) {
        if (m <= 0) throw InvalidInputException("Surge multiplier must be positive!");
        surgeMultiplier = m;
        cout << "  [ADMIN] Surge pricing set to " << m << "x\n";
    }

    double getSurge() const { return surgeMultiplier; }

    double applyCharges(double baseFare, bool night, bool peak,
                        int waitMins, double luggageKg) {
        double fare = baseFare * surgeMultiplier;
        if (surgeMultiplier != 1.0)
            cout << "  [Surge " << surgeMultiplier << "x] Applied\n";

        if (night)  { fare *= 1.20; cout << "  [Night Charge +20%] Applied\n"; }
        if (peak)   { fare *= 1.15; cout << "  [Peak Hour +15%] Applied\n";    }

        if (waitMins > 0) {
            double wc = waitMins * 2.0;
            fare += wc;
            cout << "  [Waiting Rs." << fixed << setprecision(2) << wc
                 << " for " << waitMins << " min] Applied\n";
        }
        if (luggageKg > 0) {
            double lc = luggageKg * 10.0;
            fare += lc;
            cout << "  [Luggage Rs." << fixed << setprecision(2) << lc
                 << " for " << luggageKg << " kg] Applied\n";
        }
        return fare;
    }

    // Baggage check-in: per-bag fee by size, with a free-bag allowance for
    // some vehicle types (applied to the costliest bag category first).
    double applyBaggageFee(int smallBags, int mediumBags, int largeBags,
                            int freeAllowance, bool& freeApplied) {
        if (smallBags < 0 || mediumBags < 0 || largeBags < 0)
            throw InvalidBaggageException("Bag counts cannot be negative!");

        double fee = smallBags * SMALL_BAG_FEE
                   + mediumBags * MEDIUM_BAG_FEE
                   + largeBags * LARGE_BAG_FEE;

        freeApplied = false;
        int totalBags = smallBags + mediumBags + largeBags;
        if (freeAllowance > 0 && totalBags > 0) {
            if (largeBags > 0)       { fee -= LARGE_BAG_FEE;  freeApplied = true; }
            else if (mediumBags > 0) { fee -= MEDIUM_BAG_FEE; freeApplied = true; }
            else if (smallBags > 0)  { fee -= SMALL_BAG_FEE;  freeApplied = true; }
        }
        return max(fee, 0.0);
    }

    double applyDiscount(double fare, const Customer& cust) {
        double disc = 0.0;

        if      (cust.getType() == "student") { disc = 0.10; cout << "  [Student Discount 10%]\n"; }
        else if (cust.getType() == "senior")  { disc = 0.15; cout << "  [Senior Citizen 15%]\n";   }
        else if (cust.getType() == "member")  { disc = 0.20; cout << "  [Membership 20%]\n";       }

        string coupon = cust.getCoupon();
        if      (coupon == "SAVE10")  { disc += 0.10; cout << "  [Coupon SAVE10 +10%]\n";    }
        else if (coupon == "FIRST50") { disc += 0.50; cout << "  [Coupon FIRST50 +50%]\n";   }
        else if (coupon == "VIT25")   { disc += 0.25; cout << "  [Coupon VIT25 +25%]\n";     }
        else if (!coupon.empty())     { throw InvalidCouponException(); }

        disc = min(disc, 0.50);
        return fare * (1.0 - disc);
    }

    static double roundFare(double fare) { return round(fare); }
};

constexpr double FareCalculator::SMALL_BAG_FEE;
constexpr double FareCalculator::MEDIUM_BAG_FEE;
constexpr double FareCalculator::LARGE_BAG_FEE;

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
              << " | Bag Fee: Rs." << tr.baggageFee
              << " | Rating: " << (tr.rating >= 1 ? to_string(tr.rating) + "/5" : string("N/A"))
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

class ReceiptGenerator {
public:
    static void print(const string& custName, const string& vehicle,
                      double dist, double base, double baggageFee,
                      double final_fare, const string& ts) {
        cout << "\n";
        cout << "  +------------------------------------------+\n";
        cout << "  |     SMART VEHICLE FARE  -  RECEIPT        |\n";
        cout << "  +------------------------------------------+\n";
        cout << fixed << setprecision(2);
        cout << "  Customer     : " << custName   << "\n";
        cout << "  Vehicle      : " << vehicle    << "\n";
        cout << "  Distance     : " << dist       << " km\n";
        cout << "  Base Fare    : Rs." << base     << "\n";
        cout << "  Baggage Fee  : Rs." << baggageFee << "\n";
        cout << "  Final Fare   : Rs." << final_fare << "\n";
        cout << "  Date & Time  : " << ts         << "\n";
        cout << "  +------------------------------------------+\n";
        cout << "  |   Thank you for riding with SmartFare!    |\n";
        cout << "  +------------------------------------------+\n\n";
    }
};

class AdminPanel {
    FareCalculator& calc;
    vector<TripRecord>& trips;
    string password;

public:
    AdminPanel(FareCalculator& c, vector<TripRecord>& t, const string& pwd = "admin123")
        : calc(c), trips(t), password(pwd) {}

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
                 << "  6. Vehicle Ratings Summary\n"
                 << "  7. Exit Admin\n"
                 << "  Choice: ";
            if (!(cin >> ch)) { cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); ch = 0; }

            switch (ch) {
                case 1: cout << "  Total Vehicles: " << Vehicle::getTotalVehicles() << "\n"; break;
                case 2: cout << fixed << setprecision(2)
                             << "  Total Earnings: Rs." << Vehicle::getTotalEarnings() << "\n"; break;
                case 3: cout << "  Total Customers: " << Customer::getTotalCustomers() << "\n"; break;
                case 4: FileHandler::showAllTrips(); break;
                case 5: {
                    double m;
                    cout << "  Enter surge multiplier (e.g. 1.5): ";
                    if (!(cin >> m)) {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cout << "  ERROR: Invalid number entered!\n";
                        break;
                    }
                    try { calc.setSurge(m); }
                    catch (InvalidInputException& e) { cout << "  ERROR: " << e.what() << "\n"; }
                    break;
                }
                case 6: {
                    cout << "\n  ===== VEHICLE RATINGS SUMMARY =====\n";
                    static const string types[NUM_VEHICLES] = {
                        "Car", "Bus", "Bike", "Auto", "Luxury Car",
                        "SUV", "MiniVan", "Truck", "Bicycle"
                    };
                    for (const string& vt : types) {
                        double sum = 0; int count = 0;
                        for (const auto& t : trips)
                            if (t.vehicleType == vt && t.rating >= 1) { sum += t.rating; ++count; }
                        cout << "  " << left << setw(12) << vt << ": ";
                        if (count > 0)
                            cout << fixed << setprecision(2) << (sum / count)
                                 << " / 5  (" << count << " rating(s))\n";
                        else
                            cout << "No ratings yet\n";
                    }
                    break;
                }
                case 7: cout << "  Exiting admin panel.\n"; break;
                default: cout << "  Invalid choice.\n";
            }
        } while (ch != 7);
    }
};

string currentTime() {
    time_t now = time(nullptr);
    string s = ctime(&now);
    if (!s.empty() && s.back() == '\n') s.pop_back();
    return s;
}

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

int main() {
    cout << "\n  ============================================\n";
    cout << "     SMART VEHICLE FARE SYSTEM  -  VIT 2026\n";
    cout << "  ============================================\n\n";

    cout << "  Initialising vehicles...\n";
    Vehicle* fleet[NUM_VEHICLES];
    fleet[0] = new Car();
    fleet[1] = new Bus();
    fleet[2] = new Bike();
    fleet[3] = new Auto();
    fleet[4] = new LuxuryCar();
    fleet[5] = new SUV();
    fleet[6] = new MiniVan();
    fleet[7] = new Truck();
    fleet[8] = new Bicycle();
    cout << "\n";

    FareCalculator       calc;
    vector<Customer*>    customers;
    vector<TripRecord>   allTrips;
    AdminPanel           admin(calc, allTrips);
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

        case 1: {
            string name, type;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "  Enter name: ";
            getline(cin, name);
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

        case 2: {
            if (!current) { cout << "  Please register a customer first!\n"; break; }
            try {
                cout << "\n  ===== AVAILABLE VEHICLES =====\n";
                for (int i = 0; i < NUM_VEHICLES; ++i) {
                    cout << "  " << (i + 1) << ". ";
                    fleet[i]->displayDetails();
                }

                int vch = safeIntInput("  Select vehicle (1-9): ");
                if (vch < 1 || vch > NUM_VEHICLES)
                    throw InvalidInputException("Vehicle number must be between 1 and 9!");

                double dist = safeDoubleInput("  Enter distance (km): ");
                if (dist < 0)
                    throw InvalidDistanceException("Distance cannot be negative!");

                char nChar, pChar;
                cout << "  Night time ride? (y/n): "; cin >> nChar;
                cout << "  Peak hour?       (y/n): "; cin >> pChar;
                int    waitMins  = safeIntInput("  Waiting minutes (0 if none): ");
                double luggageKg = safeDoubleInput("  Luggage kg (0 if none): ");

                if (waitMins < 0)  throw InvalidInputException("Waiting time cannot be negative!");
                if (luggageKg < 0) throw InvalidInputException("Luggage weight cannot be negative!");

                Vehicle* chosen = fleet[vch - 1];

                int smallBags = 0, mediumBags = 0, largeBags = 0;
                double baggageFee = 0.0;
                bool   freeApplied = false;

                if (chosen->allowsBaggage()) {
                    smallBags  = safeIntInput("  Small bags to check-in  (0 if none): ");
                    mediumBags = safeIntInput("  Medium bags to check-in (0 if none): ");
                    largeBags  = safeIntInput("  Large bags to check-in  (0 if none): ");
                    baggageFee = calc.applyBaggageFee(smallBags, mediumBags, largeBags,
                                                       chosen->freeBagAllowance(), freeApplied);
                } else {
                    cout << "  NOTE: " << chosen->getVehicleName()
                         << " does not support baggage check-in.\n";
                }

                double baseFare = chosen->calculateFare(dist);

                cout << "\n  ===== FARE BREAKDOWN =====\n";
                cout << fixed << setprecision(2)
                     << "  Base Fare : Rs." << baseFare << "\n";

                double chargedFare = calc.applyCharges(baseFare,
                    nChar == 'y' || nChar == 'Y',
                    pChar == 'y' || pChar == 'Y',
                    waitMins, luggageKg);

                if (smallBags + mediumBags + largeBags > 0) {
                    cout << "  [Baggage Check-in] " << (smallBags + mediumBags + largeBags)
                         << " bag(s) -> Rs." << fixed << setprecision(2) << baggageFee;
                    if (freeApplied) cout << "  (1 bag free)";
                    cout << "\n";
                }
                chargedFare += baggageFee;

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
                    chosen->getVehicleName(),
                    dist, baseFare, baggageFee, finalFare, ts);

                int rating = safeIntInput("  Rate this ride (1-5, 0 to skip): ");
                if (rating < 1 || rating > 5) rating = -1;

                TripRecord tr(current->getName(),
                              chosen->getVehicleName(),
                              dist, finalFare, ts, baggageFee, rating);
                current->addTrip(tr);
                allTrips.push_back(tr);
                FileHandler::saveTrip(tr);
                Vehicle::addEarnings(finalFare);

            } catch (const InvalidDistanceException& e) {
                cout << "  [EXCEPTION] " << e.what() << "\n";
            } catch (const InvalidBaggageException& e) {
                cout << "  [EXCEPTION] " << e.what() << "\n";
            } catch (const InvalidInputException& e) {
                cout << "  [EXCEPTION] " << e.what() << "\n";
            } catch (const exception& e) {
                cout << "  [ERROR] " << e.what() << "\n";
            }
            break;
        }

        case 3: {
            cout << "\n  ===== ALL VEHICLES =====\n";
            for (int i = 0; i < NUM_VEHICLES; ++i) {
                cout << "  " << (i + 1) << ". ";
                fleet[i]->displayDetails();
            }
            break;
        }

        case 4: {
            if (!current) { cout << "  No customer registered.\n"; break; }
            current->showHistory();
            cout << fixed << setprecision(2)
                 << "  Total Spent: Rs." << current->totalSpent() << "\n";
            break;
        }

        case 5: {
            try {
                double dist = safeDoubleInput("  Distance for comparison (km): ");
                int v1 = safeIntInput("  First vehicle  (1-9): ");
                int v2 = safeIntInput("  Second vehicle (1-9): ");
                if (v1 < 1 || v1 > NUM_VEHICLES || v2 < 1 || v2 > NUM_VEHICLES)
                    throw InvalidInputException("Vehicle numbers must be 1-9!");
                compareFares(*fleet[v1 - 1], *fleet[v2 - 1], dist);
            } catch (const exception& e) {
                cout << "  [EXCEPTION] " << e.what() << "\n";
            }
            break;
        }

        case 6: {
            double threshold = safeDoubleInput("  Show trips with fare > Rs.: ");

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
            Customer merged = *customers[c1-1] + *customers[c2-1];
            merged.showHistory();
            cout << fixed << setprecision(2)
                 << "  Combined Total Spent: Rs." << merged.totalSpent() << "\n";
            break;
        }

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

    cout << "\n  ===== SESSION SUMMARY =====\n";
    cout << "  Total Vehicle Types   : " << Vehicle::getTotalVehicles() << "\n";
    cout << "  Total Customers       : " << Customer::getTotalCustomers() << "\n";
    cout << fixed << setprecision(2)
         << "  Total Earnings (Rs.)  : " << Vehicle::getTotalEarnings() << "\n";
    cout << "  ===========================\n\n";
    cout << "  All trip records have been saved to 'trip_history.txt'.\n";

    cout << "  Releasing vehicle objects...\n";
    for (int i = 0; i < NUM_VEHICLES; ++i) delete fleet[i];

    cout << "  Releasing customer objects...\n";
    for (auto* c : customers) delete c;

    return 0;
}
