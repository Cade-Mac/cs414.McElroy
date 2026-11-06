class Person:
    def __init__(self, *, name, **kwargs):
        print("Entering Person.__init__", id(self))
        self.name = name
        super().__init__(**kwargs)

    def role(self):
        return "Person"


class Student(Person):
    def __init__(self, *, major, **kwargs):
        print("Entering Student.__init__", id(self))
        self.major = major
        super().__init__(**kwargs)

    def rename_as_student(self, new_name):
        self.name = new_name

    def role(self):
        return "Student majoring in " + self.major


class Employee(Person):
    def __init__(self, *, employee_id, **kwargs):
        print("Entering Employee.__init__", id(self))
        self.employee_id = employee_id
        super().__init__(**kwargs)

    def rename_as_employee(self, new_name):
        self.name = new_name

    def role(self):
        return "Employee #" + str(self.employee_id)


class TeachingAssistant(Student, Employee):
    def __init__(self, *, course, **kwargs):
        print("Entering TeachingAssistant.__init__", id(self))
        self.course = course
        super().__init__(**kwargs)


class ReversedTeachingAssistant(Employee, Student):
    def __init__(self, *, course, **kwargs):
        print("Entering ReversedTeachingAssistant.__init__", id(self))
        self.course = course
        super().__init__(**kwargs)


class RobotAssistant:
    def role(self):
        return "Robot assistant"


def announce(value):
    print(value.role())


print("--- Constructing one TeachingAssistant ---")
ta = TeachingAssistant(
    name="Alex Rivera",
    major="Computer Science",
    employee_id=1042,
    course="CS 240",
)
print("id(ta) after construction:", id(ta))

print("\n--- 1. TeachingAssistant.__mro__ ---")
print(TeachingAssistant.__mro__)

print("\n--- 2. vars(ta) ---")
print(vars(ta))

print("\n--- 3. Renaming through Student and Employee methods ---")
ta.rename_as_student("Alex R.")
print("After rename_as_student:", ta.name)
ta.rename_as_employee("A. Rivera")
print("After rename_as_employee:", ta.name)
print("vars(ta):", vars(ta))

print("\n--- 4. ta.role() with no TeachingAssistant.role ---")
print("Prediction: Student comes first in the MRO, so Student.role runs.")
print("Result:", ta.role())

print("\n--- 5. ta.role() after adding TeachingAssistant.role ---")


def teaching_assistant_role(self):
    return "Teaching assistant for " + self.course


# Adds the method to the class after the fact, the same as if it had been
# written inside the class body.
TeachingAssistant.role = teaching_assistant_role
print("Result:", ta.role())

print("\n--- 6. ReversedTeachingAssistant (bases reversed, no role override) ---")
rta = ReversedTeachingAssistant(
    name="Alex Rivera",
    major="Computer Science",
    employee_id=1042,
    course="CS 240",
)
print(ReversedTeachingAssistant.__mro__)
print("Result:", rta.role())

print("\n--- Polymorphism without common ancestry ---")
robot = RobotAssistant()
print("announce(ta):    ", end="")
announce(ta)
print("announce(robot): ", end="")
announce(robot)
print("announce(42):    ", end="")
try:
    announce(42)
except AttributeError as error:
    print("AttributeError:", error)

print("\nProgram finished normally.")
