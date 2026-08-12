# 🚗 Smart Vehicle Fare System

A console-based **Smart Vehicle Fare System** developed in modern C++ to simulate a real-world ride booking and fare calculation platform.

The project demonstrates major **Object-Oriented Programming (OOP)** concepts including inheritance, runtime polymorphism, abstraction, operator overloading, friend functions, static members, exception handling, lambda expressions, and file handling.

## ✨ Features

* 👤 Customer registration and customer switching
* 🚗 Multiple vehicle types:

  * Car
  * Bus
  * Bike
  * Auto
  * Luxury Car
* 💰 Vehicle-specific fare calculation
* 📈 Admin-controlled surge pricing
* 🌙 Night ride charges
* ⏰ Peak-hour charges
* 🕐 Waiting-time charges
* 🧳 Luggage charges
* 🎓 Student discount
* 👴 Senior citizen discount
* ⭐ Membership discount
* 🎟️ Coupon system
* 🧾 Automatic receipt generation
* 📜 Individual customer trip history
* 💾 Persistent trip storage using `trip_history.txt`
* ⚖️ Fare comparison between vehicles
* 🔎 Fare-based trip filtering using lambda expressions
* ➕ Customer history merging using operator overloading
* 🔐 Password-protected admin panel
* ⚠️ Custom exception handling
* 📊 Session summary with total customers, vehicles, and earnings

## 🛠️ OOP Concepts Demonstrated

| Concept              | Implementation                                      |
| -------------------- | --------------------------------------------------- |
| Abstraction          | `Vehicle` abstract base class                       |
| Inheritance          | `Car`, `Bus`, `Bike`, `Auto`, `LuxuryCar`           |
| Runtime Polymorphism | Virtual functions and base-class pointers           |
| Encapsulation        | Private/protected class data with public interfaces |
| Constructors         | Vehicle and customer initialization                 |
| Destructors          | Resource cleanup                                    |
| Static Members       | Vehicle/customer counters and total earnings        |
| Friend Function      | Vehicle fare comparison                             |
| Operator Overloading | `Customer + Customer`                               |
| Exception Handling   | Custom exceptions                                   |
| Lambda Expression    | Fare-based trip filtering                           |
| File Handling        | Saving and reading trip history                     |

## 🚘 Vehicle Fare Structure

| Vehicle       | Base/Minimum Fare | Pricing                         |
| ------------- | ----------------: | ------------------------------- |
| 🚗 Car        |               ₹80 | ₹15/km up to 15 km, then ₹12/km |
| 🚌 Bus        |               ₹30 | ₹5/km after 10 km               |
| 🏍️ Bike      |               ₹40 | ₹8/km after 3 km                |
| 🛺 Auto       |               ₹50 | ₹10/km after 4 km               |
| 🚘 Luxury Car |              ₹200 | ₹25/km                          |

Additional charges and discounts are applied after the base fare is calculated.

## 💸 Discounts & Coupons

### Customer Discounts

* **Student:** 10%
* **Senior Citizen:** 15%
* **Member:** 20%

### Coupon Codes

| Coupon    | Discount |
| --------- | -------: |
| `SAVE10`  |      10% |
| `FIRST50` |      50% |
| `VIT25`   |      25% |

The maximum combined discount is capped at **50%**.

## 📈 Additional Charges

The system supports:

* Surge multiplier configured by the admin
* **Night ride:** +20%
* **Peak hour:** +15%
* **Waiting:** ₹2/minute
* **Luggage:** ₹10/kg

The final fare is rounded to the nearest whole rupee.

## 👨‍💼 Admin Panel

The password-protected admin panel provides:

1. Total vehicles created
2. Total earnings
3. Total registered customers
4. View all saved trip records
5. Configure surge multiplier
6. Exit admin panel

**Default admin password:**

```text
admin123
```

> For demonstration purposes only. This password should be changed before using the project in a real application.

## 📋 Main Menu

```text
============================
MAIN MENU
1. Register / Switch Customer
2. Book a Ride
3. View Available Vehicles
4. My Trip History
5. Compare Two Vehicle Fares
6. Filter Trips by Fare
7. Merge Two Customers
8. Admin Panel
9. Exit
============================
```

## 🧾 Trip Records

Every completed ride stores:

* Customer name
* Vehicle type
* Distance
* Final fare
* Date and time

Trip records are automatically appended to:

```text
trip_history.txt
```

## 📁 Project Structure

```text
Smart-Vehicle-Fare-System/
│
├── Vehicle_Fare.cpp
├── trip_history.txt
└── README.md
```

`trip_history.txt` is generated automatically when trips are saved.

## ⚙️ Requirements

* C++ compiler supporting **C++11 or later**
* Terminal / Command Prompt
* Standard C++ libraries

The program uses standard libraries such as:

```text
iostream
string
vector
cmath
fstream
algorithm
iomanip
stdexcept
ctime
functional
limits
```

## ▶️ How to Run

### 1. Clone the repository

```bash
git clone https://github.com/your-username/Smart-Vehicle-Fare-System.git
cd Smart-Vehicle-Fare-System
```

### 2. Compile

Using `g++`:

```bash
g++ -std=c++11 Vehicle_Fare.cpp -o SmartFare
```

### 3. Run

Linux/macOS:

```bash
./SmartFare
```

Windows:

```bash
SmartFare.exe
```

## 🔄 System Flow

```text
Start
  ↓
Initialize Vehicle Fleet
  ↓
Register Customer
  ↓
Select Vehicle
  ↓
Enter Distance
  ↓
Calculate Base Fare
  ↓
Apply Surge / Night / Peak / Waiting / Luggage Charges
  ↓
Apply Customer Discount / Coupon
  ↓
Generate Receipt
  ↓
Save Trip Record
  ↓
Update Total Earnings
  ↓
View History / Compare / Filter / Admin
  ↓
Exit
```

## 🎯 Project Objective

The main objective of this project is to implement a realistic vehicle fare management system while demonstrating practical applications of **C++ Object-Oriented Programming concepts**.

Instead of implementing each vehicle independently, the system uses an abstract `Vehicle` class and derived vehicle classes, allowing fare calculations to be performed through runtime polymorphism.

## 🚀 Future Improvements

Possible future upgrades include:

* GUI-based interface
* Database integration
* Online payment support
* GPS and real-time distance calculation
* Driver registration and management
* Ride cancellation system
* User authentication
* Digital payment receipts
* Booking IDs and ride status tracking
* Dynamic surge pricing based on demand
* Multi-file project architecture
* REST API integration

## 📚 Academic Context

**Course:** BACSE104 — Structured and Object-Oriented Programming
**Institution:** VIT
**Semester:** Winter Semester 2025–2026
**Slot:** D1

## 👨‍💻 Author

**Arpit**

---

⭐ If you found this project interesting, consider giving the repository a star!
