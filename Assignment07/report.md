Assignment 07 Report - How C++ and Python Implement Objects

Compiled with g++ 13.3 using -std=c++20. Python 3.13. Addresses and ids change every run, so only compare them to each other.


Part 1 - C++ with ordinary multiple inheritance

Output:

    Person constructor
    Student constructor
    Person constructor
    Employee constructor
    TeachingAssistant constructor

    student_person  address: 0x7fff0363db80  name: Alex Rivera
    employee_person address: 0x7fff0363dbc0  name: Alex Rivera

    (after setting student_person.name = "Alex R.")
    student_person  name: Alex R.
    employee_person name: Alex Rivera

Compiler errors for the two invalid lines:

    std::cout << ta.name << '\n';
    error: request for member 'name' is ambiguous

    Person* person = &ta;
    error: 'Person' is an ambiguous base of 'TeachingAssistant'

A class is the blueprint, like Person. A base-class subobject is an actual chunk of memory inside a bigger object that holds that base class's data. With normal inheritance, Student has its own Person chunk inside it and Employee has its own Person chunk inside it. TeachingAssistant contains both a Student and an Employee, so it ends up with two separate Person chunks. That's why the Person constructor prints twice, why the two addresses are different, and why changing the name through one doesn't change the other. They are two different strings.

The compiler rejects ta.name and &ta because it can't tell which Person you mean. There are two of them, so asking for "the" name or "the" Person is ambiguous, and C++ won't guess.


Part 2 - C++ with virtual inheritance

Output:

    Person constructor
    Student constructor
    Employee constructor
    TeachingAssistant constructor

    student_person  address: 0x7fff90f81ff8  name: Alex Rivera
    employee_person address: 0x7fff90f81ff8  name: Alex Rivera

    (after setting student_person.name = "Alex R.")
    student_person  name: Alex R.
    employee_person name: Alex R.

    ta.name             -> Alex R.
    Person* person = &ta -> compiles
    role() through Person& -> Teaching assistant for CS 240

Now Person is only built once, both paths give the same address, and changing the name through one side shows up on the other. ta.name and &ta compile because there is only one Person to pick.

Virtual inheritance solves the duplicate-base problem. It makes Student and Employee share one Person instead of each having their own.

The most-derived class, TeachingAssistant, has to construct Person because Person is shared. If Student and Employee both got to build it, Person would be built twice with possibly different arguments. So C++ skips their Person(name) calls when they're part of a TeachingAssistant and only uses the one from TeachingAssistant. Since Person has no default constructor, TeachingAssistant has to call Person(name) itself or it won't compile.

Compiler error with TeachingAssistant::role() removed:

    error: no unique final overrider for 'virtual std::string Person::role() const' in 'TeachingAssistant'

Virtual inheritance only fixes the shared data. It doesn't fix the functions. There is still one role() slot in the shared Person, and both Student and Employee override it. Neither one is more specific than the other, so C++ has no rule to pick a winner and refuses to compile. TeachingAssistant has to give its own role() to settle it. I restored the override after recording the error.


Part 3 - Python with cooperative multiple inheritance

Initializer trace (same number every line):

    Entering TeachingAssistant.__init__ 140585706155248
    Entering Student.__init__ 140585706155248
    Entering Employee.__init__ 140585706155248
    Entering Person.__init__ 140585706155248
    id(ta) after construction: 140585706155248

MRO and state:

    TeachingAssistant.__mro__:
    (TeachingAssistant, Student, Employee, Person, object)

    vars(ta):
    {'course': 'CS 240', 'major': 'Computer Science', 'employee_id': 1042, 'name': 'Alex Rivera'}

    After rename_as_student:  Alex R.
    After rename_as_employee: A. Rivera

    ta.role() with no TeachingAssistant.role -> Student majoring in Computer Science
    ta.role() after adding it              -> Teaching assistant for CS 240

    ReversedTeachingAssistant.__mro__:
    (ReversedTeachingAssistant, Employee, Student, Person, object)
    rta.role() -> Employee #1042

The id is the same the whole time because Python makes one object first and then every __init__ just fills in that same object. None of the initializers create anything new. They all receive the same self.

There's only one name because a Python object stores its data in one dictionary (vars shows it). Every class writes into that same dictionary, so self.name is one key no matter which class sets it. That's why rename_as_student and rename_as_employee both change the same value, and the last one wins.

Base order decides the MRO, and the MRO decides lookup. Python searches the classes in MRO order and uses the first role() it finds. With (Student, Employee), Student comes first, so Student.role ran, which matched my prediction. With the bases reversed, Employee comes first, so Employee.role ran.

super() means "go to the next class in the MRO of the actual object," not "go to my parent." Student's parent is Person, but in a TeachingAssistant the trace shows Student's super().__init__ went to Employee, not Person, because Employee comes after Student in this object's MRO. That's how every initializer runs exactly once and Person runs last.


Part 4 - Polymorphism without common ancestry

C++ results:

    announce_person(ta)      -> compiles, prints Teaching assistant for CS 240
    announce_person(robot)   -> does not compile:
        error: invalid initialization of reference of type 'const Person&' from expression of type 'RobotAssistant'
    announce_generic(ta)     -> compiles, prints Teaching assistant for CS 240
    announce_generic(robot)  -> compiles, prints Robot assistant
    announce_generic(42)     -> does not compile (extra test):
        error: request for member 'role' in 'value', which is of non-class type 'const int'

Python results:

    announce(ta)     -> Teaching assistant for CS 240
    announce(robot)  -> Robot assistant
    announce(42)     -> AttributeError: 'int' object has no attribute 'role'

Inheritance-based polymorphism (announce_person) only takes things that actually inherit from Person. The virtual function picks the right role() at runtime, but the type has to be a Person, and that is checked at compile time. RobotAssistant has role() but isn't a Person, so it's rejected.

Templates (announce_generic) don't care about inheritance. The compiler makes a separate copy of the function for each type you use and checks that value.role() works for that type. RobotAssistant works because it has role(). An int fails, but it fails at compile time, when the template gets built for int.

Python duck typing doesn't check anything ahead of time. announce just tries value.role() when it runs. Anything with a role() works, and anything without one fails at runtime with an AttributeError.

So the missing role() is caught at compile time in both C++ versions, and only at runtime in Python.


Analysis questions

1. What state is duplicated in the nonvirtual C++ diamond, and what changes under virtual inheritance?
The whole Person part is duplicated, which here is the name string. There are two Person subobjects at two addresses, the constructor runs twice, and the names can drift apart. With virtual inheritance there is one shared Person, one constructor call, one address and one name.

2. How do virtual inheritance and virtual functions serve different purposes?
Virtual inheritance is about data layout: how many copies of a base class are inside the object. Virtual functions are about which function runs when you call through a base reference or pointer. They both use the word virtual but they solve different problems. Part 2 shows this: virtual inheritance gave one Person, but role() still needed its own fix.

3. Why does Python's implementation avoid automatically creating two Person subobjects? Does that eliminate all possible initialization or naming conflicts?
Python objects don't have subobjects. There is one object with one dictionary of attributes, and the classes only supply methods. Every __init__ writes into that same dictionary, so there is nothing to duplicate. It doesn't remove every conflict, though. If Student and Employee both used an attribute with the same name for different meanings, they'd overwrite each other, like the two rename methods did. Initialization only works if every class cooperates by taking its own keyword arguments and calling super(). If one class called its parent directly or forgot super(), some initializers could be skipped or run twice.

4. Why can Python select an inherited role() using its MRO while the C++ virtual diamond requires a unique final override?
Python has a fixed rule: line the classes up in MRO order and take the first match, so there's always an answer, and it depends on base order. C++ has no ordering rule between sibling bases. Student::role and Employee::role are equally specific, and C++ won't pick one based on the order you listed the bases, so it makes you write a final override yourself.

5. Does sharing Python's universal base object guarantee that a value supports role()?
No. Every class in Python inherits from object, including int, but object doesn't have a role() method. Calling announce(42) raised AttributeError: 'int' object has no attribute 'role'. Having a common base only tells you what that base provides, and object doesn't provide role(). Whether a value has role() is only found out when the call actually runs.

6. Which design would you choose for this personnel model: multiple inheritance or composition with separate student and employee roles?
I would use composition. A Person would hold an optional StudentRole (major) and an optional EmployeeRole (employee_id), and a TA is just a person who has both plus a course. Being a student or an employee is something a person has for a while, not what they are. Someone can graduate and stay employed, or stop working and keep studying, and with composition you just add or remove a role. With inheritance you'd have to switch the object to a different class, which neither language really lets you do. Composition also avoids the whole diamond problem: no duplicate Person, no virtual inheritance, no ambiguous role(), and no depending on base order. Each role is simple to test by itself. Multiple inheritance works, as this assignment shows, but it takes extra rules in both languages to make it behave.
