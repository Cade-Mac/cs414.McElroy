#include <iostream>
#include <string>

class Person {
public:
    std::string name;

    Person(const std::string& name) : name(name) {
        std::cout << "Person constructor\n";
    }
};

class Student : public Person {
public:
    std::string major;

    Student(const std::string& name, const std::string& major)
        : Person(name), major(major) {
        std::cout << "Student constructor\n";
    }
};

class Employee : public Person {
public:
    int employee_id;

    Employee(const std::string& name, int employee_id)
        : Person(name), employee_id(employee_id) {
        std::cout << "Employee constructor\n";
    }
};

class TeachingAssistant : public Student, public Employee {
public:
    std::string course;

    TeachingAssistant(const std::string& name, const std::string& major,
                      int employee_id, const std::string& course)
        : Student(name, major), Employee(name, employee_id), course(course) {
        std::cout << "TeachingAssistant constructor\n";
    }
};

int main() {
    std::cout << "--- Constructing one TeachingAssistant ---\n";
    TeachingAssistant ta("Alex Rivera", "Computer Science", 1042, "CS 240");

    Person& student_person = static_cast<Student&>(ta);
    Person& employee_person = static_cast<Employee&>(ta);

    std::cout << "\n--- The two Person subobjects ---\n";
    std::cout << "student_person  address: " << &student_person
              << "  name: " << student_person.name << '\n';
    std::cout << "employee_person address: " << &employee_person
              << "  name: " << employee_person.name << '\n';

    std::cout << "\n--- Change the name through student_person ---\n";
    student_person.name = "Alex R.";
    std::cout << "student_person  name: " << student_person.name << '\n';
    std::cout << "employee_person name: " << employee_person.name << '\n';

    // These do not compile because ta contains two Person subobjects.
    // std::cout << ta.name << '\n';
    // Person* person = &ta;

    return 0;
}
