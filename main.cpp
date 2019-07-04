#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <time.h>
#include <unistd.h>

using namespace std;

int tableSize = 0;
int **holes = 0;
int holePlace[2][2]={};


struct Player {
    string Fname;
    string Lname;
    string username;
    string password;
    int loseCount = 0;
    int winCount = 0;
};

Player AllPlayers[1000]={};
int numAllPlayers = 0;

void readAllPlayers(){
    FILE *fp = fopen("players.txt", "r");
    char line[256];

    while (fgets(line, sizeof(line), fp)) {
        char tempFname[20];
        char tempLname[20];
        char tempUsmame[20];
        char tempPass[20];
        int temploss;
        int tempwin;

        printf("%s", line);
        sscanf(line, "%s %s %s %s %d %d \n",tempFname, tempLname, tempPass, tempUsmame, &temploss, &tempwin);
        AllPlayers[numAllPlayers].Fname = tempFname;
        AllPlayers[numAllPlayers].Lname = tempLname;
        AllPlayers[numAllPlayers].username = tempUsmame;
        AllPlayers[numAllPlayers].password = tempPass;
        AllPlayers[numAllPlayers].loseCount = temploss;
        AllPlayers[numAllPlayers].winCount = tempwin;
        numAllPlayers++;
    }
    fclose(fp);
};

void sort(){
    for (int i=0; i<numAllPlayers; i++){
        for (int j=0; j<i; j++)
        {
            if(AllPlayers[i].winCount < AllPlayers[j].winCount)
            {
                Player temp = AllPlayers[i];
                AllPlayers[i] = AllPlayers[j];
                AllPlayers[j]=temp;
            }
        }
    }
}

void reset(){
    tableSize = 0;
    holes = 0;
}

void clearConsole(){
    cout << string( 100, '\n' );
}

int randomNumber(int from,int to){
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    srand((time_t)ts.tv_nsec);
    return rand() % (to - from) + from;
}

int dice(string playerName){
    int d =0;
    do {
        d += randomNumber(1, 7);
        cout << "dice for " << playerName << " please enter: ";
        cin.ignore();
        cout << "dicing =";
        for (int i = 0; i < 5; ++i) {
            cout.flush();
            usleep(100000 * 4);
            cout << "=";
        }
        cout << ">> " << d << "\n";
    }
    while(d%6==0);

    return d;
}

int **createHole(int tableSize) {
    if(holes == 0){
        holes = new int *[tableSize];
        for (int i = 0; i < tableSize; ++i) {
            for (int i = 0; i < tableSize; i++)
                holes[i] = new int[10];
        }

        for (int i = 0; i < 2; ++i) {
            int t1 = randomNumber(1,tableSize-1);
            int t2 = randomNumber(1,tableSize);
            holes[t1][t2] = 1;
            holePlace[i][0]= t1;
            holePlace[i][1] = t2;
        }
    }
    return holes;
}

void createTable() {
    if(tableSize == 0){
        tableSize = randomNumber(5,10);
        cout << tableSize<<endl;
    }

    int table[tableSize][tableSize];

    holes = createHole(tableSize);

    for (int i = 0; i < tableSize; ++i) {
        for (int j = 0; j < tableSize; ++j) {
            if (holes[i][j] == 1) {
                cout << "[*]";
            } else {
                cout << "[ ]";
            }

        }
        cout << "\n";
    }
}

bool findPlayerInFile(Player player){
    int ans[2]={};
    for (int i=0; i<numAllPlayers; i++)
    {
        if(player.username == AllPlayers[i].username && player.password == AllPlayers[i].password)
            return 1;
    }
    return 0;
}

int findPlayerPlace(Player player){
    int ans[2]={};
    for (int i=0; i<numAllPlayers; i++)
    {
        if(player.username == AllPlayers[i].username && player.password == AllPlayers[i].password)
            return i;
    }
    return 0;
}

bool registerPlayer(Player player){

    FILE *fp = fopen("players.txt", "a");
    int a = fprintf(fp, "%s %s %s %s %d %d \n",player.Fname.c_str(), player.Lname.c_str(), player.password.c_str(), player.username.c_str(), player.loseCount, player.winCount);
    fclose(fp);
    if(a)
        return 1;
    else
        return 0;
}


bool checkEnd(int x, int y){
    if(x == tableSize-1 && y == tableSize-1)
        return 1;
    else
        return 0;
}


bool checkHole(int x, int y){
    for (int i=0; i<2; i++)
    {
        if(x==holePlace[i][0] && y==holePlace[i][1])
            return 1;
    }
    return 0;
}

bool checkEqual(int p1x, int p1y, int p2x, int p2y)
{
    if(p1x == p2x && p1y == p2y && p1x!=0 && p1y!=0)
        return 1;
    else
        return 0;
}

void startGame(Player player1,Player player2) {
    clearConsole();
    createTable();
    int p1x=0,p1y=0,p2x=0,p2y=0;
    int turn = randomNumber(1,2);
    bool end = 0, allow1 = 0, allow2 = 0;
    while(!end){
        if(turn == 1){
            cout << "It's first player turn"<<endl;
            int d = dice(player1.username);
            if (d >= 6)
                allow1=1;
            if(allow1)
            {
                p1x += (p1y+d)/tableSize;
                p1y = (p1y+d)%tableSize;
                cout << "X1: "<<p1x<<"   "<<"Y1:"<<p1y<<endl;

                if(checkHole(p1x, p1y))
                {
                    p1x = p1x - 1;
                }
                end = checkEnd(p1x, p1y);
                if(checkEqual(p1x, p1y, p2x, p2y)){
                    p1x = 0;
                    p1y = 0;
                }
            }

            //ploting the table
            int table[tableSize][tableSize];

            for (int i = tableSize - 1; i >= 0; --i) {
                for (int j = tableSize -1; j >= 0; --j) {
                    if (holes[i][j] == 1) {
                        cout << "[*]";
                    }
                    else if(i == p1x && j == p1y && allow1){
                        cout << "[A]";
                    }
                    else if(i == p2x && j == p2y && allow2){
                        cout << "[B]";
                    }
                    else {
                        cout << "[ ]";
                    }

                }
                cout << "\n";
            }


            turn = 2;
        }

        if(turn == 2){
            cout << "It's second player turn"<<endl;
            int d = dice(player2.username);
            if (d >= 6)
                allow2=1;
            if(allow2) {
                p2x += (p2y + d) / tableSize;
                p2y = (p2y + d) % tableSize;
                cout << "X2: " << p2x << "   " << "Y2:" << p2y << endl;

                if (checkHole(p2x, p2y)) {
                    p2x = p2x - 1;
                }
                end = checkEnd(p2x, p2y);
                if(checkEqual(p1x, p1y, p2x, p2y)){
                    p2x = 0;
                    p2y = 0;

                }
            }
            //ploting the table
            int table[tableSize][tableSize];

            for (int i = tableSize - 1; i >= 0; --i) {
                for (int j = tableSize - 1; j >= 0; --j) {
                    if (holes[i][j] == 1) {
                        cout << "[*]";
                    }
                    else if(i == p2x && j == p2y && allow2){
                        cout << "[B]";
                    }
                    else if(i == p1x && j == p1y && allow1){
                        cout << "[A]";
                    }
                    else {
                        cout << "[ ]";
                    }
                }
                cout << "\n";
            }


            turn = 1;
        }
    }

    if(turn == 1){
        for(int i =0; i<numAllPlayers; i++) {
            if (AllPlayers[i].username == player2.username)
                AllPlayers[i].winCount++;
            if(AllPlayers[i].username == player1.username)
                AllPlayers[i].loseCount++;
        }
    }

    if(turn == 2){
        for(int i =0; i<numAllPlayers; i++) {
            if (AllPlayers[i].username == player1.username)
                AllPlayers[i].winCount++;
            if(AllPlayers[i].username == player2.username)
                AllPlayers[i].loseCount++;
        }
    }




    sort();

    FILE *fp = fopen("players.txt", "w");
    for (int i=0; i<numAllPlayers ; i++) {
        int a = fprintf(fp, "%s %s %s %s %d %d \n", AllPlayers[i].Fname.c_str(), AllPlayers[i].Lname.c_str(), AllPlayers[i].password.c_str(),
                        AllPlayers[i].username.c_str(), AllPlayers[i].loseCount, AllPlayers[i].winCount);

    }
    fclose(fp);

    cout << "best players :"<<endl;
    for(int i=0; i<3; i++)
        cout << AllPlayers[i].username << " "<< AllPlayers[i].winCount<<endl;
}


void getPlayers() {
    string isPlayer1Register;
    string isPlayer2Register;
    Player player1;
    Player player2;
    Player players[] = {player1,player2};
    cout << "Do you want to play?(y/n)";
    char c = getchar();
    bool prob=0;
    while (c != 'n') {
        for (int i = 1; i <= 2; ++i) {
            cout << "-------------------------Player" << i << "-------------------------\n";
            cout << "Do you have a account in game?(yes/no)" << "\n";
            cin >> isPlayer1Register;
            clearConsole();
            if (isPlayer1Register == "yes") {
                cout << "Please Enter your username? ";
                cin >> players[i - 1].username;
                clearConsole();
                cout << "Please Enter your password? ";
                cin >> players[i - 1].password;
                clearConsole();
                if (findPlayerInFile(players[i - 1])) {
                    players[i - 1] = AllPlayers[findPlayerPlace(players[i - 1])];
                    cout << "Ok, Welcome dear: " << players[i - 1].Fname << " " << players[i - 1].Lname << "\n";
                } else {
                    cout << "sorry we can't find you user.:(\n";
                    prob=1;
                    break;
                }
            } else {
                cout << "Ok, you must register in game" << "\n";
                clearConsole();
                cout << "Please Enter your fist name? ";
                cin >> players[i - 1].Fname;
                clearConsole();
                cout << "Please Enter your last name? ";
                cin >> players[i - 1].Lname;
                clearConsole();
                cout << "Please Enter your username? ";
                cin >> players[i - 1].username;
                clearConsole();
                cout << "Please Enter your password? ";
                cin >> players[i - 1].password;
                clearConsole();

                if (registerPlayer(players[i - 1])) {
                    cout << "Ok, Welcome dear: " << players[i - 1].Fname << " " << players[i - 1].Lname << "\n";
                } else {
                    cout << "your user name already has in system please change it\n";
                    prob = 1;
                    break;
                }
            }
        }



        if(!prob)
        {
            startGame(players[0], players[1]);
        }
        cout << holePlace[0][0]<<"  "<<holePlace[0][1]<<endl;
        cout << holePlace[1][0]<<"  "<<holePlace[1][1]<<endl;
        cout << "Do you want to play?(y/n)";
        prob = 0;
        getchar();
        c = getchar();
    }
}

int main() {
    readAllPlayers();
    getPlayers();


    return 0;
}
