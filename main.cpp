#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <conio.h>
using namespace std;

// Abstract base class representing a flight
class Flight {
protected:
    string flightNo, franchise, departure, destination;
    string departureTime, stopover, stayDuration;
    string status, delayedTime;
    int capacity, reserved;

public:
    // Constructor to initialize flight details
    Flight(string fNo, string fran, string dep, string des, string time, int cap,
           string stop, string stay, string stat, string delayed)
        : flightNo(fNo), franchise(fran), departure(dep), destination(des),
          departureTime(time), capacity(cap), stopover(stop), stayDuration(stay),
          status(stat), delayedTime(delayed), reserved(0) {}

    virtual string getFlightType() const = 0;

    // Reserves a seat and returns its code
    string reserveSeat() {
        if (reserved < capacity) {
            int s = reserved++;
            char row = 'A' + (s / 6);
            int col = (s % 6) + 1;
            return string(1, row) + to_string(col);
        }
        return "";
    }

    int getRemainingSeats() const { return capacity - reserved; }
    string getFlightNo() const { return flightNo; }
    string getDepartureTime() const { return departureTime; }

    // Displays flight information in tabular format
    virtual void display() const {
        cout << left << setw(8) << flightNo
             << "| " << setw(17) << franchise
             << "| " << setw(10) << departure << "--> " << setw(11) << destination
             << "| " << setw(7) << departureTime
             << "| " << setw(13) << getFlightType()
             << "| " << setw(10) << (getFlightType() == "Connecting" ? stopover : "-")
             << "| " << setw(6) << (getFlightType() == "Connecting" ? stayDuration : "-")
             << "| " << setw(10) << status
             << "| " << setw(8) << (status == "Delayed" ? delayedTime : "-") << endl;
    }
};

// Derived class for domestic flights
class DomesticFlight : public Flight {
public:
    DomesticFlight(string fNo, string fran, string dep, string des, string time, int cap,
                   string stop, string stay, string stat, string delayed)
        : Flight(fNo, fran, dep, des, time, cap, stop, stay, stat, delayed) {}

    string getFlightType() const override { return "Domestic"; }
};

// Derived class for international flights
class InternationalFlight : public Flight {
public:
    InternationalFlight(string fNo, string fran, string dep, string des, string time, int cap,
                        string stop, string stay, string stat, string delayed)
        : Flight(fNo, fran, dep, des, time, cap, stop, stay, stat, delayed) {}

    string getFlightType() const override {
        return stopover != "-" ? "Connecting" : "International";
    }
};

// Structure to store user information
struct User {
    string username, password, gender, contact, passport;
};

// Class that manages the reservation system
class ReservationSystem {
    vector<Flight*> flights;
    vector<User> users;

    // Method to securely input password with masking
    string inputPassword() {
        string pwd; char ch;
        while (true) {
            ch = _getch();
            if (ch == '\r') break;
            if (ch == '\b' && !pwd.empty()) {
                pwd.pop_back(); cout << "\b \b";
            } else if (pwd.size() < 16 && isprint(ch)) {
                pwd.push_back(ch); cout << "*";
            }
        }
        cout << endl;
        if (pwd.size() < 8) {
            cout << "Password must be 8-16 characters. Try again:\n";
            return inputPassword();
        }
        return pwd;
    }

    // Loads flight data from CSV
    void loadFlights() {
        ifstream in("flights.csv");
        string line;
        while (getline(in, line)) {
            string fNo, fran, dep, des, time, capStr, type, stop, stay, stat, delayed;
            stringstream ss(line);
            getline(ss, fNo, ','); getline(ss, fran, ','); getline(ss, dep, ','); getline(ss, des, ',');
            getline(ss, time, ','); getline(ss, capStr, ','); getline(ss, type, ',');
            getline(ss, stop, ','); getline(ss, stay, ','); getline(ss, stat, ','); getline(ss, delayed);
            if (capStr.empty() || !isdigit(capStr[0])) continue;
            int cap = stoi(capStr);
            if (type == "Domestic")
                flights.push_back(new DomesticFlight(fNo, fran, dep, des, time, cap, stop, stay, stat, delayed));
            else
                flights.push_back(new InternationalFlight(fNo, fran, dep, des, time, cap, stop, stay, stat, delayed));
        }
    }

    // Loads user data from CSV
    void loadUsers() {
        ifstream in("users.csv");
        string line;
        while (getline(in, line)) {
            string u, p, g, c, pp;
            stringstream ss(line);
            getline(ss, u, ','); getline(ss, p, ','); getline(ss, g, ','); getline(ss, c, ','); getline(ss, pp);
            if (!u.empty()) users.push_back({u, p, g, c, pp});
        }
    }

    // Saves a new user into the CSV file
    void saveUser(const User& u) {
        ofstream out("users.csv", ios::app);
        out << u.username << "," << u.password << "," << u.gender << "," << u.contact << "," << u.passport << "\n";
    }

    // Saves booking info into the CSV file
    void saveBooking(const string& uname, const string& fno, int nSeats, const string& passList) {
        ofstream out("bookings.csv", ios::app);
        out << uname << "," << fno << "," << nSeats << "," << passList << "\n";
    }

    // Displays available flights in formatted table
    void showFlights() {
        cout << "\n==========================================================================================================================\n";
        cout << "                                  SKY BROTHERS AIRLINE - AVAILABLE FLIGHTS\n";
        cout << "==========================================================================================================================\n";
        cout << left << setw(8) << " Flight"
             << "| " << setw(17) << "Franchise"
             << "| " << setw(25) << "From --> To"
             << "| " << setw(7) << "Time"
             << "| " << setw(13) << "Type"
             << "| " << setw(10) << "Stopover"
             << "| " << setw(6) << "Stay"
             << "| " << setw(10) << "Status"
             << "| " << setw(8) << "Delayed" << endl;
        cout << "--------|------------------|--------------------------|--------|--------------|-----------|-------|-----------|-----------\n";
        vector<int> idx(flights.size());
        iota(idx.begin(), idx.end(), 0);
        random_shuffle(idx.begin(), idx.end());
        for (int i = 0; i < min((int)idx.size(), 35); i++)
            flights[idx[i]]->display();
    }

    // Admin feature: View all bookings from bookings.csv
    void viewAllBookings() {
        ifstream in("bookings.csv");
        string line;
        cout << "\n========================= ALL BOOKINGS =========================\n";
        while (getline(in, line)) {
            stringstream ss(line);
            string uname, fno, seats, passList;
            getline(ss, uname, ','); getline(ss, fno, ','); getline(ss, seats, ','); getline(ss, passList);
            cout << "\nUsername: " << uname << ", Flight: " << fno << ", Seats: " << seats << "\n";
        }
        cout << "=================================================================\n";
    }

    // Admin feature: Cancel any user's booking
    void cancelAnyBooking() {
        cout << "Enter Username: "; string uname; cin >> uname;
        cout << "Enter Flight No to cancel: "; string fno; cin >> fno;
        ifstream in("bookings.csv");
        ofstream out("temp.csv");
        string line; bool found = false;
        while (getline(in, line)) {
            stringstream ss(line);
            string u, f, s, d;
            getline(ss, u, ','); getline(ss, f, ','); getline(ss, s, ','); getline(ss, d);
            if (u == uname && f == fno) found = true;
            else out << u << "," << f << "," << s << "," << d << "\n";
        }
        in.close(); out.close();
        remove("bookings.csv");
        rename("temp.csv", "bookings.csv");
        if (found) cout << "Booking cancelled.\n";
        else cout << "No such booking found.\n";
    }

    // Admin menu logic
    void adminMenu() {
        while (true) {
            cout << "\nAdmin Menu:\n1. View Flights\n2. View All Bookings\n3. Cancel Any Booking\n0. Logout\nChoice: ";
            int ch; cin >> ch;
            if (ch == 1) showFlights();
            else if (ch == 2) viewAllBookings();
            else if (ch == 3) cancelAnyBooking();
            else if (ch == 0) break;
            else cout << "Invalid choice.\n";
        }
    }

public:
    // Constructor loads flight and user data
    ReservationSystem() { loadFlights(); loadUsers(); }

     // Main system menu for login/signup
    void run() {
        cout << "\n===========================================================\n";
        cout << "            WELCOME TO SKY BROTHERS AIRLINE SYSTEM\n";
        cout << "===========================================================\n";
        while (true) {
            int choice;
            cout << "\nMenu:\n1. Admin\n2. User\n0. Exit\nEnter your choice: ";
            cin >> choice;
            if (choice == 0) break;

            if (choice == 1) {
                cout << "Admin Username: "; string un; cin >> un;
                cout << "Password: "; string pw = inputPassword();
                if (un == "admin" && pw == "admin123") adminMenu();
                else cout << "Invalid admin login.\n";
            }
            else if (choice == 2) {
                cout << "1. Sign Up\n2. Login\nChoice: ";
                int opt; cin >> opt;
                if (opt == 1) {
                    User u;
                    cout << "Signup\nUsername: "; cin >> u.username;
                    cout << "Password: "; u.password = inputPassword();
                    cout << "Gender: "; cin >> u.gender;
                    cout << "Contact: "; cin >> u.contact;
                    cout << "Passport: "; cin >> u.passport;
                    users.push_back(u); saveUser(u);
                    cout << "Signup successful!\n";
                } else if (opt == 2) {
                    string un, pw;
                    cout << "Login\nUsername: "; cin >> un;
                    cout << "Password: "; pw = inputPassword();
                    bool found = false;
                    for (auto& u : users)
                        if (u.username == un && u.password == pw) {
                            userMenu(u); found = true; break;
                        }
                    if (!found) cout << "Invalid credentials\n";
                }
            } else cout << "Invalid choice\n";
        }
        cout << "\nThank you for using SKY BROTHERS AIRLINE RESERVATION SYSTEM!\n";
    }

    // User menu logic
    void userMenu(User& u) {
        while (true) {
            cout << "\nUser Menu:\n1. View & Book Flights\n2. Cancel Booking\n3. View My Bookings\n0. Logout\nChoice: ";
            int c; cin >> c;
            if (c == 1) reserve(u);
            else if (c == 2) cancelBooking(u.username);
            else if (c == 3) viewUserBookings(u.username);
            else if (c == 0) break;
            else cout << "Invalid choice\n";
        }
    }

    // Booking logic for logged-in user
    void reserve(User& u) {
        showFlights();
        cout << "\nEnter Flight No: "; string fno; cin >> fno;
        Flight* f = nullptr;
        for (auto fl : flights) if (fl->getFlightNo() == fno) f = fl;
        if (!f) { cout << "Flight not found.\n"; return; }

        cout << "How many seats? "; int cnt; cin >> cnt;
        if (cnt > f->getRemainingSeats()) {
            cout << "Only " << f->getRemainingSeats() << " seats available.\n"; return;
        }

        vector<string> seats;
        string passList = "";
        for (int i = 0; i < cnt; i++) {
            cout << "\nPassenger " << i + 1 << " Name: "; string nm; cin >> ws; getline(cin, nm);
            cout << "Age: "; int age; cin >> age;
            cout << "Gender: "; string g; cin >> g;
            cout << "Passport: "; string pp; cin >> pp;
            seats.push_back(f->reserveSeat());
            passList += nm + "|" + to_string(age) + "|" + g + "|" + pp + (i + 1 < cnt ? ";" : "");
        }

        saveBooking(u.username, fno, cnt, passList);
        printTicket(u, f->getFlightNo(), f->getDepartureTime(), seats, passList);
    }

    // Cancel specific user's booking
    void cancelBooking(const string& username) {
        cout << "Enter Flight No to cancel: ";
        string fno; cin >> fno;
        ifstream in("bookings.csv");
        ofstream temp("temp.csv");
        string line; bool cancelled = false;
        while (getline(in, line)) {
            stringstream ss(line);
            string un, fn, seats, data;
            getline(ss, un, ','); getline(ss, fn, ','); getline(ss, seats, ','); getline(ss, data);
            if (un == username && fn == fno) cancelled = true;
            else temp << un << "," << fn << "," << seats << "," << data << "\n";
        }
        in.close(); temp.close();
        remove("bookings.csv");
        rename("temp.csv", "bookings.csv");
        if (cancelled) cout << "Booking cancelled successfully.\n";
        else cout << "No booking found for that flight.\n";
    }

     // View all bookings made by a specific user
    void viewUserBookings(const string& username) {
        ifstream in("bookings.csv");
        string line;
        bool found = false;
        cout << "\n========================= YOUR BOOKINGS =========================\n";
        while (getline(in, line)) {
            stringstream ss(line);
            string uname, fno, seats, passList;
            getline(ss, uname, ','); getline(ss, fno, ','); getline(ss, seats, ','); getline(ss, passList);
            if (uname == username) {
                found = true;
                cout << "\nFlight No       : " << fno << "\n";
                cout << "No. of Seats    : " << seats << "\n";
                cout << "Passengers:\n";
                stringstream passengers(passList); string entry; int i = 1;
                while (getline(passengers, entry, ';')) {
                    stringstream pss(entry);
                    string name, age, gender, passport;
                    getline(pss, name, '|'); getline(pss, age, '|'); getline(pss, gender, '|'); getline(pss, passport, '|');
                    cout << "  " << i++ << ". " << name << ", Age: " << age << ", Gender: " << gender << ", Passport: " << passport << "\n";
                }
            }
        }
        if (!found) cout << "No bookings found.\n";
        cout << "=================================================================\n";
    }

    // Print formatted ticket after booking
    void printTicket(const User& u, const string& flightNo, const string& time, const vector<string>& seats, const string& passList) {
        cout << "\n========================= TICKET RECEIPT =========================\n";
        cout << "Username         : " << u.username << "\n";
        cout << "Contact          : " << u.contact << "\n";
        cout << "Passport         : " << u.passport << "\n";
        cout << "------------------------------------------------------------\n";
        cout << "Flight No        : " << flightNo << "\n";
        cout << "Departure Time   : " << time << "\n";
        cout << "Seats            : ";
        for (int i = 0; i < seats.size(); ++i)
            cout << seats[i] << (i + 1 < seats.size() ? ", " : "\n");
        cout << "------------------------------------------------------------\n";
        cout << "Passengers:\n";
        stringstream d(passList); string p; int i = 1;
        while (getline(d, p, ';')) {
            stringstream ps(p);
            string n, a, g, pp;
            getline(ps, n, '|'); getline(ps, a, '|'); getline(ps, g, '|'); getline(ps, pp, '|');
            cout << " " << i++ << ". " << n << ", Age: " << a << ", Gender: " << g << ", Passport: " << pp << "\n";
        }
        cout << "==================================================================\n";
    }

     // Destructor to deallocate flights
    ~ReservationSystem() {
        for (auto f : flights) delete f;
    }
};

int main() {
    ReservationSystem rs;
    rs.run();
    return 0;
}
