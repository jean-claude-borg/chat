#ifndef CHAT_USERCREATION_H
#define CHAT_USERCREATION_H
#include <string>
#include <vector>
#include <stdio.h>
void createUser(std::string name, std::string surname, std::string username, std::string password);
bool userExists(std::string username);
class User{
private:
    std::string name;
    std::string surname;
    std::string username;
    std::string password;

public:
    User(std::string name, std::string surname, std::string username, std::string password)
    {
        this->name = name;
        this->surname = surname;
        this->username = username;
        this->password = password;
    }
    std::string getUsername()
    {
        return this->username;
    };
};

std::vector<User*> userList;

void createUser(std::string name, std::string surname, std::string username, std::string password)
{
    userList.push_back(new User(name, surname, username, password));
}

bool userExists(std::string username)
{
    int numberOfUsers = userList.size();
    printf("\n Number of users %d", numberOfUsers);
    for(int i = 0; i < numberOfUsers; i++)
    {
        if(userList[i]->getUsername().compare(username) == 0)
            return true;
    }
    return false;
}
#endif //CHAT_USERCREATION_H
