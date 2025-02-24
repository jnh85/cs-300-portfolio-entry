#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cctype>

using namespace std;

/**
* Course struct to store individual course info
*/
struct Course {
	string courseNumber;
	string courseName;
	vector<string> prerequisites;
};

// Size of the hash table
const int TABLE_SIZE = 17;

/**
* Implements a hash table data structure for storing Course objects
* Uses linear probing to handle collisions
*/
class HashTable {
private:
	// Vector of Course pointers to store the courses
	vector<Course*> table;

	/**
	* Hash function to generate index for course storing
	* @param key (The course number to hash)
	* @return calculated hash index
	*/
	int hash(string key) {
		unsigned int hashValue = 0;
		for (char c : key) {
			hashValue += c;
		}
		return hashValue % TABLE_SIZE;
	}

public:
	/**
	* Constructor initializes hash table with nullptr values
	*/
	HashTable() {
		table = vector<Course*>(TABLE_SIZE, nullptr);
	}

	/**
	* Inserts a course into the hash table
	* Uses linear probing to handle collisions
	* @ param course
	*/
	void insert(Course course) {
		int index = hash(course.courseNumber);
		
		// Linear probe until we find an empty slot or the same course number
		while (table[index] != nullptr && table[index]->courseNumber != course.courseNumber) {
			index = (index + 1) % TABLE_SIZE;
		}

		Course* newCourse = new Course(course);

		// Delete existing course if updating
		if (table[index] != nullptr) {
			delete table[index];
		}

		table[index] = newCourse;
	}

	/**
	* Searches for a course in the hash table
	* @param courseNumber (The course number to search for
	* @return pointer to the found course or nullptr if not found
	*/
	Course* search(string courseNumber) {
		int index = hash(courseNumber);
		int originalIndex = index;
		while (table[index] != nullptr) {
			if (table[index]->courseNumber == courseNumber) {
				return table[index];
			}
			index = (index + 1) % TABLE_SIZE;
			if (index == originalIndex) break;
		}
		return nullptr;
	}

	/**
	* Returns all courses in the hash table as a vector
	* @return vector containing all courses
	*/
	vector<Course> getAllCourses() {
		vector<Course> courses;
		for (int i = 0; i < TABLE_SIZE; i++) {
			if (table[i] != nullptr) {
				courses.push_back(*table[i]);
			}
		}
		return courses;
	}
	
	/**
	* Checks if the hash table is empty
	@return true if empty, return false otherwise
	*/
	bool empty() {
		for (int i = 0; i < TABLE_SIZE; i++) {
			if (table[i] != nullptr) return false;
		}
		return true;
	}

	/**
	* Clears all courses from hash table
	* Properly deallocates memory
	*/
	void clear() {
		for (int i = 0; i < TABLE_SIZE; i++) {
			if (table[i] != nullptr) {
				delete table[i];
				table[i] = nullptr;
			}
		}
	}

	/**
	* Destructor ensures proper cleanup of allocated memory
	*/
	~HashTable() {
		clear();
	}
};

// Hash table to store all course data
HashTable courseData;


/**
* Validates course data from csv file
* Ensures that file exists and has valid course format and prerequisites
* @param filename (Name of the file to load)
*/
void loadCourseDataFromFile(string filename) {
	ifstream file(filename);
	if (!file.is_open()) {
		cout << "Error: File not found" << endl;
		return;
	}

	vector<string> validCourseNumbers;
	string line;

	// First pass collects all valid course numbers and validates basic format
	while (getline(file, line)) {
		stringstream ss(line);
		string courseNumber;
		getline(ss, courseNumber, ',');

		// Check if line has at least course number and name
		if (ss.peek() == EOF) {
			cout << "Invalid course format: " << line << endl;
			file.close();
			return;
		}
		
		validCourseNumbers.push_back(courseNumber);
	}

	// Reset file to beginning for second pass
	file.clear();
	file.seekg(0);

	// Second pass validates prerequisites against course numbers
	while (getline(file, line)) {
		stringstream ss(line);
		string courseNumber;
		string courseName;
		string prereq;
		vector<string> prereqs;

		getline(ss, courseNumber, ',');
		getline(ss, courseName, ',');

		// Collect prerequisites
		while (getline(ss, prereq, ',')) {
			prereqs.push_back(prereq);
		}

		// Validate each prerequisite
		for (int i = 2; i < prereqs.size(); ++i) {
			bool found = false;
			for (const string& validCourse : validCourseNumbers) {
				if (validCourse == prereqs[i]) {
					found = true;
					break;
				}
			}

			if (!found) {
				cout << "Invalid prerequisites: " << prereqs[i] << endl;
				file.close();
				return;
			}
		}
	}

	file.close();
}

/**
* Creates Course objects from file data and stores them in the hash table
* @ param filename (Name of the file to load)
*/
void createAndStoreCourses(string filename) {
	ifstream file(filename);
	if (!file.is_open()) {
		return;
	}

	// Clear existing data before loading new data
	courseData.clear();
	string line;

	while (getline(file, line)) {
		stringstream ss(line);
		Course course;
		string prereq;

		// Get course number
		getline(ss, course.courseNumber, ',');

		// Get course name
		getline(ss, course.courseName, ',');

		// Get prerequisites
		while (getline(ss, prereq, ',')) {
			if (!prereq.empty()) {
				course.prerequisites.push_back(prereq);
			}
		}

		courseData.insert(course);
	}

	file.close();
}

/**
* Searches for and displays course information
* Shows course number, name, and prerequisites if any
* @param courseNumber (The course number to search for
*/
void searchCourse(string courseNumber) {
	Course* course = courseData.search(courseNumber);

	if (course != nullptr) {
		cout << course->courseNumber << ", " << course->courseName << endl;

		if (!course->prerequisites.empty()) {
			cout << "Prerequisites: ";
			bool first = true;
			for (const string& prereq : course->prerequisites) {
				if (!first) {
					cout << ", ";
				}

				first = false;
				cout << prereq;
			}
			cout << endl;
		}
	}
	else {
		cout << "Course not found." << endl;
	}
}

/**
* Prints all courses in alphanumeric order
* Uses selection sort to sort courses by course number
*/
void printSortedCourseList() {
	if (courseData.empty()) {
		cout << "No courses to display" << endl;
		return;
	}

	vector<Course> sortedCourses = courseData.getAllCourses();

	// Selection sort
	for (int i = 0; i < sortedCourses.size() - 1; ++i) {
		int minIndex = i;
		for (int j = i + 1; j < sortedCourses.size(); j++) {
			if (sortedCourses[j].courseNumber < sortedCourses[minIndex].courseNumber) {
				minIndex = j;
			}
		}

		// Swap if minimum is not at current position
		if (minIndex != i) {
			Course temp = sortedCourses[i];
			sortedCourses[i] = sortedCourses[minIndex];
			sortedCourses[minIndex] = temp;
		}
	}

	// Display sorted courses
	cout << "Here is a sample schedule:" << endl << endl;
	for (const Course& course : sortedCourses) {
		cout << course.courseNumber << ", " << course.courseName << endl;
	}
}

/**
* Displays and handles the main menu
* Provides options for...
* Loading data
* printing course list
* searching for specific courses
* exiting the program
*/
void displayMenu() {
	bool running = true;

	while (running) {
		// Display menu options
		cout << "\n1. Load Data Structure." << endl;
		cout << "2. Print Course List." << endl;
		cout << "3. Print Course." << endl;
		cout << "9. Exit" << endl;
		cout << endl;
		cout << "What would you like to do? ";

		int choice;
		cin >> choice;

		// Handle menu selection
		switch (choice) {
			case 1: {
				string filename;
				cout << "Enter filename: ";
				cin >> filename;

				loadCourseDataFromFile(filename);
				createAndStoreCourses(filename);
				cout << "Data loaded successfully" << endl;
				break;
			}
			case 2: {
				if (courseData.empty()) {
					cout << "Please load data first" << endl;
				}
				else {
					printSortedCourseList();
				}
				break;
			}
			case 3: {
				if (courseData.empty()) {
					cout << "Please load data first" << endl;
				}
				else {
					string courseNumber;
					cout << "What course do you want to know about? ";
					cin >> courseNumber;
					
					for (char& c : courseNumber) {
						c = toupper(c);
					}

					searchCourse(courseNumber);
				}
				break;
			}
			case 9:
				running = false;
				cout << "Thank you for using the course planner!" << endl;
				break;
			default:
				cout << choice << " is not a valid option." << endl;
		}
	}
}

/**
* Main function initializes program and start the menu interface
*/
int main() {
	cout << "Welcome to the course planner." << endl;
	displayMenu();
	return 0;
}
