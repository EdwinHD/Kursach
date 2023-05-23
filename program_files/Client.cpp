/**
 * @file Client.cpp
 * @brief Файл взаимодействие с сервером
 */
 
#include "md5.h"
#include "Client.h"

/**
* @brief Взаимодействие с сервером
* @param IPserver адрес сервера (IP)
* @param PORTserver порт сервера
* @throw client_error класс вызываемый при возникновении ошибки
*/

int Client::Server(string IPserver, string PORTserver)
{
    //Проверка на введенный порт, если порт не введен выводится, то вводится значение по умолчанию
    if(PORTserver == "") {
        PORTserver = "33333";
    }

    //Проверка на введенный файл аунтификанции, если файл аунтификанции не введен выводится, то вводится значение по умолчанию (user P@ssW0rd)
    if(autf_file == "") {
        autf_file = "./config/vclient.conf";
    }

    // Открытие файла для аутентификация
    ifstream fault(autf_file); //fault объект класса ifstream связан с файлом autf_file

    //2ошибки
    if(!fault.is_open()) {
        throw client_error(string("fun:Server, param:autf_file.\nОшибка отрытия файла для аутентификации!"));
    }
    if(fault.peek() == EOF) {
        fault.close();
        throw client_error(string("fun:Server, param:autf_file.\nФайл для аутентификация пуст!"));
    }

    //Считывание логина и пароля
    getline(fault, line); //Считывает строку из файла в line
    int k = line.find(" "); //Подсчет символов до пробела
    username = line.substr(0, k); //Возвращает подстроку данной строки начиная с начала строки и до кол-ва сиволов до пробела k
    pswd = line.erase(0, k+1); //Удаляет логин который считан выше виесте с пробелом и заносит остаток строки в пароль

    // Открытие файла для чтения векторов
    ifstream fvector(vectorFILE); //fault объект класса ifstream связан с файлом vectorFILE

    //Вывод ошибки с файлом
    if(!fvector.is_open()) {
        fvector.close();
        throw client_error(string("fun:Server, param:vectorFILE.\nОшибка отрытия файла с векторами!"));
    }
    if(fvector.peek() == EOF) {
        fvector.close();
        throw client_error(string("fun:Server, param:vectorFILE.\nФайл с векторами пуст!"));
    }
    // Открытие файла для записи суммы
    ofstream fresultat(resultFILE); //fresultat объект класса ofstream связан с файлом resultFILE
    
    //Ошибка
    if(!fresultat.is_open()) {
        fresultat.close();
        throw client_error(string("fun:Server, param:resultFILE.\nОшибка отрытия файла для результатов!"));
    }
    //буфер для адреса
    char buf[255];

    try {
        //буфер для адреса
        strcpy(buf,IPserver.c_str());   //Функция strcpy() используется для копирования содержимого IPserver в буфер, c_str формирует массив и возвращает указатель на него
    } catch (...) {
        throw client_error(std::string("fun:Server, param:buf.\nНе возможно получить адрес!"));
    }

    try {
        //Порт
        port = stoi(PORTserver); // stoi преобразует из строки в целочисленное
    } catch (...) {
        throw client_error(string("fun:Server, param:port.\nНе возможно получить порт!"));
    }

    // структура с адресом нашей программы (клиента)
    sockaddr_in * selfAddr = new (sockaddr_in);
    selfAddr->sin_family = AF_INET; // интернет протокол IPv4
    selfAddr->sin_port = 0;         // любой порт на усмотрение ОС
    selfAddr->sin_addr.s_addr = 0; //  адрес

    // структура с адресом "на той стороне" (сервера)
    sockaddr_in * remoteAddr = new (sockaddr_in);
    remoteAddr->sin_family = AF_INET;  // интернет протокол IPv4
    remoteAddr->sin_port = htons(port); //Порт
    remoteAddr->sin_addr.s_addr = inet_addr(buf); // адрес

    // создаём сокет
    int mySocket = socket(AF_INET, SOCK_STREAM, 0); //tcp протокол
    if(mySocket == -1) {
        close(mySocket);
        throw client_error(string("fun:Server, param:mySocket.\nОшибка создания сокета!"));
    }
    
    //связываем сокет с адрессом
    int rc = bind(mySocket,(const sockaddr *) selfAddr, sizeof(sockaddr_in));
    if (rc == -1) {
        close(mySocket);
        throw client_error(string("fun:Server, param:selfAddr.\nОшибка привязки сокета с локальным адресом!"));
    }
    
    //установливаем соединение
    rc = connect(mySocket, (const sockaddr*) remoteAddr, sizeof(sockaddr_in));
    if (rc == -1) {
        close(mySocket);
        throw client_error(string("fun:Server, param:remoteAddr.\nОшибка подключения сокета к удаленному серверу!"));
    }
    
    // буфер для передачи и приема данных
    char *buffer = new char[4096];
    strcpy(buffer,username.c_str()); //Функция strcpy() используется для копирования содержимого username в буфер, c_str формирует массив и возвращает указатель на него
    int msgLen = strlen(buffer);  //вычисляем длину строки

    // передаём сообщение из буффера
    rc = send(mySocket, buffer, msgLen, 0);
    if (rc == -1) {
        close(mySocket);
        throw client_error(string("fun:Server, param:buffer.\nОшибка оправки логина!"));
    }
    cout << "Мы отправляем логин: " << buffer << endl;

    // принимаем ответ в буффер
    rc = recv(mySocket, buffer, 4096, 0);
    if (rc == -1) {
        close(mySocket);
        throw client_error(string("fun:Server, param:buffer.\nОшибка получения ответа!"));
    }
    string s1 = string(buffer);
    buffer[rc] = '\0'; // конец принятой строки
    cout << "Мы получаем соль: " << buffer << endl; // вывод полученного сообщения от сервера

    // Вычисление хэша-кода от SALT+PASSWORD
    string hsh = s1 + pswd;
    msg = MD5_hash(hsh);

    // Отправка хэша от SALT+PASSWORD
    strcpy(buffer,msg.c_str());
    rc = send(mySocket, buffer, msg.length(), 0);
    if (rc == -1) {
        close(mySocket);
        throw client_error(string("fun:Server, param:msg.\nОшибка оправки хэша!"));
    }
    cout << "Мы отправляем хэш: " << buffer << endl;

    // Получене ответа об аутентификации
    rc = recv(mySocket, buffer, sizeof(buffer), 0);
    buffer[rc] = '\0'; // конец принятой строки
    if (rc == -1) {
        close(mySocket);
        throw client_error(string("fun:Server, param:buffer.\nОшибка получения ответа об аунтефикации!"));
    }
    cout << "Мы получаем ответ: " << buffer << endl; // вывод полученного сообщения от сервера

    // Получение количества векторов
    getline(fvector, line);
    int len = stoi(line); //из стринга в инт

    // Отправка количества векторов
    rc = send(mySocket, &len, sizeof(len), 0);
    if (rc == -1) {
        close(mySocket);
        throw client_error(string("fun:Server, param:buffer.\nОшибка оправки кол-ва векторов!"));
    }
    cout << "Мы отправляем кол-во векторов: " << len << endl; // вывод полученного сообщения от сервера
    
    //Векторы
    for(int l = 0; l < len; l++) {
        getline(fvector, line);

        // Получение длины вектора
        int r=0;
        int strl = line.length();
        for (int k=0; k<strl; k++)
            if (line[k]==' ') r++; //посик пробелов  и подсчет их
        int size = r+1; //+1 чтобы получить кол-во значений

        // Получение значений вектора
        double array[size] = {0};
        for(int j = 0; j < size; j++) {
            string a;
            int i = line.find(" "); //Подсчет символов до пробела
            a = line.substr(0, i); //Возвращает подстроку данной строки начиная с начала строки и до кол-ва сиволов до пробела k
            line = line.erase(0, i+1); //Удаляет значение которое считано выше вместе с пробелом и заносит остаток строки в line, так продолжаешь до конца строки
            array[j] = stod(a); //из стринга в дабл и предаем массивом
        }

        // Отправка размера векторов
        rc = send(mySocket, &size, sizeof(size), 0);
        if (rc == -1) {
            close(mySocket);
            throw client_error(string("fun:Server, param:buffer.\nОшибка оправки размера векторов!"));
        }
        cout << "Мы отправляем размер вектора: " << size << endl; // вывод полученного сообщения от сервера

        // Отправка векторов
        rc = send(mySocket, &array, sizeof(array), 0);
        if (rc == -1) {
            close(mySocket);
            throw client_error(string("fun:Server, param:buffer.\nОшибка оправки самих векторов!"));
        }
        cout << "Мы отправляем сам вектор: " << array << endl; // вывод полученного сообщения от сервера

        // Получене ответа в виде суммы вектора
        rc = recv(mySocket, &sum, sizeof(sum), 0);
        if (rc == -1) {
            close(mySocket);
            throw client_error(string("fun:Server, param:buffer.\nОшибка получения ответа в виде суммы!"));
        }
        cout << "Мы получаем результат вычисления: " << sum << endl; // вывод полученного сообщения от сервера
		
		FILE *y; //описываем файловую переменную
        const char *h = resultFILE.c_str();
        y=fopen(h, "r+b");
        fwrite(&sum, sizeof(double), 1, y);
		
    }

    // закрыем сокет
    close(mySocket);

    delete selfAddr;
    delete remoteAddr;
    delete[] buffer;
    return 0;
}
