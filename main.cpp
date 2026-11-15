#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <exception>
using namespace std;

class QuizException :public std::exception
{
	string message;
public:
	QuizException(string message = "Error")
	{
		this->message = message;
	}
	virtual const char* what() const override {
		return this->message.c_str();
	}
};

enum Menu { EXIT, CREATE, START, BOARD };

string readLine() {
	char buffer[200];
	cin.getline(buffer, 200);
	string text = buffer;
	return text;
}

//Nomreni yazi ile muqayise edirik, uygun nomre tapilsa geri qaytaririq
int readRange(string text, int min, int max) {
	while (true)
	{
		cout << text;
		string line = readLine();
		for (int i = min; i <= max; i++)
		{
			if (line == to_string(i))
			{
				return i;
			}
		}
		cout << "Wrong input" << endl;
	}
}

bool isUsed(vector<int>& used, int number) {
	for (size_t i = 0; i < used.size(); i++)
	{
		if (used[i] == number)
		{
			return true;
		}
	}
	return false;
}

vector<int> randomOrder(int count) {
	vector<int> order;
	while ((int)order.size() < count)
	{
		int number = rand() % count;
		if (!isUsed(order, number))
		{
			order.push_back(number);
		}
	}
	return order;
}

class Question {
	string text;
	vector<string> answers;
	int correct;
public:
	Question(string text, vector<string> answers, int correct)
	{
		this->text = text;
		this->answers = answers;
		this->correct = correct;
	}

	string getText() { return this->text; }
	string getAnswer(int index) { return this->answers[index]; }
	int getAnswerCount() { return this->answers.size(); }
	int getCorrect() { return this->correct; }
};

class Quiz {
	string name;
	vector<Question> questions;
public:
	Quiz(string name)
	{
		this->name = name;
	}

	string getName() { return this->name; }
	int getCount() { return this->questions.size(); }
	Question getQuestion(int index) { return this->questions[index]; }
	void addQuestion(const Question& question) {
		this->questions.push_back(question);
	}
};

struct Result {
	string playerName;
	string quizName;
	int correct;
	int wrong;
	int pass;

	Result()
	{
		this->playerName = "";
		this->quizName = "";
		this->correct = 0;
		this->wrong = 0;
		this->pass = 0;
	}
};

ostream& operator<<(ostream& output, const Result& result) {
	output << "Result" << endl;
	output << "Correct: " << result.correct << endl;
	output << "InCorrect: " << result.wrong << endl;
	output << "Pass: " << result.pass << endl;
	return output;
}

struct QuestionState {
	int questionIndex;
	vector<int> answerOrder;
	int chosen;

	QuestionState()
	{
		this->questionIndex = 0;
		this->chosen = -1;
	}
};

class LeaderBoard {
	vector<Result> results;
public:
	void add(const Result& result) {
		this->results.push_back(result);
	}

	void show() {
		if (this->results.size() == 0)
		{
			cout << "Leader board is empty" << endl;
			return;
		}

		vector<Result> sorted = this->results;
		int count = sorted.size();

		//Bubble sort
		bool swapped;
		for (int i = 0; i < count - 1; i++)
		{
			swapped = false;
			for (int j = 0; j < count - i - 1; j++)
			{
				if (sorted[j].correct < sorted[j + 1].correct
					|| (sorted[j].correct == sorted[j + 1].correct && sorted[j].wrong > sorted[j + 1].wrong))
				{
					Result temp = sorted[j];
					sorted[j] = sorted[j + 1];
					sorted[j + 1] = temp;
					swapped = true;
				}
			}
			if (swapped == false)
			{
				break;
			}
		}

		cout << "Top 10" << endl;
		for (int i = 0; i < count && i < 10; i++)
		{
			cout << i + 1 << ". " << sorted[i].playerName << " | " << sorted[i].quizName
				<< " | C:" << sorted[i].correct << " W:" << sorted[i].wrong
				<< " P:" << sorted[i].pass << endl;
		}
	}
};

class User {
protected:
	string name;
	User()
	{
		this->name = "";
	}
public:
	User(string name)
	{
		this->name = name;
	}
	string getName() { return this->name; }
	void setName(string name) { this->name = name; }
	virtual bool canCreate() = 0;  //Pure virtual method
	virtual string getRole() = 0;  //Pure virtual method
};

class Admin :public User {
	string password;
public:
	Admin()
		:User("admin")
	{
		this->password = "admin123";
	}

	bool login(string name, string password) {
		return this->name == name && this->password == password;
	}
	bool canCreate() override { return true; }
	string getRole() override { return "Admin"; }
};

class Guest :public User {
public:
	Guest()
		:User("")
	{

	}
	bool canCreate() override { return false; }
	string getRole() override { return "Guest"; }
};

class App {
	Admin admin;
	Guest guest;
	User* user = nullptr;
	vector<Quiz> quizzes;
	LeaderBoard board;
public:
	void login() {
		cout << "1 : Admin" << endl;
		cout << "2 : Guest" << endl;
		int choice = readRange("Choice: ", 1, 2);
		string name;

		if (choice == 1)
		{
			string password;
			cout << "Username: ";
			name = readLine();
			cout << "Password: ";
			password = readLine();

			if (!admin.login(name, password))
			{
				throw QuizException("Wrong username or password");
			}
			user = &admin;
		}
		else
		{
			cout << "Name: ";
			name = readLine();
			guest.setName(name);
			user = &guest;
		}
	}

	void createQuiz() {
		string name;
		cout << "Enter Quiz name: ";
		name = readLine();
		Quiz quiz(name);

		int number = 1;
		while (true)
		{
			string text;
			cout << "Question " << number << " : ";
			text = readLine();

			vector<string> answers;
			while (true)
			{
				string answer;
				cout << "Answer " << answers.size() + 1 << " : ";
				answer = readLine();
				if (answer == "")
				{
					if (answers.size() < 2)
					{
						cout << "Min 2 answers" << endl;
						continue;
					}
					break;
				}
				answers.push_back(answer);
			}

			int correct = readRange("Correct answer number: ", 1, (int)answers.size());
			quiz.addQuestion(Question(text, answers, correct - 1));
			number++;

			cout << "1 : new" << endl;
			cout << "2 : back" << endl;
			cout << "3 : save" << endl;
			int choice = readRange("Choice: ", 1, 3);
			if (choice == 2)
			{
				cout << "Quiz canceled" << endl;
				return;
			}
			if (choice == 3)
			{
				quizzes.push_back(quiz);
				cout << "Quiz saved" << endl;
				return;
			}
		}
	}

	void startQuiz() {
		if (quizzes.size() == 0)
		{
			throw QuizException("No quiz yet");
		}

		string playerName;
		cout << "Enter your name: ";
		playerName = readLine();

		cout << "Select quiz:" << endl;
		for (int i = 0; i < (int)quizzes.size(); i++)
		{
			cout << i + 1 << " : " << quizzes[i].getName() << endl;
		}

		int choice = readRange("Quiz: ", 1, (int)quizzes.size());
		Quiz quiz = quizzes[choice - 1];
		int count = quiz.getCount();

		//Suallarin ve cavablarin ardicilligi random olur
		vector<int> order = randomOrder(count);
		vector<QuestionState> states;
		for (int i = 0; i < count; i++)
		{
			QuestionState state;
			state.questionIndex = order[i];
			state.answerOrder = randomOrder(quiz.getQuestion(order[i]).getAnswerCount());
			states.push_back(state);
		}

		int index = 0;

		while (true)
		{
			Question question = quiz.getQuestion(states[index].questionIndex);
			cout << endl << "Question " << index + 1 << "/" << count << ": " << question.getText() << endl;

			for (int k = 0; k < (int)states[index].answerOrder.size(); k++)
			{
				cout << k + 1 << " : " << question.getAnswer(states[index].answerOrder[k]);
				if (states[index].chosen == states[index].answerOrder[k])
				{
					cout << "  <-";
				}
				cout << endl;
			}

			cout << "(number = answer, p = Previous, n = Next, s = Submit)" << endl;
			string line;
			cout << "> ";
			line = readLine();

			if (line == "p")
			{
				if (index > 0)
				{
					index--;
				}
			}
			else if (line == "n")
			{
				if (index < count - 1)
				{
					index++;
				}
			}
			else if (line == "s")
			{
				break;
			}
			else
			{
				int number = 0;
				for (int k = 1; k <= (int)states[index].answerOrder.size(); k++)
				{
					if (line == to_string(k))
					{
						number = k;
					}
				}

				if (number == 0)
				{
					cout << "Wrong input" << endl;
				}
				else
				{
					states[index].chosen = states[index].answerOrder[number - 1];
				}
			}
		}

		Result result;
		result.playerName = playerName;
		result.quizName = quiz.getName();
		for (int i = 0; i < count; i++)
		{
			if (states[i].chosen == -1)
			{
				result.pass++;
			}
			else if (states[i].chosen == quiz.getQuestion(states[i].questionIndex).getCorrect())
			{
				result.correct++;
			}
			else
			{
				result.wrong++;
			}
		}

		cout << endl << result;
		board.add(result);
	}

	void run() {
		while (user == nullptr)
		{
			try
			{
				login();
			}
			catch (const exception& ex)
			{
				cout << ex.what() << endl;
			}
		}
		cout << "Welcome " << user->getName() << " (" << user->getRole() << ")" << endl;

		while (true)
		{
			cout << endl;
			cout << "1) Create Quiz" << endl;
			cout << "2) Start Quiz" << endl;
			cout << "3) Leader Board" << endl;
			cout << "0) Exit" << endl;
			int choice = readRange("Choice: ", 0, 3);

			try
			{
				switch (choice)
				{
				case CREATE:
					if (user->canCreate())
					{
						createQuiz();
					}
					else
					{
						cout << "Only admin can create quiz" << endl;
					}
					break;
				case START:
					startQuiz();
					break;
				case BOARD:
					board.show();
					break;
				case EXIT:
					return;
				}
			}
			catch (const exception& ex)
			{
				cout << ex.what() << endl;
			}
		}
	}
};

void main() {
	srand(time(0));
	App app;
	app.run();
}
