/**
* @file Client.h
* @author Нафтаев Е.П.
* @date 18.02.2023
* @copyright ИБСТ ПГУ
* @brief Заголовочный файл для модуля Client
* @version 1.0
*/
#include <iostream>
#include <fstream>
#include <unistd.h> //close()
#include <arpa/inet.h>

using namespace std;

/**
* @brief Класс для подключения к серверу
* @param port порт
* @param sum сумма
* @param msg отправляемый хэш
* @param vectorFILE имя файла для считывания векторов
* @param resultFILE имя файла для записи суммы векторов
* @param autf_file имя файла, в котором храняться логины и пароли клиентов
* @param username логин клиента
* @param line строка из файла
*/

class Client
{
public:
    int Server(string IPserver, string PORTserver);
    int port; //порт
    double sum; //сумма
    string msg; //отправляемый хэш
    string vectorFILE; //файл векторов
    string resultFILE; //файл результатов
    string autf_file; //файл аунтификации
    string username; //логин
    string pswd; //пароль
    string line; //строка из файла
};

/**
* @brief Класс обработки ошибок client_error
* @details Класс выводящий сообщения об ошибках
*/

class client_error: public invalid_argument
{
public:
    explicit client_error (const string& what_arg):
        invalid_argument(what_arg) {}
    explicit client_error (const char* what_arg):
        invalid_argument(what_arg) {}
};
