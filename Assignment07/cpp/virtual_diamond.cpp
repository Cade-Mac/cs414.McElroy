#include <iostream>
#include <string>

class Person {
public:
    std::string name;

    Person(const std::string& name) : name(name) {
        std::cout << "Person constructor\n";
    }

    virtual std::string role() const {
        return "Person";
    }

    virtual ~Person() = default;
};

class Student : virtual public Person {
public:
    std::string major;

    Student(const std::string& name, const std::string& major)
        : Person(name), major(major) {
        std::cout << "Student constructor\n";
    }

    std::string role() const override {
        return "Student majoring in " + major;
    }
};

class Employee : virtual public Person {
public:
    int employee_id;

    Employee(const std::string& name, int employee_id)
        : Person(name), employee_id(employee_id) {
        std::cout << "Employee constructor\n";
    }

    std::string role() const override {
        return "Employee #" + std::to_string(employee_id);
    }
};

class TeachingAssistant : public Student, public Employee {
public:
    std::string course;

    // Person is a virtual base, so the most-derived class must build it.
    TeachingAssistant(const std::string& name, const std::string& major,
                      int employee_id, const std::string& course)
        : Person(name), Student(name, major), Employee(name, employee_id),
          course(course) {
        std::cout << "TeachingAssistant constructor\n";
    }

    // Removing this override makes the program fail to compile, because
    // Student::role and Employee::role would both compete to be the final one.
    std::string role() const override {
        return "Teaching assistant for " + course;
    }
};

class RobotAssistant {
public:
    std::string role() const {
        return "Robot assistant";
    }
};

void announce_person(const Person& person) {
    std::cout << person.role() << '\n';
}

template<typename T>
void announce_generic(const T& value) {
    std::cout << value.role() << '\n';
}

int main() {
    std::cout << "--- Constructing one TeachingAssistant ---\n";
    TeachingAssistant ta("Alex Rivera", "Computer Science", 1042, "CS 240");

    Person& student_person = static_cast<Student&>(ta);
    Person& employee_person = static_cast<Employee&>(ta);

    std::cout << "\n--- The Person subobject seen through each side ---\n";
    std::cout << "student_person  address: " << &student_person
              << "  name: " << student_person.name << '\n';
    std::cout << "employee_person address: " << &employee_person
              << "  name: " << employee_person.name << '\n';

    std::cout << "\n--- Change the name through student_person ---\n";
    student_person.name = "Alex R.";
    std::cout << "student_person  name: " << student_person.name << '\n';
    std::cout << "employee_person name: " << employee_person.name << '\n';

    std::cout << "\n--- These now compile ---\n";
    std::cout << ta.name << '\n';
    Person* person = &ta;
    std::cout << "Person* points to: " << person << '\n';

    std::cout << "\n--- role() through a Person& ---\n";
    Person& as_person = ta;
    std::cout << as_person.role() << '\n';

    std::cout << "\n--- Polymorphism without common ancestry ---\n";
    RobotAssistant robot;

    std::cout << "announce_person(ta):     ";
    announce_person(ta);

    // Does not compile: RobotAssistant is not a Person.
    // announce_person(robot);

    std::cout << "announce_generic(ta):    ";
    announce_generic(ta);

    std::cout << "announce_generic(robot): ";
    announce_generic(robot);

    return 0;
}
