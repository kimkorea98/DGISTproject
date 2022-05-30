#include <iostream>
#include <list>
#include <map>
#include <fstream>
#include <iterator>
#include <cstring>

using namespace std;

class Bank;
class ATM;
class Account;
//class Card;
class User;

class User {
private:
	string userName;
	list<Account*> accountList;
public:
	User(string name) { this->userName = name; }
	~User() {};
	string getUserName() { return userName; }
	void addAccountList(Account* account);
	void printAccountList();

};
class Bank {
private:
	string bankName;
	int bankCode;
	map<long long, Account*> accountMap;
	list<ATM*> atmList;

public:
	Bank() { bankName = "Default Bank"; }
	Bank(string bankName, int bkcode) { this->bankName = bankName; bankCode = bkcode; }
	~Bank() {};
	void setAccountList(map<long long, Account*> accountMap);

	int getBankCode() { return bankCode; }
	void printBankATMList();
	void addATM(ATM* atm);
	void addBalanceATM(int serial, int balance);

	Account* findAccount(long long cardnum);
	bool checkCard(long long cardnum);
	bool checkPassword(long long cardnum, int password);


};
class ATM {
protected:
	long long adminCard;
	list<int*> history;
	Bank* primaryBank;
	int serialNum;
	int atmBalance; //Only Cash
	bool authorization;
public:
	ATM() {};
	ATM(Bank* primaryBank, int serialNum)
	{
		this->primaryBank = primaryBank;
		this->serialNum = serialNum;
		this->atmBalance = 0;
		this->authorization = false;
		this->adminCard = primaryBank->getBankCode() * 1000000000000;
		primaryBank->addATM(this);
	}
	~ATM() {};
	int getSerialNum() { return serialNum; }
	int checkBalance() { return atmBalance; }
	void addBalance(int papers) { atmBalance += papers; }
	bool insert_card();
	void deposit(Account* my_acc);
	void withdrawal(Account* my_acc);
	void transfer();

	//user authorization
	virtual Bank* bringBank(long long cardnum);
	bool checkCard(long long cardnum, Bank* bankPtr);
	bool checkPassword(long long cardnum, Bank* bankPtr);
	void authorized() { this->authorization = true; }
	void deauthorized() { this->authorization = false; }
	Account* getUserAccount(long long cardnum, Bank* bankPtr);

	//admin
	bool isAdmin(long long cardnum) {
		if (cardnum == this->adminCard) {
			cout << "Yes, Master?  Press any number key to browse transaction history." << endl;
			int any_key;
			cin >> any_key;
			get_history();
			return true;
		}
		return false;
	}
	void get_history();
	void show_result(int* ptr);
};
class MB_ATM : public ATM {
protected:
	int fee = 1;
	list<Bank*> secondaryBank;
public:
	MB_ATM(Bank* primaryBank, int serialNum, list<Bank*> bankList)
	{
		this->primaryBank = primaryBank;
		this->serialNum = serialNum;
		this->atmBalance = 0;
		this->secondaryBank = bankList; this->authorization = false;
		this->adminCard = primaryBank->getBankCode() * 1000000000000;
		primaryBank->addATM(this);

	}
	list<Bank*> getBankList() { return secondaryBank; }
	Bank* bringBank(long long cardnum);
};
class BI_ATM : public ATM {
protected:
	int lan = 0;
public:
	BI_ATM(Bank* primaryBank, int serialNum) : ATM(primaryBank, serialNum) {};
	bool insert_card();
	void changeLan() { lan == 0 ? lan = 1 : lan = 0; }
	bool checkCard(long long cardnum, Bank* bankPtr);
	bool checkPassword(long long cardnum, Bank* bankPtr);
	void deposit(Account* my_acc);
	void withdrawal(Account* my_acc);
	void transfer();
	void show_result(int* ptr);

};
class BIMB_ATM : public MB_ATM {
protected:
	int lan = 0;
public:
	BIMB_ATM(Bank* primaryBank, int serialNum, list<Bank*> bankList) : MB_ATM(primaryBank, serialNum, bankList) {};
	bool insert_card();
	void changeLan() { lan == 0 ? lan = 1 : lan = 0; }
	bool checkCard(long long cardnum, Bank* bankPtr);
	bool checkPassword(long long cardnum, Bank* bankPtr);
	void deposit(Account* my_acc);
	void withdrawal(Account* my_acc);
	void transfer();
	void show_result(int* ptr);

};
class Account {
private:
	User* user;
	Bank* primaryBank;
	long long accountNum;
	int balance;
	int password;

public:
	Account(User* user, Bank* bank, int password, long long accountNum);
	~Account() {};
	string getUserName() { return user->getUserName(); };
	int getPassword() { return password; }
	long long getAccountNum() { return accountNum; }
	int getbalance() { return balance; }
	Bank* bringBank() { return primaryBank; }
	void addbalance(int fund) { balance = this->getbalance() + fund; }
};

void User::addAccountList(Account* account) {
	accountList.push_back(account);
}
void User::printAccountList() {
	cout << "Account List of the " << userName << endl;
	for (auto iter = accountList.begin(); iter != accountList.end(); iter++) {
		cout << (*iter)->getAccountNum() << endl;
	}
}


Account::Account(User* user, Bank* bank, int password, long long accnum) {
	this->user = user;
	this->primaryBank = bank;
	this->password = password;
	this->balance = 0;
	this->accountNum = accnum;
	user->addAccountList(this);
}

// bank function
Account* Bank::findAccount(long long cardnum) {
	for (auto iter = accountMap.begin(); iter != accountMap.end(); iter++) {
		if (iter->first == cardnum) {
			return (iter->second);
		}
	}
	cout << "no account" << endl;
	return nullptr;
}
bool Bank::checkCard(long long cardnum) {
	Account* accountPtr = findAccount(cardnum);
	if (accountPtr == nullptr) { return false; }
	else if (accountPtr->getAccountNum() == cardnum) {
		return true;
	}
	return false;

}
bool Bank::checkPassword(long long cardnum, int password) {
	Account* accountPtr = findAccount(cardnum);
	if (accountPtr->getPassword() == password) {
		return true;
	}
	else {
		return false;
	}
}
void Bank::setAccountList(map<long long, Account*> accountMap) {
	this->accountMap = accountMap;
}
void Bank::printBankATMList() {
	cout << "ATM List of the " << this->bankName << endl;
	for (auto iter = atmList.begin(); iter != atmList.end(); iter++) {
		cout << (*iter)->getSerialNum() << endl;
	}
}
void Bank::addATM(ATM* atm) {
	atmList.push_back(atm);
}
void Bank::addBalanceATM(int serial, int balance) {
	for (auto iter = atmList.begin(); iter != atmList.end(); iter++) {
		if ((*iter)->getSerialNum() == serial) {
			(*iter)->addBalance(balance);
		}
	}
}

//ATM member function
Bank* ATM::bringBank(long long cardnum) {
	int bankType = cardnum / 1000000000000;
	if (this->primaryBank->getBankCode() == bankType) {
		return this->primaryBank;
	}
	else {
		cout << "unavailable access." << endl;
		return nullptr;

	}
}
bool ATM::checkCard(long long cardnum, Bank* bankPtr) {

	if (bankPtr == nullptr) {
		cout << "Invalid card; ending session." << endl;
		cout << "Please take your card." << endl;
		return false;
	}
	else {

		bool check = bankPtr->checkCard(cardnum);

		if (check == true) {
			cout << "This card is valid." << endl;
			return true;
		}
		else {
			cout << "This card is invalid." << endl;
			return false;
		}
	}
}
bool ATM::checkPassword(long long cardnum, Bank* bankPtr) {//�н����尡 �´��� Ȯ��
	if (bankPtr == nullptr) {
		cout << "this atm is single bank atm" << endl;
		return false;
	}
	else {
		int count = 0;
		int password;
		while (1) {
			cout << "Press your password to process. If you want to exit the session, press 0." << endl;
			cout << "password : ";
			cin >> password;
			if (cin.fail()) {
				cin.clear();
				cin.ignore(100, '\n');
				cout << "invalid input type! " << endl;
				return false;
			}
			bool check = bankPtr->checkPassword(cardnum, password);
			if (check == true) {
				cout << "password is correct" << endl;
				authorized();
				return true;
			}
			else if (password == 0) {
				cout << "Good bye!" << endl;
				return false;
			}
			else {
				cout << "Wrong password" << endl;
				count++;
				if (count == 3) {
					cout << "Terminates the session because of 3 times password errors." << endl;
					cout << "Please take your card." << endl;
					return false;
				}
			}
		}
	}
}
Account* ATM::getUserAccount(long long cardnum, Bank* bankPtr) {
	if (this->authorization == true) {
		return bankPtr->findAccount(cardnum);
	}
	else {
		cout << "error" << endl;
		return nullptr;
	}
}
//overriding function
Bank* MB_ATM::bringBank(long long cardnum) {
	int bankType = cardnum / 1000000000000;
	for (list<Bank*>::iterator itor = secondaryBank.begin(); itor != secondaryBank.end(); itor++)
	{
		if ((*itor)->getBankCode() == bankType) {
			return *itor;
		}
	}
	if (primaryBank->getBankCode() == bankType) {
		return primaryBank;
	}
	else {
		return nullptr;
	}
}
bool BI_ATM::checkCard(long long cardnum, Bank* bankPtr) {

	if (bankPtr == nullptr) {
		lan == 0 ? cout << "Invalid card. session end." << endl : cout << "유효한 카드 아님; 세션 종료!." << endl;
		lan == 0 ? cout << "Please take your card." << endl : cout << "카드를 가져가십시오." << endl;

		return false;
	}
	else {

		bool check = bankPtr->checkCard(cardnum);

		if (check == true) {
			lan == 0 ? cout << "This card is valid" << endl : cout << "유효한 카드입니다." << endl;
			return true;
		}
		else {
			lan == 0 ? cout << "This card is invalid" << endl : cout << "유효하지 않은 카드입니다." << endl;
			return false;
		}
	}
}
bool BI_ATM::checkPassword(long long cardnum, Bank* bankPtr) {//�н����尡 �´��� Ȯ��
	if (bankPtr == nullptr) {
		lan == 0 ? cout << "single ATM." << endl : cout << "싱글 뱅크 에이티엠임." << endl;
		return false;
	}
	else {
		int count = 0;
		int password;
		while (1) {
			lan == 0 ? cout << "Press your password. If you want to exit session, press [0]" << endl : cout << "비밀번호를 입력하세요. 세션을 종료하고 싶으시면 [0]을 누르세요." << endl;
			lan == 0 ? cout << "password : " << endl : cout << "비밀번호 : " << endl;
			cin >> password;
			if (cin.fail()) {
				cin.clear();
				cin.ignore(100, '\n');
				lan == 0 ? cout << "invalid input type!" << endl : cout << "유효하지 않은 입력" << endl;
				return false;
			}
			bool check = bankPtr->checkPassword(cardnum, password);
			if (check == true) {
				lan == 0 ? cout << "correct password" << endl : cout << "비밀번호 맞음" << endl;
				authorized();
				return true;
			}
			else if (password == 0) {
				lan == 0 ? cout << "Good Bye." << endl : cout << "안녕히가세요." << endl;
				return false;
			}
			else {
				lan == 0 ? cout << "Wrong password." << endl : cout << "비밀번호 틀림." << endl;
				count++;
				if (count == 3) {
					lan == 0 ? cout << "You wrong password three times. session end." << endl : cout << "3번 비번을 틀렸으니 세션을 종료하겠소" << endl;
					lan == 0 ? cout << "Please take your card." << endl : cout << "카드를 가져가십시오." << endl;

					return false;
				}
			}
		}
	}
}
bool BIMB_ATM::checkCard(long long cardnum, Bank* bankPtr) {

	if (bankPtr == nullptr) {
		lan == 0 ? cout << "Invalid card. session end." << endl : cout << "유효한 카드 아님; 세션 종료!." << endl;
		lan == 0 ? cout << "Please take your card." << endl : cout << "카드를 가져가십시오." << endl;

		return false;
	}
	else {

		bool check = bankPtr->checkCard(cardnum);

		if (check == true) {
			lan == 0 ? cout << "This card is valid" << endl : cout << "유효한 카드입니다." << endl;
			return true;
		}
		else {
			lan == 0 ? cout << "This card is invalid" << endl : cout << "유효하지 않은 카드입니다." << endl;
			return false;
		}
	}
}
bool BIMB_ATM::checkPassword(long long cardnum, Bank* bankPtr) {//�н����尡 �´��� Ȯ��
	if (bankPtr == nullptr) {
		lan == 0 ? cout << "single ATM." << endl : cout << "싱글 뱅크 에이티엠임." << endl;
		return false;
	}
	else {
		int count = 0;
		int password;
		while (1) {
			lan == 0 ? cout << "Press your password. If you want to exit session, press [0]" << endl : cout << "비밀번호를 입력하세요. 세션을 종료하고 싶으시면 [0]을 누르세요." << endl;
			lan == 0 ? cout << "password : " << endl : cout << "비밀번호 : " << endl;
			cin >> password;
			if (cin.fail()) {
				cin.clear();
				cin.ignore(100, '\n');
				lan == 0 ? cout << "invalid input type!" << endl : cout << "유효하지 않은 입력" << endl;
				return false;
			}
			bool check = bankPtr->checkPassword(cardnum, password);
			if (check == true) {
				lan == 0 ? cout << "correct password" << endl : cout << "비밀번호 맞음" << endl;
				authorized();
				return true;
			}
			else if (password == 0) {
				lan == 0 ? cout << "Good Bye." << endl : cout << "안녕히가세요." << endl;
				return false;
			}
			else {
				lan == 0 ? cout << "Wrong password." << endl : cout << "비밀번호 틀림." << endl;
				count++;
				if (count == 3) {
					lan == 0 ? cout << "You wrong password three times. session end." << endl : cout << "3번 비번을 틀렸으니 세션을 종료하겠소" << endl;
					lan == 0 ? cout << "Please take your card." << endl : cout << "카드를 가져가십시오." << endl;
					return false;
				}
			}
		}
	}
}


void setting_print() {
	for (int i = 0; i < 100; i++) {
		cout << '-';
	}


	cout << endl; cout << "<User List>" << endl;
	cout << "DGIST(1111) password: 1111" << endl;
	cout << "Ezreal(2222)password: 1234" << endl;
	cout << endl;

	cout << "<Bank List>" << endl;
	cout << "NH(1000)" << endl;
	cout << "\t<Account List>" << endl;
	cout << "\tAccount: 1000111100017463, 1000111100028479, 1000222200030983 " << endl;
	cout << "\t<Admin Card>" << endl;
	cout << "\t1000000000000000" << endl;

	cout << "IBK(2000)" << endl;
	cout << "\t<Account List>" << endl;
	cout << "\tAccount: 2000111100014832, 2000222200029472, 2000222200033956 " << endl;
	cout << "\t<Admin Card>" << endl;
	cout << "\t2000000000000000" << endl;

	cout << endl;
	cout << "<ATM List>" << endl;
	cout << "type code : Single Bank(10), Single Bank with Bilingual(11), Multi Bank(20), Multi Bank with Bilingual(21)" << endl;
	cout << endl;

	for (int i = 0; i < 102; i++) {
		cout << '-';
	}
	cout << endl;
}
bool ATM::insert_card() {
	deauthorized();
	long long cardnum;
	for (int i = 0; i < 100; i++) { cout << "-"; }
	cout << endl << "Insert Card" << endl;
	cin >> cardnum;
	if (cin.fail()) {
		cin.clear();
		cin.ignore(100, '\n');
		cout << "invalid input type! " << endl;
		return true;
	}
	bool Master = isAdmin(cardnum);
	if (Master == true) { return true; }
	int transaction_type;
	int lang = 1;
	int ok_cancel;
	Account* my_acc;
	Bank* prt = bringBank(cardnum);

	for (int i = 0; i < 100; i++) {
		cout << '-';
	}

	cout << " " << endl;
	cout << "* * * * * * * * Welcome * * * * * * * *" << endl;
	cout << "To start, press [1]. If you want to exit session, press [2]." << endl;
	cin >> ok_cancel;
	if (cin.fail()) {
		cin.clear();
		cin.ignore(100, '\n');
		cout << "invalid input type! " << endl;
		return true;
	}
	if (ok_cancel == 2) {
		cout << "Good bye!" << endl;
		return true;
	}
	else if (ok_cancel == 1) {
		//if (Account.isadmin() == true) { 
		//    this->get_history();
		//}
		//if (cardnum = 1234567890) {
		//   this->get_history();
		//}
		if (checkCard(cardnum, prt) == true) {

			if (checkPassword(cardnum, prt) == true) {
				my_acc = bringBank(cardnum)->findAccount(cardnum);
				auto first = history.begin();
				while (1) {
					//cout << "Your account : " << my_acc << endl;
					cout << "Select your task: (1) deposit, (2) withdrawal, (3) transfer, (4) exit" << endl;
					cin >> transaction_type;
					if (cin.fail()) {
						cin.clear();
						cin.ignore(100, '\n');
						cout << "invalid input type! " << endl;
					}
					if (transaction_type == 1) { this->deposit(my_acc); }
					else if (transaction_type == 2) { this->withdrawal(my_acc); }
					else if (transaction_type == 3) { this->transfer(); }
					else if (transaction_type == 4) {
						for (auto iter = history.begin(); iter != first; iter++) {
							this->show_result(*iter);

						}
						cout << "account Balance :" << prt->findAccount(cardnum)->getbalance() << endl;
						cout << "ATM Balance :" << checkBalance() << endl;
						cout << "Good Bye!" << endl;
						return true;
					}
					else {
						cout << "This is Invalid Input. End the session." << endl;
						cout << "Please take your card." << endl;
						return true;
					}
				}
			}
		}
	}
	else {
		cout << "Invalid Card; ending session." << endl;
		cout << "Please take your card." << endl;
		return true;
	}
	cout << "Session End." << endl; //transactionreport
	cout << "Please take your card." << endl;
	return true;// t_id;
}
bool BI_ATM::insert_card() {
	deauthorized();
	lan = 0;
	long long int cardnum;
	for (int i = 0; i < 100; i++) { cout << "-"; }
	cout << endl << "Insert Card" << endl;
	cin >> cardnum;
	if (cin.fail()) {
		cin.clear();
		cin.ignore(100, '\n');
		cout << "invalid input type! " << endl;
		return true;
	}
	bool Master = isAdmin(cardnum);
	if (Master == true) { return true; }
	int transaction_type;
	int lang = 1;
	int ok_cancel;
	int l_ok_cancel;
	Account* my_acc;
	Bank* prt = bringBank(cardnum);

	for (int i = 0; i < 100; i++) {
		cout << '-';
	}

	cout << " " << endl;
	cout << "* * * * * * * * Welcome * * * * * * * *" << endl;
	cout << "To start, press [1]. If you want to exit session, press [2]. If you want to change language, press [3]" << endl;
	cin >> ok_cancel;
	if (cin.fail()) {
		cin.clear();
		cin.ignore(100, '\n');
		cout << "invalid input type! " << endl;
		return true;
	}
	if (ok_cancel == 2) {
		lan = 0;
		cout << "Good bye!" << endl;
		return true;
	}
	else if (ok_cancel == 1) {
		lan = 0;
		//if (Account.isadmin() == true) { 
		//    this->get_history();
		//}
		//if (cardnum = 1234567890) {
		//   this->get_history();
		//}
		if (checkCard(cardnum, prt) == true) {

			if (checkPassword(cardnum, prt) == true) {
				my_acc = bringBank(cardnum)->findAccount(cardnum);
				auto first = history.begin();
				while (1) {
					//cout << "Your account : " << my_acc << endl;
					cout << "Select your task: (1) deposit, (2) withdrawal, (3) transfer, (4) exit" << endl;
					cin >> transaction_type;
					if (cin.fail()) {
						cin.clear();
						cin.ignore(100, '\n');
						cout << "invalid input type! " << endl;
						return true;
					}
					if (transaction_type == 1) { this->deposit(my_acc); }
					else if (transaction_type == 2) { this->withdrawal(my_acc); }
					else if (transaction_type == 3) { this->transfer(); }
					else if (transaction_type == 4) {
						for (auto iter = history.begin(); iter != first; iter++) {
							this->show_result(*iter);
						}
						cout << "account Balance :" << prt->findAccount(cardnum)->getbalance() << endl;
						cout << "ATM Balance :" << checkBalance() << endl;
						cout << "Good Bye!" << endl;
						return true;
					}
					else {
						cout << "This is Invalid Input. End the session." << endl;
						return true;
					}
				}
			}
		}
	}
	else if (ok_cancel == 3) {
		changeLan();
		cout << "* * * * * * * * 도우너 어서오고 * * * * * * * *" << endl;
		cout << "세션을 시작하려면 [1]을 누르세요. 세션에서 나가고 싶으시면 [2]를 누르세요." << endl;
		cin >> l_ok_cancel;
		if (cin.fail()) {
			cin.clear();
			cin.ignore(100, '\n');
			cout << "유효하지 않은 입력! " << endl;
			return true;
		}
		if (l_ok_cancel == 2) {
			cout << "안녕히가세요!" << endl;
			return true;
		}
		else if (l_ok_cancel == 1) {
			if (checkCard(cardnum, prt) == true) {
				if (checkPassword(cardnum, prt) == true) {
					my_acc = bringBank(cardnum)->findAccount(cardnum);
					auto first = history.begin();
					while (1) {

						cout << "수행하실 작업을 선택하세요: (1) 입금, (2) 출금, (3) 계좌이체, (4) 나가기" << endl;
						cin >> transaction_type;
						if (cin.fail()) {
							cin.clear();
							cin.ignore(100, '\n');
							cout << "invalid input type! " << endl;
							return true;
						}
						if (transaction_type == 1) { this->deposit(my_acc); }
						else if (transaction_type == 2) { this->withdrawal(my_acc); }
						else if (transaction_type == 3) { this->transfer(); }
						else if (transaction_type == 4) {
							for (auto iter = history.begin(); iter != first; iter++) {
								this->show_result(*iter);
							}
							cout << "계좌 잔액 :" << prt->findAccount(cardnum)->getbalance() << endl;
							cout << "ATM 잔액 :" << checkBalance() << endl;
							cout << "잘가고!" << endl;
							return true;
						}
						else {
							cout << "유효한 입력이 아닙니다. 세션을 종료합니다." << endl;
							return true;
						}
					}
				}
			}
		}
		else {
			cout << "유효한 카드가 아닙니다; 세션을 종료합니다." << endl;
			return true;
		}
		cout << "거래 성공." << endl; //transactionreport
		return true;
	}
	else {
		cout << "Invalid Card; ending session. Go fuck yourself" << endl;
		cout << "Please take your card." << endl;
		return true;
	}
	lan = 0;
	cout << "Session End." << endl; //transactionreport
	cout << "Please take your card." << endl;
	return true;
}
bool BIMB_ATM::insert_card() {
	deauthorized();
	lan = 0;
	long long int cardnum;
	for (int i = 0; i < 100; i++) { cout << "-"; }
	cout << endl << "Insert Card" << endl;
	cin >> cardnum;
	if (cin.fail()) {
		cin.clear();
		cin.ignore(100, '\n');
		cout << "invalid input type! " << endl;
		return true;
	}
	bool Master = isAdmin(cardnum);
	if (Master == true) { return true; }
	int transaction_type;
	int lang = 1;
	int ok_cancel;
	int l_ok_cancel;
	Account* my_acc;
	Bank* prt = bringBank(cardnum);

	for (int i = 0; i < 100; i++) {
		cout << '-';
	}

	cout << " " << endl;
	cout << "* * * * * * * * Welcome * * * * * * * *" << endl;
	cout << "To start, press [1]. If you want to exit session, press [2]. If you want to change language, press [3]" << endl;
	cin >> ok_cancel;
	if (cin.fail()) {
		cin.clear();
		cin.ignore(100, '\n');
		cout << "invalid input type! " << endl;
		return true;
	}
	if (ok_cancel == 2) {
		lan = 0;
		cout << "Good bye!" << endl;
		return true;
	}
	else if (ok_cancel == 1) {
		lan = 0;
		//if (Account.isadmin() == true) { 
		//    this->get_history();
		//}
		//if (cardnum = 1234567890) {
		//   this->get_history();
		//}
		if (checkCard(cardnum, prt) == true) {

			if (checkPassword(cardnum, prt) == true) {
				my_acc = bringBank(cardnum)->findAccount(cardnum);
				auto first = history.begin();
				while (1) {
					//cout << "Your account : " << my_acc << endl;
					cout << "Select your task: (1) deposit, (2) withdrawal, (3) transfer, (4) exit" << endl;
					cin >> transaction_type;
					if (cin.fail()) {
						cin.clear();
						cin.ignore(100, '\n');
						cout << "invalid input type! " << endl;
						return true;
					}
					if (transaction_type == 1) { this->deposit(my_acc); }
					else if (transaction_type == 2) { this->withdrawal(my_acc); }
					else if (transaction_type == 3) { this->transfer(); }
					else if (transaction_type == 4) {
						for (auto iter = history.begin(); iter != first; iter++) {
							this->show_result(*iter);
						}
						cout << "account Balance :" << prt->findAccount(cardnum)->getbalance() << endl;
						cout << "ATM Balance :" << checkBalance() << endl;
						cout << "Good Bye!" << endl;
						return true;
					}
					else {
						cout << "This is Invalid Input. End the session." << endl;
						cout << "Please take your card." << endl;
						return true;
					}
				}
			}
		}
	}
	else if (ok_cancel == 3) {
		changeLan();
		cout << "* * * * * * * * 도우너 어서오고 * * * * * * * *" << endl;
		cout << "세션을 시작하려면 [1]을 누르세요. 세션에서 나가고 싶으시면 [2]를 누르세요." << endl;
		cin >> l_ok_cancel;
		if (cin.fail()) {
			cin.clear();
			cin.ignore(100, '\n');
			cout << "유효하지 않은 입력! " << endl;
			return true;
		}
		if (l_ok_cancel == 2) {
			cout << "안녕히가세요!" << endl;
			return true;
		}
		else if (l_ok_cancel == 1) {
			if (checkCard(cardnum, prt) == true) {
				if (checkPassword(cardnum, prt) == true) {
					my_acc = bringBank(cardnum)->findAccount(cardnum);
					auto first = history.begin();
					while (1) {
						cout << "수행하실 작업을 선택하세요: (1) 입금, (2) 출금, (3) 계좌이체, (4) 나가기" << endl;
						cin >> transaction_type;
						if (cin.fail()) {
							cin.clear();
							cin.ignore(100, '\n');
							cout << "invalid input type! " << endl;
							return true;
						}
						if (transaction_type == 1) { this->deposit(my_acc); }
						else if (transaction_type == 2) { this->withdrawal(my_acc); }
						else if (transaction_type == 3) { this->transfer(); }
						else if (transaction_type == 4) {
							for (auto iter = history.begin(); iter != first; iter++) {
								this->show_result(*iter);
							}
							cout << "계좌 잔액 :" << prt->findAccount(cardnum)->getbalance() << endl;
							cout << "ATM 잔액 :" << checkBalance() << endl;
							cout << "잘가고!" << endl;
							return true;
						}
						else {
							cout << "유효한 입력이 아닙니다. 세션을 종료합니다." << endl;
							return true;
						}
					}
				}
			}
		}
		else {
			cout << "유효한 카드가 아닙니다; 세션을 종료합니다." << endl;
			return true;
		}
		cout << "거래 성공." << endl; //transactionreport
		return true;
	}
	else {
		cout << "Invalid Card; ending session. Go fuck yourself" << endl;
		cout << "Please take your card." << endl;
		return true;
	}
	lan = 0;
	cout << "Session End." << endl; //transactionreport
	cout << "Please take your card." << endl;
	return true;
}

//korean version: show transaction result on console
//e.g. 10000 Won was deposited into account 0000-0000-0000-0000
void BI_ATM::show_result(int* ptr) {
	if (lan == 1) {
		if (ptr[2] == 1) {
			cout << (ptr[8] == 1 ? " 현금 " : " 수표 ");
			//cut the amount to 3 digits
			if (ptr[7] > 1000000) {
				cout << (ptr[7] / 1000000) << ",";
				cout.width(3);
				cout.fill('0');
				cout << ((ptr[7] % 1000000) / 1000) << ",";
			}
			else {
				cout << (ptr[7] / 1000) << ",";
			}
			cout.width(3);
			cout.fill('0');
			cout << (ptr[7] % 1000);
			cout << " \\ 이 ";
			cout.width(4);
			cout.fill('0');
			cout << (ptr[3] / 10000) << "-";
			cout.width(4);
			cout.fill('0');
			cout << (ptr[3] % 10000) << "-";
			cout.width(4);
			cout.fill('0');
			cout << (ptr[4] / 10000) << "-";
			cout.width(4);
			cout.fill('0');
			cout << (ptr[4] % 10000);
			cout << " 계좌로 입금되었습니다." << endl;
			cout << endl << endl;
		}
		else if (ptr[2] == 2) {
			//cut the amount to 3 digits
			if (ptr[7] > 1000000) {
				cout << (ptr[7] / 1000000) << ",";
				cout.width(3);
				cout.fill('0');
				cout << ((ptr[7] % 1000000) / 1000) << ",";
			}
			else {
				cout << (ptr[7] / 1000) << ",";
			}
			cout.width(3);
			cout.fill('0');
			cout << (ptr[7] % 1000);
			cout << " \\ 이  ";
			cout.width(4);
			cout.fill('0');
			cout << (ptr[3] / 10000) << "-";
			cout.width(4);
			cout.fill('0');
			cout << (ptr[3] % 10000) << "-";
			cout.width(4);
			cout.fill('0');
			cout << (ptr[4] / 10000) << "-";
			cout.width(4);
			cout.fill('0');
			cout << (ptr[4] % 10000);
			cout << " 계좌에서 인출되었습니다." << endl;
			cout << endl << endl;
		}
		else {
			//cut the amount to 3 digits
			cout << "현금 ";
			if (ptr[7] > 1000000) {
				cout << (ptr[7] / 1000000) << ",";
				cout.width(3);
				cout.fill('0');
				cout << ((ptr[7] % 1000000) / 1000) << ",";
			}
			else {
				cout << (ptr[7] / 1000) << ",";
			}
			if (ptr[8] == 1) {

				cout.width(3);
				cout.fill('0');
				cout << (ptr[7] % 1000);
				cout << " \\ 이 ";
				cout.width(4);
				cout.fill('0');
				cout << (ptr[5] / 10000) << "-";
				cout.width(4);
				cout.fill('0');
				cout << (ptr[5] % 10000) << "-";
				cout.width(4);
				cout.fill('0');
				cout << (ptr[6] / 10000) << "-";
				cout.width(4);
				cout.fill('0');
				cout << (ptr[6] % 10000);
				cout << "  계좌로 송금되었습니다.";
			}
			else {
				cout.width(3);
				cout.fill('0');
				cout << (ptr[7] % 1000);
				cout << " \\ 이 ";
				cout.width(4);
				cout.fill('0');
				cout << (ptr[3] / 10000) << "-";
				cout.width(4);
				cout.fill('0');
				cout << (ptr[3] % 10000) << "-";
				cout.width(4);
				cout.fill('0');
				cout << (ptr[4] / 10000) << "-";
				cout.width(4);
				cout.fill('0');
				cout << (ptr[4] % 10000);
				cout << "  계좌에서  ";
				cout.width(4);
				cout.fill('0');
				cout << (ptr[5] / 10000) << "-";
				cout.width(4);
				cout.fill('0');
				cout << (ptr[5] % 10000) << "-";
				cout.width(4);
				cout.fill('0');
				cout << (ptr[6] / 10000) << "-";
				cout.width(4);
				cout.fill('0');
				cout << (ptr[6] % 10000);
				cout << "  계좌로 송금되었습니다.";
				cout << endl << endl;
			}
		}
	}
	else {
		if (ptr[2] == 1) {
			cout << (ptr[8] == 1 ? " Deposit Cash" : " Deposit Check") << endl;
			//cut the amount to 3 digits
			if (ptr[7] > 1000000) {
				cout << (ptr[7] / 1000000) << ",";
				cout.width(3);
				cout.fill('0');
				cout << ((ptr[7] % 1000000) / 1000) << ",";
			}
			else {
				cout << (ptr[7] / 1000) << ",";
			}
			cout.width(3);
			cout.fill('0');
			cout << (ptr[7] % 1000);
			cout << " \\ was Deposited into Account ";
			cout.width(4);
			cout.fill('0');
			cout << (ptr[3] / 10000) << "-";
			cout.width(4);
			cout.fill('0');
			cout << (ptr[3] % 10000) << "-";
			cout.width(4);
			cout.fill('0');
			cout << (ptr[4] / 10000) << "-";
			cout.width(4);
			cout.fill('0');
			cout << (ptr[4] % 10000) << endl;
			cout << endl << endl;
		}
		else if (ptr[2] == 2) {
			//cut the amount to 3 digits
			if (ptr[7] > 1000000) {
				cout << (ptr[7] / 1000000) << ",";
				cout.width(3);
				cout.fill('0');
				cout << ((ptr[7] % 1000000) / 1000) << ",";
			}
			else {
				cout << (ptr[7] / 1000) << ",";
			}
			cout.width(3);
			cout.fill('0');
			cout << (ptr[7] % 1000);
			cout << " \\ was Withdrawn out of Account ";
			cout.width(4);
			cout.fill('0');
			cout << (ptr[3] / 10000) << "-";
			cout.width(4);
			cout.fill('0');
			cout << (ptr[3] % 10000) << "-";
			cout.width(4);
			cout.fill('0');
			cout << (ptr[4] / 10000) << "-";
			cout.width(4);
			cout.fill('0');
			cout << (ptr[4] % 10000) << endl;
			cout << endl << endl;
		}
		else {
			//cut the amount to 3 digits
			if (ptr[7] > 1000000) {
				cout << (ptr[7] / 1000000) << ",";
				cout.width(3);
				cout.fill('0');
				cout << ((ptr[7] % 1000000) / 1000) << ",";
			}
			else {
				cout << (ptr[7] / 1000) << ",";
			}
			cout.width(3);
			cout.fill('0');
			cout << (ptr[7] % 1000);
			if (ptr[8] == 1) {
				cout << " \\ was Transfered to Account ";
				cout.width(4);
				cout.fill('0');
				cout << (ptr[5] / 10000) << "-";
				cout.width(4);
				cout.fill('0');
				cout << (ptr[5] % 10000) << "-";
				cout.width(4);
				cout.fill('0');
				cout << (ptr[6] / 10000) << "-";
				cout.width(4);
				cout.fill('0');
				cout << (ptr[6] % 10000);
				cout << endl << endl;
			}
			else {
				cout << " \\ was Transfered from Account ";
				cout.width(4);
				cout.fill('0');
				cout << (ptr[3] / 10000) << "-";
				cout.width(4);
				cout.fill('0');
				cout << (ptr[3] % 10000) << "-";
				cout.width(4);
				cout.fill('0');
				cout << (ptr[4] / 10000) << "-";
				cout.width(4);
				cout.fill('0');
				cout << (ptr[4] % 10000);
				cout << "  to Account ";
				cout.width(4);
				cout.fill('0');
				cout << (ptr[5] / 10000) << "-";
				cout.width(4);
				cout.fill('0');
				cout << (ptr[5] % 10000) << "-";
				cout.width(4);
				cout.fill('0');
				cout << (ptr[6] / 10000) << "-";
				cout.width(4);
				cout.fill('0');
				cout << (ptr[6] % 10000);
				cout << endl << endl;
			}
		}
	}
}
void BIMB_ATM::show_result(int* ptr) {
	if (lan == 1) {
		if (ptr[2] == 1) {
			cout << (ptr[8] == 1 ? " 현금 " : " 수표 ");
			//cut the amount to 3 digits
			if (ptr[7] > 1000000) {
				cout << (ptr[7] / 1000000) << ",";
				cout.width(3);
				cout.fill('0');
				cout << ((ptr[7] % 1000000) / 1000) << ",";
			}
			else {
				cout << (ptr[7] / 1000) << ",";
			}
			cout.width(3);
			cout.fill('0');
			cout << (ptr[7] % 1000);
			cout << " \\ 이 ";
			cout.width(4);
			cout.fill('0');
			cout << (ptr[3] / 10000) << "-";
			cout.width(4);
			cout.fill('0');
			cout << (ptr[3] % 10000) << "-";
			cout.width(4);
			cout.fill('0');
			cout << (ptr[4] / 10000) << "-";
			cout.width(4);
			cout.fill('0');
			cout << (ptr[4] % 10000);
			cout << " 계좌로 입금되었습니다." << endl;
			cout << endl << endl;
		}
		else if (ptr[2] == 2) {
			//cut the amount to 3 digits
			if (ptr[7] > 1000000) {
				cout << (ptr[7] / 1000000) << ",";
				cout.width(3);
				cout.fill('0');
				cout << ((ptr[7] % 1000000) / 1000) << ",";
			}
			else {
				cout << (ptr[7] / 1000) << ",";
			}
			cout.width(3);
			cout.fill('0');
			cout << (ptr[7] % 1000);
			cout << " \\ 이  ";
			cout.width(4);
			cout.fill('0');
			cout << (ptr[3] / 10000) << "-";
			cout.width(4);
			cout.fill('0');
			cout << (ptr[3] % 10000) << "-";
			cout.width(4);
			cout.fill('0');
			cout << (ptr[4] / 10000) << "-";
			cout.width(4);
			cout.fill('0');
			cout << (ptr[4] % 10000);
			cout << " 계좌에서 인출되었습니다." << endl;
			cout << endl << endl;
		}
		else {
			//cut the amount to 3 digits
			cout << "현금 ";
			if (ptr[7] > 1000000) {
				cout << (ptr[7] / 1000000) << ",";
				cout.width(3);
				cout.fill('0');
				cout << ((ptr[7] % 1000000) / 1000) << ",";
			}
			else {
				cout << (ptr[7] / 1000) << ",";
			}
			if (ptr[8] == 1) {

				cout.width(3);
				cout.fill('0');
				cout << (ptr[7] % 1000);
				cout << " \\ 이 ";
				cout.width(4);
				cout.fill('0');
				cout << (ptr[5] / 10000) << "-";
				cout.width(4);
				cout.fill('0');
				cout << (ptr[5] % 10000) << "-";
				cout.width(4);
				cout.fill('0');
				cout << (ptr[6] / 10000) << "-";
				cout.width(4);
				cout.fill('0');
				cout << (ptr[6] % 10000);
				cout << "  계좌로 송금되었습니다.";
			}
			else {
				cout.width(3);
				cout.fill('0');
				cout << (ptr[7] % 1000);
				cout << " \\ 이 ";
				cout.width(4);
				cout.fill('0');
				cout << (ptr[3] / 10000) << "-";
				cout.width(4);
				cout.fill('0');
				cout << (ptr[3] % 10000) << "-";
				cout.width(4);
				cout.fill('0');
				cout << (ptr[4] / 10000) << "-";
				cout.width(4);
				cout.fill('0');
				cout << (ptr[4] % 10000);
				cout << "  계좌에서  ";
				cout.width(4);
				cout.fill('0');
				cout << (ptr[5] / 10000) << "-";
				cout.width(4);
				cout.fill('0');
				cout << (ptr[5] % 10000) << "-";
				cout.width(4);
				cout.fill('0');
				cout << (ptr[6] / 10000) << "-";
				cout.width(4);
				cout.fill('0');
				cout << (ptr[6] % 10000);
				cout << "  계좌로 송금되었습니다.";
				cout << endl << endl;
			}
		}
	}
	else {
		if (ptr[2] == 1) {
			cout << (ptr[8] == 1 ? " Deposit Cash" : " Deposit Check") << endl;
			//cut the amount to 3 digits
			if (ptr[7] > 1000000) {
				cout << (ptr[7] / 1000000) << ",";
				cout.width(3);
				cout.fill('0');
				cout << ((ptr[7] % 1000000) / 1000) << ",";
			}
			else {
				cout << (ptr[7] / 1000) << ",";
			}
			cout.width(3);
			cout.fill('0');
			cout << (ptr[7] % 1000);
			cout << " \\ was Deposited into Account ";
			cout.width(4);
			cout.fill('0');
			cout << (ptr[3] / 10000) << "-";
			cout.width(4);
			cout.fill('0');
			cout << (ptr[3] % 10000) << "-";
			cout.width(4);
			cout.fill('0');
			cout << (ptr[4] / 10000) << "-";
			cout.width(4);
			cout.fill('0');
			cout << (ptr[4] % 10000) << endl;
			cout << endl << endl;
		}
		else if (ptr[2] == 2) {
			//cut the amount to 3 digits
			if (ptr[7] > 1000000) {
				cout << (ptr[7] / 1000000) << ",";
				cout.width(3);
				cout.fill('0');
				cout << ((ptr[7] % 1000000) / 1000) << ",";
			}
			else {
				cout << (ptr[7] / 1000) << ",";
			}
			cout.width(3);
			cout.fill('0');
			cout << (ptr[7] % 1000);
			cout << " \\ was Withdrawn out of Account ";
			cout.width(4);
			cout.fill('0');
			cout << (ptr[3] / 10000) << "-";
			cout.width(4);
			cout.fill('0');
			cout << (ptr[3] % 10000) << "-";
			cout.width(4);
			cout.fill('0');
			cout << (ptr[4] / 10000) << "-";
			cout.width(4);
			cout.fill('0');
			cout << (ptr[4] % 10000) << endl;
			cout << endl << endl;
		}
		else {
			//cut the amount to 3 digits
			if (ptr[7] > 1000000) {
				cout << (ptr[7] / 1000000) << ",";
				cout.width(3);
				cout.fill('0');
				cout << ((ptr[7] % 1000000) / 1000) << ",";
			}
			else {
				cout << (ptr[7] / 1000) << ",";
			}
			cout.width(3);
			cout.fill('0');
			cout << (ptr[7] % 1000);
			if (ptr[8] == 1) {
				cout << " \\ was Transfered to Account ";
				cout.width(4);
				cout.fill('0');
				cout << (ptr[5] / 10000) << "-";
				cout.width(4);
				cout.fill('0');
				cout << (ptr[5] % 10000) << "-";
				cout.width(4);
				cout.fill('0');
				cout << (ptr[6] / 10000) << "-";
				cout.width(4);
				cout.fill('0');
				cout << (ptr[6] % 10000);
				cout << endl << endl;
			}
			else {
				cout << " \\ was Transfered from Account ";
				cout.width(4);
				cout.fill('0');
				cout << (ptr[3] / 10000) << "-";
				cout.width(4);
				cout.fill('0');
				cout << (ptr[3] % 10000) << "-";
				cout.width(4);
				cout.fill('0');
				cout << (ptr[4] / 10000) << "-";
				cout.width(4);
				cout.fill('0');
				cout << (ptr[4] % 10000);
				cout << "  to Account ";
				cout.width(4);
				cout.fill('0');
				cout << (ptr[5] / 10000) << "-";
				cout.width(4);
				cout.fill('0');
				cout << (ptr[5] % 10000) << "-";
				cout.width(4);
				cout.fill('0');
				cout << (ptr[6] / 10000) << "-";
				cout.width(4);
				cout.fill('0');
				cout << (ptr[6] % 10000);
				cout << endl << endl;
			}
		}
	}
}
//show transaction result on console
//e.g. 10000 Won was deposited into account 0000-0000-0000-0000
void ATM::show_result(int* ptr) {
	if (ptr[2] == 1) {
		cout << (ptr[8] == 1 ? " Deposit Cash" : " Deposit Check") << endl;
		//cut the amount to 3 digits
		if (ptr[7] > 1000000) {
			cout << (ptr[7] / 1000000) << ",";
			cout.width(3);
			cout.fill('0');
			cout << ((ptr[7] % 1000000) / 1000) << ",";
		}
		else {
			cout << (ptr[7] / 1000) << ",";
		}
		cout.width(3);
		cout.fill('0');
		cout << (ptr[7] % 1000);
		cout << " \\ was Deposited into Account ";
		cout.width(4);
		cout.fill('0');
		cout << (ptr[3] / 10000) << "-";
		cout.width(4);
		cout.fill('0');
		cout << (ptr[3] % 10000) << "-";
		cout.width(4);
		cout.fill('0');
		cout << (ptr[4] / 10000) << "-";
		cout.width(4);
		cout.fill('0');
		cout << (ptr[4] % 10000) << endl;
		cout << endl << endl;
	}
	else if (ptr[2] == 2) {
		//cut the amount to 3 digits
		if (ptr[7] > 1000000) {
			cout << (ptr[7] / 1000000) << ",";
			cout.width(3);
			cout.fill('0');
			cout << ((ptr[7] % 1000000) / 1000) << ",";
		}
		else {
			cout << (ptr[7] / 1000) << ",";
		}
		cout.width(3);
		cout.fill('0');
		cout << (ptr[7] % 1000);
		cout << " \\ was Withdrawn out of Account ";
		cout.width(4);
		cout.fill('0');
		cout << (ptr[3] / 10000) << "-";
		cout.width(4);
		cout.fill('0');
		cout << (ptr[3] % 10000) << "-";
		cout.width(4);
		cout.fill('0');
		cout << (ptr[4] / 10000) << "-";
		cout.width(4);
		cout.fill('0');
		cout << (ptr[4] % 10000) << endl;
		cout << endl << endl;
	}
	else {
		//cut the amount to 3 digits
		if (ptr[7] > 1000000) {
			cout << (ptr[7] / 1000000) << ",";
			cout.width(3);
			cout.fill('0');
			cout << ((ptr[7] % 1000000) / 1000) << ",";
		}
		else {
			cout << (ptr[7] / 1000) << ",";
		}
		cout.width(3);
		cout.fill('0');
		cout << (ptr[7] % 1000);
		if (ptr[8] == 1) {
			cout << " \\ was Transfered to Account ";
			cout.width(4);
			cout.fill('0');
			cout << (ptr[5] / 10000) << "-";
			cout.width(4);
			cout.fill('0');
			cout << (ptr[5] % 10000) << "-";
			cout.width(4);
			cout.fill('0');
			cout << (ptr[6] / 10000) << "-";
			cout.width(4);
			cout.fill('0');
			cout << (ptr[6] % 10000);
			cout << endl << endl;
		}
		else {
			cout << " \\ was Transfered from Account ";
			cout.width(4);
			cout.fill('0');
			cout << (ptr[3] / 10000) << "-";
			cout.width(4);
			cout.fill('0');
			cout << (ptr[3] % 10000) << "-";
			cout.width(4);
			cout.fill('0');
			cout << (ptr[4] / 10000) << "-";
			cout.width(4);
			cout.fill('0');
			cout << (ptr[4] % 10000);
			cout << "  to Account ";
			cout.width(4);
			cout.fill('0');
			cout << (ptr[5] / 10000) << "-";
			cout.width(4);
			cout.fill('0');
			cout << (ptr[5] % 10000) << "-";
			cout.width(4);
			cout.fill('0');
			cout << (ptr[6] / 10000) << "-";
			cout.width(4);
			cout.fill('0');
			cout << (ptr[6] % 10000);
			cout << endl << endl;
		}
	}
}


//Admin option
//desplay the whole transaction history on console
//and the same ones are saved in .txt file.
void ATM::get_history() {
	ofstream fout("TermProject_Admin.txt", ios_base::out);		//create a text file named "Admin_History.txt"

	list<int*>::iterator iter;

	cout << "It is Transaction Logbook of this ATM" << endl << endl << endl;	//desplay on console window
	fout << "It is Transaction Logbook of this ATM" << endl << endl << endl;	//write the same words on text file

	int flag = size(history);		//desplay chronological order

	for (iter = history.begin(); iter != history.end(); iter++) {
		cout << "(#" << flag << ")" << "Transaction Serial Number : ";
		fout << "(#" << flag << ")" << "Transaction Serial Number : ";
		for (int i = 0; i < 9; i++) {
			cout << (*iter)[i];
			fout << (*iter)[i];
		}

		//cut the card number to 4 digits
		cout << endl << "Inserted Card number : ";
		fout << endl << "Inserted Card number : ";
		cout.width(4);
		cout.fill('0');
		fout.width(4);
		fout.fill('0');
		cout << ((*iter)[0] / 10000) << "-";
		fout << ((*iter)[0] / 10000) << "-";
		cout.width(4);
		cout.fill('0');
		fout.width(4);
		fout.fill('0');
		cout << ((*iter)[0] % 10000) << "-";
		fout << ((*iter)[0] % 10000) << "-";
		cout.width(4);
		cout.fill('0');
		fout.width(4);
		fout.fill('0');
		cout << ((*iter)[1] / 10000) << "-";
		fout << ((*iter)[1] / 10000) << "-";
		cout.width(4);
		cout.fill('0');
		fout.width(4);
		fout.fill('0');
		cout << ((*iter)[1] % 10000) << endl;
		fout << ((*iter)[1] % 10000) << endl;

		//deposit history
		if ((*iter)[2] == 1) {
			cout << "Transaction Type :";
			fout << "Transaction Type :";
			cout << ((*iter)[8] == 1 ? " Deposit Cash" : " Deposit Check") << endl;
			fout << ((*iter)[8] == 1 ? " Deposit Cash" : " Deposit Check") << endl;
			//cut the amount to 3 digits
			if ((*iter)[7] > 1000000) {
				cout << ((*iter)[7] / 1000000) << ",";
				fout << ((*iter)[7] / 1000000) << ",";
				cout.width(3);
				cout.fill('0');
				fout.width(3);
				fout.fill('0');
				cout << (((*iter)[7] % 1000000) / 1000) << ",";
				fout << (((*iter)[7] % 1000000) / 1000) << ",";
			}
			else {
				cout << ((*iter)[7] / 1000) << ",";
				fout << ((*iter)[7] / 1000) << ",";
			}
			cout.width(3);
			cout.fill('0');
			fout.width(3);
			fout.fill('0');
			cout << ((*iter)[7] % 1000);
			fout << ((*iter)[7] % 1000);

			cout << " \\ was Deposited into Account ";
			fout << " \\ was Deposited into Account ";
			cout.width(4);
			cout.fill('0');
			fout.width(4);
			fout.fill('0');
			cout << ((*iter)[3] / 10000) << "-";
			fout << ((*iter)[3] / 10000) << "-";
			cout.width(4);
			cout.fill('0');
			fout.width(4);
			fout.fill('0');
			cout << ((*iter)[3] % 10000) << "-";
			fout << ((*iter)[3] % 10000) << "-";
			cout.width(4);
			cout.fill('0');
			fout.width(4);
			fout.fill('0');
			cout << ((*iter)[4] / 10000) << "-";
			fout << ((*iter)[4] / 10000) << "-";
			cout.width(4);
			cout.fill('0');
			fout.width(4);
			fout.fill('0');
			cout << ((*iter)[4] % 10000) << endl;
			fout << ((*iter)[4] % 10000) << endl;
			cout << endl << endl;
			fout << endl << endl;
		}

		//withdrawl history
		else if ((*iter)[2] == 2) {
			cout << "Transaction Type : Withdrawal" << endl;
			fout << "Transaction type : Withdrawal" << endl;
			if ((*iter)[7] > 1000000) {
				cout << ((*iter)[7] / 1000000) << ",";
				fout << ((*iter)[7] / 1000000) << ",";
				cout.width(3);
				cout.fill('0');
				fout.width(3);
				fout.fill('0');
				cout << (((*iter)[7] % 1000000) / 1000) << ",";
				fout << (((*iter)[7] % 1000000) / 1000) << ",";
			}
			else {
				cout << ((*iter)[7] / 1000) << ",";
				fout << ((*iter)[7] / 1000) << ",";
			}
			cout.width(3);
			cout.fill('0');
			fout.width(3);
			fout.fill('0');
			cout << ((*iter)[7] % 1000);
			fout << ((*iter)[7] % 1000);
			cout << " \\ was Withdrawn out of Account ";
			fout << " \\ was Withdrawn out of Account ";
			cout.width(4);
			cout.fill('0');
			fout.width(4);
			fout.fill('0');
			cout << ((*iter)[3] / 10000) << "-";
			fout << ((*iter)[3] / 10000) << "-";
			cout.width(4);
			cout.fill('0');
			fout.width(4);
			fout.fill('0');
			cout << ((*iter)[3] % 10000) << "-";
			fout << ((*iter)[3] % 10000) << "-";
			cout.width(4);
			cout.fill('0');
			fout.width(4);
			fout.fill('0');
			cout << ((*iter)[4] / 10000) << "-";
			fout << ((*iter)[4] / 10000) << "-";
			cout.width(4);
			cout.fill('0');
			fout.width(4);
			fout.fill('0');
			cout << ((*iter)[4] % 10000) << endl;
			fout << ((*iter)[4] % 10000) << endl;
			cout << endl << endl;
			fout << endl << endl;
		}
		else {
			cout << "Transaction Type : ";
			fout << "Transaction Type : ";
			cout << ((*iter)[8] == 1 ? " Cash Transfer" : " Account Transfer") << endl;
			fout << ((*iter)[8] == 1 ? " Cash Transfer" : " Account Transfer") << endl;
			if ((*iter)[7] > 1000000) {
				cout << ((*iter)[7] / 1000000) << ",";
				fout << ((*iter)[7] / 1000000) << ",";
				cout.width(3);
				cout.fill('0');
				fout.width(3);
				fout.fill('0');
				cout << (((*iter)[7] % 1000000) / 1000) << ",";
				fout << (((*iter)[7] % 1000000) / 1000) << ",";
			}
			else {
				cout << ((*iter)[7] / 1000) << ",";
				fout << ((*iter)[7] / 1000) << ",";
			}
			cout.width(3);
			cout.fill('0');
			fout.width(3);
			fout.fill('0');
			cout << ((*iter)[7] % 1000);
			fout << ((*iter)[7] % 1000);
			if ((*iter)[8] == 1) {
				cout << " \\ was Transfered to Account ";
				fout << " \\ was Transfered to Account ";
				cout.width(4);
				cout.fill('0');
				fout.width(4);
				fout.fill('0');
				cout << ((*iter)[5] / 10000) << "-";
				fout << ((*iter)[5] / 10000) << "-";
				cout.width(4);
				cout.fill('0');
				fout.width(4);
				fout.fill('0');
				cout << ((*iter)[5] % 10000) << "-";
				fout << ((*iter)[5] % 10000) << "-";
				cout.width(4);
				cout.fill('0');
				fout.width(4);
				fout.fill('0');
				cout << ((*iter)[6] / 10000) << "-";
				fout << ((*iter)[6] / 10000) << "-";
				cout.width(4);
				cout.fill('0');
				fout.width(4);
				fout.fill('0');
				cout << ((*iter)[6] % 10000) << endl;
				fout << ((*iter)[6] % 10000) << endl;
				cout << endl << endl;
				fout << endl << endl;
			}
			else {
				cout << " \\ was Transfered from Account ";
				fout << " \\ was Transfered from Account ";
				cout.width(4);
				cout.fill('0');
				fout.width(4);
				fout.fill('0');
				cout << ((*iter)[3] / 10000) << "-";
				fout << ((*iter)[3] / 10000) << "-";
				cout.width(4);
				cout.fill('0');
				fout.width(4);
				fout.fill('0');
				cout << ((*iter)[3] % 10000) << "-";
				fout << ((*iter)[3] % 10000) << "-";
				cout.width(4);
				cout.fill('0');
				fout.width(4);
				fout.fill('0');
				cout << ((*iter)[4] / 10000) << "-";
				fout << ((*iter)[4] / 10000) << "-";
				cout.width(4);
				cout.fill('0');
				fout.width(4);
				fout.fill('0');
				cout << ((*iter)[4] % 10000);
				fout << ((*iter)[4] % 10000);
				cout << " to Account ";
				fout << " to Account ";
				cout.width(4);
				cout.fill('0');
				fout.width(4);
				fout.fill('0');
				cout << ((*iter)[5] / 10000) << "-";
				fout << ((*iter)[5] / 10000) << "-";
				cout.width(4);
				cout.fill('0');
				fout.width(4);
				fout.fill('0');
				cout << ((*iter)[5] % 10000) << "-";
				fout << ((*iter)[5] % 10000) << "-";
				cout.width(4);
				cout.fill('0');
				fout.width(4);
				fout.fill('0');
				cout << ((*iter)[6] / 10000) << "-";
				fout << ((*iter)[6] / 10000) << "-";
				cout.width(4);
				cout.fill('0');
				fout.width(4);
				fout.fill('0');
				cout << ((*iter)[6] % 10000) << endl;
				fout << ((*iter)[6] % 10000) << endl;
				cout << endl << endl;
				fout << endl << endl;
			}
		}
		flag--;
	}

}

/*
void ATM::kor_get_history() {
	ofstream fout("Admin_History.txt", ios_base::out);		//create a text file named "Admin_History.txt"

	list<int*>::iterator iter = history.begin();

	cout << "ATM의 거래 기록입니다." << endl << endl << endl;	//desplay on console window
	fout << "ATM의 거래 기록입니다." << endl << endl << endl;	//write the same words on text file

	int flag = size(history);		//desplay chronological order

	for (iter = history.begin(); iter != history.end(); iter++) {
		cout << "(#" << flag << " 번째 거래를 불러옵니다. )" << "거래 일련번호 : ";
		fout << "(#" << flag << " 번째 거래를 불러옵니다. )" << "거래 일련번호 : ";
		for (int i = 0; i < 9; i++) {
			cout << (*iter)[i];
			fout << (*iter)[i];
		}

		//cut the card number to 4 digits
		cout << endl << "투입된 카드 번호 : ";
		fout << endl << "투입된 카드 번호 : ";
		cout.width(4);
		cout.fill('0');
		fout.width(4);
		fout.fill('0');
		cout << ((*iter)[0] / 10000) << "-";
		fout << ((*iter)[0] / 10000) << "-";
		cout.width(4);
		cout.fill('0');
		fout.width(4);
		fout.fill('0');
		cout << ((*iter)[0] % 10000) << "-";
		fout << ((*iter)[0] % 10000) << "-";
		cout.width(4);
		cout.fill('0');
		fout.width(4);
		fout.fill('0');
		cout << ((*iter)[1] / 10000) << "-";
		fout << ((*iter)[1] / 10000) << "-";
		cout.width(4);
		cout.fill('0');
		fout.width(4);
		fout.fill('0');
		cout << ((*iter)[1] % 10000) << endl;
		fout << ((*iter)[1] % 10000) << endl;

		//deposit history
		if ((*iter)[2] == 1) {
			cout << "거래 종류 :";
			fout << "거래 종류 :";
			cout << ((*iter)[8] == 1 ? " 현금 입금" : " 수표 입금") << endl;
			fout << ((*iter)[8] == 1 ? " 현금 입금" : " 수표 입금") << endl;
			//cut the amount to 3 digits
			if ((*iter)[7] > 1000000) {
				cout << ((*iter)[7] / 1000000) << ",";
				fout << ((*iter)[7] / 1000000) << ",";
				cout.width(3);
				cout.fill('0');
				fout.width(3);
				fout.fill('0');
				cout << (((*iter)[7] % 1000000) / 1000) << ",";
				fout << (((*iter)[7] % 1000000) / 1000) << ",";
			}
			else {
				cout << ((*iter)[7] / 1000) << ",";
				fout << ((*iter)[7] / 1000) << ",";
			}
			cout.width(3);
			cout.fill('0');
			fout.width(3);
			fout.fill('0');
			cout << ((*iter)[7] % 1000);
			fout << ((*iter)[7] % 1000);

			cout << " \\ was 이 ";
			fout << " \\ was 이 ";
			cout.width(4);
			cout.fill('0');
			fout.width(4);
			fout.fill('0');
			cout << ((*iter)[3] / 10000) << "-";
			fout << ((*iter)[3] / 10000) << "-";
			cout.width(4);
			cout.fill('0');
			fout.width(4);
			fout.fill('0');
			cout << ((*iter)[3] % 10000) << "-";
			fout << ((*iter)[3] % 10000) << "-";
			cout.width(4);
			cout.fill('0');
			fout.width(4);
			fout.fill('0');
			cout << ((*iter)[4] / 10000) << "-";
			fout << ((*iter)[4] / 10000) << "-";
			cout.width(4);
			cout.fill('0');
			fout.width(4);
			fout.fill('0');
			cout << ((*iter)[4] % 10000);
			cout << " 계좌로 입금되었습니다." << endl;
			fout << ((*iter)[4] % 10000) << endl;
			fout << " 계좌로 입금되었습니다." << endl;
			cout << endl << endl;
			fout << endl << endl;
		}

		//withdrawl history
		else if ((*iter)[2] == 2) {
			cout << "거래 종류 : 출금" << endl;
			fout << "거래 종류 : 출금" << endl;
			if ((*iter)[7] > 1000000) {
				cout << ((*iter)[7] / 1000000) << ",";
				fout << ((*iter)[7] / 1000000) << ",";
				cout.width(3);
				cout.fill('0');
				fout.width(3);
				fout.fill('0');
				cout << (((*iter)[7] % 1000000) / 1000) << ",";
				fout << (((*iter)[7] % 1000000) / 1000) << ",";
			}
			else {
				cout << ((*iter)[7] / 1000) << ",";
				fout << ((*iter)[7] / 1000) << ",";
			}
			cout.width(3);
			cout.fill('0');
			fout.width(3);
			fout.fill('0');
			cout << ((*iter)[7] % 1000);
			fout << ((*iter)[7] % 1000);
			cout << " \\ 이 ";
			fout << " \\ 이 ";
			cout.width(4);
			cout.fill('0');
			fout.width(4);
			fout.fill('0');
			cout << ((*iter)[3] / 10000) << "-";
			fout << ((*iter)[3] / 10000) << "-";
			cout.width(4);
			cout.fill('0');
			fout.width(4);
			fout.fill('0');
			cout << ((*iter)[3] % 10000) << "-";
			fout << ((*iter)[3] % 10000) << "-";
			cout.width(4);
			cout.fill('0');
			fout.width(4);
			fout.fill('0');
			cout << ((*iter)[4] / 10000) << "-";
			fout << ((*iter)[4] / 10000) << "-";
			cout.width(4);
			cout.fill('0');
			fout.width(4);
			fout.fill('0');
			cout << ((*iter)[4] % 10000);
			cout << " 계좌에서 인출되었습니다." << endl;
			fout << ((*iter)[4] % 10000);
			fout << " 계좌에서 인출되었습니다." << endl;
			cout << endl << endl;
			fout << endl << endl;
		}
		else {
			cout << "거래 종류 : 송금" << endl;
			fout << "거래 종류 : 송금" << endl;
			if ((*iter)[7] > 1000000) {
				cout << ((*iter)[7] / 1000000) << ",";
				fout << ((*iter)[7] / 1000000) << ",";
				cout.width(3);
				cout.fill('0');
				fout.width(3);
				fout.fill('0');
				cout << (((*iter)[7] % 1000000) / 1000) << ",";
				fout << (((*iter)[7] % 1000000) / 1000) << ",";
			}
			else {
				cout << ((*iter)[7] / 1000) << ",";
				fout << ((*iter)[7] / 1000) << ",";
			}
			cout.width(3);
			cout.fill('0');
			fout.width(3);
			fout.fill('0');
			cout << ((*iter)[7] % 1000);
			fout << ((*iter)[7] % 1000);
			cout << " \\ 이 ";
			fout << " \\ 이 ";
			cout.width(4);
			cout.fill('0');
			fout.width(4);
			fout.fill('0');
			cout << ((*iter)[3] / 10000) << "-";
			fout << ((*iter)[3] / 10000) << "-";
			cout.width(4);
			cout.fill('0');
			fout.width(4);
			fout.fill('0');
			cout << ((*iter)[3] % 10000) << "-";
			fout << ((*iter)[3] % 10000) << "-";
			cout.width(4);
			cout.fill('0');
			fout.width(4);
			fout.fill('0');
			cout << ((*iter)[4] / 10000) << "-";
			fout << ((*iter)[4] / 10000) << "-";
			cout.width(4);
			cout.fill('0');
			fout.width(4);
			fout.fill('0');
			cout << ((*iter)[4] % 10000);
			fout << ((*iter)[4] % 10000);
			cout << " 계좌에서 ";
			fout << " 계좌에서 ";
			cout.width(4);
			cout.fill('0');
			fout.width(4);
			fout.fill('0');
			cout << ((*iter)[5] / 10000) << "-";
			fout << ((*iter)[5] / 10000) << "-";
			cout.width(4);
			cout.fill('0');
			fout.width(4);
			fout.fill('0');
			cout << ((*iter)[5] % 10000) << "-";
			fout << ((*iter)[5] % 10000) << "-";
			cout.width(4);
			cout.fill('0');
			fout.width(4);
			fout.fill('0');
			cout << ((*iter)[6] / 10000) << "-";
			fout << ((*iter)[6] / 10000) << "-";
			cout.width(4);
			cout.fill('0');
			fout.width(4);
			fout.fill('0');
			cout << ((*iter)[6] % 10000);
			cout << " 계좌로 송금되었습니다." << endl;
			fout << ((*iter)[6] % 10000);
			cout << " 계좌로 송금되었습니다." << endl;
			cout << endl << endl;
			fout << endl << endl;
		}
		flag--;
	}

}*/

int* t_id(long long my_acc, long long dest_acc, int t_type, int fund, int fund_type) {
	int* t_id = new int[9];
	t_id[0] = my_acc / 100000000;
	t_id[1] = my_acc % 100000000;
	t_id[2] = t_type;
	t_id[3] = t_id[0];
	t_id[4] = t_id[1];
	t_id[5] = dest_acc / 100000000;;
	t_id[6] = dest_acc % 100000000;
	t_id[7] = fund;
	t_id[8] = fund_type;

	return t_id;
}

void ATM::deposit(Account* my_acc) { //returns transactionID
	int fund = 0;
	int fund_type; // Cash or Check


	while (1) { // Cash deposit
		cout << "Would you like to deposit cash or check? \n";
		cout << "(1) Cash (2) Check\n";
		cin >> fund_type;
		if (cin.fail()) {
			cin.clear();
			cin.ignore(100, '\n');
			cout << "invalid input type! " << endl;
		}
		if (fund_type == 1 || fund_type == 2) { break; }
		cout << "Invalid input. Please try again. ";
	}

	if (fund_type == 1) {
		int n = 0;
		while (1) {
			int bill;
			cout << "Input the amount of fund you wish to deposit in units of 10,000 KRW.\n Slot:";
			cin >> bill;
			if (cin.fail()) {
				cin.clear();
				cin.ignore(100, '\n');
				cout << "invalid input type! " << endl;
			}
			if (bill > 500000) {
				cout << "Invalid input. You can only deposit 50 bills at a time." << endl;
				cin.clear();
				bill = 0;
				continue;
			}
			else {
				fund += bill;
				break;
			}
		}

	}
	if (fund_type == 2) { // check deposit
		int n;
		int bill;
		cout << "Please insert check to the dispensor slot.\nSlot:";
		while (1) {
			cout << "Insert check value. Press (0) to finish inserting.";
			cin >> bill;
			if (cin.fail()) {
				cin.clear();
				cin.ignore(100, '\n');
				cout << "invalid input type! " << endl;
			}
			if (bill == 0) { break; }
			if (bill < 0) {
				cout << "Invalid input. Please try again.";
				continue;
			}
			if (bill > 100000000) {
				cout << "Invalid input. You can only deposit checks less than 100,000,000." << endl;
				cin.clear();
				bill = 0;
				continue;
			}

			cout << "How many " << bill << "KRW checks?\n";
			cin >> n;
			if (cin.fail()) {
				cin.clear();
				cin.ignore(100, '\n');
				cout << "invalid input type! " << endl;
			}
			if (n > 30) {
				cout << "You can deposit up to 30 checks. Please try again.\n";
				continue;
			}
			else if (n <= 0) {
				cout << "Invalid input. Please try again.";
				continue;
			}
			else { fund += bill * n; }

		}
	}
	//Deposit fee
	if (fund_type == 1) { this->addBalance(fund); }
	if (this->primaryBank != my_acc->bringBank()) { fund -= 500; }

	my_acc->addbalance(fund);
	long long ac_no = my_acc->getAccountNum();

	int* ptr;
	ptr = t_id(ac_no, 0, 1, fund, fund_type);
	this->history.push_front(ptr);
	this->show_result(ptr);
	cout << endl << endl;


	//for (int i=0; i<9; i++) {cout << i << ":" << *(ptr+i) << endl;}
}
void BI_ATM::deposit(Account* my_acc) { //returns transactionID
	int fund = 0;
	int fund_type; // Cash or Check


	while (1) { // Cash deposit
		lan == 0 ? cout << "Would you like to deposit cash or check?\n" << endl : cout << "현금과 수표 중 어떤 것을 입금하시겠습니까?" << endl;
		lan == 0 ? cout << "(1) cash (2) check\n" << endl : cout << "(1) 현금 (2) 수표\n" << endl;
		cin >> fund_type;
		if (cin.fail()) {
			cin.clear();
			cin.ignore(100, '\n');
			lan == 0 ? cout << "invalid input type!" << endl : cout << "유효하지 않은 입력입니다." << endl;
		}
		if (fund_type == 1 || fund_type == 2) { break; }
		lan == 0 ? cout << "invalid input. Retry." << endl : cout << "유효하지 않은 입력입니다. 다시 시도하세요." << endl;
	}

	if (fund_type == 1) {
		int n = 0;
		while (1) {
			int bill;
			lan == 0 ? cout << "Enter the deposit amount in units of 10,000 KRW.\n Slot:" << endl : cout << "입금액을 10,000원 단위로 입력합니다.\n Slot:" << endl;
			cin >> bill;
			if (cin.fail()) {
				cin.clear();
				cin.ignore(100, '\n');
				lan == 0 ? cout << "invalid input type!" << endl : cout << "유효하지 않은 입력입니다." << endl;
			}
			if (bill > 500000) {
				lan == 0 ? cout << "Invalid input. You can only deposit 50 bills at a time." << endl : cout << "잘못된 입력입니다. 한 번에 50개의 지폐만 입금할 수 있습니다." << endl;
				cin.clear();
				bill = 0;
				continue;
			}
			else {
				fund += bill;
				break;
			}
		}

	}
	if (fund_type == 2) { // check deposit
		int n;
		int bill;
		lan == 0 ? cout << "Insert the check into the dispenser slot.\nSlot:" << endl : cout << "수표를 디스펜서 슬롯에 삽입하십시오.\nSlot:" << endl;
		while (1) {
			lan == 0 ? cout << "Insert a check value. Press [0] to finish inserting." << endl : cout << "수표 값을 삽입합니다. (0)을 눌러 삽입을 마칩니다." << endl;
			cin >> bill;
			if (cin.fail()) {
				cin.clear();
				cin.ignore(100, '\n');
				lan == 0 ? cout << "invalid input type!" << endl : cout << "유효하지 않은 입력입니다." << endl;
			}
			if (bill == 0) { break; }
			if (bill < 0) {
				lan == 0 ? cout << "invalid input. Retry." << endl : cout << "유효하지 않은 입력입니다. 다시 시도하세요." << endl;
				continue;
			}
			if (bill > 100000000) {
				lan == 0 ? cout << "Invalid input. You can only deposit checks for less than 100,000,000 KRW." << endl : cout << "잘못된 입력입니다. 100,000,000원 미만의 수표만 입금할 수 있습니다." << endl;
				cin.clear();

				bill = 0;
				continue;
			}
			lan == 0 ? cout << "How many" << bill << "KRW check?\n" << endl : cout << "몇개의" << bill << "원화 수표?\n" << endl;
			cin >> n;
			if (cin.fail()) {
				cin.clear();
				cin.ignore(100, '\n');
				lan == 0 ? cout << "invalid input type!" << endl : cout << "유효하지 않은 입력입니다." << endl;
			}
			if (n > 30) {
				lan == 0 ? cout << "You can deposit up to 30 checks. Please try again.\n" << endl : cout << "수표는 최대 30개까지 입금할 수 있습니다. 다시 시도해 주세요.\n" << endl;
				continue;
			}
			else if (n <= 0) {
				lan == 0 ? cout << "invalid input. Retry." << endl : cout << "유효하지 않은 입력입니다. 다시 시도하세요." << endl;
				continue;
			}
			else { fund += bill * n; }

		}
	}
	//Deposit fee

	if (fund_type == 1) { this->addBalance(fund); }
	if (this->primaryBank != my_acc->bringBank()) { fund -= 500; }
	my_acc->addbalance(fund);
	long long ac_no = my_acc->getAccountNum();

	int* ptr;
	ptr = t_id(ac_no, 0, 1, fund, fund_type);
	this->history.push_front(ptr);
	this->show_result(ptr);
	cout << endl << endl;
	//for (int i=0; i<9; i++) {cout << i << ":" << *(ptr+i) << endl;}
}
void BIMB_ATM::deposit(Account* my_acc) { //returns transactionID
	int fund = 0;
	int fund_type; // Cash or Check


	while (1) { // Cash deposit
		lan == 0 ? cout << "Would you like to deposit cash or check?\n" << endl : cout << "현금과 수표 중 어떤 것을 입금하시겠습니까?" << endl;
		lan == 0 ? cout << "(1) cash (2) check\n" << endl : cout << "(1) 현금 (2) 수표\n" << endl;
		cin >> fund_type;
		if (cin.fail()) {
			cin.clear();
			cin.ignore(100, '\n');
			lan == 0 ? cout << "invalid input type!" << endl : cout << "유효하지 않은 입력입니다." << endl;
		}
		if (fund_type == 1 || fund_type == 2) { break; }
		lan == 0 ? cout << "invalid input. Retry." << endl : cout << "유효하지 않은 입력입니다. 다시 시도하세요." << endl;
	}

	if (fund_type == 1) {
		int n = 0;
		while (1) {
			int bill;
			lan == 0 ? cout << "Enter the deposit amount in units of 10,000 KRW.\n Slot:" << endl : cout << "입금액을 10,000원 단위로 입력합니다.\n Slot:" << endl;
			cin >> bill;
			if (cin.fail()) {
				cin.clear();
				cin.ignore(100, '\n');
				lan == 0 ? cout << "invalid input type!" << endl : cout << "유효하지 않은 입력입니다." << endl;
			}
			if (bill > 500000) {
				lan == 0 ? cout << "Invalid input. You can only deposit 50 bills at a time." << endl : cout << "잘못된 입력입니다. 한 번에 50개의 지폐만 입금할 수 있습니다." << endl;
				cin.clear();
				bill = 0;
				continue;
			}
			else {
				fund += bill;
				break;
			}
		}

	}
	if (fund_type == 2) { // check deposit
		int n;
		int bill;
		lan == 0 ? cout << "Insert the check into the dispenser slot.\nSlot:" << endl : cout << "수표를 디스펜서 슬롯에 삽입하십시오.\nSlot:" << endl;
		while (1) {
			lan == 0 ? cout << "Insert a check value. Press [0] to finish inserting." << endl : cout << "수표 값을 삽입합니다. (0)을 눌러 삽입을 마칩니다." << endl;
			cin >> bill;
			if (cin.fail()) {
				cin.clear();
				cin.ignore(100, '\n');
				lan == 0 ? cout << "invalid input type!" << endl : cout << "유효하지 않은 입력입니다." << endl;
			}
			if (bill == 0) { break; }
			if (bill < 0) {
				lan == 0 ? cout << "invalid input. Retry." << endl : cout << "유효하지 않은 입력입니다. 다시 시도하세요." << endl;
				continue;
			}
			if (bill > 100000000) {
				lan == 0 ? cout << "Invalid input. You can only deposit checks for less than 100,000,000 KRW." << endl : cout << "잘못된 입력입니다. 100,000,000원 미만의 수표만 입금할 수 있습니다." << endl;
				cin.clear();

				bill = 0;
				continue;
			}
			lan == 0 ? cout << "How many" << bill << "KRW check?\n" << endl : cout << "몇개의" << bill << "원화 수표?\n" << endl;
			cin >> n;
			if (cin.fail()) {
				cin.clear();
				cin.ignore(100, '\n');
				lan == 0 ? cout << "invalid input type!" << endl : cout << "유효하지 않은 입력입니다." << endl;
			}
			if (n > 30) {
				lan == 0 ? cout << "You can deposit up to 30 checks. Please try again.\n" << endl : cout << "수표는 최대 30개까지 입금할 수 있습니다. 다시 시도해 주세요.\n" << endl;
				continue;
			}
			else if (n <= 0) {
				lan == 0 ? cout << "invalid input. Retry." << endl : cout << "유효하지 않은 입력입니다. 다시 시도하세요." << endl;
				continue;
			}
			else { fund += bill * n; }

		}
	}
	//Deposit fee
	if (fund_type == 1) { this->addBalance(fund); }
	if (this->primaryBank != my_acc->bringBank()) { fund -= 500; }
	cout << "check" << fund << endl;

	my_acc->addbalance(fund);
	long long ac_no = my_acc->getAccountNum();

	int* ptr;
	ptr = t_id(ac_no, 0, 1, fund, fund_type);
	this->history.push_front(ptr);
	this->show_result(ptr);
	cout << endl << endl;
	//for (int i=0; i<9; i++) {cout << i << ":" << *(ptr+i) << endl;}
}


void ATM::withdrawal(Account* my_acc) {
	long long ac_no = my_acc->getAccountNum();
	int* ptr = nullptr;
	for (int i = 0; i < 3; i++) {
		int fund = 0;
		cout << "Input withdrawal amount." << endl; //REQ 5.1
		cin >> fund;
		if (fund > 1000000) {
			cout << "You can withdraw up to 1,000,000 KRW at a time." << endl;
			continue;
		}
		else if (this->checkBalance() < fund) {
			cout << "Inquiry exceeds the amount of cash in this ATM. Please try again.\n";
			continue; //REQ 5.2
		}
		else if (my_acc->getbalance() < fund + 500) {
			cout << "Not enough balance in your account." << endl;
			continue;
		}

		else {
			cout << "Transaction complete" << endl;
			my_acc->addbalance(-fund); //REQ 5.3
			// if (my_acc.bringBank() != this_ATM.bringBank(ac_no)) {my_acc.addbalance(-1000);}
			my_acc->addbalance(-500); // withdrawal fee for primary bank
			this->addBalance(-fund); //REQ 5.5
			if (this->primaryBank != my_acc->bringBank()) { my_acc->addbalance(-500); }

			ptr = t_id(ac_no, 0, 2, fund, 0);
			this->history.push_front(ptr);
			this->show_result(ptr);

			cout << "Current Balance:" << my_acc->getbalance() << endl;
			cout << "Please take " << fund << "KRW from the dispenser slot." << endl;
			cout << endl;

			//Withdrawal fee for 2ndary banks must be implemented here (REQ 5.4)

			break;
		}
	}
	if (ptr == nullptr) {
		cout << "Failed withdrawing balance too many times. Please try again." << endl;
	}
}
void BI_ATM::withdrawal(Account* my_acc) {
	long long ac_no = my_acc->getAccountNum();
	int* ptr = nullptr;
	cout << "test" << endl;
	for (int i = 0; i < 3; i++) {
		int fund = 0;
		lan == 0 ? cout << "Input withdrawal amount." << endl : cout << "출금 금액을 입력하세요." << endl; //REQ 5.1
		cin >> fund;
		if (fund > 1000000) {
			lan == 0 ? cout << "You can withdraw up to 1,000,000 KRW at a time." << endl : cout << "한 번에 최대 1,000,000원까지 출금할 수 있습니다." << endl;
			continue;
		}
		else if (this->checkBalance() < fund) {
			lan == 0 ? cout << "Inquiry exceeds the amount of cash in this ATM. Please try again.\n" << endl : cout << "inquiry가 이 ATM의 현금 금액을 초과합니다. 다시 시도해 주세요.\n" << endl; //REQ 5.1
			continue; //REQ 5.2
		}
		else if (my_acc->getbalance() < fund + 500) {
			lan == 0 ? cout << "Not enough balance in your account." << endl : cout << "계좌에 잔액이 충분하지 않습니다." << endl; //REQ 5.1
			continue;
		}

		else {
			lan == 0 ? cout << "Transaction complete" << endl : cout << "거래 완료" << endl; //REQ 5.1
			my_acc->addbalance(-fund); //REQ 5.3
			// if (my_acc.bringBank() != this_ATM.bringBank(ac_no)) {my_acc.addbalance(-1000);}
			my_acc->addbalance(-500); // withdrawal fee for primary bank
			if (this->primaryBank != my_acc->bringBank()) { my_acc->addbalance(-500); }
			this->addBalance(-fund); //REQ 5.5
			ptr = t_id(ac_no, 0, 2, fund, 0);
			this->history.push_front(ptr);
			this->show_result(ptr);

			lan == 0 ? cout << "Current Balance:" << my_acc->getbalance() << endl : cout << "현재 잔액:" << my_acc->getbalance() << endl;
			lan == 0 ? cout << "Please take " << fund << "KRW from the dispenser slot." << endl : cout << "디스펜서 슬롯에서 " << fund << "원을 가져가세요" << endl;
			//Withdrawal fee for 2ndary banks must be implemented here (REQ 5.4)
			break;
		}
	}
	if (ptr == nullptr) {
		lan == 0 ? cout << "Failed withdrawing balance too many times. Please try again." << endl : cout << "잔액을 인출하는 데 너무 많이 실패했습니다. 다시 시도해 주세요." << endl;
	}
}
void BIMB_ATM::withdrawal(Account* my_acc) {
	long long ac_no = my_acc->getAccountNum();
	int* ptr = nullptr;
	for (int i = 0; i < 3; i++) {
		int fund = 0;
		cout << "test" << endl;
		lan == 0 ? cout << "Input withdrawal amount." << endl : cout << "출금 금액을 입력하세요." << endl; //REQ 5.1
		cin >> fund;
		if (fund > 1000000) {
			lan == 0 ? cout << "You can withdraw up to 1,000,000 KRW at a time." << endl : cout << "한 번에 최대 1,000,000원까지 출금할 수 있습니다." << endl;
			continue;
		}
		else if (this->checkBalance() < fund) {
			lan == 0 ? cout << "Inquiry exceeds the amount of cash in this ATM. Please try again.\n" << endl : cout << "inquiry가 이 ATM의 현금 금액을 초과합니다. 다시 시도해 주세요.\n" << endl; //REQ 5.1
			continue; //REQ 5.2
		}
		else if (my_acc->getbalance() < fund + 500) {
			lan == 0 ? cout << "Not enough balance in your account." << endl : cout << "계좌에 잔액이 충분하지 않습니다." << endl; //REQ 5.1
			continue;
		}

		else {
			lan == 0 ? cout << "Transaction complete" << endl : cout << "거래 완료" << endl; //REQ 5.1
			my_acc->addbalance(-fund); //REQ 5.3
			// if (my_acc.bringBank() != this_ATM.bringBank(ac_no)) {my_acc.addbalance(-1000);}
			my_acc->addbalance(-500); // withdrawal fee for primary bank
			this->addBalance(-fund); //REQ 5.5
			if (this->primaryBank != my_acc->bringBank()) { my_acc->addbalance(-500); }
			ptr = t_id(ac_no, 0, 2, fund, 0);
			this->history.push_front(ptr);
			this->show_result(ptr);

			lan == 0 ? cout << "Current Balance:" << my_acc->getbalance() << endl : cout << "현재 잔액:" << my_acc->getbalance() << endl;
			lan == 0 ? cout << "Please take " << fund << "KRW from the dispenser slot." << endl : cout << "디스펜서 슬롯에서 " << fund << "원을 가져가세요" << endl;
			//Withdrawal fee for 2ndary banks must be implemented here (REQ 5.4)
			break;
		}
	}
	if (ptr == nullptr) {
		lan == 0 ? cout << "Failed withdrawing balance too many times. Please try again." << endl : cout << "잔액을 인출하는 데 너무 많이 실패했습니다. 다시 시도해 주세요." << endl;
	}
}

void ATM::transfer()
{
	int transfer_type;
	long long my_acc_number = 0;
	long long dest_acc_number = 0;
	int fund;
	string ox;
	Bank* my_bank;
	Bank* dest_bank;
	Account* dest_acc;
	Account* my_acc;
	this->authorization = true;

	while (1) {
		cout << "Choose transfer type: (1) Cash Transfer, (2) Account Transfer" << endl;
		cin >> transfer_type;
		if (cin.fail()) {
			cin.clear();
			cin.ignore(100, '\n');
			cout << "invalid input type! " << endl;
		}
		if (transfer_type == 1 || transfer_type == 2) { break; }
		cout << "Invalid input. Please try again. ";
	}
	while (1) {
		cout << "Enter destination account number." << endl;
		cin >> dest_acc_number;
		if (cin.fail()) {
			cin.clear();
			cin.ignore(100, '\n');
			cout << "invalid input type! " << endl;
		}

		dest_bank = (this->bringBank(dest_acc_number));

		if (dest_bank != nullptr) {
			break;
		}
		cout << "Invalid account number. Please check the account number." << endl;
		cin.clear();
	}
	dest_acc = dest_bank->findAccount(dest_acc_number);
	cout << "dest_acc:" << dest_acc->getbalance() << endl;

	if (transfer_type == 1) { // Cash transfer 
		while (1) {
			cout << "Insert cash to the dispenser slot in units of 10,000 KRW: " << endl;
			cin >> fund;
			if (cin.fail()) {
				cin.clear();
				cin.ignore(100, '\n');
				cout << "invalid input type! " << endl;
			}
			if (fund > 500000) {
				cout << "Invalid input. You can only transfer 50 bills at a time." << endl;
				cin.clear();
				fund = 0;
				continue;
			}
			else {
				cout << "Counting cash..." << endl;
				cout << "Transfering " << fund << " KRW to " << dest_acc_number << "." << endl;
				cout << "Correct? (Press OK to proceed)" << endl;
				cout << "[OK] [Cancel]";
				cin >> ox;
				if (cin.fail()) {
					cin.clear();
					cin.ignore(100, '\n');
					cout << "invalid input type! " << endl;
				}
				if (ox == "OK") { break; }
				if (ox == "Cancel") {
					cout << "Canceled transaction. Please start again." << endl;
					return;
				}
				else {
					cout << "Invalid input. Please try again." << endl;
					continue;
				}
			}
		}

		this->addBalance(fund);
		fund -= 1500;
		if (this->primaryBank != dest_acc->bringBank()) { fund -= 500; }
		dest_acc->addbalance(fund);
		cout << dest_acc->getbalance() << endl;


		cout << fund << "KRW is transfered to " << dest_acc_number << "." << endl;
		cout << "Transaction complete" << endl;

	}


	if (transfer_type == 2) { // Account transfer
		while (1) {
			cout << "Enter source account number" << endl;
			cin >> my_acc_number;
			if (cin.fail()) {
				cin.clear();
				cin.ignore(100, '\n');
				cout << "invalid input type! " << endl;
			}
			my_bank = (this->bringBank(my_acc_number));



			if (my_bank != nullptr) {
				break;
			}
			cout << "Invalid account number. Please check the account number.";
		}

		my_acc = my_bank->findAccount(my_acc_number);
		cout << "my balance:" << my_acc->getbalance() << endl;

		while (1) {

			cout << "Input transfer amount." << endl;
			cin >> fund;
			if (cin.fail()) {
				cin.clear();
				cin.ignore(100, '\n');
				cout << "invalid input type! " << endl;
			}

			if (fund > my_acc->getbalance()) {
				cout << "Not enough balance in your account.";
				cout << "Current balance: " << my_acc->getbalance() << "KRW" << endl;
				fund = 0;
				continue;
			}

			cout << "Transfering " << fund << "KRW to " << dest_acc_number << " from " << my_acc_number << endl;
			break;
		}
		dest_acc->addbalance(fund);
		if (this->primaryBank != dest_acc->bringBank()) { my_acc->addbalance(-500); }
		if (this->primaryBank != my_acc->bringBank()) { my_acc->addbalance(-500); }
		my_acc->addbalance(-(fund + 1500));

		cout << fund << "KRW is transfered to " << dest_acc_number << "." << endl;
		cout << "Transaction complete" << endl;
	}
	int* ptr;
	ptr = t_id(my_acc_number, dest_acc_number, 3, fund, transfer_type);
	this->history.push_front(ptr);
	this->show_result(ptr);
	cout << endl << endl;
	//Withdrawal fee for 2ndary banks must be implemented here (REQ 5.4)


}
void BI_ATM::transfer()
{
	int transfer_type;
	long long my_acc_number = 0;
	long long dest_acc_number = 0;
	int fund;
	string ox;
	Bank* my_bank;
	Bank* dest_bank;
	Account* dest_acc;
	Account* my_acc;
	this->authorization = true;

	while (1) {
		lan == 0 ? cout << "Choose transfer type: (1) Cash Transfer, (2) Account Transfer" << endl : cout << "이체 유형 선택: (1) 현금 이체, (2) 계좌 이체" << endl;
		cin >> transfer_type;
		if (transfer_type == 1 || transfer_type == 2) { break; }
		lan == 0 ? cout << "Invalid input. Please try again." << endl : cout << "잘못된 입력입니다. 다시 시도해 주세요." << endl;
	}
	while (1) {
		lan == 0 ? cout << "Enter destination account number." << endl : cout << "목적지 계좌번호를 입력하세요." << endl;
		cin >> dest_acc_number;
		dest_bank = (this->bringBank(dest_acc_number));
		if (dest_bank != nullptr) {
			break;
		}
		lan == 0 ? cout << "Invalid account number. Please check the account number." << endl : cout << "잘못된 계좌 번호입니다. 계좌번호를 확인해주세요." << endl;
		cin.clear();
	}
	dest_acc = dest_bank->findAccount(dest_acc_number);
	lan == 0 ? cout << "dest_acc:" << dest_acc->getbalance() << endl : cout << "목적지 계좌:" << dest_acc->getbalance() << endl;



	if (transfer_type == 1) { // Cash transfer 
		while (1) {
			lan == 0 ? cout << "Insert cash to the dispenser slot in units of 10,000 KRW: " << endl : cout << "10,000원 단위로 디스펜서 슬롯에 현금을 넣으세요: " << endl;
			cin >> fund;
			if (fund > 500000) {
				lan == 0 ? cout << "Invalid input. You can only transfer 50 bills at a time." << endl : cout << "잘못된 입력입니다. 한 번에 50개의 지폐만 전송할 수 있습니다." << endl;
				cin.clear();
				fund = 0;
				continue;
			}
			else {
				lan == 0 ? cout << "Counting cash..." << endl : cout << "현금 세는 중..." << endl;
				lan == 0 ? cout << "Transfering " << fund << " KRW to " << dest_acc_number << "." << endl : cout << fund << " 원 을 " << dest_acc_number << "로 전송 중..." << endl;
				lan == 0 ? cout << "Correct? (Press OK to proceed)" << endl : cout << "맞나요? (진행을 위해 OK를 누르세요.)" << endl;
				lan == 0 ? cout << "[OK] [Cancel]" << endl : cout << "[OK] [Cancel]" << endl;

				cin >> ox;
				if (ox == "OK") { break; }
				if (ox == "Cancel") {
					lan == 0 ? cout << "Canceled transaction. Please start again." << endl : cout << "거래가 취소되었습니다. 다시 시작하십시오." << endl;
					return;
				}
				else {
					lan == 0 ? cout << "Invalid input. Please try again." << endl : cout << "잘못된 입력입니다. 다시 시도해 주세요." << endl;
					continue;
				}
			}

			this->addBalance(fund);
			fund -= 1500; //transfer fee b/w primary banks
			if (this->primaryBank != dest_acc->bringBank()) { fund -= 500; }

			dest_acc->addbalance(fund);
			cout << dest_acc->getbalance() << endl;


			lan == 0 ? cout << fund << "KRW is transfered to " << dest_acc_number << "." << endl : cout << fund << "원이 " << dest_acc_number << "로 전송되었습니다." << endl;
			lan == 0 ? cout << "Transaction complete" << endl : cout << "거래 완료" << endl;
		}


		if (transfer_type == 2) { // Account transfer
			while (1) {
				lan == 0 ? cout << "Enter source account number" << endl : cout << "소스 계좌번호를 입력하세요." << endl;
				cin >> my_acc_number;
				my_bank = (this->bringBank(my_acc_number));


				if (my_bank != nullptr) {
					break;
				}
				lan == 0 ? cout << "Invalid account number. Please check the account number." : cout << "잘못된 계좌 번호입니다. 계좌번호를 확인해주세요.";
			}

			my_acc = my_bank->findAccount(my_acc_number);
			lan == 0 ? cout << "my balance:" << my_acc->getbalance() << endl : cout << "내 잔고:" << my_acc->getbalance() << endl;

			while (1) {
				lan == 0 ? cout << "Input transfer amount." << endl : cout << "이체 금액을 입력합니다." << endl;
				cin >> fund;

				if (fund > my_acc->getbalance()) {
					lan == 0 ? cout << "Not enough balance in your account." : cout << "계좌에 잔액이 충분하지 않습니다.";
					lan == 0 ? cout << "Current balance: " << my_acc->getbalance() << "KRW" << endl : cout << "현재 잔액: " << my_acc->getbalance() << "원" << endl;
					fund = 0;
					continue;
				}
				lan == 0 ? cout << "Transfering " << fund << "KRW to " << dest_acc_number << " from " << my_acc_number << endl : cout << fund << "원을 " << my_acc_number << "에서" << dest_acc_number << "로 전송중" << endl;
				break;
			}
			if (this->primaryBank != dest_acc->bringBank()) { my_acc->addbalance(-500); }
			if (this->primaryBank != my_acc->bringBank()) { my_acc->addbalance(-500); }
			my_acc->addbalance(-(fund + 1500));
			dest_acc->addbalance(fund);

			lan == 0 ? cout << fund << "KRW is transfered to " << dest_acc_number << "." << endl : cout << fund << "원이 " << dest_acc_number << "로 전송되었습니다." << endl;
			lan == 0 ? cout << "Transaction complete" << endl : cout << "거래 완료" << endl;
		}
		int* ptr;
		ptr = t_id(my_acc_number, dest_acc_number, 3, fund, transfer_type);
		this->history.push_front(ptr);
		this->show_result(ptr);
		cout << endl << endl;
		//Withdrawal fee for 2ndary banks must be implemented here (REQ 5.4)


	}
}
void BIMB_ATM::transfer()
{
	int transfer_type;
	long long my_acc_number = 0;
	long long dest_acc_number = 0;
	int fund;
	string ox;
	Bank* my_bank;
	Bank* dest_bank;
	Account* dest_acc;
	Account* my_acc;
	this->authorization = true;

	while (1) {
		lan == 0 ? cout << "Choose transfer type: (1) Cash Transfer, (2) Account Transfer" << endl : cout << "이체 유형 선택: (1) 현금 이체, (2) 계좌 이체" << endl;
		cin >> transfer_type;
		if (cin.fail()) {
			cin.clear();
			cin.ignore(100, '\n');
			lan == 0 ? cout << "invalid input type!" << endl : cout << "잘못된 입력입니다." << endl;
		}
		if (transfer_type == 1 || transfer_type == 2) { break; }
		lan == 0 ? cout << "Invalid input. Please try again." << endl : cout << "잘못된 입력입니다. 다시 시도해 주세요." << endl;
	}
	while (1) {
		lan == 0 ? cout << "Enter destination account number." << endl : cout << "목적지 계좌번호를 입력하세요." << endl;
		cin >> dest_acc_number;
		if (cin.fail()) {
			cin.clear();
			cin.ignore(100, '\n');
			lan == 0 ? cout << "invalid input type!" << endl : cout << "잘못된 입력입니다." << endl;
		}
		dest_bank = (this->bringBank(dest_acc_number));

		if (dest_bank != nullptr) {
			break;
		}
		lan == 0 ? cout << "Invalid account number. Please check the account number." << endl : cout << "잘못된 계좌 번호입니다. 계좌번호를 확인해주세요." << endl;
		cin.clear();
	}
	dest_acc = dest_bank->findAccount(dest_acc_number);

	lan == 0 ? cout << "dest_acc:" << dest_acc->getbalance() << endl : cout << "목적지 계좌:" << dest_acc->getbalance() << endl;



	if (transfer_type == 1) { // Cash transfer 
		while (1) {
			lan == 0 ? cout << "Insert cash to the dispenser slot in units of 10,000 KRW: " << endl : cout << "10,000원 단위로 디스펜서 슬롯에 현금을 넣으세요: " << endl;
			cin >> fund;
			if (cin.fail()) {
				cin.clear();
				cin.ignore(100, '\n');
				lan == 0 ? cout << "invalid input type!" << endl : cout << "잘못된 입력입니다." << endl;
			}
			if (fund > 500000) {
				lan == 0 ? cout << "Invalid input. You can only transfer 50 bills at a time." << endl : cout << "잘못된 입력입니다. 한 번에 50개의 지폐만 전송할 수 있습니다." << endl;
				cin.clear();
				fund = 0;
				continue;
			}
			else {
				lan == 0 ? cout << "Counting cash..." << endl : cout << "현금 세는 중..." << endl;
				lan == 0 ? cout << "Transfering " << fund << " KRW to " << dest_acc_number << "." << endl : cout << fund << " 원 을 " << dest_acc_number << "로 전송 중..." << endl;
				lan == 0 ? cout << "Correct? (Press OK to proceed)" << endl : cout << "맞나요? (진행을 위해 OK를 누르세요.)" << endl;
				lan == 0 ? cout << "[OK] [Cancel]" << endl : cout << "[OK] [Cancel]" << endl;
				cin >> ox;
				if (cin.fail()) {
					cin.clear();
					cin.ignore(100, '\n');
					lan == 0 ? cout << "invalid input type!" << endl : cout << "잘못된 입력입니다." << endl;
				}
				if (ox == "OK") { break; }
				if (ox == "Cancel") {
					lan == 0 ? cout << "Canceled transaction. Please start again." << endl : cout << "거래가 취소되었습니다. 다시 시작하십시오." << endl;
					return;
				}
				else {
					lan == 0 ? cout << "Invalid input. Please try again." << endl : cout << "잘못된 입력입니다. 다시 시도해 주세요." << endl;
					continue;
				}
			}
		}
		this->addBalance(fund);
		fund -= 1500; //transfer fee b/w primary banks
		if (this->primaryBank != dest_acc->bringBank()) { fund -= 500; }

		dest_acc->addbalance(fund);
		cout << dest_acc->getbalance() << endl;


		lan == 0 ? cout << fund << "KRW is transfered to " << dest_acc_number << "." << endl : cout << fund << "원이 " << dest_acc_number << "로 전송되었습니다." << endl;
		lan == 0 ? cout << "Transaction complete" << endl : cout << "거래 완료" << endl;

	}


	if (transfer_type == 2) { // Account transfer
		while (1) {
			lan == 0 ? cout << "Enter source account number" << endl : cout << "소스 계좌번호를 입력하세요." << endl;
			cin >> my_acc_number;
			if (cin.fail()) {
				cin.clear();
				cin.ignore(100, '\n');
				lan == 0 ? cout << "invalid input type!" << endl : cout << "잘못된 입력입니다." << endl;
			}
			my_bank = (this->bringBank(my_acc_number));


			if (my_bank != nullptr) {
				break;
			}
			lan == 0 ? cout << "Invalid account number. Please check the account number." : cout << "잘못된 계좌 번호입니다. 계좌번호를 확인해주세요.";
		}

		my_acc = my_bank->findAccount(my_acc_number);
		lan == 0 ? cout << "my balance:" << my_acc->getbalance() << endl : cout << "내 잔고:" << my_acc->getbalance() << endl;

		while (1) {

			lan == 0 ? cout << "Input transfer amount." << endl : cout << "이체 금액을 입력합니다." << endl;
			cin >> fund;
			if (cin.fail()) {
				cin.clear();
				cin.ignore(100, '\n');
				lan == 0 ? cout << "invalid input type!" << endl : cout << "잘못된 입력입니다." << endl;
			}

			if (fund > my_acc->getbalance()) {
				lan == 0 ? cout << "Not enough balance in your account." : cout << "계좌에 잔액이 충분하지 않습니다.";
				lan == 0 ? cout << "Current balance: " << my_acc->getbalance() << "KRW" << endl : cout << "현재 잔액: " << my_acc->getbalance() << "원" << endl;
				fund = 0;
				continue;
			}

			lan == 0 ? cout << "Transfering " << fund << "KRW to " << dest_acc_number << " from " << my_acc_number << endl : cout << fund << "원을 " << my_acc_number << "에서" << dest_acc_number << "로 전송중" << endl;
			break;
		}
		if (this->primaryBank != dest_acc->bringBank()) { my_acc->addbalance(-500); }
		if (this->primaryBank != my_acc->bringBank()) { my_acc->addbalance(-500); }
		my_acc->addbalance(-(fund + 1500));
		dest_acc->addbalance(fund);

		lan == 0 ? cout << fund << "KRW is transfered to " << dest_acc_number << "." << endl : cout << fund << "원이 " << dest_acc_number << "로 전송되었습니다." << endl;
		lan == 0 ? cout << "Transaction complete" << endl : cout << "거래 완료" << endl;
	}
	int* ptr;
	ptr = t_id(my_acc_number, dest_acc_number, 3, fund, transfer_type);
	this->history.push_front(ptr);
	this->show_result(ptr);
	cout << endl << endl;
	//Withdrawal fee for 2ndary banks must be implemented here (REQ 5.4)


}



//The session start Trigger
int Booting_program() {
	int start;
	while (true) {
		cout << "Start Program" << endl;
		cout << "\n \n(1)  NH - Sing Bank \n(2) IBK - Multi Bank";
		cout << "\n(3) IBK - Sing Bank with Bilingual\n(4)  NH - Multi Bank with Bilingual" << endl;
		cout << "\nchoose ATM / 원하시는 ATM에 해당하는 번호를 선택하세요 :";
		cin >> start;

		//system("cls"); //clean console

		if (start == 1 || start == 2 || start == 3 || start == 4 || start == 5) { break; }

		else if (cin.fail()) {
			cin.clear();
			cin.ignore(100, '\n');
			cout << "invalid input type! " << endl;
		}
		else { cout << "You choosed Wrong number." << endl; }

		cout << "\nRe";
	}
	return start;
}

int main() {
	///user
	User Ezreal("Ezreal");
	User DGIST("DGIST");

	//bank
	Bank NH("NH Bank", 1000);
	Bank IBK("IBK Bank", 2000);

	//bank list
	list<Bank*> bankList = { &NH, &IBK };

	//kind of ATM
	ATM NH_ATM(&NH, 1001); //single bank
	MB_ATM IBK_MB_ATM(&IBK, 2001, bankList);
	BI_ATM IBK_BI_ATM(&IBK, 2002);
	BIMB_ATM NH_BIMB_ATM(&NH, 1002, bankList);

	//account in NH
	Account ac1(&DGIST, &NH, 1111, 1000111100017463);
	Account ac2(&DGIST, &NH, 1111, 1000111100028479);
	Account ac3(&Ezreal, &NH, 1234, 1000222200030983);

	//account in IBK
	Account ac4(&DGIST, &IBK, 1111, 2000111100014832);
	Account ac5(&Ezreal, &IBK, 1234, 2000222200029472);
	Account ac6(&Ezreal, &IBK, 1234, 2000222200033956);


	//set NH Account List
	map<long long, Account*> accountMap1;
	accountMap1.insert({ 1000111100017463, &ac1 });
	accountMap1.insert({ 1000111100028479, &ac2 });
	accountMap1.insert({ 1000222200030983, &ac3 });
	NH.setAccountList(accountMap1);

	//set IBK Account List
	map<long long, Account*> accountMap2;
	accountMap2.insert({ 2000111100014832, &ac4 });
	accountMap2.insert({ 2000222200029472, &ac5 });
	accountMap2.insert({ 2000222200033956, &ac6 });
	IBK.setAccountList(accountMap2);

	//add balance to ATM by bank
	NH.addBalanceATM(1001, 10000000);
	NH.addBalanceATM(1002, 10000000);
	IBK.addBalanceATM(2001, 10000000);
	IBK.addBalanceATM(2002, 10000000);


	Ezreal.printAccountList();
	DGIST.printAccountList();
	NH.printBankATMList();
	IBK.printBankATMList();


	//Session starts
	int start = Booting_program();
	//(1)Single Bank (2)Multi Bank (3)Single Bilingual Bank (4)Multi Bilingual Bank

	if (start == 1) {
		while (1) {
			cout << "Welcome to NH BANK." << endl;
			setting_print();
			if (NH_ATM.insert_card() == false) {
				break;
			}
		}
	}
	else if (start == 2) {
		while (1) {
			cout << "Welcome to IBK BANK." << endl;
			setting_print();
			if (IBK_MB_ATM.insert_card() == false) {
				break;
			}
		}
	}
	else if (start == 3) {
		while (1) {
			cout << "Welcome to IBK BANK." << endl;
			setting_print();
			if (IBK_BI_ATM.insert_card() == false) {
				break;
			}
		}
	}
	else if (start == 4) {
		while (1) {

			cout << "Welcome to NH BANK." << endl;
			setting_print();
			if (NH_BIMB_ATM.insert_card() == false) {
				break;
			}
		}
	}

	else if (start == 5) {
		cout << "감사합니다!" << endl;
	}


	return 0;

}
