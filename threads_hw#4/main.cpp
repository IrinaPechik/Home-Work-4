#include <iostream>
#include <vector>
#include <thread>
#include <queue>
#include <unistd.h>

#define ROOMS_NUMBER 25

using namespace std;

// Класс посетителей гостиницы;
class Visitor {
public:
    int id = 0;
    int cache = 0;
    queue<int> available_rooms;
    int room = -1;
    Visitor() = default;

    void firstCategory() {
        for (int i = 0; i < 10; i++) {
            available_rooms.push(i);
        }
    }
    void secondCategory() {
        for (int i = 0; i < 20; i++) {
            available_rooms.push(i);
        }
    }
    void thirdCategory() {
        for (int i = 0; i < 25; i++) {
            available_rooms.push(i);
        }
    }
};

// Класс номера гостиницы.
class Room {
public:
    int id = 0;
    bool isEmpty = true;
    Room() = default;
};

// мьютекс для условных переменных
pthread_mutex_t mutex;

// Список активных посетителей гостиницы (индекс = номер в отеле).
std::vector<Visitor*> active_visitors(ROOMS_NUMBER);

// Создаём вектор со всеми номерами гостиницы.
std::vector<Room> rooms(ROOMS_NUMBER);

// Вектор с доступными комнатами (буфер).
vector<Room> available_rooms(ROOMS_NUMBER);

// Функция потоков - гостей.
void* VisitorFunc(void* param) {
    Visitor* visitor = (Visitor*)param;

    // Посетитель приходит в гостиницу с некоторым бюджетом.
    std::cout << "\nVisitor#" + std::to_string(visitor->id + 1) +
                 " come to the Hotel with budget " + std::to_string(visitor->cache) +
                 " \t\t\tclock: " + std::to_string(clock());

    // Если у него нет средств ни на один номер, он покидает гостиницу.
    if (visitor->available_rooms.empty()) {
        cout << "\nFor visitor#" + to_string(visitor->id + 1) + " there are no available rooms!\n" +
                " \t\t\tclock: " + std::to_string(clock());
        cout << "\nVisitor#" +to_string(visitor->id + 1) + " went away..." + " \t\t\t\t\tclock: " + to_string(clock());
        return nullptr;
    }

    // Поиск номера для гостя.
    // Флаг для проверки того, была ли найдена комната для посетителя.
    bool isRoomHaveFound = false;
    pthread_mutex_lock(&mutex);
       for (int i = 0; i < available_rooms.size(); i++) {
           // Ищем нужный номер в отеле (из этого списка).
            int available_room = visitor->available_rooms.front();
            // Если номер не занят.
            if (active_visitors[available_room] == nullptr && rooms[available_room].isEmpty) {
                // Заселяем
                active_visitors[available_room] = visitor;
                cout << "\nThe room: " + to_string(i + 1) + " was occupied by a resident: " + to_string(active_visitors[available_room]->id + 1) +
                        " \t\tclock: " + to_string((clock())) + "\n";
                rooms[available_room].isEmpty = false;
                available_rooms[available_room].isEmpty = false;
                // Назначаем посетителю комнату.
                visitor->room = i;
                //visitor->available_rooms.pop();
                // Заканчиваем и выходим из цикла.
                isRoomHaveFound = true;
                break;
            }
            // Если комната занята, то удаляем её из списка доступных комнат.
            visitor->available_rooms.pop();
        }
       if (!isRoomHaveFound) {
           cout << "\nFor visitor#" + to_string(visitor->id + 1) + " there are no available rooms!\n" +
                   " \t\t\tclock: " + std::to_string(clock());
           cout << "\nVisitor#" + to_string(visitor->id + 1) + " went away..." + " \t\t\t\t\tclock: " + to_string(clock()) + "\n";
       }
    pthread_mutex_unlock(&mutex);
       if (isRoomHaveFound) {
           int y= rand()%4 + 1;
           sleep(y);
           std::cout << "\n Visitor: " +
                        to_string(visitor->id + 1) + " vacated the room"
                                                     " \t\t\tclock: " + std::to_string(clock());
           active_visitors[visitor->room] = nullptr;
           rooms[visitor->room].isEmpty = true;
           cout << "\nVisitor#" + to_string(visitor->id + 1) + " went away..." + " \t\t\t\t\tclock: " + to_string(clock()) + "\n";
       }
    return nullptr;
}

int main() {
    // Ввод данных с консоли - количество посетителей гостиницы.
    int number_of_visitors;
    std::cout << "Enter the number of hotel visitors (number > 0):\n";
    std::cin >> number_of_visitors;
    if (number_of_visitors < 0) {
        std::cout << "Incorrect data\n";
        return 0;
    }

    // Записываем все id номеров в вектор.
    std::vector<int> numbers(ROOMS_NUMBER);
    for (int i = 0; i < ROOMS_NUMBER; ++i) {
        numbers[i] = i;
    }

    srand(clock());
    int i;

    // Инициализация mutex.
    pthread_mutex_init(&mutex, nullptr);

    cout << "\nThe hotel is opening\n";
    cout << "\nChecking the status of the rooms...\n";
    cout << "\nAvailable rooms:\n";
    // Запуск номеров гостиницы.
    for (i = 0; i < rooms.size(); i++) {
        // Задаём id для номера.
        rooms[i].id = i;

        // Задаём пустой номер, или уже занят кем-то рандомно.
        int val = rand() % 2;
        if (val == 1) {
            rooms[i].isEmpty = false;
        } else {
            rooms[i].isEmpty = true;
        }
        if (rooms[i].isEmpty)
            cout << "Room " + to_string(rooms[i].id + 1) + ";\n";
    }

    // Запускаем гостей отеля.
    cout << "\nUnlocking the doors of the hotel. Welcome!\n";
    // Создаём вектор со всеми посетителями отеля.
    std::vector<Visitor> visitors(number_of_visitors);
    std::vector<pthread_t> visitors_threads(visitors.size());

    // Создаём гостей отеля.
    for(int i = 0; i < number_of_visitors; ++i) {
        visitors[i].id = i;
        std::cout << "Visitor#" + std::to_string(i + 1);
        std::cout << "\nEnter a guest's budget:";
        int cache;
        std::cin >> cache;
        if (cache < 0) {
            continue;
        }
        visitors[i].cache = cache;

        // По бюджету (>2000) определяем доступные пользователю номера отеля.
        if ((visitors[i].cache >= 2000) && (visitors[i].cache < 4000)) {
            visitors[i].firstCategory();
        } else if ((visitors[i].cache >= 4000) && (visitors[i].cache < 6000)) {
            visitors[i].secondCategory();
        } else if (visitors[i].cache >= 6000) {
            visitors[i].thirdCategory();
        }
    }

    // Создаём потоки.
    for (int j = 0; j < visitors.size(); ++j) {
        pthread_create(&visitors_threads[j], nullptr, VisitorFunc, &visitors[j]);
    }

    // Ждем, пока закончатся все посетители.
    for (unsigned long long visitor_threads: visitors_threads) {
        pthread_join(visitor_threads, nullptr);
    }
    return 0;
}