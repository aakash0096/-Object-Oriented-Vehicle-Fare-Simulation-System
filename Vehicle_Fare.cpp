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
#include <sstream>
#include <map>
#include <cstdlib>
#include <queue>
#include <mutex>
#include <thread>
#include <chrono>

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

class InsufficientFundsException : public exception {
public:
    const char* what() const noexcept override {
        return "Insufficient funds in wallet!";
    }
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
    string tripID;
    string customerName;
    string vehicleType;
    double distance;
    double baseFare;
    double surgeMultiplier;
    double finalFare;
    double baggageFee;
    string paymentMethod;
    string status;
    string timestamp;
    double baggageFee;
    int    rating;
    string couponUsed;
    string driverName;
    string userID;
    int    etaMinutes;

    TripRecord(const string& id, const string& cn, const string& vt,
               double d, double bf, double sm, double ff,
               const string& ts, double bagFee = 0.0, int rt = -1,
               const string& cup = "", const string& drv = "", 
               const string& uid = "", int et = 0, const string& pay = "Cash")
        : tripID(id), customerName(cn), vehicleType(vt), distance(d), baseFare(bf),
          surgeMultiplier(sm), finalFare(ff), timestamp(ts), baggageFee(bagFee),
          rating(rt), couponUsed(cup), driverName(drv), userID(uid), etaMinutes(et), paymentMethod(pay) {}
};

class Driver {
public:
    string driverID;
    string name;
    string currentStatus;
    double rating;
    int totalTrips;
    string assignedTripID;

    Driver(string id, string n) : driverID(id), name(n), currentStatus("Available"), rating(5.0), totalTrips(0), assignedTripID("") {}

    void assignTrip(string tripID) {
        assignedTripID = tripID;
        currentStatus = "Busy";
    }

    void completeTrip() {
        assignedTripID = "";
        currentStatus = "Available";
        totalTrips++;
    }
};

class Vehicle {
protected:
    int    vehicleID;
    string vehicleName;
    double ratePerKm;
    double minimumFare;
    bool   isAC;

public:
    Vehicle(int id, const string& name, double rate, double minFare, bool ac = false)
        : vehicleID(id), vehicleName(name), ratePerKm(rate), minimumFare(minFare), isAC(ac) {}

    virtual ~Vehicle() {}

    virtual double calculateBaseFare(double distanceKm) = 0;

    virtual void displayDetails() const {
        cout << fixed << setprecision(2)
             << "  ID: " << vehicleID
             << "  Name: " << vehicleName
             << "  Rate/km: Rs." << ratePerKm
             << "  Min Fare: Rs." << minimumFare
             << "  AC: " << (isAC ? "Yes" : "No") << "\n";
    }

    virtual bool allowsBaggage() const { return true; }
    virtual int  freeBagAllowance() const { return 0; }

    string getVehicleName()  const { return vehicleName; }
    int    getVehicleID()    const { return vehicleID;   }
    double getRatePerKm()    const { return ratePerKm;   }
    double getMinimumFare()  const { return minimumFare; }
    bool   getIsAC()         const { return isAC; }
};

class StandardCar : public Vehicle {
public:
    StandardCar(int id) : Vehicle(id, "Standard Car", 15.0, 50.0, false) {}
    double calculateBaseFare(double distanceKm) override {
        if (distanceKm < 0) throw InvalidDistanceException("Distance cannot be negative");
        double fare = distanceKm * ratePerKm;
        return max(fare, minimumFare);
    }
    bool allowsBaggage() const override { return true; }
    int  freeBagAllowance() const override { return 1; }
};

class PremiumCar : public Vehicle {
public:
    PremiumCar(int id) : Vehicle(id, "Premium Car", 25.0, 100.0, true) {}
    double calculateBaseFare(double distanceKm) override {
        if (distanceKm < 0) throw InvalidDistanceException("Distance cannot be negative");
        double fare = distanceKm * ratePerKm;
        return max(fare, minimumFare);
    }
    bool allowsBaggage() const override { return true; }
    int  freeBagAllowance() const override { return 2; }
};

class AutoRickshaw : public Vehicle {
public:
    AutoRickshaw(int id) : Vehicle(id, "Auto Rickshaw", 10.0, 30.0, false) {}
    double calculateBaseFare(double distanceKm) override {
        if (distanceKm < 0) throw InvalidDistanceException("Distance cannot be negative");
        if (distanceKm <= 2.0) return minimumFare;
        return minimumFare + (distanceKm - 2.0) * 10.0;
    }
    bool allowsBaggage() const override { return true; }
    int  freeBagAllowance() const override { return 0; }
};

class Coupon {
public:
    string code;
    double discountPercent;
    bool active;

    Coupon(string c, double d) : code(c), discountPercent(d), active(true) {}
    bool isValid() const { return active; }
    double getDiscount() const { return discountPercent; }
};

class User {
public:
    string username;
    string password;
    string email;
    double walletBalance;

    User(string u, string p, string e, double bal = 0.0) : username(u), password(p), email(e), walletBalance(bal) {}

    bool addFunds(double amount) {
        walletBalance += amount;
        return true;
    }

    bool deductFunds(double amount) {
        if (amount > walletBalance) return false;
        walletBalance -= amount;
        return true;
    }
};

class TripManager {
private:
    vector<Vehicle*> vehicles;
    vector<TripRecord> tripHistory;
    map<string, Coupon> coupons;
    int nextVehicleID;
    vector<User> users;
    User* currentUser;
    vector<Driver> drivers;
    string nextTripIDPrefix;
    int tripCounter;
    double currentSurgeMultiplier;
    vector<string> activeTrips;

    string generateTripID() {
        ostringstream oss;
        oss << "TRIP" << tripCounter++;
        return oss.str();
    }

    string getCurrentTimestamp() {
        time_t now = time(0);
        char* dt = ctime(&now);
        return string(dt);
    }

    double applyCoupon(double fare, const string& couponCode) {
        if (couponCode.empty()) return fare;
        auto it = coupons.find(couponCode);
        if (it == coupons.end() || !it->second.isValid()) {
            throw InvalidCouponException();
        }
        double discount = fare * (it->second.getDiscount() / 100.0);
        return fare - discount;
    }

    double calculateBaggageFee(int totalBags, Vehicle& vehicle) {
        int free = vehicle.freeBagAllowance();
        if (totalBags <= free) return 0.0;
        return (totalBags - free) * 50.0;
    }

    string generateETA(double distanceKm) {
        // Assume average speed 40 km/h
        double hours = distanceKm / 40.0;
        int minutes = static_cast<int>(hours * 60);
        ostringstream oss;
        oss << minutes << " mins";
        return oss.str();
    }

    void saveToFile() {
        ofstream file("trip_history.txt");
        if (!file.is_open()) return;
        for (const auto& trip : tripHistory) {
            file << trip.tripID << "|"
                 << trip.customerName << "|"
                 << trip.vehicleType << "|"
                 << trip.distance << "|"
                 << trip.baseFare << "|"
                 << trip.surgeMultiplier << "|"
                 << trip.finalFare << "|"
                 << trip.timestamp << "|"
                 << trip.baggageFee << "|"
                 << trip.rating << "|"
                 << trip.couponUsed << "|"
                 << trip.driverName << "|"
                 << trip.userID << "|"
                 << trip.etaMinutes << "|"
                 << trip.paymentMethod << "|"
                 << trip.status << "\n";
        }
        file.close();
    }

    void loadFromFile() {
        ifstream file("trip_history.txt");
        if (!file.is_open()) return;
        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string token;
            vector<string> tokens;
            while (getline(ss, token, '|')) {
                tokens.push_back(token);
            }
            if (tokens.size() >= 16) {
                TripRecord record(tokens[0], tokens[1], tokens[2],
                                  stod(tokens[3]), stod(tokens[4]), stod(tokens[5]), stod(tokens[6]),
                                  tokens[7], stod(tokens[8]), stoi(tokens[9]), tokens[10], 
                                  tokens[11], tokens[12], stoi(tokens[13]), tokens[14], tokens[15]);
                tripHistory.push_back(record);
                if (tokens[15] == "Active") activeTrips.push_back(tokens[0]);
            }
        }
        file.close();
    }

public:
    TripManager() : nextVehicleID(1), currentUser(nullptr), tripCounter(1), currentSurgeMultiplier(1.0) {
        coupons["WELCOME10"] = Coupon("WELCOME10", 10.0);
        coupons["PREMIUM20"] = Coupon("PREMIUM20", 20.0);
        coupons["OFFPEAK15"] = Coupon("OFFPEAK15", 15.0);
        
        vehicles.push_back(new StandardCar(nextVehicleID++));
        vehicles.push_back(new PremiumCar(nextVehicleID++));
        vehicles.push_back(new AutoRickshaw(nextVehicleID++));

        loadFromFile();
        
        drivers.push_back(Driver("D001", "John Doe"));
        drivers.push_back(Driver("D002", "Jane Smith"));
        drivers.push_back(Driver("D003", "Mike Ross"));
    }

    ~TripManager() {
        for (auto v : vehicles) delete v;
    }

    void addUser(string username, string password, string email, double balance) {
        users.push_back(User(username, password, email, balance));
    }

    bool login(string username, string password) {
        for (auto& user : users) {
            if (user.username == username && user.password == password) {
                currentUser = &user;
                return true;
            }
        }
        return false;
    }

    void logout() {
        currentUser = nullptr;
    }

    bool isLoggedIn() const {
        return currentUser != nullptr;
    }

    void setSurgeMultiplier(double multiplier) {
        currentSurgeMultiplier = multiplier;
    }

    void bookTrip(const string& customerName, int vehicleIdx, double distanceKm, 
                  const string& couponCode, int bags, string paymentMethod) {
        if (vehicleIdx < 0 || vehicleIdx >= vehicles.size()) {
            throw InvalidInputException("Invalid vehicle index!");
        }
        if (distanceKm < 0) {
            throw InvalidDistanceException("Distance cannot be negative");
        }

        Vehicle* vehicle = vehicles[vehicleIdx];
        double baseFare = vehicle->calculateBaseFare(distanceKm);
        double surgeFare = baseFare * currentSurgeMultiplier;
        
        double fare = surgeFare;

        string couponUsed = couponCode;
        try {
            fare = applyCoupon(fare, couponCode);
        } catch (const InvalidCouponException& e) {
            couponUsed = "";
        }

        double bagFee = 0.0;
        if (bags > 0) {
            bagFee = calculateBaggageFee(bags, *vehicle);
            fare += bagFee;
        }

        string tripID = generateTripID();
        Driver* driver = nullptr;
        for (auto& d : drivers) {
            if (d.currentStatus == "Available") {
                driver = &d;
                driver->assignTrip(tripID);
                break;
            }
        }
        
        if (!driver) {
            throw InvalidInputException("No drivers available!");
        }

        double eta = 0;
        string etaStr = generateETA(distanceKm);

        TripRecord record(tripID, customerName, vehicle->getVehicleName(), distanceKm, 
                          baseFare, currentSurgeMultiplier, fare, getCurrentTimestamp(), 
                          bagFee, -1, couponUsed, driver->name, 
                          isLoggedIn() ? currentUser->username : "Guest", 
                          stoi(etaStr), paymentMethod);
        
        tripHistory.push_back(record);
        activeTrips.push_back(tripID);

        if (paymentMethod == "Wallet") {
            if (!isLoggedIn()) {
                throw InvalidInputException("Wallet payment requires login!");
            }
            if (!currentUser->deductFunds(fare)) {
                throw InsufficientFundsException();
            }
        }

        saveToFile();

        cout << "\n  ===== TRIP BOOKED SUCCESSFULLY =====\n";
        cout << "  Trip ID: " << tripID << "\n";
        cout << "  Driver:  " << driver->name << "\n";
        cout << "  Vehicle: " << vehicle->getVehicleName() << (vehicle->getIsAC() ? " (AC)" : "") << "\n";
        cout << "  Distance: " << distanceKm << " km\n";
        cout << "  Base Fare: Rs." << fixed << setprecision(2) << baseFare << "\n";
        if (currentSurgeMultiplier != 1.0) {
            cout << "  Surge Multiplier: " << currentSurgeMultiplier << "x\n";
        }
        cout << "  Total Fare: Rs." << fare << "\n";
        cout << "  ETA: " << etaStr << "\n";
        cout << "  Status: Active\n";
        cout << "====================================\n";
    }

    void cancelTrip(string tripID) {
        bool found = false;
        for (auto& trip : tripHistory) {
            if (trip.tripID == tripID && trip.status == "Active") {
                trip.status = "Cancelled";
                // Find driver and free them
                for (auto& driver : drivers) {
                    if (driver.assignedTripID == tripID) {
                        driver.completeTrip();
                        break;
                    }
                }
                // Refund if paid via wallet
                if (trip.paymentMethod == "Wallet" && isLoggedIn()) {
                    currentUser->addFunds(trip.finalFare);
                }
                found = true;
                cout << "  Trip " << tripID << " cancelled.\n";
                break;
            }
        }
        if (!found) {
            cout << "  Trip " << tripID << " not found or not active.\n";
        }
        saveToFile();
    }

    void viewHistory() const {
        if (tripHistory.empty()) {
            cout << "  No trip history.\n";
            return;
        }
        cout << "\n  ===== TRIP HISTORY =====\n";
        for (size_t i = 0; i < tripHistory.size(); ++i) {
            const auto& trip = tripHistory[i];
            cout << fixed << setprecision(2);
            cout << "  [" << i + 1 << "] ID: " << trip.tripID;
            cout << " | User: " << trip.userID;
            cout << " | Customer: " << trip.customerName;
            cout << " | Vehicle: " << trip.vehicleType;
            cout << " | Dist: " << trip.distance << " km";
            cout << " | Fare: Rs." << trip.finalFare;
            cout << " | Status: " << trip.status;
            if (trip.rating != -1) {
                cout << " | Rating: " << trip.rating << "/5";
            } else {
                cout << " | Rating: Not rated";
            }
            cout << "\n";
        }
        cout << "==========================\n";
    }

    void rateTrip(string tripID, int rating) {
        bool found = false;
        for (auto& trip : tripHistory) {
            if (trip.tripID == tripID) {
                if (trip.status == "Cancelled") {
                    cout << "  Cannot rate a cancelled trip.\n";
                    return;
                }
                trip.rating = rating;
                found = true;
                break;
            }
        }
        if (!found) {
            cout << "  Trip " << tripID << " not found.\n";
        } else {
            saveToFile();
            cout << "  Trip " << tripID << " rated " << rating << "/5\n";
        }
    }

    void displayVehicles() const {
        cout << "\n  ===== AVAILABLE VEHICLES =====\n";
        for (size_t i = 0; i < vehicles.size(); ++i) {
            cout << "  [" << i << "] ";
            vehicles[i]->displayDetails();
        }
        cout << "================================\n";
    }

    void addVehicle(int id, string name, double rate, double minFare, bool ac) {
        Vehicle* newVehicle;
        if (name == "Standard Car") newVehicle = new StandardCar(id);
        else if (name == "Premium Car") newVehicle = new PremiumCar(id);
        else if (name == "Auto Rickshaw") newVehicle = new AutoRickshaw(id);
        else {
            newVehicle = new StandardCar(id);
        }
        vehicles.push_back(newVehicle);
        nextVehicleID++;
        cout << "  Vehicle " << name << " added.\n";
    }

    void displayStats() const {
        cout << "\n  ===== SYSTEM STATS =====\n";
        cout << "  Total Vehicles: " << vehicles.size() << "\n";
        cout << "  Active Drivers: " << count_if(drivers.begin(), drivers.end(), [](const Driver& d){ return d.currentStatus == "Available"; }) << "\n";
        cout << "  Active Trips: " << activeTrips.size() << "\n";
        cout << "  Registered Users: " << users.size() << "\n";
        cout << "  Surge Multiplier: " << currentSurgeMultiplier << "x\n";
        cout << "==========================\n";
    }

    void compareTwoVehicles(double distance) {
        if (vehicles.size() < 2) return;
        cout << "\n  ===== FARE COMPARISON (" << distance << " km, Surge: " << currentSurgeMultiplier << "x) =====\n";
        for (size_t i = 0; i < vehicles.size(); ++i) {
            for (size_t j = i + 1; j < vehicles.size(); ++j) {
                double fare1 = vehicles[i]->calculateBaseFare(distance) * currentSurgeMultiplier;
                double fare2 = vehicles[j]->calculateBaseFare(distance) * currentSurgeMultiplier;
                cout << "  " << vehicles[i]->getVehicleName() << " (Rs." << fare1 << ") vs " << vehicles[j]->getVehicleName() << " (Rs." << fare2 << ")\n";
            }
        }
    }

    void exportHistory() {
        ofstream file("report.csv");
        if (!file.is_open()) return;
        file << "TripID,Customer,Vehicle,Distance,Fare,Rating,Coupon,Driver,Status\n";
        for (const auto& trip : tripHistory) {
            file << trip.tripID << "," << trip.customerName << "," << trip.vehicleType << "," 
                 << trip.distance << "," << trip.finalFare << "," << trip.rating << "," 
                 << trip.couponUsed << "," << trip.driverName << "," << trip.status << "\n";
        }
        file.close();
        cout << "  [EXPORTED] Report saved to 'report.csv'\n";
    }

    void simulateSurge() {
        cout << "  Setting surge multiplier to 2.5x\n";
        setSurgeMultiplier(2.5);
    }

    void refillWallet() {
        if (currentUser) {
            currentUser->addFunds(1000.0);
            cout << "  Wallet refilled with Rs. 1000.\n";
        }
    }
};

void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void pause() {
    cout << "\n  Press Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

int main() {
    TripManager manager;
    manager.addUser("admin", "password123", "admin@example.com", 5000.0);
    manager.addUser("user1", "user1pass", "user1@example.com", 0.0);

    int choice;

    while (true) {
        clearScreen();
        
        if (manager.isLoggedIn()) {
            cout << "\n  [LOGGED IN AS: " << manager.currentUser->username << " | Wallet: Rs." << fixed << setprecision(2) << manager.currentUser->walletBalance << "]\n";
        } else {
            cout << "\n  [NOT LOGGED IN]\n";
        }

        cout << "  ===== RIDE MANAGEMENT SYSTEM =====\n";
        cout << "  1. Login\n";
        cout << "  2. Book a Trip\n";
        cout << "  3. Cancel Trip\n";
        cout << "  4. View Trip History\n";
        cout << "  5. Rate a Trip\n";
        cout << "  6. Compare Vehicle Fares\n";
        cout << "  7. View Available Vehicles\n";
        cout << "  8. Manage Vehicles (Add)\n";
        cout << "  9. View System Stats\n";
        cout << "  10. Export Report (CSV)\n";
        cout << "  11. Simulate Surge Pricing\n";
        cout << "  12. Refill Wallet (If Logged In)\n";
        cout << "  13. Logout\n";
        cout << "  14. Exit\n";
        cout << "====================================\n";
        cout << "  Enter choice: ";
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        try {
            switch (choice) {
                case 1: {
                    string user, pass;
                    cout << "  Username: ";
                    getline(cin, user);
                    cout << "  Password: ";
                    getline(cin, pass);
                    if (!manager.login(user, pass)) {
                        cout << "  Login failed.\n";
                    }
                    break;
                }
                case 2: {
                    string name;
                    int vIdx;
                    double dist;
                    string coupon;
                    int bags;
                    string payMethod;

                    cout << "  Enter customer name: ";
                    getline(cin, name);

                    manager.displayVehicles();
                    cout << "  Select vehicle index (0-" << manager.vehicles.size() - 1 << "): ";
                    cin >> vIdx;

                    cout << "  Enter distance (km): ";
                    cin >> dist;

                    cout << "  Enter coupon code (or press Enter for none): ";
                    getline(cin, coupon);
                    coupon.erase(remove_if(coupon.begin(), coupon.end(), ::isspace), coupon.end());

                    cout << "  Enter number of baggage items: ";
                    cin >> bags;

                    cout << "  Payment Method (Cash/Wallet): ";
                    cin >> payMethod;

                    manager.bookTrip(name, vIdx, dist, coupon, bags, payMethod);
                    break;
                }
                case 3: {
                    string tripID;
                    cout << "  Enter Trip ID to cancel: ";
                    cin >> tripID;
                    manager.cancelTrip(tripID);
                    break;
                }
                case 4: {
                    manager.viewHistory();
                    break;
                }
                case 5: {
                    manager.viewHistory();
                    string tripID;
                    int rating;
                    cout << "  Enter Trip ID to rate: ";
                    cin >> tripID;
                    cout << "  Enter rating (1-5): ";
                    cin >> rating;
                    manager.rateTrip(tripID, rating);
                    break;
                }
                case 6: {
                    double dist;
                    cout << "  Enter distance for comparison (km): ";
                    cin >> dist;
                    manager.compareTwoVehicles(dist);
                    break;
                }
                case 7: {
                    manager.displayVehicles();
                    break;
                }
                case 8: {
                    string name;
                    double rate, minFare;
                    bool ac = false;
                    cout << "  Enter vehicle name: ";
                    getline(cin, name);
                    cout << "  Enter rate per km: ";
                    cin >> rate;
                    cout << "  Enter minimum fare: ";
                    cin >> minFare;
                    cout << "  Is AC? (1 for Yes, 0 for No): ";
                    cin >> ac;
                    manager.addVehicle(manager.nextVehicleID, name, rate, minFare, ac);
                    break;
                }
                case 9: {
                    manager.displayStats();
                    break;
                }
                case 10: {
                    manager.exportHistory();
                    break;
                }
                case 11: {
                    manager.simulateSurge();
                    break;
                }
                case 12: {
                    manager.refillWallet();
                    break;
                }
                case 13: {
                    manager.logout();
                    break;
                }
                case 14: {
                    cout << "  Exiting system...\n";
                    return 0;
                }
                default: {
                    cout << "  Invalid choice!\n";
                }
            }
        } catch (const exception& e) {
            cout << "  [ERROR] " << e.what() << "\n";
        } catch (...) {
            cout << "  [ERROR] Unknown error occurred!\n";
        }
        
        pause();
    }

    return 0;
} 
