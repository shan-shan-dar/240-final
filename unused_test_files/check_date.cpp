#include <iostream>
#include <ctime>
using namespace std;

int main() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    char dateStr[100];
    strftime(dateStr, sizeof(dateStr), "%Y-%m-%d", ltm);
    
    cout << "Today's date: " << dateStr << endl;
    cout << "Looking for: ../data/menus/breakfast-" << dateStr << ".json" << endl;
    
    return 0;
}
