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
#include <map>

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

struct TripRecord {
    string customerName;
    string vehicleType;
    double distance;
    double fare;
    string timestamp;
    bool cancelled;

    TripRecord(const string& cn, const string& vt,
               double d, double f, const string& ts, bool c = false)
        : customerName(cn), vehicleType(vt), distance(d), fare(f), timestamp(ts), cancelled(c) {}
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

    string getVehicleName()  const { return vehicleName; }
    int    getVehicleID()    const { return vehicleID;   }
    double getRatePerKm()    const { return ratePerKm;   }
    double getMinimumFare()  const { return minimumFare; }

    static int    getTotalVehicles()        { return totalVehicles; }
    static double getTotalEarnings()        { return totalEarnings; }
    static void   addEarnings(double amt)   { totalEarnings += amt; }

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
        cout << "  [LUXURY CAR] Rate: Rs.25/km | Min: Rs.200 | Model: " << carModel << "\n";
    }
};

class Truck : public Vehicle {
    double maxLoadKg;
public:
    Truck() : Vehicle(6, "Truck", 18.0, 150.0), maxLoadKg(1000.0) {}

    double calculateFare(double dist) override {
        if (dist < 0) throw InvalidDistanceException("Distance cannot be negative!");
        double fare;
        if (dist <= 8) fare = minimumFare;
        else           fare = minimumFare + (dist - 8) * ratePerKm;
        return max(fare, minimumFare);
    }

    void displayDetails() const override {
        cout << "  [TRUCK]      Rate: Rs.18/km | Min: Rs.150 | Max Load: " << maxLoadKg << " kg\n";
    }
};

class SharedCab : public Vehicle {
    int maxCoPassengers;
public:
    SharedCab() : Vehicle(7, "Shared Cab", 7.0, 45.0), maxCoPassengers(3) {}

    double calculateFare(double dist) override {
        if (dist < 0) throw InvalidDistanceException("Distance cannot be negative!");
        double fare = (dist <= 6) ? minimumFare : minimumFare + (dist - 6) * ratePerKm;
        return max(fare, minimumFare) * 0.85;
    }

    void displayDetails() const override {
        cout << "  [SHARED CAB] Rate: Rs.7/km  | Min: Rs.45 | Up to " << maxCoPassengers << " co-passengers | 15% pooled discount\n";
    }
};

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

    const vector<TripRecord>& getTripHistory() const { return tripHistory; }

    void showHistory() const {
        if (tripHistory.empty()) { cout << "  No trips yet.\n"; return; }
        cout << "\n  ===== TRIP HISTORY: " << name << " =====\n";
        for (size_t i = 0; i < tripHistory.size(); ++i) {
            cout << fixed << setprecision(2)
                 << "  Trip " << (i + 1) << " | " << tripHistory[i].vehicleType
                 << " | " << tripHistory[i].distance << " km"
                 << " | Rs." << tripHistory[i].fare
                 << " | " << tripHistory[i].timestamp
                 << (tripHistory[i].cancelled ? " | CANCELLED" : "") << "\n";
        }
    }

    double totalSpent() const {
        double sum = 0;
        for (const auto& t : tripHistory) if (!t.cancelled) sum += t.fare;
        return sum;
    }

    double averageFare() const {
        if (tripHistory.empty()) return 0.0;
        return totalSpent() / tripHistory.size();
    }

    bool cancelLastTrip() {
        for (auto it = tripHistory.rbegin(); it != tripHistory.rend(); ++it) {
            if (!it->cancelled) { it->cancelled = true; return true; }
        }
        return false;
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
              << " | Time: " << tr.timestamp
              << (tr.cancelled ? " | CANCELLED" : "") << "\n";
        }
    }

    static void showAllTrips() {
        ifstream f("trip_history.txt");
        if (!f.is_open()) { cout << "  No saved trip records found.\n"; return; }
        cout << "\n  ===== ALL SAVED TRIPS =====\n";
        string line;
        while (getline(f, line)) cout << "  " << line << "\n";
    }

    static void exportSummaryCSV(const vector<TripRecord>& trips) {
        ofstream f("trip_summary.csv");
        if (!f.is_open()) { cout << "  Could not create summary file.\n"; return; }
        f << "Customer,Vehicle,Distance_km,Fare,Timestamp,Cancelled\n";
        for (const auto& t : trips) {
            f << t.customerName << "," << t.vehicleType << "," << t.distance << ","
              << t.fare << "," << t.timestamp << "," << (t.cancelled ? "yes" : "no") << "\n";
        }
        cout << "  Summary exported to trip_summary.csv\n";
    }
};

class ReceiptGenerator {
public:
    static void print(const string& custName, const string& vehicle,
                      double dist, double base, double final_fare,
                      const string& ts) {
        cout << "\n";
        cout << "  +------------------------------------------+\n";
        cout << "  |     SMART VEHICLE FARE  -  RECEIPT        |\n";
        cout << "  +------------------------------------------+\n";
        cout << fixed << setprecision(2);
        cout << "  Customer     : " << custName   << "\n";
        cout << "  Vehicle      : " << vehicle    << "\n";
        cout << "  Distance     : " << dist       << " km\n";
        cout << "  Base Fare    : Rs." << base     << "\n";
        cout << "  Final Fare   : Rs." << final_fare << "\n";
        cout << "  Date & Time  : " << ts         << "\n";
        cout << "  +------------------------------------------+\n";
        cout << "  |   Thank you for riding with SmartFare!    |\n";
        cout << "  +------------------------------------------+\n\n";
    }
};

class AdminPanel {
    FareCalculator& calc;
    string password;

public:
    AdminPanel(FareCalculator& c, const string& pwd = "admin123")
        : calc(c), password(pwd) {}

    void run(const vector<TripRecord>& allTrips) {
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
                 << "  6. Export Trip Summary to CSV\n"
                 << "  7. Exit Admin\n"
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
                case 6: FileHandler::exportSummaryCSV(allTrips); break;
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

void searchTripsByCustomer(const vector<TripRecord>& allTrips) {
    cin.ignore();
    string query;
    cout << "  Enter customer name to search: ";
    getline(cin, query);

    string queryLower = query;
    transform(queryLower.begin(), queryLower.end(), queryLower.begin(), ::tolower);

    bool found = false;
    cout << "\n  ===== SEARCH RESULTS FOR \"" << query << "\" =====\n";
    for (const auto& tr : allTrips) {
        string nameLower = tr.customerName;
        transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);
        if (nameLower.find(queryLower) != string::npos) {
            cout << fixed << setprecision(2)
                 << "  " << tr.customerName << " | " << tr.vehicleType
                 << " | " << tr.distance << " km | Rs." << tr.fare
                 << " | " << tr.timestamp
                 << (tr.cancelled ? " | CANCELLED" : "") << "\n";
            found = true;
        }
    }
    if (!found) cout << "  No matching trips found.\n";
}

void showStatistics(const vector<TripRecord>& allTrips) {
    if (allTrips.empty()) { cout << "  No trips recorded yet.\n"; return; }

    map<string, int>    vehicleCount;
    map<string, double> vehicleEarnings;
    double totalFare = 0.0;
    int activeTrips = 0;

    for (const auto& tr : allTrips) {
        if (tr.cancelled) continue;
        vehicleCount[tr.vehicleType]++;
        vehicleEarnings[tr.vehicleType] += tr.fare;
        totalFare += tr.fare;
        activeTrips++;
    }

    cout << "\n  ===== FLEET STATISTICS =====\n";
    cout << fixed << setprecision(2);
    cout << "  Active Trips        : " << activeTrips << "\n";
    cout << "  Total Fare Collected: Rs." << totalFare << "\n";
    if (activeTrips > 0)
        cout << "  Average Fare/Trip   : Rs." << (totalFare / activeTrips) << "\n";

    string mostUsed;
    int maxCount = 0;
    for (const auto& p : vehicleCount) {
        cout << "  " << p.first << " -> " << p.second << " trips, Rs."
             << vehicleEarnings[p.first] << " earned\n";
        if (p.second > maxCount) { maxCount = p.second; mostUsed = p.first; }
    }
    if (!mostUsed.empty())
        cout << "  Most Popular Vehicle: " << mostUsed << " (" << maxCount << " trips)\n";
    cout << "  =============================\n";
}

int main() {
    cout << "\n  ============================================\n";
    cout << "     SMART VEHICLE FARE SYSTEM  -  VIT 2026\n";
    cout << "  ============================================\n\n";

    cout << "  Initialising vehicles...\n";
    vector<Vehicle*> fleet;
    fleet.push_back(new Car());
    fleet.push_back(new Bus());
    fleet.push_back(new Bike());
    fleet.push_back(new Auto());
    fleet.push_back(new LuxuryCar());
    fleet.push_back(new Truck());
    fleet.push_back(new SharedCab());
    int fleetSize = (int)fleet.size();
    cout << "\n";

    FareCalculator       calc;
    AdminPanel           admin(calc);
    vector<Customer*>    customers;
    vector<TripRecord>   allTrips;
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
        cout << "  9.  Cancel My Last Trip\n";
        cout << "  10. Search Trips by Customer Name\n";
        cout << "  11. Show Fleet Statistics\n";
        cout << "  12. Exit\n";
        cout << "  ============================\n";
        choice = safeIntInput("  Choice: ");

        switch (choice) {

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

        case 2: {
            if (!current) { cout << "  Please register a customer first!\n"; break; }
            try {
                cout << "\n  ===== AVAILABLE VEHICLES =====\n";
                for (int i = 0; i < fleetSize; ++i) {
                    cout << "  " << (i + 1) << ". ";
                    fleet[i]->displayDetails();
                }

                int vch = safeIntInput("  Select vehicle (1-" + to_string(fleetSize) + "): ");
                if (vch < 1 || vch > fleetSize)
                    throw InvalidInputException("Vehicle number is out of range!");

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

        case 3: {
            cout << "\n  ===== ALL VEHICLES =====\n";
            for (int i = 0; i < fleetSize; ++i) {
                cout << "  " << (i + 1) << ". ";
                fleet[i]->displayDetails();
            }
            break;
        }

        case 4: {
            if (!current) { cout << "  No customer registered.\n"; break; }
            current->showHistory();
            cout << fixed << setprecision(2)
                 << "  Total Spent  : Rs." << current->totalSpent() << "\n"
                 << "  Average Fare : Rs." << current->averageFare() << "\n";
            break;
        }

        case 5: {
            try {
                double dist = safeDoubleInput("  Distance for comparison (km): ");
                int v1 = safeIntInput("  First vehicle  (1-" + to_string(fleetSize) + "): ");
                int v2 = safeIntInput("  Second vehicle (1-" + to_string(fleetSize) + "): ");
                if (v1 < 1 || v1 > fleetSize || v2 < 1 || v2 > fleetSize)
                    throw InvalidInputException("Vehicle numbers are out of range!");
                compareFares(*fleet[v1 - 1], *fleet[v2 - 1], dist);
            } catch (const exception& e) {
                cout << "  [EXCEPTION] " << e.what() << "\n";
            }
            break;
        }

        case 6: {
            double threshold = safeDoubleInput("  Show trips with fare > Rs.: ");

            auto isAboveThreshold = [threshold](const TripRecord& tr) {
                return !tr.cancelled && tr.fare > threshold;
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
            admin.run(allTrips);
            break;

        case 9: {
            if (!current) { cout << "  No customer registered.\n"; break; }
            if (current->cancelLastTrip())
                cout << "  Last active trip for " << current->getName() << " has been cancelled.\n";
            else
                cout << "  No active trip to cancel.\n";
            break;
        }

        case 10:
            searchTripsByCustomer(allTrips);
            break;

        case 11:
            showStatistics(allTrips);
            break;

        case 12:
            cout << "  Goodbye! Thank you for using SmartFare.\n";
            break;

        default:
            cout << "  Invalid choice. Please enter 1-12.\n";
        }

    } while (choice != 12);

    cout << "\n  ===== SESSION SUMMARY =====\n";
    cout << "  Total Vehicle Types   : " << Vehicle::getTotalVehicles() << "\n";
    cout << "  Total Customers       : " << Customer::getTotalCustomers() << "\n";
    cout << fixed << setprecision(2)
         << "  Total Earnings (Rs.)  : " << Vehicle::getTotalEarnings() << "\n";
    cout << "  ===========================\n\n";
    cout << "  All trip records have been saved to 'trip_history.txt'.\n";

    cout << "  Releasing vehicle objects...\n";
    for (int i = 0; i < fleetSize; ++i) delete fleet[i];

    cout << "  Releasing customer objects...\n";
    for (auto* c : customers) delete c;

    return 0;
}
