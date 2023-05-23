/**
* @file main.cpp
* @brief Главный модуль программы для получения параметров от пользователя
* @param param переменная для работы с параметрами командной строки
* @param optarg переменная для получения парметров командной строки
*/
#include "Client.h"
using namespace std;

int main (int argc, char *argv[])
{
    Client connect;    //класс для пердачи айпи и порта в main

    //Если параметры не заданы, выводится справка
    if(argc == 1) {
        cout << "Программа клиента" << endl;
        cout << "Параметры для запуска клиента:" << endl;
        cout << "\t-h — справка" << endl;
        cout << "\t-i — адрес сервера (обязательный)" << endl;
        cout << "\t-p — порт сервера (необязательный — по умолчанию 33333)" << endl;
        cout << "\t-v — имя файла с исходными данными (обязательный)" << endl;
        cout << "\t-r — имя файла для сохранения результатов (обязательный)" << endl;
        cout << "\t-l — имя файла с ID и PASSWORD клиента (необязательный — по умолчанию 〜/.config/vclient.conf)" << endl;
        return 0;
    }

    string IPserver;		//айпи
    string PORTserver;		//порт

    //Функция getopt последовательно перебирает переданные параметры в программу
    //optarg переменная для получения парметров командной строки
    //param переменная для работы с параметрами командной строки
    int param;
    while ((param = getopt (argc,argv,"hi:p:v:r:l:"))!=-1) {
        switch(param) {
            
        //Кейс справки
        case 'h':
            cout << "Программа клиента" << endl;
            cout << "Параметры для запуска клиента:" << endl;
            cout << "\t-h — справка" << endl;
            cout << "\t-i — адрес сервера (обязательный)" << endl;
            cout << "\t-p — порт сервера (необязательный — по умолчанию 33333)" << endl;
            cout << "\t-v — имя файла с исходными данными (обязательный)" << endl;
            cout << "\t-r — имя файла для сохранения результатов (обязательный)" << endl;
            cout << "\t-l — имя файла с ID и PASSWORD клиента (необязательный — по умолчанию 〜/.config/vclient.conf)" << endl;
            return 0;

        //Кейс ip
        case 'i':
            IPserver = argv[2];
            break;

        //Кейс порта
        case 'p':
                PORTserver = string(optarg);
            break;

        //Кейс файла с векторами
        case 'v':
            connect.vectorFILE = string(optarg);
            break;

        //Кейс файла с результатов
        case 'r':
            connect.resultFILE = string(optarg);
            break;

        //Кейс файла с логином и паролем
        case 'l':
            connect.autf_file = string(optarg);
            break;

        //Кейс если параметры не заданы
        case '?':
            cout << "Параметр не задан или задан неверно" << endl;
            return 0;
        };
    };
    connect.Server(IPserver, PORTserver);
    return 0;
};
